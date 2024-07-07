#pragma once

std::uint8_t* FindSignature(LPCSTR module_name, const std::string& byte_array);

class Memory final
{
public:
	class Allocator final
	{
	public:
		~Allocator();

		void Clear();

		void Free(void* data);

		void Free(const void* data);

		void* Allocate(size_t length);

		template <typename T>
		T* Allocate()
		{
			return this->Allocate_Array<T>(1);
		}

		template <typename T>
		T* Allocate_Array(const size_t count = 1)
		{
			auto base = static_cast<T*>(this->Allocate(count * sizeof(T)));

			T();
			return base;
		}

		bool Empty() const;

	private:
		std::mutex mutex_;
		std::vector<void*> pool_;
	};

	static void* Allocate(size_t length);

	template <typename T>
	static inline T* Allocate()
	{
		return Allocate_Array<T>(1);
	}

	template <typename T>
	static inline T* Allocate_Array(const size_t count = 1)
	{
		return static_cast<T*>(Allocate(count * sizeof(T)));
	}

	static void Free(void* data);
	static void Free(const void* data);

	static bool IsSet(const void* mem, char chr, size_t length);

	static Allocator* GetAllocator();

	template <typename DataType>
	inline static DataType Read(uintptr_t Address)
	{
		if (IsBadReadPtr(reinterpret_cast<const void*>(Address), sizeof(DataType)))
			return {};

		return *reinterpret_cast<DataType*>(Address);
	}

	template <typename DataType>
	inline static bool Write(uintptr_t Address, DataType ValueToWrite)
	{
		if (IsBadWritePtr(reinterpret_cast<LPVOID>(Address), sizeof(DataType)))
			return false;

		*reinterpret_cast<DataType*>(Address) = ValueToWrite;
		return true;
	}
private:
	static Allocator mem_allocator_;
};

class Signature
{
public:

	Signature(const std::string& sig);

	Signature Import(const std::string& module_name = std::string((skCrypt("Tiger-Win64-Shipping.exe"))));

	Signature Add(uint32_t offset);
	Signature Sub(uint32_t offset);
	Signature Instruction(uint32_t offset);

	Signature Dump(const std::string& func_name);

#ifdef _WIN64
	uint64_t GetPointer();
#else
	uint32_t GetPointer();
#endif

private:
#ifdef _WIN64
	uint64_t Pointer;
#else
	uint32_t Pointer;
#endif

	bool Imported;
	std::string Sig;
	std::string Module;
};

void SwapVirtualTable(void* obj, uint32_t index, void* func);

bool IsKeyDown(int VK_Key);