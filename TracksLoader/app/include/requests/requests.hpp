#pragma once

namespace network
{
    struct response_t
    {
        std::uint32_t status_code {};
        nlohmann::json data {};
    };

    class requests
    {
    public:
        [[nodiscard]] response_t get( const std::string &, const std::string & = "" ) const;
        [[nodiscard]] response_t post( const std::string &, const nlohmann::json &, const std::string & = "" ) const;
        [[nodiscard]] response_t post_with_image( const std::string &, const std::vector<uint8_t> &, const nlohmann::json & ) const;
        [[nodiscard]] std::vector<uint8_t> download(const std::string&) const;

    private:
        static constexpr const char *base_url_ = "https://api.undetect.su/";
        static constexpr const char *api_key_ = "e278017841df";
        static inline const nlohmann::json common_error_ = { {E( "statusCode"), 0}, {E( "message"), E( "Internal server error!")} };

        static size_t write_callback( void *contents, size_t size, size_t nmemb, void *userp )
        {
            std::string *buffer = static_cast< std::string * >( userp );
            buffer->append( static_cast< char * >( contents ), size * nmemb );
            return size * nmemb;
        }

        static size_t write_data_callback( void *contents, size_t size, size_t nmemb, void *userp )
        {
            auto *buffer = static_cast< std::vector<uint8_t>* >( userp );
            size_t total_size = size * nmemb;
            buffer->insert( buffer->end( ), static_cast< uint8_t * >( contents ), static_cast< uint8_t * >( contents ) + total_size );
            return total_size;
        }

        response_t perform_request( const std::string &, const std::string &, const std::string &, const std::string & ) const;
        response_t handle_response( CURLcode , const std::string &, long ) const;
    };
}

inline auto g_requests = std::make_unique<network::requests>( );
