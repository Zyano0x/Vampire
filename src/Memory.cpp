#include "pch.h"

Memory::Allocator Memory::mem_allocator_;

Memory::Allocator::~Allocator()
{
	this->Clear();
}

void Memory::Allocator::Clear()
{
	std::lock_guard _(this->mutex_);

	for (auto& data : this->pool_)
	{
		Memory::Free(data);
	}

	this->pool_.clear();
}

void Memory::Allocator::Free(void* data)
{
	std::lock_guard _(this->mutex_);

	const auto j = std::find(this->pool_.begin(), this->pool_.end(), data);
	if (j != this->pool_.end())
	{
		Memory::Free(data);
		this->pool_.erase(j);
	}
}

void Memory::Allocator::Free(const void* data)
{
	this->Free(const_cast<void*>(data));
}

void* Memory::Allocator::Allocate(const size_t length)
{
	std::lock_guard _(this->mutex_);

	const auto data = Memory::Allocate(length);
	this->pool_.push_back(data);
	return data;
}

bool Memory::Allocator::Empty() const
{
	return this->pool_.empty();
}

void* Memory::Allocate(const size_t length)
{
	return calloc(length, 1);
}

void Memory::Free(void* data)
{
	if (data)
	{
		::free(data);
	}
}

void Memory::Free(const void* data)
{
	free(const_cast<void*>(data));
}

bool Memory::IsSet(const void* mem, const char chr, const size_t length)
{
	const auto mem_arr = static_cast<const char*>(mem);

	for (size_t i = 0; i < length; ++i)
	{
		if (mem_arr[i] != chr)
		{
			return false;
		}
	}

	return true;
}

Memory::Allocator* Memory::GetAllocator()
{
	return &Memory::mem_allocator_;
}

std::uint8_t* FindSignature(LPCSTR module_name, const std::string& byte_array)
{
	HMODULE module = LI_FN(GetModuleHandleA).safe()(module_name);

	if (!module)
		return nullptr;

	static const auto pattern_to_byte = [](const char* pattern)
		{
			auto bytes = std::vector<int>{};
			const auto start = const_cast<char*>(pattern);
			const auto end = const_cast<char*>(pattern) + std::strlen(pattern);

			for (auto current = start; current < end; ++current)
			{
				if (*current == '?')
				{
					++current;

					if (*current == '?')
						++current;

					bytes.push_back(-1);
				}
				else
				{
					bytes.push_back(std::strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

	const auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
	const auto nt_headers =
		reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(module) + dos_header->e_lfanew);

	const auto size_of_image = nt_headers->OptionalHeader.SizeOfImage;
	const auto pattern_bytes = pattern_to_byte(byte_array.c_str());
	const auto scan_bytes = reinterpret_cast<std::uint8_t*>(module);

	const auto pattern_size = pattern_bytes.size();
	const auto pattern_data = pattern_bytes.data();

	for (auto i = 0ul; i < size_of_image - pattern_size; ++i)
	{
		auto found = true;

		for (auto j = 0ul; j < pattern_size; ++j)
		{
			if (scan_bytes[i + j] == pattern_data[j] || pattern_data[j] == -1)
				continue;
			found = false;
			break;
		}
		if (!found)
			continue;
		return &scan_bytes[i];
	}

	return nullptr;
}

Signature::Signature(const std::string& sig)
{
	this->Imported = false;
	this->Sig = sig;
}

Signature Signature::Import(const std::string& module_name)
{
	this->Imported = true;
	this->Module = module_name;
	this->Pointer = (uint64_t)FindSignature(this->Module.data(), this->Sig);
	return *this;
}

Signature Signature::Add(uint32_t value)
{
	if (!this->Imported)
		*this = this->Import();

	this->Pointer += value;
	return *this;
}

Signature Signature::Sub(uint32_t value)
{
	if (!this->Imported)
		*this = this->Import();

	this->Pointer -= value;
	return *this;
}

Signature Signature::Dump(const std::string& func_name)
{
	if (!this->Imported)
		*this = this->Import();

	return *this;
}

Signature Signature::Instruction(uint32_t offset)
{
	if (!this->Imported)
		*this = this->Import();

	this->Pointer = *(int*)(this->Pointer + offset) + this->Pointer;
	return *this;
}

#ifdef _WIN64
uint64_t Signature::GetPointer()
{
	if (!this->Imported)
		*this = this->Import();
	return this->Pointer;
}
#else
uint32_t Signature::GetPointer()
{
	if (!this->imported)
		*this = this->import();
	return this->pointer;
}
#endif

static inline void memcpy_(void* _Dst, void const* _Src, size_t _Size)
{
	auto csrc = (char*)_Src;
	auto cdest = (char*)_Dst;

	for (int i = 0; i < _Size; i++)
	{
		cdest[i] = csrc[i];
	}
}

void SwapVirtualTable(void* obj, uint32_t index, void* func)
{
	auto currVt = *(void**)(obj);

	auto vtable = *(void***)(obj);
	int i = 0;

	for (; vtable[i]; i++)
		__noop();

	auto newVt = new uintptr_t[i];

	memcpy_(newVt, currVt, i * 0x8);

	newVt[index] = (uintptr_t)func;

	*(uintptr_t**)(obj) = newVt;
}

bool IsKeyDown(int VK_Key)
{
	return ((GetAsyncKeyState(VK_Key) & 0x8000) ? 1 : 0);
}