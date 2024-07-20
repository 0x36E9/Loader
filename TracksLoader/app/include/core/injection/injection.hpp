#pragma once

namespace core
{
	inline LPVOID map_vector_to_memory( const std::vector<std::uint8_t> &data )
	{
		const auto size = data.size( );
		if ( size == 0 )
		{
			return nullptr;
		}

		auto mem_block = std::make_unique<char[ ]>( size );
		std::ranges::copy( data, mem_block.get( ) );

		return reinterpret_cast< HANDLE >( mem_block.release( ) );
	}

	bool initialize_skript( );
	bool initialize_gosth( );
}
