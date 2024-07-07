#pragma once

#include <Windows.h>
#include <iostream>
#include <string>
#include <unordered_set>

#define CREATE_DEFAULT_OBJECT_INDEX 0x00
#define POST_RENDER_INDEX 0x3A || 0x86 || 0x87
#define GET_BONE_MATRIX_OFFSET 0x2D7C560
#define GET_VIEW_POINT_INDEX 0x00
#define FNAME_POOL

namespace Offsets
{
	constexpr int32_t GObjects = 0x5BF1AC8;
	constexpr int32_t GNames = 0x5BD92C0;
	constexpr int32_t GWorld = 0x5D209C0;
	constexpr int32_t AppendString = 0x01787520;
	constexpr int32_t ProcessEvent = 0x0197E150;
	constexpr int32_t ProcessEventIdx = 0x00000043;
}

class UObject;

template<typename Fn>
Fn GetVFunction(const void* instance, size_t index)
{
	auto vtable = *static_cast<const void***>(const_cast<void*>(instance));
	return reinterpret_cast<Fn>(const_cast<void(*)>(vtable[index]));
}

//=============================================================================================================================================

template<typename T>
class TArray
{
private:
	T* _data;                                                   // 0x0000(0x0000)
	int32_t _count;                                             // 0x0000(0x0000)
	int32_t _max;                                               // 0x0000(0x0000)

	friend class FString;

public:
	TArray()
	{
		_data = nullptr;
		_count = 0;
		_max = 0;
	}

	T* Data() const
	{
		return _data;
	}

	int32_t Count() const
	{
		return _count;
	}

	int32_t Max() const
	{
		return _max;
	}

	bool IsValidIndex(int32_t i) const
	{
		return i < _count;
	}

	T& operator[](int32_t i)
	{
		return _data[i];
	}

	const T& operator[](int32_t i) const
	{
		return _data[i];
	}
};

class FString : public TArray<wchar_t>
{
public:
	FString();
	FString(const wchar_t* other);
	const wchar_t* wc_str() const;
	const char* c_str() const;
	bool IsValid() const;
	std::string ToString() const;
	std::wstring ToStringW() const;
};

//=============================================================================================================================================

enum class ObjectFlags : int32_t
{
	None = 0,
	Native = 1 << 25,
	Async = 1 << 26,
	AsyncLoading = 1 << 27,
	Unreachable = 1 << 28,
	PendingKill = 1 << 29,
	RootSet = 1 << 30,
	NoStrongReference = 1 << 31
};

class FUObjectItem
{
public:
	class UObject* Object;                                           // 0x0000(0x0008)
	int32_t Flags;                                                   // 0x0008(0x0004)
	int32_t ClusterIndex;                                            // 0x000C(0x0004)
	int32_t SerialNumber;                                            // 0x0010(0x0004)
	unsigned char pad_B3EWF6XCOE[0x04];                              // 0x0014(0x0004)

public:
	bool IsUnreachable() const;
	bool IsPendingKill() const;
};

class TUObjectArray
{
private:
	static const constexpr int32_t NumElementsPerChunk = 64 * 1024;     // 0x0000(0x0000)
	FUObjectItem** Objects;                                             // 0x0000(0x0000)
	FUObjectItem* PreAllocatedObjects;                                  // 0x0000(0x0000)
public:
	int32_t MaxElements;												// 0x0000(0x0000)
	int32_t NumElements;												// 0x0000(0x0000)
	int32_t MaxChunks;													// 0x0000(0x0000)
	int32_t NumChunks;													// 0x0000(0x0000)

public:
	int32_t Count() const;
	int32_t Max() const;
	bool IsValidIndex(int32_t Index) const;
	FUObjectItem* GetObjectPtr(int32_t Index) const;
	UObject* GetByIndex(int32_t index) const;
	FUObjectItem* GetItemByIndex(int32_t index) const;
	UObject* operator[](int32_t i);
	const UObject* operator[](int32_t i) const;
};

class FNameEntryHeader
{
public:
	static const constexpr uint32_t ProbeHashBits = 5;                // 0x0000(0x0000)
	uint16_t bIsWide : 1;                                             // 0x0000(0x0000)
	uint16_t LowercaseProbeHash : ProbeHashBits;                      // 0x0000(0x0000)
	uint16_t Len : 10;                                                // 0x0000(0x0000)
};

class FNameEntry
{
public:
	FNameEntryHeader Header;                                          // 0x0000(0x0000)
	union
	{
		char AnsiName[1024];                                          // 0x0000(0x0000)
		wchar_t WideName[1024];                                       // 0x0000(0x0000)
	};

public:
	int32_t GetLength() const;
	bool IsWide() const;
	int32_t GetId() const;
	std::string GetAnsiName() const;
	std::wstring GetWideName() const;
	std::string GetName() const;
};

class FNameEntryAllocator
{
private:
	uint8_t FrwLock[0x8];                                            // 0x0000(0x0000)

public:
	static const constexpr int32_t Stride = 0x02;                    // 0x0000(0x0000)
	static const constexpr int32_t MaxOffset = Stride * (1 << 16);   // 0x0000(0x0000)
	int32_t CurrentBlock;                                            // 0x0000(0x0000)
	int32_t CurrentByteCursor;                                       // 0x0000(0x0000)
	uint8_t* Blocks[8192];                                           // 0x0000(0x0000)

public:
	int32_t NumBlocks() const;
	FNameEntry* GetById(int32_t key) const;
	bool IsValidIndex(int32_t key) const;
	bool IsValidIndex(int32_t key, uint32_t block, uint16_t offset) const;
};

class FNamePool
{
public:
	FNameEntryAllocator Allocator;                                   // 0x0000(0x0000)
	int32_t AnsiCount;                                               // 0x0000(0x0000)
	int32_t WideCount;                                               // 0x0000(0x0000)

public:
	FNameEntry* GetNext(uintptr_t& nextFNameAddress, uint32_t* comparisonId) const;
	int32_t Count() const;
	bool IsValidIndex(int32_t index) const;
	FNameEntry* GetById(int32_t id) const;
	FNameEntry* operator[](int32_t id) const;
};

class FName
{
public:
	static inline FNamePool* GNames = nullptr;                       // 0x0000(0x0000)
	int32_t ComparisonIndex;                                         // 0x0000(0x0000)
	int32_t Number;                                                  // 0x0000(0x0000)

public:
	FName();
	FName(int32_t i);
	FName(const char* nameToFind);
	FName(const wchar_t* nameToFind);
	static FNamePool& GetGlobalNames();
	std::string GetNameA() const;
	std::wstring GetNameW() const;
	std::string GetName() const;
};

class FTextData
{
private:
	uint8_t UnknownData[0x28];                                       // 0x0000(0x0000)
public:
	wchar_t* Name;                                                   // 0x0000(0x0000)
	int32_t* Length;                                                 // 0x0000(0x0000)
};

class FText
{
private:
	FTextData* Data;                                                 // 0x0000(0x0000)
	uint8_t UnknownData[0x10];                                       // 0x0000(0x0000)

public:
	wchar_t* Get() const;
	std::string ToString() const;
	std::wstring ToWString() const;
};

struct FKey
{
public:
	class FName                                                KeyName;                                                 // 0x0000(0x0008) ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate
	unsigned char                                              UnknownData_U1GX[0x10];                                  // 0x0008(0x0010) MISSED OFFSET (PADDING)

public:
	FKey()
	{
	}

	FKey(const FName InName)
		: KeyName(InName)
	{
	}

	FKey(const TCHAR* InName)
		: KeyName(FName(InName))
	{
	}
};

//=============================================================================================================================================

class UObject
{
public:
	static inline class TUObjectArray* GObjects = nullptr;         // 0x0000(0x0008) PREDEFINED PROPERTY
	void* VfTable;                                                 // 0x0000(0x0008) PRE-DEFINED PROPERTY
	int32_t Flags;                                                 // 0x0008(0x0004) PRE-DEFINED PROPERTY
	int32_t InternalIndex;                                         // 0x000C(0x0004) PRE-DEFINED PROPERTY
	class UClass* Class;                                           // 0x0010(0x0008) PRE-DEFINED PROPERTY
	FName Name;                                                    // 0x0018(0x0008) PRE-DEFINED PROPERTY
	class UObject* Outer;                                          // 0x0020(0x0008) PRE-DEFINED PROPERTY

public:
	static TUObjectArray& GetGlobalObjects();
	std::string GetName() const;
	std::string GetFullName() const;

	template<typename T>
	static T* FindObject(const std::string& name)
	{
		for (int32_t i = 0; i < UObject::GetGlobalObjects().Count(); ++i)
		{
			auto object = UObject::GetGlobalObjects().GetByIndex(i);

			if (!object)
				continue;

			if (object->GetFullName() == name)
				return static_cast<T*>(object);
		}
		return nullptr;
	}

	template<typename T>
	static T* FindObject()
	{
		auto v = T::StaticClass();
		for (int32_t i = 0; i < UObject::GetGlobalObjects().Count(); ++i)
		{
			auto object = UObject::GetGlobalObjects().GetByIndex(i);

			if (!object)
				continue;

			if (object->IsA(v))
				return static_cast<T*>(object);
		}
		return nullptr;
	}

	template<typename T>
	static std::vector<T*> FindObjects(const std::string& name)
	{
		std::vector<T*> ret;
		for (int32_t i = 0; i < UObject::GetGlobalObjects().Count(); ++i)
		{
			auto object = UObject::GetGlobalObjects().GetByIndex(i);

			if (!object)
				continue;

			if (object->GetFullName() == name)
				ret.push_back(static_cast<T*>(object));
		}
		return ret;
	}

	template<typename T>
	static std::vector<T*> FindObjects()
	{
		std::vector<T*> ret;
		auto v = T::StaticClass();
		for (int i = 0; i < UObject::GetGlobalObjects().Count(); ++i)
		{
			auto object = UObject::GetGlobalObjects().GetByIndex(i);

			if (!object)
				continue;

			if (object->IsA(v))
				ret.push_back(static_cast<T*>(object));
		}
		return ret;
	}

	static UClass* FindClass(const std::string& name);

	template<typename T>
	static T* GetObjectCasted(size_t index)
	{
		return static_cast<T*>(UObject::GetGlobalObjects().GetByIndex(index));
	}

	bool IsA(UClass* cmp) const;
	void ExecuteUbergraph(int32_t EntryPoint);
	void ProcessEvent(class UFunction* function, void* parms);
	static UClass* StaticClass();
};

class UField : public UObject
{
public:
	class UField* Next;                                              // 0x0028(0x0008) PRE-DEFINED PROPERTY

public:
	static UClass* StaticClass();
};

class FStructBaseChain
{
private:
	FStructBaseChain** StructBaseChainArray;                         // 0x0000(0x0000)
	int32_t NumStructBasesInChainMinusOne;                           // 0x0000(0x0000)
	uint8_t Padding_0[0x04];                                         // 0x0000(0x0000)
};

class UStruct : public UField, public FStructBaseChain
{
public:
	class UStruct* SuperField;                  // 0x0040(0x0008) PRE-DEFINED PROPERTY
	class UField* Children;                     // 0x0048(0x0008) PRE-DEFINED PROPERTY
	class FField* ChildProperties;              // 0x0050(0x0008) PRE-DEFINED PROPERTY
	int32_t PropertySize;                       // 0x0058(0x0004) PRE-DEFINED PROPERTY
	int32_t MinAlignment;                       // 0x005C(0x0004) PRE-DEFINED PROPERTY
	TArray<uint8_t>nScript;                     // 0x0060(0x0010) PRE-DEFINED PROPERTY
	class FProperty* PropertyLink;              // 0x0070(0x0008) PRE-DEFINED PROPERTY
	class FProperty* RefLink;                   // 0x0078(0x0008) PRE-DEFINED PROPERTY
	class FProperty* DestructorLink;            // 0x0080(0x0008) PRE-DEFINED PROPERTY
	class FProperty* PostConstructLink;         // 0x0088(0x0008) PRE-DEFINED PROPERTY
	TArray<UObject*> ScriptObjectReferences;    // 0x0090(0x0010) PRE-DEFINED PROPERTY

public:
	static UClass* StaticClass();
};

class UClass : public UStruct
{
public:
	unsigned char                                              UnknownData_Z0T6[0x180];                                 // 0x00B0(0x0180) MISSED OFFSET (PADDING)

public:
	template<typename T>
	T* CreateDefaultObjectOfType()
	{
		return static_cast<T*>(CreateDefaultObject());
	}

	UObject* CreateDefaultObject();
	static UClass* StaticClass();
};

class UFunction : public UStruct
{
public:
	int32_t FunctionFlags;                                          // 0x00A0(0x0004) PRE-DEFINED PROPERTY
	int16_t	RepOffset;                                              // 0x00A4(0x0002) PRE-DEFINED PROPERTY
	int8_t NumParms;                                                // 0x00A6(0x0001) PRE-DEFINED PROPERTY
	unsigned char pad_BZWH9N1UI9[0x01];                             // 0x00A7(0x0001) PRE-DEFINED PROPERTY
	uint16_t ParmsSize;                                             // 0x00A8(0x0002) PRE-DEFINED PROPERTY
	uint16_t ReturnValueOffset;                                     // 0x00AA(0x0002) PRE-DEFINED PROPERTY
	uint16_t RPCId;                                                 // 0x00AC(0x0002) PRE-DEFINED PROPERTY
	uint16_t RPCResponseId;                                         // 0x00AE(0x0002) PRE-DEFINED PROPERTY
	class FProperty* FirstPropertyToInit;                           // 0x00B0(0x0008) PRE-DEFINED PROPERTY
	class UFunction* EventGraphFunction;                            // 0x00B8(0x0008) PRE-DEFINED PROPERTY
	int32_t EventGraphCallOffset;                                   // 0x00C0(0x0004) PRE-DEFINED PROPERTY
	unsigned char pad_FI1JSC4I2H[0x04];                             // 0x00C4(0x0004) PRE-DEFINED PROPERTY
	void* Func;                                                     // 0x00C8(0x0008) PRE-DEFINED PROPERTY

public:
	static UClass* StaticClass();
};

//==============================================================================================================================================

struct FRotator
{
public:
	float                                                        Pitch;                                                   //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                        Yaw;                                                     //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                        Roll;                                                    //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic

public:
	FRotator();
	FRotator(float pitch, float yaw, float roll);
	FRotator operator +(float scalar) const;
	FRotator operator +(const FRotator& other) const;
	FRotator operator -(float scalar) const;
	FRotator operator -(const FRotator& other) const;
	FRotator operator *(float scalar) const;
	FRotator operator *(const FRotator& other) const;
	FRotator operator /(float scalar) const;
	FRotator operator /(const FRotator& other) const;
	FRotator& operator =(const FRotator& other);
	FRotator& operator +=(float scalar);
	FRotator& operator +=(const FRotator& other);
	FRotator& operator -=(float scalar);
	FRotator& operator -=(const FRotator& other);
	FRotator& operator *=(const FRotator& other);
	FRotator& operator *=(const float other);
	FRotator& operator /=(const FRotator& other);
	FRotator& operator /=(const float other);
	float Size() const;
	FRotator Clamp() const;
};

struct FVector
{
public:
	float                                                        X;                                                       //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                        Y;                                                       //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                        Z;                                                       //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic

public:
	FVector();
	FVector(float x, float y, float z);
	FVector operator +(float scalar) const;
	FVector operator +(const FVector& other) const;
	FVector operator -(float scalar) const;
	FVector operator -(const FVector& other) const;
	FVector operator *(float scalar) const;
	FVector operator *(const FVector& other) const;
	FVector operator /(float scalar) const;
	FVector operator /(const FVector& other) const;
	FVector& operator =(const FVector& other);
	FVector& operator +=(float scalar);
	FVector& operator +=(const FVector& other);
	FVector& operator -=(float scalar);
	FVector& operator -=(const FVector& other);
	FVector& operator *=(const FVector& other);
	FVector& operator *=(float scalar);
	bool IsValid();
	void Normalize360(FVector& angle);
	float Dot(const FVector& b) const;
	float MagnitudeSqr() const;
	float Magnitude() const;
	FVector Unit() const;
	FRotator ToRotator() const;
	float Distance(const FVector& v) const;
	float DistanceMeter(FVector& v) const;
	friend bool operator ==(const FVector& first, const FVector& second)
	{
		return first.X == second.X && first.Y == second.Y && first.Z == second.Z;
	}
	friend bool operator !=(const FVector& first, const FVector& second)
	{
		return !(first == second);
	}
};

struct FVector2D
{
public:
	float                                                        X;                                                       //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                        Y;                                                       //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic

public:
	FVector2D();
	FVector2D(float x, float y);
	bool IsValid();
	float Distance(FVector2D& v) const;
	FVector2D operator +(const FVector2D& other) const;
	FVector2D operator +(float scalar) const;
	FVector2D operator -(float scalar) const;
	FVector2D operator -(const FVector2D& other) const;
	FVector2D operator *(float scalar) const;
	FVector2D operator *(const FVector2D& other) const;
	FVector2D operator /(float scalar) const;
	FVector2D operator /(const FVector2D& other) const;
	FVector2D& operator =(const FVector2D& other);
	FVector2D& operator +=(float scalar);
	FVector2D& operator +=(const FVector2D& other);
	FVector2D& operator -=(float scalar);
	FVector2D& operator -=(const FVector2D& other);
	FVector2D& operator *=(const FVector2D& other);
	FVector2D& operator *=(float scalar);
};

struct FQuat
{
public:
	float                                           X;                                                       //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                           Y;                                                       //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                           Z;                                                       //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                           W;                                                       //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
};

struct FLinearColor
{
public:
	float                                                        R;                                                       //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                        G;                                                       //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                        B;                                                       //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                        A;                                                       //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic

public:
	FLinearColor();
	FLinearColor(float r, float g, float b, float a);
};

struct FPlane : public FVector
{
public:
	float                                           W;                                                       //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
};

struct FMatrix
{
public:
	FPlane                                          XPlane;                                                  //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	FPlane                                          YPlane;                                                  //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	FPlane                                          ZPlane;                                                  //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	FPlane                                          WPlane;                                                  //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic

public:
	FMatrix operator *(const FMatrix& other) const;
};

struct FTransform
{
public:
	FQuat                                           Rotation;                                                //  Edit, BlueprintVisible, SaveGame, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic
	FVector                                         Translation;                                             //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	uint8_t                                         UnknownData_0000[0x4];                                   //  MISSED OFFSET (FIX SPACE BETWEEN PREVIOUS PROPERTY)
	FVector                                         Scale3D;                                                 //  Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	uint8_t                                         UnknownData_0001[0x4];                                   //  MISSED OFFSET (PADDING)

public:
	FMatrix ToMatrixWithScale() const;
};