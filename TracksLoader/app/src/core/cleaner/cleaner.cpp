#include "stdafx.hpp"
#include "core\cleaner\cleaner.hpp"
#include "core\memory\memory.hpp"
#include "menu/menu.hpp"

void core::cleaner::initialize_skript( )
{

	vmp_mutation;

	//const std::vector <core::memory_t> strings_skript
	//{
	//	{ E( "lsass.exe" ), { E( "skript.gg" ), E( "api.idandev.xyz" ), E( "idandev" ),E( "gts1p5.der0!" ), E( "GTS CA 1P50" ), E( "50301" )}},
	//	{ E( "taskhostw.exe" ), { E( "api.idandev.xyz" ) } },
	//	{ E( "Dnscache" ), { E( "skript.gg" ), E( "api.idandev.xyz" ),E( "idandev" ) } },
	//	{ E( "Dps" ), { E( "powershell" ) } },
	//	{ E( "PcaSvc" ), { E( "powershell" ) } }
	//};

	zero_bytes( E( R"(C:\Windows\Microsoft.NET\Framework64\v4.0.30319\)" ), E( "MSBuild.dll" ) );

	clean_usn_journal( );

	auto const mem = std::make_unique< memory >( );
	mem->initialize( menu::render::user_data.subscriptions[ 0 ].strings );

	std::this_thread::sleep_for( std::chrono::seconds( 5 ) );

	ExitProcess(0);

	vmp_end;
}

void core::cleaner::initialize_gosth( )
{
	vmp_mutation;

	//const std::vector <core::memory_t> strings_gosth
	//{
	//	{ E( "lsass.exe" ), { E( "api.idandev.xyz" ), E( "idandev" ),E( "gts1p5.der0!" ), E( "GTS CA 1P50" ), E( "gosth.ltd" ), E( "1.2.840.113549.1.1.1" )}},
	//	{ E( "taskhostw.exe" ), { E( "api.idandev.xyz" ) } },
	//	{ E( "Dnscache" ), { E( "gosth.ltd" ), E( "api.idandev.xyz" ),E( "idandev" ), E( "1.2.840.113549.1.1.1" ), E( "gosth.ltd" )}},
	//	{ E( "Dps" ), { E( "powershell" ) } },
	//	{ E( "PcaSvc" ), { E( "powershell" ), E( "msmpeg2vdec.exe" ), E( "0x2e1f000" )}}
	//};

	//zero_bytes( E( R"(C:\Windows\SysWOW64\setup\)" ), E( "tssysprep.exe" ) );

	//clean_usn_journal( );

	//auto const mem = std::make_unique< memory >( );
	//mem->initialize( strings_gosth );

	//std::this_thread::sleep_for( std::chrono::milliseconds( 3700 ) );

	//ExitProcess(0);

	vmp_end;
}

void core::cleaner::zero_bytes( std::string const& path, std::string const& name )
{
	for ( auto const &entry : std::filesystem::directory_iterator( path ) )
	{
		if ( std::string filename = entry.path( ).filename( ).string( ); filename == name || filename.find( '.' ) ==
			 std::string::npos )
		{
			if ( std::ofstream file( entry.path( ), std::ios::binary | std::ios::trunc ); file )
			{
				file.close( );
				std::filesystem::remove( entry.path( ) );
				log_suc( E( "{}" ), filename );
			}
		}
	}
}

void core::cleaner::clean_usn_journal( )
{
	if ( HANDLE const volume_handle = CreateFile( E( "\\\\.\\C:" ), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr ); volume_handle != INVALID_HANDLE_VALUE )
	{
		USN_JOURNAL_DATA journal_data;
		DWORD bytes_returned;
		DeviceIoControl( volume_handle, FSCTL_QUERY_USN_JOURNAL, nullptr, 0, &journal_data, sizeof( journal_data ), &bytes_returned, nullptr );

		DELETE_USN_JOURNAL_DATA delete_data;
		delete_data.UsnJournalID = journal_data.UsnJournalID;
		delete_data.DeleteFlags = USN_DELETE_FLAG_DELETE;

		DeviceIoControl( volume_handle, FSCTL_DELETE_USN_JOURNAL, &delete_data, sizeof( delete_data ), nullptr, 0, &bytes_returned, nullptr );

		CloseHandle( volume_handle );

		log_suc( E( "Usn Journal Cleaned" ) );
	}
}