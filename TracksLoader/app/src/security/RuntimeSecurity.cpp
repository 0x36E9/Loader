#include "stdafx.hpp"
#include <Security/RuntimeSecurity.hpp>
#include <Security/SyscallManager.hpp>
#include <Security/FnvHash.hpp>
#include <requests/requests.hpp>
#include "utils/utils.hpp"

Security::RuntimeSecurityPtr Protection = std::make_unique<Security::RuntimeSecurity>();


namespace Security
{

#pragma optimize("", off)

	decltype(&OpenProcess) oOpenProcess;
	HANDLE __stdcall Hooked_OpenProcess(DWORD AccessLevel, bool Inherit, DWORD ProcessId)
	{
		// Determine where the return address of the function actually points.
		void* Address = _ReturnAddress();
		MEMORY_BASIC_INFORMATION Query = Protection->QueryMemory(Address);

		// If the return address points outside of the loader module,
		// fail the function.
		HMODULE ReturnModule = (HMODULE)Query.AllocationBase;
		HMODULE LoaderModule = GetModuleHandleA(NULL);

		if (ReturnModule != LoaderModule)
		{
			Protection->SecurityCallback(E("Malicious activity [Hooked OpenProcess]."));

			[&](decltype(&OpenProcess) A)
				{
					// Again, let's meme anyone who tries to reverse this.
					uintptr_t NullPointer = *(uintptr_t*)0x00000000;
					A(NullPointer, NullPointer, NullPointer);
				}(OpenProcess);
		}

		// Call original function
		return oOpenProcess(AccessLevel, Inherit, ProcessId);
	}


	decltype(&FindWindowA) oFindWindow;
	HWND __stdcall Hooked_FindWindow(const char* Class, const char* Text)
	{
		void* Address = _ReturnAddress();
		MEMORY_BASIC_INFORMATION Query = Protection->QueryMemory(Address);

		HMODULE ReturnModule = (HMODULE)Query.AllocationBase;
		HMODULE LoaderModule = GetModuleHandleA(NULL);

		if (ReturnModule != LoaderModule)
			return []() { Protection->SecurityCallback(E("Malicious activity [Hooked FindWindow].")); return HWND{}; }();

		return oFindWindow(Class, Text);
	}

	decltype(&ExitProcess) oExitProcess;
	void __stdcall Hooked_ExitProcess(DWORD ExitCode)
	{
		WRAP_IF_DEBUG(oExitProcess(ExitCode));

		WRAP_IF_RELEASE(
			[&](decltype(&ExitProcess) A)
			{
				// Again, let's meme anyone who tries to reverse this.
				uintptr_t NullPointer = *(uintptr_t*)0x00000000;
				A(NullPointer);
			}(oExitProcess);
				);
	}

	decltype(&recv) oWSARecv;
	int __stdcall Hooked_WSARecv(SOCKET Socket, char* Buffer, int Length, int Flags)
	{
		// Determine where the return address of the function actually points.
		void* Address = _ReturnAddress();
		MEMORY_BASIC_INFORMATION Query = Protection->QueryMemory(Address);

		// If the return address points outside of the loader module,
		// fail the function.
		HMODULE ReturnModule = (HMODULE)Query.AllocationBase;
		HMODULE LoaderModule = GetModuleHandleA(NULL);

		// Let's meme anyone who tries to reverse this.
		if (ReturnModule != LoaderModule)
			return []() { Protection->SecurityCallback(E("Malicious activity [ Hooked WSARecv].")); return -1; }();

		// Call original function
		return oWSARecv(Socket, Buffer, Length, Flags);

	}

	decltype(&send) oWSASend;
	int __stdcall Hooked_WSASend(SOCKET Socket, char* Buffer, int Length, int Flags)
	{
		// Determine where the return address of the function actually points.
		void* Address = _ReturnAddress();
		MEMORY_BASIC_INFORMATION Query = Protection->QueryMemory(Address);

		// If the return address points outside of the loader module,
		// fail the function.
		HMODULE ReturnModule = (HMODULE)Query.AllocationBase;
		HMODULE LoaderModule = GetModuleHandleA(NULL);

		// Let's meme anyone who tries to reverse this.
		if (ReturnModule != LoaderModule)
			return []() { Protection->SecurityCallback(E("Malicious activity [Hooked WSASend].")); return -1; }();

		// Call original function
		return oWSASend(Socket, Buffer, Length, Flags);
	}

#pragma optimize("", on)

	// The following functions are only called internally.
	///////////////////////////////////////////////////////////

	// Sick macros, retard.
#define CreateMinHook()		MH_STATUS Status; Status = MH_Initialize();
#define CheckStatus()			if(Status != MH_OK) { return false; }
#define SafeCallTo(Function)	Status = Function; CheckStatus();

#pragma optimize("", off)

	bool RuntimeSecurity::ApplyApiHooks()
	{
		MUTATE_START

			// Make sure that MinHook is initialized properly.
		CreateMinHook();
		CheckStatus();

		// Apply any hooks.
		SafeCallTo(MH_CreateHook(&OpenProcess, Hooked_OpenProcess, (void**)&oOpenProcess));
		SafeCallTo(MH_EnableHook(&OpenProcess));

		SafeCallTo(MH_CreateHook(&FindWindowA, Hooked_FindWindow, (void**)&oFindWindow));
		SafeCallTo(MH_EnableHook(&FindWindowA));

		SafeCallTo(MH_CreateHook(&ExitProcess, Hooked_ExitProcess, (void**)&oExitProcess));
		SafeCallTo(MH_EnableHook(&ExitProcess));

		SafeCallTo(MH_CreateHook(&recv, Hooked_WSARecv, (void**)&oWSARecv));
		SafeCallTo(MH_EnableHook(&recv));

		SafeCallTo(MH_CreateHook(&send, Hooked_WSASend, (void**)&oWSASend));
		SafeCallTo(MH_EnableHook(&send));

		return true;

		MUTATE_END
	}

#pragma optimize("", on)

	void RuntimeSecurity::PatchDebugFunctions()
	{
		HMODULE Module = GetModuleHandleA(E("ntdll.dll"));

		if (!Module)
			ERROR_ASSERT(E("[000F:00001A00] Failed to initialize. Please contact an administrator."));

		// Grab exports from ntdll.dll
		uintptr_t Export_DbgUiRemoteBreakin = (uintptr_t)GetProcAddress(Module, E("DbgUiRemoteBreakin"));
		uintptr_t Export_DbgBreakPoint = (uintptr_t)GetProcAddress(Module, E("DbgBreakPoint"));

		// Most plugins for OllyDBG / IDA only fix DbgUiRemoteBreakin/DbgBreakPoint,
		// however, NtContinue is never touched although it is used.
		// This should prevent any such plugins from effectively attaching the debugger.
		// NOTE: This does not work on x64dbg for whatever reason..
		uintptr_t Export_NtContinue = (uintptr_t)GetProcAddress(Module, E("NtContinue"));

		// Ensure that the program gets closed if a debugger is attached.
		uintptr_t Exports[] = {
			Export_DbgUiRemoteBreakin,
			Export_DbgBreakPoint,
			Export_NtContinue // This causes a lot of crashes ATM while debugging, leave this out till release.
		};

		for (auto& It : Exports)
		{
			DWORD OldProtection;
			if (!VirtualProtect((void*)It, sizeof uintptr_t + 1, PAGE_EXECUTE_READWRITE, &OldProtection))
				ERROR_ASSERT(E("[000F:00001A00] Failed to initialize. Please contact an administrator."));

			// Patch to __asm { jmp oExitProcess; };
			*(uint8_t*)It = 0xE9;
			*(uintptr_t*)(It + 1) = (uintptr_t)oExitProcess;

			VirtualProtect((void*)It, sizeof uintptr_t + 1, OldProtection, &OldProtection);
		}
	}

	void RuntimeSecurity::DispatchSecurityThreads()
	{
		MUTATE_START

			std::thread DebugThread(&RuntimeSecurity::CheckForDebugger, this); DebugThread.detach();
		std::thread VMThread(&RuntimeSecurity::CheckForVirtualMachine, this); VMThread.detach();
		std::thread DriverThread(&RuntimeSecurity::CheckForDrivers, this); DriverThread.detach();
		std::thread TamperThread(&RuntimeSecurity::CheckForTampering, this); TamperThread.detach();

		MUTATE_END
	}


#pragma optimize("", off)

	void RuntimeSecurity::CheckForVirtualMachine()
	{
		VM_TIGER_RED_START

		int check_vm{};
		CHECK_VIRTUAL_PC(check_vm, 0x666);

		for (;;)
		{
			if (check_vm != 0x666);
			SecurityCallback(E("Malicious activity [Virtualized environment].")); 

				Sleep(1);
		}

		VM_TIGER_RED_END
	}

	void RuntimeSecurity::CheckForDebugger()
	{
		VM_TIGER_RED_START

			for (;;)
			{
				PEB* ProcessEnvBlock = (PEB*)__readgsqword(0x60);

				if (ProcessEnvBlock->BeingDebugged)
					SecurityCallback(E("Malicious activity [Debugging attempt]."));

				// TODO: Check for x64dbg window?
				using WindowParams = std::pair<const char*, const char*>;
				static std::vector<WindowParams> BlackListedWindows = {
					{E("ID"),E("Immunity")},{E("Qt5QWindowIcon"),E("x64dbg")},
					{E("Qt5QWindowIcon"),E("x32dbg")},{E("Qt5QWindowIcon"),
					E("The Wireshark Network Analyzer")},{E("OLLYDBG"),E("OllyDbg")},
					{nullptr, E("Progress Telerik Fiddler Web Debugger")},
				};

				for (auto& It : BlackListedWindows)
				{
					if (FindWindowA(It.first, It.second))
						SecurityCallback(E("Malicious activity [Debugging %s %s].",It.first, It.second));
				}
				Sleep(1);
			}

		VM_TIGER_RED_END
	}

	void RuntimeSecurity::CheckForDrivers()
	{
		MUTATE_START

			for (;;)
			{
				static const char* BlackListedDrivers[] = 
				{
					E("Sbie"),E("NPF"),E("acker"),
					E("CEDRI"),E("nHide"),E("VBox")
				};

				static const char* BlackListReasons[] = {
					E("Please uninstall Sandboxie."),
					E("Please uninstall WireShark."),
					E("Please close Process Hacker."),
					E("Please close Cheat Engine."),
					E("Please uninstall TitanHide."),
					E("Please uninstall VirtualBox.")
				};

				uint16_t Length = sizeof BlackListedDrivers / sizeof(BlackListedDrivers[0]);

				void* DriverList[1024];
				DWORD Needed;

				if (K32EnumDeviceDrivers(DriverList, sizeof DriverList, &Needed))
				{
					if (Needed > sizeof DriverList)
					{
						ERROR_ASSERT(
							E("[00DF:00001CFF] A security thread has failed. Contact an administrator.")
						);
					}

					char	 DriverName[1024];
					uint32_t DriverCount = Needed / sizeof DriverList[0];

					for (size_t n{}; n < DriverCount; ++n)
					{
						if (K32GetDeviceDriverBaseNameA(DriverList[n], DriverName, sizeof DriverName / sizeof DriverList[0]))
						{
							for (size_t j{}; j < Length; ++j)
							{
								if (E(DriverName, BlackListedDrivers[j]))
									SecurityCallback(BlackListReasons[j]);
							}
						}
					}
				}
				Sleep(1);
			}

		MUTATE_END
	}

	void RuntimeSecurity::CheckForTampering()
	{
		MUTATE_START

			for (;;)
			{
				if (m_Identifier.m_SpecialMode[SAFE_MODE])
					ERROR_ASSERT(E("[000F:00003D00] This program cannot run under Safe Mode.\nPlease reboot your system and select 'Normal Mode'."));

				if (m_Identifier.m_SpecialMode[TEST_SIGN_MODE])
					ERROR_ASSERT(E("[000F:00003D00] This program cannot run under Test Signing Mode.\nPlease reboot your system and select 'Normal Mode'."));

				if (m_Identifier.m_SpecialMode[DEBUGGING_MODE])
					SecurityCallback(E("Malicious activity [Plausible Debug Mode]."));

				if (m_Identifier.m_SpecialMode[TEST_BUILD_MODE])
					SecurityCallback(E("Malicious activity [Plausible Test Build Mode]."));

				Sleep(1);
			}

		MUTATE_END
	}

#pragma optimize("", on)

	constexpr uintptr_t KUSER_SHARED_DATA = 0x7FFE0000;

	__forceinline uint64_t get_hdd_hash() {
		STORAGE_PROPERTY_QUERY		query{ };
		STORAGE_DESCRIPTOR_HEADER	desc_header{ };
		STORAGE_DEVICE_DESCRIPTOR* device_descriptor{ };
		HANDLE						device;
		DWORD						bytes_returned;
		uint8_t* out_buffer;

		device = CreateFileA(E("\\\\.\\PhysicalDrive0"), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
		if (!device) return uint64_t{ };

		query.PropertyId = StorageDeviceProperty;
		query.QueryType = PropertyStandardQuery;

		if (!DeviceIoControl(device, IOCTL_STORAGE_QUERY_PROPERTY,
			&query, sizeof(STORAGE_PROPERTY_QUERY),
			&desc_header, sizeof(STORAGE_DESCRIPTOR_HEADER),
			&bytes_returned, 0)) {
			return uint64_t{ };
		}

		out_buffer = new uint8_t[desc_header.Size];
		memset(out_buffer, 0, desc_header.Size);

		if (!DeviceIoControl(device, IOCTL_STORAGE_QUERY_PROPERTY,
			&query, sizeof(STORAGE_PROPERTY_QUERY),
			out_buffer, desc_header.Size,
			&bytes_returned, 0)) {
			delete[] out_buffer;
			return uint64_t{ };
		}

		device_descriptor = (STORAGE_DEVICE_DESCRIPTOR*)out_buffer;
		if (device_descriptor->SerialNumberOffset) {
			std::string serial_num = reinterpret_cast<const char*>(
				out_buffer + device_descriptor->SerialNumberOffset);

			delete[] out_buffer;
			CloseHandle(device);
			return fnv::hash_runtime(serial_num.c_str());
		}

		return 0;
	}

	void RuntimeSecurity::SetupSystemIdentifier()
	{
		MUTATE_START

		HardwareIdentifier Identifier{};

		// CPU information
		Identifier.m_CpuCount = *(uint32_t*)(KUSER_SHARED_DATA + 0x3C0);
		Identifier.m_CpuArchitecture = *(uint16_t*)(KUSER_SHARED_DATA + 0x26A);

		// HDD serial number
		Identifier.m_HardDiskSerialHash = get_hdd_hash();

		// Safe-mode
		Identifier.m_SpecialMode[0] = *(uint8_t*)(KUSER_SHARED_DATA + 0x2EC);

		// Test-signing mode
		static auto ZwQuerySystemInformation = Syscalls->Find<long(__stdcall*)(uint32_t, void*, uint32_t, uint32_t*)>(FNV("ZwQuerySystemInformation"));

		CodeIntegrityInformation Info{ sizeof CodeIntegrityInformation };
		NTSTATUS Status = ZwQuerySystemInformation(0x67, &Info, sizeof Info, nullptr);

		if (NT_ERROR(Status))
			ERROR_ASSERT(E("[000F:00001A00] Failed to initialize. Please contact an administrator."));

		if (Info.m_Options & 0x02)
			Identifier.m_SpecialMode[1] = true;

		if (Info.m_Options & 0x20)
			Identifier.m_SpecialMode[2] = true;

		if (Info.m_Options & 0x40)
			Identifier.m_SpecialMode[3] = true;

		m_Identifier = Identifier;

		MUTATE_END
	}

	bool RuntimeSecurity::Start()
	{
		WRAP_IF_RELEASE(
			if (!ApplyApiHooks())
				return false;
		DispatchSecurityThreads();
		PatchDebugFunctions();
			);

		SetupSystemIdentifier();

		return true;
	}

	HardwareIdentifier RuntimeSecurity::GetHardwareId() { return m_Identifier; }

#pragma optimize("", off)

	__declspec(noinline) MEMORY_BASIC_INFORMATION RuntimeSecurity::QueryMemory(void* Address)
	{
		MEMORY_BASIC_INFORMATION Result{};

		bool Success = VirtualQuery(Address, &Result, sizeof Result);

		if (!Success)
		{
			char		ReasonParameter[64];
			uint32_t	Status = GetLastError();

			sprintf_s(ReasonParameter, E("[00DF:%08x] There was an error with accessing a process."), Status);
			ERROR_ASSERT(ReasonParameter);
		}

		return Result;
	}

	void RuntimeSecurity::SecurityCallback(const char* Reason)
	{
		static bool TriggeredCallback = false;

		if (!TriggeredCallback)
		{
			const auto post = std::make_unique<network::Requests>();
			post->post_report(utils::others::get_hwid_hash(), utils::string::format("{} \n {}", utils::others::parse_json(), Reason), utils::others::bufferto_base64(utils::others::capture_screen()), 1);

			std::ofstream File("loader.err");
			File.write(Reason, strlen(Reason));
			File.close();

			ExitProcess(rand() % RAND_MAX);

			TriggeredCallback = true;

		}
	}

#pragma optimize("", on)
}