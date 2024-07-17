#include "stdafx.hpp"
#include <Security/SyscallManager.hpp>

Wrapper::SyscallManagerPtr Syscalls = std::make_unique<Wrapper::SyscallManager>();

namespace Wrapper
{
	void SyscallStub::SetIndex(uint32_t Index)
	{
		DWORD OldProtection{};

		if (VirtualProtect(m_Shellcode, sizeof m_Shellcode, PAGE_EXECUTE_READWRITE, &OldProtection))
		{
			*(uint32_t*)(&m_Shellcode[4]) = Index;
		}
	}

	ByteArray SyscallManager::GetNtdllFromDisk()
	{
		char SystemPath[MAX_PATH];
		GetSystemDirectoryA(SystemPath, MAX_PATH);

		strcat_s(SystemPath, E("\\ntdll.dll"));

		std::ifstream FileHandle(SystemPath, std::ios::in | std::ios::binary);
		FileHandle.unsetf(std::ios::skipws);

		if (!FileHandle.is_open())
			return ByteArray{};

		ByteArray Bytes;
		Bytes.insert(
			Bytes.begin(),
			std::istream_iterator<uint8_t>(FileHandle),
			std::istream_iterator<uint8_t>()
		);

		FileHandle.close();

		return Bytes;
	}

	uintptr_t SyscallManager::GetRawOffsetByRva(IMAGE_SECTION_HEADER* SectionHeader, uintptr_t Sections, uintptr_t FileSize, uintptr_t Rva)
	{
		IMAGE_SECTION_HEADER* Header = GetSectionByRva(SectionHeader, Sections, Rva);

		if (!Header)
			return 0;

		uintptr_t Delta = Rva - Header->VirtualAddress;
		uintptr_t Offset = Header->PointerToRawData + Delta;

		if (Offset >= FileSize)
			return 0;

		return Offset;
	}

	IMAGE_SECTION_HEADER* SyscallManager::GetSectionByRva(IMAGE_SECTION_HEADER* SectionHeader, uintptr_t Sections, uintptr_t Rva)
	{
		IMAGE_SECTION_HEADER* Header = SectionHeader;

		for (size_t i{}; i < Sections; ++i, ++Header)
		{
			uintptr_t VirtualAddress = Header->VirtualAddress;
			uintptr_t AddressBounds = VirtualAddress + Header->SizeOfRawData;

			if (Rva >= VirtualAddress && Rva < AddressBounds)
				return Header;
		}

		return nullptr;
	}

#define GetRvaPointer(Rva) (Buffer + GetRawOffsetByRva(SectionHeader, SectionCount, FileSize, Rva))

	bool SyscallManager::Start()
	{
		ByteArray Ntdll = GetNtdllFromDisk();

		if (Ntdll.empty())
			return false;

		uint8_t* Buffer = Ntdll.data();
		size_t    FileSize = Ntdll.size();
		if (*(uint16_t*)Buffer != IMAGE_DOS_SIGNATURE)
			return false;

		IMAGE_DOS_HEADER* DosHeader = (IMAGE_DOS_HEADER*)Buffer;
		IMAGE_NT_HEADERS* NtHeaders = (IMAGE_NT_HEADERS*)(Buffer + DosHeader->e_lfanew);

		uint64_t SectionCount = NtHeaders->FileHeader.NumberOfSections;

		IMAGE_SECTION_HEADER* SectionHeader = (IMAGE_SECTION_HEADER*)(Buffer + DosHeader->e_lfanew + sizeof IMAGE_NT_HEADERS);

		if (!SectionHeader)
			return false;

		uintptr_t ExportRva = NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
		uintptr_t ExportSize = NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
		uintptr_t ExportRaw = GetRawOffsetByRva(SectionHeader, SectionCount, FileSize, ExportRva);

		if (!ExportRva || !ExportSize || !ExportRaw)
			return false;

		IMAGE_EXPORT_DIRECTORY* ExportDirectory = (IMAGE_EXPORT_DIRECTORY*)(Buffer + ExportRaw);

		uint32_t* Functions = (uint32_t*)GetRvaPointer(ExportDirectory->AddressOfFunctions);
		uint16_t* Ordinals = (uint16_t*)GetRvaPointer(ExportDirectory->AddressOfNameOrdinals);
		uint32_t* Names = (uint32_t*)GetRvaPointer(ExportDirectory->AddressOfNames);

		if (!Functions || !Ordinals || !Names)
			return false;


		for (uint32_t n{}; n < ExportDirectory->NumberOfNames; ++n)
		{
			uint32_t NameRva = Names[n];
			uint32_t FunctionRva = Functions[Ordinals[n]];

			uintptr_t NameRawOffset = GetRawOffsetByRva(SectionHeader, SectionCount, FileSize, NameRva);
			uintptr_t FunctionRawOffset = GetRawOffsetByRva(SectionHeader, SectionCount, FileSize, FunctionRva);

			uint8_t* Opcodes = (uint8_t*)(Buffer + FunctionRawOffset);

			if (!memcmp(Opcodes, E("\x4C\x8B\xD1\xB8"), 4))
			{
				uint32_t SyscallIndex = *(uint32_t*)(Buffer + FunctionRawOffset + 4);

				char* SyscallName = (char*)(Buffer + NameRawOffset);
				uint64_t SyscallNameHash = fnv::hash_runtime(SyscallName);

				m_Syscalls[SyscallNameHash].SetIndex(SyscallIndex);
			}
		}

		if (m_Syscalls.empty())
			return false;

		return true;
	}
}