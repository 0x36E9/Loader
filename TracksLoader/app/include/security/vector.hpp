#pragma once

std::vector<std::string> vm_files
{
	E( R"(C:\system32\drivers\VBoxMouse.sys)" ), E( R"(C\system32\drivers\VBoxGuest.sys)" ), E( R"(C:\system32\drivers\VBoxSF.sys)" ),
	E( R"(C:\system32\drivers\VBoxVideo.sys)" ), E( R"(C:\system32\vboxdisp.dll)" ), E( R"(C:\system32\vboxhook.dll)" ),
	E( R"(C:\system32\vboxmrxnp.dll)" ),E( R"(C:\system32\vboxogl.dll)" ), E( R"(C:\system32\vboxoglarrayspu.dll)" ),
	E( R"(C:\system32\vboxoglcrutil.dll)" ), E( R"(C:\system32\vboxoglerrorspu.dll)" ), E( R"(C:\system32\vboxoglfeedbackspu.dll)" ),
	E( R"(C:\system32\vboxoglpackspu.dll)" ), E( R"(C:\system32\vboxoglpassthroughspu.dll)" ), E( R"(C:\system32\vboxservice.exe)" ),
	E( R"(C:\system32\vboxtray.exe)" ), E( R"(C:\system32\VBoxControl.exe)" ), E( R"(C:\system32\drivers\vmmouse.sys)" ),
	E( R"(C:\system32\drivers\vmhgfs.sys)" ), E( R"(C:\system32\drivers\vm3dmp.sys)" ), E( R"(C:\system32\drivers\vmci.sys)" ),
	E( R"(C:\system32\drivers\vmhgfs.sys)" ), E( R"(C:\system32\drivers\vmmemctl.sys)" ), E( R"(C:\system32\drivers\vmmouse.sys)" ),
	E( R"(C:\system32\drivers\vmrawdsk.sys)" ), E( R"(C:\system32\drivers\vmusbmouse.sys)" )
};

std::vector<std::string> process
{
	E( "ollydbg.exe" ),E( "tcpview.exe" ), E( "autoruns.exe" ),E( "autorunsc.exe" ),
	E( "filemon.exe" ), E( "procmon.exe" ), E( "regmon.exe" ), E( "procexp.exe" ),
	E( "ida.exe" ), E( "ida64.exe" ), E( "ImmunityDebugger.exe" ), E( "Wireshark.exe" ),
	E( "dumpcap.exe" ), E( "HookExplorer.exe" ), E( "ImportREC.exe" ), E( "PETools.exe" ),
	E( "LordPE.exe" ), E( "SysInspector.exe" ), E( "proc_analyzer.exe" ), E( "sysAnalyzer.exe" ),
	E( "sniff_hit.exe" ), E( "windbg.exe" ), E( "joeboxcontrol.exe" ), E( "joeboxserver.exe" ),
	E( "joeboxserver.exe" ), E( "ResourceHacker.exe" ), E( "x32dbg.exe" ), E( "x64dbg.exe" ),
	E( "Fiddler.exe" ), E( "httpdebugger.exe" ), E( "idaq.exe" ), E( "idaq64.exe" ), E( "SystemInformer.exe" )
};