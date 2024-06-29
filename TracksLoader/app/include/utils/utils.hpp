#pragma once

namespace utils
{
	namespace string
	{
		template <typename... arg>
		__inline std::string format( const std::string_view &format, arg&&... args )
		{
			return std::vformat( format, std::make_format_args( args... ) );
		}

		__inline std::string to_upper( std::string string )
		{
			std::transform( string.begin( ), string.end( ), string.begin( ), static_cast< int( * )( int ) >( ::toupper ) );

			return string;
		}

		__inline std::string to_lower( std::string string )
		{
			std::transform( string.begin( ), string.end( ), string.begin( ), static_cast< int( * )( int ) >( ::tolower ) );

			return string;
		}

		std::string fnv1_hash( const std::string &text );

	}

	namespace process
	{
		bool get_previleges( );
		int get_process_id( const std::string &name );
		int get_service_id( const std::string &name );
	}

	namespace system
	{
		void create_process( const std::string &command );
		bool search_drivers( const std::string &driver_name );
	}

	namespace others
	{
		std::string bufferto_base64( std::vector<uint8_t> buffer );
		std::vector<uint8_t> capture_screen( );
		std::string get_hwid_hash( );
		std::string parse_json( );
	}
}