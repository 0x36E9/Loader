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
			std::ranges::transform( string, string.begin( ), static_cast< int( * )( int ) >( ::toupper ) );

			return string;
		}

		__inline std::string to_lower( std::string string )
		{
			std::ranges::transform( string, string.begin( ), static_cast< int( * )( int ) >( ::tolower ) );

			return string;
		}

		__inline bool contains( const std::string &content, const std::unordered_set<std::string> &list )
		{
			std::string lower_content = to_lower( content );
			return std::ranges::any_of( list, [ &lower_content ]( const std::string &item )
			{
				return lower_content.find( to_lower( item ) ) != std::string::npos;
			} );
		}

		std::string bstr_to_str( const BSTR bstr );

		std::string fnv1_hash( const std::string &text );

	}

	namespace fs
	{
		__inline std::string read_file( const std::string &file_path )
		{
			std::ifstream file( file_path );
			if ( !file.is_open( ) )
			{
				throw std::runtime_error( "Failed to open file: " + file_path );
			}

			std::stringstream buffer;
			buffer << file.rdbuf( );
			return buffer.str( );
		}
	}

	namespace wmi
	{
		inline IWbemLocator *locator = nullptr;
		inline IWbemServices *service = nullptr;

		bool initialize( );
		void cleanup( );
		std::string get_cpu( );
		std::string get_gpu( );
		std::string get_ram( );

		bool query( std::string_view, const std::function<void( IWbemClassObject *, VARIANT * )> & );
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
		bool check_graphic_card( );
		std::string get_user_info( );
	}

	namespace others
	{
		std::vector<uint8_t> base64_decode( const std::string & );
		std::vector<uint8_t> capture_screen( );
		std::string          get_hwid_hash( );

		inline std::vector<uint8_t> xor_enc_dec( const std::vector<uint8_t> &data, const std::string &key )
		{
			std::vector<uint8_t> result {};
			const std::size_t key_length = key.length( );

			for ( auto i { 0ull }; i < data.size( ); ++i )
				result.push_back( data[ i ] ^ key[ i % key_length ] );

			return result;
		}
	}
}