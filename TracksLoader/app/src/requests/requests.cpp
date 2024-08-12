#include "stdafx.hpp"
#include "requests\requests.hpp"

network::response_t network::requests::perform_request( const std::string &method, const std::string &url, const std::string &body, const std::string &bearer_token ) const
{
    const auto curl = curl_easy_init( );

    if ( !curl )
    {
        return { 0, common_error_ };
    }

    curl_slist *headers = nullptr;
    headers = curl_slist_append( headers, "Content-Type: application/json" );
    headers = curl_slist_append( headers, "User-Agent: Services/1.0.0 (24aeeb2b43db)" );
    headers = curl_slist_append( headers, ( "X-Api-Key: " + std::string( api_key_ ) ).c_str( ) );

    if ( !bearer_token.empty( ) )
    {
        headers = curl_slist_append( headers, ( "Authorization: Bearer " + bearer_token ).c_str( ) );
    }

    curl_easy_setopt( curl, CURLOPT_URL, url.c_str( ) );
    curl_easy_setopt( curl, CURLOPT_PINNEDPUBLICKEY, "sha256//fx0czMpWrXr/jRX1QqUwB/+DVJSS9xC69PcXnVKgcHY=" );
    curl_easy_setopt( curl, CURLOPT_HTTPHEADER, headers );
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_callback );

    std::string buffer;
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, &buffer );

    if ( method == "POST" )
    {
        curl_easy_setopt( curl, CURLOPT_POST, 1L );
        curl_easy_setopt( curl, CURLOPT_POSTFIELDS, body.c_str( ) );
    }

    const auto res = curl_easy_perform( curl );

    long http_code = 0;
    curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &http_code );

    curl_slist_free_all( headers );
    curl_easy_cleanup( curl );

    return handle_response( res, buffer, http_code );
}

network::response_t network::requests::handle_response( CURLcode res, const std::string &buffer, long http_code ) const
{
    if ( res != CURLE_OK || buffer.empty( ) )
    {
        return { 0, common_error_ };
    }

    nlohmann::json json_data;
    try
    {
        json_data = nlohmann::json::parse( buffer );
    }
    catch ( ... )
    {
        return { 0, common_error_ };
    }

    return { static_cast< std::uint32_t >( http_code ), json_data };
}

network::response_t network::requests::post_with_image( const std::string &path, const std::vector<uint8_t> &image_data, const nlohmann::json &json ) const
{
    CURL *curl;
    CURLcode res;
    std::string response_buffer;

    curl_mime *mime;
    curl_mimepart *part;
    struct curl_slist *headers = nullptr;

    curl = curl_easy_init( );
    if ( !curl )
    {
        return { 0, common_error_ };
    }

    mime = curl_mime_init( curl );
    if ( !mime )
    {
        curl_easy_cleanup( curl );
        return { 0, common_error_ };
    }

    part = curl_mime_addpart( mime );
    curl_mime_name( part, "image" );
    curl_mime_filename( part, "image.png" );
    curl_mime_data( part, reinterpret_cast< const char * >( image_data.data( ) ), image_data.size( ) );

    for ( auto &[key, value] : json.items( ) )
    {
        part = curl_mime_addpart( mime );
        curl_mime_name( part, key.c_str( ) );
        curl_mime_data( part, value.get<std::string>( ).c_str(), CURL_ZERO_TERMINATED );
    }

    headers = curl_slist_append( headers, "User-Agent: Services/1.0.0 (24aeeb2b43db)" );
    headers = curl_slist_append( headers, ( std::string( "X-Api-Key: " ) + api_key_ ).c_str( ) );

    curl_easy_setopt( curl, CURLOPT_URL, ( base_url_ + path ).c_str( ) );
    curl_easy_setopt( curl, CURLOPT_HTTPHEADER, headers );
    curl_easy_setopt( curl, CURLOPT_MIMEPOST, mime );
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_callback );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, &response_buffer );
    curl_easy_setopt( curl, CURLOPT_PINNEDPUBLICKEY, "sha256//fx0czMpWrXr/jRX1QqUwB/+DVJSS9xC69PcXnVKgcHY=" );

    res = curl_easy_perform( curl );

    long http_code = 0;
    curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &http_code );

    curl_slist_free_all( headers );
    curl_mime_free( mime );
    curl_easy_cleanup( curl );

    return handle_response( res, response_buffer, http_code );
}

std::vector<uint8_t> network::requests::download( const std::string &path ) const
{
    CURL *curl;
    CURLcode res;
    std::vector<uint8_t> response_buffer;

    curl = curl_easy_init( );

    if ( !curl )
    {
        return {};
    }

    curl_easy_setopt( curl, CURLOPT_URL, path.c_str( ) );
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_data_callback );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, &response_buffer );

    res = curl_easy_perform( curl );

    curl_easy_cleanup( curl );

    if ( res != CURLE_OK )
    {
        return {};
    }

    return response_buffer;
}

network::response_t network::requests::get( const std::string &path, const std::string &bearer_token ) const
{
    const auto response = perform_request( "GET", base_url_ + path, "", bearer_token );
    log_dbg( "[network] request path /{} status {}", path, response.status_code );
    return response;
}

network::response_t network::requests::post( const std::string &path, const nlohmann::json &json, const std::string &bearer_token ) const
{
    const auto response = perform_request( "POST", base_url_ + path, json.dump( ), bearer_token );
    log_dbg( "[network] request path /{} status {}", path, response.status_code );
    return response;
}