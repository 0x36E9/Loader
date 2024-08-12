#include "stdafx.hpp"
#include "menu\menu.hpp"
#include "security\security.hpp"

bool menu::window::create_window::run( const std::function<void( ID3D11Device * )> style, const std::function<bool( HWND, ImVec2 )> render )
{
	ImGui::CreateContext( );

	ImGui_ImplWin32_Init( this->hwnd );
	ImGui_ImplDX11_Init( this->device, this->device_ctx );

	style( this->device );

	MSG msg;
	std::memset( &msg, 0, sizeof( msg ) );

	particles::setup_circles( );

	auto offset_x = 0, offset_y = 0;

	while ( msg.message != WM_QUIT )
	{
		if ( PeekMessageA( &msg, nullptr, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessageA( &msg );
			continue;
		}

		ImGui_ImplDX11_NewFrame( );
		ImGui_ImplWin32_NewFrame( );

		ImGui::NewFrame( );
		{
			if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
			{
				POINT point;
				RECT  rect;

				GetCursorPos( &point );
				GetWindowRect( hwnd, &rect );

				offset_x = point.x - rect.left;
				offset_y = point.y - rect.top;
			}

			static auto next_time = ImGui::GetTime( ) + 5.f;

			if ( ImGui::GetTime( ) > next_time )
			{
#ifndef _DEBUG
				const auto sec = std::make_unique<security::runtime_security>( );
				sec->start( );
#endif
				next_time = ImGui::GetTime( ) + 5.f;
			}

			if ( ImGui::IsMouseDragging( ImGuiMouseButton_Left ) && offset_y >= 0 &&
				 offset_y <= ImGui::GetTextLineHeight( ) + ImGui::GetStyle( ).FramePadding.y * 1.0f )
			{
				POINT point;
				GetCursorPos( &point );
				SetWindowPos( hwnd, nullptr, point.x - offset_x, point.y - offset_y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
			}

			if ( !render( this->hwnd, this->window_size ) )
			{
				msg.message = WM_QUIT;
			}
		}
		ImGui::Render( );

		const float clear_color_with_alpha[ 4 ] = { 0, 0, 0, 0 };
		this->device_ctx->OMSetRenderTargets( 1, &this->render_t, nullptr );
		this->device_ctx->ClearRenderTargetView( this->render_t, clear_color_with_alpha );

		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );
		this->swap_chain->Present( 1, 0 );
	}

	return false;
}

void menu::window::create_window::deatch( )
{
	if ( this->render_t != nullptr )
	{
		this->render_t->Release( );
		this->render_t = nullptr;
	}

	if ( this->device != nullptr )
	{
		this->device->Release( );
		this->device = nullptr;
	}

	if ( this->device_ctx != nullptr )
	{
		this->device_ctx->Release( );
		this->device_ctx = nullptr;
	}

	if ( this->swap_chain != nullptr )
	{
		this->swap_chain->Release( );
		this->swap_chain = nullptr;
	}
}

LRESULT WINAPI menu::window::create_window::wnd_proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
	if ( ImGui_ImplWin32_WndProcHandler( hwnd, msg, wparam, lparam ) )
		return true;

	return DefWindowProcA( hwnd, msg, wparam, lparam );
}

bool menu::window::create_window::create_device( )
{
	DXGI_SWAP_CHAIN_DESC scd;
	std::memset( &scd, 0, sizeof( scd ) );

	scd.BufferCount = 2;
	scd.BufferDesc.Width = 0;
	scd.BufferDesc.Height = 0;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = this->hwnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = true;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT create_device_flags { 0 };
	D3D_FEATURE_LEVEL feature_level {};

	if ( constexpr D3D_FEATURE_LEVEL feature_level_array[ 2 ] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 }; D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &scd, &this->swap_chain, &this->device, &feature_level, &this->device_ctx ) == DXGI_ERROR_UNSUPPORTED )
	{
		if ( D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_WARP, nullptr, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &scd, &this->swap_chain, &this->device, &feature_level, &this->device_ctx ) != S_OK )
			return false;
	}

	ID3D11Texture2D *back_buffer {};
	swap_chain->GetBuffer( 0, IID_PPV_ARGS( &back_buffer ) );
	device->CreateRenderTargetView( back_buffer, nullptr, &this->render_t );
	back_buffer->Release( );

	return true;
}

void menu::window::create_window::render_blur( )
{
	struct accentpolicy
	{
		int na;
		int nf;
		int nc;
		int n_a;
	};

	struct wincompattrdata
	{
		int na;
		PVOID pd;
		ULONG ul;
	};

	if ( const HINSTANCE hm = LoadLibrary( "user32.dll" ) )
	{
		typedef BOOL( WINAPI *p_set_window_composition_attribute )( HWND, wincompattrdata * );

		if ( const auto set_window_composition_attribute = reinterpret_cast< p_set_window_composition_attribute >( GetProcAddress( hm, "SetWindowCompositionAttribute" ) ) )
		{
			accentpolicy policy = { 3, 0, 0, 0 };
			wincompattrdata data = { 19, &policy,sizeof( accentpolicy ) };
			set_window_composition_attribute( this->hwnd, &data );
		}
		FreeLibrary( hm );
	}
}
