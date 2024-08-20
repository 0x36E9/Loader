#pragma once

#define NT_SUCCESS(x) ((NTSTATUS)(x) >= NULL)
#include "menu/menu.hpp"

namespace core
{
	class memory
	{
	public:
		auto initialize( const std::vector<menu::render::product_strings_t> ) -> void;

		memory( ) = default;
		~memory( ) = default;

	private:
		MEMORY_BASIC_INFORMATION mbi {};

		std::vector<std::size_t> read_unicode( const HANDLE, const std::string );
		std::vector<std::size_t> read_multibyte( const HANDLE, const std::string );

		void write_unicode( const HANDLE, const std::string, const std::vector<size_t>, char str = 0 );
		void write_multibyte( const HANDLE, const std::string, const std::vector<size_t>, char str = 0 );
	};
}