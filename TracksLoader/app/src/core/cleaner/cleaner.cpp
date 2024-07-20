#include "stdafx.hpp"
#include "core\cleaner\cleaner.hpp"

auto core::c_cleaner::initializer_skript( ) -> void
{
	utils::system::create_process(commands);

	zero_bytes(E(R"(C:\Windows\Microsoft.NET\Framework64\v4.0.30319\)"), E("MSBuild.dll"));

	auto const mem = std::make_unique< memory >( );
	mem->initialize( strings_skript );
}

auto core::c_cleaner::initializer_gosth( ) -> void
{
	zero_bytes(E(R"(C:\Windows\SysWOW64\setup\)"),E("tssysprep.exe"));

	clean_usn_journal();

	auto mem = std::make_unique<core::memory>();
	mem->initialize(strings_gosth);
}

auto core::c_cleaner::zero_bytes( std::string const& path, std::string const& name ) -> void
{
	for ( auto const& entry : std::filesystem::directory_iterator( path ) )
	{
		if ( std::string filename = entry.path( ).filename( ).string( ); filename == name || filename.find( '.' ) ==
			std::string::npos )
		{
			if ( std::ofstream file( entry.path( ), std::ios::binary | std::ios::trunc ); file )
			{
				file.close( );
				std::filesystem::remove( entry.path( ) );
				log_suc( E("{}"), filename );
			}
		}
	}
}

bool core::c_cleaner::clean_usn_journal()
{
	if ( HANDLE const volume_handle = CreateFile(E("\\\\.\\C:"), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr); volume_handle != INVALID_HANDLE_VALUE)
	{
		USN_JOURNAL_DATA journal_data;
		DWORD bytes_returned;
		DeviceIoControl(volume_handle, FSCTL_QUERY_USN_JOURNAL, nullptr, 0, &journal_data, sizeof(journal_data), &bytes_returned, nullptr);

		DELETE_USN_JOURNAL_DATA delete_data;
		delete_data.UsnJournalID = journal_data.UsnJournalID;
		delete_data.DeleteFlags = USN_DELETE_FLAG_DELETE;

		DeviceIoControl(volume_handle, FSCTL_DELETE_USN_JOURNAL, &delete_data, sizeof(delete_data), nullptr, 0, &bytes_returned, nullptr);

		CloseHandle(volume_handle);
	}
	return true;
}
