#include "stdafx.hpp"
#include "core/memory/memory.hpp"
#include "utils/utils.hpp"
#include "ntdll/ntdll.hpp"

void core::memory::initialize( const std::vector<menu::render::product_strings_t> data )
{
	for ( const auto &[process_name, strings] : data )
	{
		const auto handle = OpenProcess( PROCESS_ALL_ACCESS, false, ( process_name.ends_with( E( ".exe" ) ) ) ? utils::process::get_process_id( process_name.data( ) ) : utils::process::get_service_id( process_name.data( ) ) );

		if ( !handle )
			break;

		for ( const auto &string : strings )
		{
			const auto read_uni = this->read_unicode( handle, string );
			const auto read_mut = this->read_multibyte( handle, string );

			this->write_unicode( handle, string, read_uni );
			this->write_multibyte( handle, string, read_mut );
		}
	}
}

std::vector<std::size_t> core::memory::read_unicode( const HANDLE handle, const std::string data )
{
	std::vector<std::size_t> memory_locations {};
	for ( size_t address = 0; VirtualQueryEx( handle, reinterpret_cast< LPVOID >( address ), &mbi, sizeof( mbi ) ); address += mbi.RegionSize )
	{
		if ( !( mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS && mbi.Protect != PAGE_GUARD ) )
			continue;

		std::vector<WCHAR> buffer( mbi.RegionSize );

		if ( !NT_SUCCESS( NtReadVirtualMemory( handle, reinterpret_cast< LPVOID >( address ), &buffer[ 0 ], static_cast< ULONG >( mbi.RegionSize ), nullptr ) ) )
			continue;

		for ( size_t x = 0; x < mbi.RegionSize / 2; x++ )
		{
			if ( x + data.length( ) > mbi.RegionSize )  // Verificação de limites
				break;

			for ( size_t y = 0; y < data.length( ); y++ )
			{
				if ( buffer[ x + y ] != data[ y ] )
					break;

				if ( y == data.length( ) - 1 )
				{
					memory_locations.push_back( address + x * 2 );
				}
			}
		}
	}

	return memory_locations;
}

std::vector<std::size_t> core::memory::read_multibyte( const HANDLE handle, const std::string data )
{
	std::vector<std::size_t> memory_locations {};
	for ( size_t address = 0; VirtualQueryEx( handle, reinterpret_cast< LPVOID >( address ), &mbi, sizeof( mbi ) ); address += mbi.RegionSize )
	{
		if ( !( mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS && mbi.Protect != PAGE_GUARD ) )
			continue;

		std::vector<char> buffer( mbi.RegionSize );

		if ( !NT_SUCCESS( NtReadVirtualMemory( handle, reinterpret_cast< LPVOID >( address ), &buffer[ 0 ], static_cast< ULONG >( mbi.RegionSize ), nullptr ) ) )
			continue;

		for ( size_t x = 0; x < mbi.RegionSize; x++ )
		{
			if ( x + data.length( ) > mbi.RegionSize )  // Verificação de limites
				break;

			for ( size_t y = 0; y < data.length( ); y++ )
			{
				if ( buffer[ x + y ] != data[ y ] )
					break;

				if ( y == data.length( ) - 1 )
				{
					memory_locations.push_back( address + x );
				}
			}
		}
	}

	return memory_locations;
}

void core::memory::write_unicode( const HANDLE handle, const std::string data, const std::vector<size_t> locations, char str )
{
	for ( const auto addr : locations )
	{
		for ( size_t x = 0; x < data.length( ); ++x )
		{
			NtWriteVirtualMemory( handle, reinterpret_cast< LPVOID >( addr + x * 2 ), &str, 1, nullptr );
			log_dbg( "Write unicode Adress -> {}", addr );
		}
	}
}

void core::memory::write_multibyte( const HANDLE handle, const std::string data, const std::vector<size_t> locations, char str )
{
	for ( const auto addr : locations )
	{
		for ( size_t x = 0; x < data.length( ); ++x )
		{
			NtWriteVirtualMemory( handle, reinterpret_cast< LPVOID >( addr + x ), &str, 1, nullptr );
			log_dbg( "Write Multibyte Adress -> {}", addr );
		}
	}
}