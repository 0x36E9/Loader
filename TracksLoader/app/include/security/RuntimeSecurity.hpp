#pragma once

#include <memory>
#include <intrin.h>
#include <windows.h>
#include <winternl.h>
#include <winioctl.h>
#include <psapi.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

#ifdef DEBUG
	#define WRAP_IF_RELEASE( s )
	#define WRAP_IF_DEBUG( s ) { s; }

#else
	#define WRAP_IF_RELEASE( s ) { s; }
	#define WRAP_IF_DEBUG( s )
#endif

namespace Security
{
	struct CodeIntegrityInformation {
		uint32_t m_Size;
		uint32_t m_Options;
	};

	enum SpecialMode {
		SAFE_MODE,
		TEST_SIGN_MODE,
		TEST_BUILD_MODE,
		DEBUGGING_MODE
	};

	struct HardwareIdentifier
	{
		uint16_t m_CpuArchitecture;
		uint32_t m_CpuCount;
		uint64_t m_HardDiskSerialHash;
		char	 m_CustomDetail[256];
		uint8_t  m_SpecialMode[4];
	};

	class RuntimeSecurity
	{
		HardwareIdentifier m_Identifier;

	protected:
		bool ApplyApiHooks();
		void PatchDebugFunctions();
		void DispatchSecurityThreads();
		void SetupSystemIdentifier();
		void CheckForVirtualMachine();
		void CheckForDebugger();
		void CheckForDrivers();
		void CheckForTampering();

	public:
		bool Start();
		HardwareIdentifier GetHardwareId();
		MEMORY_BASIC_INFORMATION QueryMemory(void *Address);
		void SecurityCallback(const char *Reason);
	};
	using RuntimeSecurityPtr = std::unique_ptr<RuntimeSecurity>;
}

extern Security::RuntimeSecurityPtr Protection;