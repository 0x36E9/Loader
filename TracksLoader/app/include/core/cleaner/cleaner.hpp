#pragma once
#include "core\cleaner\vectors.hpp"
#include "utils\utils.hpp"


namespace core
{
	class c_cleaner
	{

	public:
		auto initializer_skript( ) -> void;
		auto initializer_gosth( ) -> void;

	private:
		static auto zero_bytes(std::string const& path, std::string const& name) -> void;
		auto clean_usn_journal() -> bool;
	};
}
