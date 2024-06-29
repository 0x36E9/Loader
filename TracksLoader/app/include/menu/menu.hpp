#pragma once

#include "menu\custom\custom.hpp"
#include "particles\imgui_particles.hpp"
#include "utils\utils.hpp"
#include "requests\requests.hpp"

extern IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND, UINT, WPARAM, LPARAM );

namespace menu::window
{
	class create_window
	{
	private:
		WNDCLASSEX	wc = {};
		ImVec2		window_size = {};
		HWND		hwnd = {};

		ID3D11Device *device = {};
		ID3D11DeviceContext *device_ctx = {};
		ID3D11RenderTargetView *render_t = {};
		IDXGISwapChain *swap_chain = {};

	private:
		static LRESULT WINAPI wnd_proc( HWND, UINT, WPARAM, LPARAM );
		bool create_device( );
		void render_blur( );

	public:
		create_window( const ImVec2 size )
		{
			this->wc =
			{
				sizeof( wc ),
				CS_CLASSDC,
				wnd_proc,
				0L,
				0L,
				GetModuleHandle( nullptr ),
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				" ",
				nullptr
			};

			RegisterClassExA( &wc );

			this->hwnd = CreateWindowExA( WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW, " ", " ", WS_POPUP | CW_USEDEFAULT,
										  GetSystemMetrics( SM_CXSCREEN ) / 2 - ( size.x / 2 ),
										  GetSystemMetrics( SM_CYSCREEN ) / 2 - ( size.y / 2 ),
										  size.x, size.y, nullptr, nullptr, wc.hInstance, nullptr );

			this->window_size = size;
			this->create_device( );

			MARGINS margins { -1 };
			DwmExtendFrameIntoClientArea( this->hwnd, &margins );

			this->render_blur( );

			ShowWindow( this->hwnd, SW_SHOWDEFAULT );
			UpdateWindow( this->hwnd );
		};

		~create_window( )
		{
			DestroyWindow( this->hwnd );
			UnregisterClassA( this->wc.lpszClassName, this->wc.hInstance );
		};

	public:
		bool run( const std::function<void( ID3D11Device * )>, const std::function<bool( HWND, ImVec2 )> );
		void deatch( );

	};
}

namespace menu::render
{

	enum class cheat_t : std::uint32_t
	{
		ESKRIPT = 0,
		EGOSTH = 1
	};

	inline cheat_t cheat_type { cheat_t::ESKRIPT };

	static struct user_data_t
	{
		int days;
		std::vector<std::string> subscriptions;
	}	user_data;

	enum class t_tabs : std::uint32_t
	{
		LLOGIN = 0,
		LLOADINGLOGIN = 1,
		LREGISTER = 2,
		LLOADINGINJECT = 3,
		LLOADINGDESTRUCT = 4,
		LINJECTOR = 5,
		LCLEANER = 6,
		LFINALIZING = 7
	};

#ifdef _DEBUG
	inline t_tabs state { t_tabs::LLOGIN };
#else
	inline t_tabs state { t_tabs::LLOGIN };
#endif

	inline ImFont *poppins, *poppins_mn, *open_sans_bd, *poppins_bg, *codicon, *codicon_mn;

	void style( ID3D11Device * );
	bool paint( HWND, ImVec2 );
}