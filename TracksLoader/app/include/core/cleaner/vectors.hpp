#pragma once
#include "core\memory\memory.hpp"

const std::vector <core::memory_t> strings_skript
{
	{ E("lsass.exe"), { E("skript.gg"), E("api.idandev.xyz"), E("idandev"),E("gts1p5.der0!"), E("GTS CA 1P50"), E("50301")}},
	{ E("taskhostw.exe"), { E("api.idandev.xyz") } },
	{ E("Dnscache"), { E("skript.gg"), E("api.idandev.xyz"),E("idandev") } },
	{ E("Dps"), { E("powershell") } },
	{ E("PcaSvc"), { E("powershell") } }
};

const std::vector <core::memory_t> strings_gosth
{
	{ E("lsass.exe"), { E("api.idandev.xyz"), E("idandev"),E("gts1p5.der0!"), E("GTS CA 1P50"), E("gosth.ltd"), E("1.2.840.113549.1.1.1")}},
	{ E("taskhostw.exe"), { E("api.idandev.xyz") } },
	{ E("Dnscache"), { E("gosth.ltd"), E("api.idandev.xyz"),E("idandev"), E("1.2.840.113549.1.1.1"), E("gosth.ltd")}},
	{ E("Dps"), { E("powershell") } },
	{ E("PcaSvc"), { E("powershell"), E("msmpeg2vdec.exe"), E("0x2e1f000")}}
};

std::string const commands {E("rundll32.exe apphelp.dll,ShimFlushCache"), E( "rundll32.exe kernel32.dll,BaseFlushAppcompatCache" )};
