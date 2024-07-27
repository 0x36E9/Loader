#pragma once

namespace core::cleaner
{
	void initialize_skript( );
	void initialize_gosth( );
	void zero_bytes( std::string const &path, std::string const &name );
	void clean_usn_journal( );
}
