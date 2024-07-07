#include "pch.h"

//=============================================================================================================================================

FString::FString() {}

FString::FString(const wchar_t* other)
{
	_max = _count = *other ? static_cast<int32_t>(std::wcslen(other)) + 1 : 0;
	if (_count)
		_data = const_cast<wchar_t*>(other);
}

const wchar_t* FString::wc_str() const
{
	return _data;
}

const char* FString::c_str() const
{
	return (const char*)_data;
}

bool FString::IsValid() const
{
	return _data != nullptr;
}

std::string FString::ToString() const
{
	size_t length = std::wcslen(_data);
	std::string str(length, '\0');
	std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(_data, _data + length, '?', &str[0]);
	return str;
}

std::wstring FString::ToStringW() const
{
	std::wstring str(_data);
	return str;
}

//=============================================================================================================================================

bool FUObjectItem::IsUnreachable() const
{
	return !!(Flags & static_cast<std::underlying_type_t<ObjectFlags>>(ObjectFlags::Unreachable));
}

bool FUObjectItem::IsPendingKill() const
{
	return !!(Flags & static_cast<std::underlying_type_t<ObjectFlags>>(ObjectFlags::PendingKill));
}

int32_t TUObjectArray::Count() const
{
	return NumElements;
}

int32_t TUObjectArray::Max() const
{
	return MaxElements;
}

bool TUObjectArray::IsValidIndex(int32_t Index) const
{
	return Index < Count() && Index >= 0;
}

FUObjectItem* TUObjectArray::GetObjectPtr(int32_t Index) const
{
	const int32_t ChunkIndex = Index / NumElementsPerChunk;
	const int32_t WithinChunkIndex = Index % NumElementsPerChunk;
	if (!IsValidIndex(Index)) return nullptr;
	if (ChunkIndex > NumChunks) return nullptr;
	if (Index > MaxElements) return nullptr;
	FUObjectItem* Chunk = Objects[ChunkIndex];
	if (!Chunk) return nullptr;
	return Chunk + WithinChunkIndex;
}

UObject* TUObjectArray::GetByIndex(int32_t index) const
{
	FUObjectItem* ItemPtr = GetObjectPtr(index);
	if (!ItemPtr) return nullptr;
	return (*ItemPtr).Object;
}

FUObjectItem* TUObjectArray::GetItemByIndex(int32_t index) const
{
	FUObjectItem* ItemPtr = GetObjectPtr(index);
	if (!ItemPtr) return nullptr;
	return ItemPtr;
}

UObject* TUObjectArray::operator[](int32_t i)
{
	return GetByIndex(i);
}

const UObject* TUObjectArray::operator[](int32_t i) const
{
	return GetByIndex(i);
}

int32_t FNameEntry::GetLength() const
{
	return Header.Len;
}

bool FNameEntry::IsWide() const
{
	return Header.bIsWide;
}

int32_t FNameEntry::GetId() const
{
	throw std::exception("This game doesn't use 'FNAME_POOL_WITH_CASE_PRESERVING_NAME' so 'ComparisonId' not stored in 'FNameEntry'");
}

std::string FNameEntry::GetAnsiName() const
{
	uint32_t len = GetLength();
	if (len > 1024) return "[Error: Overflow]";
	return std::string((const char*)AnsiName, len);
}

std::wstring FNameEntry::GetWideName() const
{
	uint32_t len = GetLength();
	return std::wstring((const wchar_t*)WideName, len);
}

std::string FNameEntry::GetName() const
{
	return GetAnsiName();
}

int32_t FNameEntryAllocator::NumBlocks() const
{
	return CurrentBlock + 1;
}

FNameEntry* FNameEntryAllocator::GetById(int32_t key) const
{
	int block = key >> 16;
	int offset = (uint16_t)key;
	if (!IsValidIndex(key, block, offset))
		return reinterpret_cast<FNameEntry*>(Blocks[0] + 0); // "None"
	return reinterpret_cast<FNameEntry*>(Blocks[block] + ((uint64_t)offset * Stride));
}

bool FNameEntryAllocator::IsValidIndex(int32_t key) const
{
	uint32_t block = key >> 16;
	uint16_t offset = key;
	return IsValidIndex(key, block, offset);
}

bool FNameEntryAllocator::IsValidIndex(int32_t key, uint32_t block, uint16_t offset) const
{
	return (key >= 0 && block < static_cast<uint32_t>(NumBlocks()) && offset * Stride < MaxOffset);
}

FNameEntry* FNamePool::GetNext(uintptr_t& nextFNameAddress, uint32_t* comparisonId) const
{
	static int lastBlock = 0;
	if (!nextFNameAddress)
	{
		lastBlock = 0;
		nextFNameAddress = reinterpret_cast<uintptr_t>(Allocator.Blocks[0]);
	}
RePlay:
	int32_t nextFNameComparisonId = MAKELONG((uint16_t)((nextFNameAddress - reinterpret_cast<uintptr_t>(Allocator.Blocks[lastBlock])) / 2), (uint16_t)lastBlock);
	int32_t block = nextFNameComparisonId >> 16;
	int32_t offset = (uint16_t)nextFNameComparisonId;
	int32_t offsetFromBlock = static_cast<int32_t>(nextFNameAddress - reinterpret_cast<uintptr_t>(Allocator.Blocks[lastBlock]));

	// Get entry information
	const uintptr_t entryOffset = nextFNameAddress;
	const int toAdd = 0x00 + 0x02; // HeaderOffset + HeaderSize
	const uint16_t nameHeader = *reinterpret_cast<uint16_t*>(entryOffset);
	int nameLength = nameHeader >> 6;
	bool isWide = (nameHeader & 1) != 0;
	if (isWide)
		nameLength += nameLength;

	// if odd number (odd numbers are aligned with 0x00)
	if (!isWide && nameLength % 2 != 0)
		nameLength += 1;

	// Block end ?
	if (offsetFromBlock + toAdd + (nameLength * 2) >= 0xFFFF * FNameEntryAllocator::Stride || nameHeader == 0x00 || block == Allocator.CurrentBlock && offset >= Allocator.CurrentByteCursor)
	{
		nextFNameAddress = reinterpret_cast<uintptr_t>(Allocator.Blocks[++lastBlock]);
		goto RePlay;
	}

	// We hit last Name in last Block
	if (lastBlock > Allocator.CurrentBlock)
		return nullptr;

	// Get next name address
	nextFNameAddress = entryOffset + toAdd + nameLength;

	// Get name
	FNameEntry* ret = Allocator.GetById(nextFNameComparisonId);

	if (comparisonId)
		*comparisonId = nextFNameComparisonId;

	return ret;
}

int32_t FNamePool::Count() const
{
	return AnsiCount;
}

bool FNamePool::IsValidIndex(int32_t index) const
{
	return Allocator.IsValidIndex(static_cast<int32_t>(index));
}

FNameEntry* FNamePool::GetById(int32_t id) const
{
	return Allocator.GetById(id);
}

FNameEntry* FNamePool::operator[](int32_t id) const
{
	return GetById(id);
}

FName::FName()
{
	ComparisonIndex = 0;
	Number = 0;
}

FName::FName(int32_t i)
{
	ComparisonIndex = i;
	Number = 0;
}

FName::FName(const char* nameToFind)
{
	Number = 0;
	static std::unordered_set<int> cache;
	for (auto i : cache)
	{
		if (GetGlobalNames()[i]->GetAnsiName() == nameToFind)
		{
			ComparisonIndex = i;
#ifdef FNAME_POOL_WITH_CASE_PRESERVING_NAME
			DisplayIndex = i;
#endif
			return;
		}
	}

#ifdef FNAME_POOL
	uintptr_t lastFNameAddress = NULL;
	uint32_t curComparisonId = 0;
	for (FNameEntry* name = GetGlobalNames().GetNext(lastFNameAddress, &curComparisonId); name != nullptr; name = GetGlobalNames().GetNext(lastFNameAddress, &curComparisonId))
	{
		if (name->GetAnsiName() == nameToFind)
		{
			cache.insert(curComparisonId);
			ComparisonIndex = curComparisonId;
#ifdef FNAME_POOL_WITH_CASE_PRESERVING_NAME
			DisplayIndex = curComparisonId;
#endif
			return;
		}
	}
#else
	for (int32_t i = 0; i < GetGlobalNames().Count(); ++i)
	{
		if (GetGlobalNames()[i]->GetAnsiName() == nameToFind)
		{
			cache.insert(i);
			ComparisonIndex = i;
			return;
		}
	}
#endif
}

FName::FName(const wchar_t* nameToFind)
{
	Number = 0;
	static std::unordered_set<int> cache;
	for (auto i : cache)
	{
		if (GetGlobalNames()[i]->GetWideName() == nameToFind)
		{
			ComparisonIndex = i;
#ifdef FNAME_POOL_WITH_CASE_PRESERVING_NAME
			DisplayIndex = i;
#endif
			return;
		}
	}

#ifdef FNAME_POOL
	uintptr_t lastFNameAddress = NULL;
	uint32_t curComparisonId = 0;
	for (FNameEntry* name = GetGlobalNames().GetNext(lastFNameAddress, &curComparisonId); name != nullptr; name = GetGlobalNames().GetNext(lastFNameAddress, &curComparisonId))
	{
		if (name->GetWideName() == nameToFind)
		{
			cache.insert(curComparisonId);
			ComparisonIndex = curComparisonId;
#ifdef FNAME_POOL_WITH_CASE_PRESERVING_NAME
			DisplayIndex = curComparisonId;
#endif
			return;
		}
	}
#else
	for (int32_t i = 0; i < GetGlobalNames().Count(); ++i)
	{
		if (GetGlobalNames()[i]->GetWideName() == nameToFind)
		{
			cache.insert(i);
			ComparisonIndex = i;
			return;
		}
	}
#endif
}

FNamePool& FName::GetGlobalNames()
{
	return *GNames;
}

std::string FName::GetNameA() const
{
	return GetGlobalNames()[ComparisonIndex]->GetAnsiName();
}

std::wstring FName::GetNameW() const
{
	return GetGlobalNames()[ComparisonIndex]->GetWideName();
}

std::string FName::GetName() const
{
	return GetNameA();
}

wchar_t* FText::Get() const
{
	return Data != nullptr ? Data->Name : nullptr;
}

std::string FText::ToString() const
{
	wchar_t* name = Get();
	if (!name)
		return "NOT FOUND";

	size_t length = std::wcslen(name);
	std::string str(length, '\0');
	std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(name, name + length, '?', &str[0]);

	return str;
}

std::wstring FText::ToWString() const
{
	wchar_t* name = Get();
	if (!name)
		return L"NOT FOUND";

	std::wstring str(name);
	return str;
}

//=============================================================================================================================================

TUObjectArray& UObject::GetGlobalObjects()
{
	return *GObjects;
}

std::string UObject::GetName() const
{
	std::string name(Name.GetName());
	if (Name.Number > 0)
		name += '_' + std::to_string(Name.Number);
	auto pos = name.rfind('/');
	if (pos == std::string::npos)
		return name;
	return name.substr(pos + 1);
}

std::string UObject::GetFullName() const
{
	std::string name;
	if (Class != nullptr)
	{
		std::string temp;
		for (auto p = Outer; p; p = p->Outer)
		{
			temp = p->GetName() + "." + temp;
		}
		name = Class->GetName();
		name += " ";
		name += temp;
		name += GetName();
	}
	return name;
}

UClass* UObject::FindClass(const std::string& name)
{
	return FindObject<UClass>(name);
}

bool UObject::IsA(UClass* cmp) const
{
	for (auto super = Class; super; super = static_cast<UClass*>(super->SuperField))
	{
		if (super == cmp)
			return true;
	}

	return false;
}

void UObject::ExecuteUbergraph(int32_t EntryPoint)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function CoreUObject.Object.ExecuteUbergraph");

	struct UObject_ExecuteUbergraph_Params
	{
	public:
		int32_t                                                    EntryPoint;                                              // 0x0000(0x0004)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	UObject_ExecuteUbergraph_Params params{};
	params.EntryPoint = EntryPoint;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;
}

void UObject::ProcessEvent(class UFunction* function, void* parms)
{
	GetVFunction<void(*)(UObject*, class UFunction*, void*)>(this, Offsets::ProcessEventIdx)(this, function, parms);
}

UClass* UObject::StaticClass()
{
	static UClass* ptr = nullptr;
	if (!ptr)
		ptr = UObject::FindClass("Class CoreUObject.Object");
	return ptr;
}

UClass* UField::StaticClass()
{
	static UClass* ptr = nullptr;
	if (!ptr)
		ptr = UObject::FindClass("Class CoreUObject.Field");
	return ptr;
}

UClass* UStruct::StaticClass()
{
	static UClass* ptr = nullptr;
	if (!ptr)
		ptr = UObject::FindClass("Class CoreUObject.Struct");
	return ptr;
}

UObject* UClass::CreateDefaultObject()
{
	return GetVFunction<UObject * (*)(UClass*)>(this, CREATE_DEFAULT_OBJECT_INDEX)(this);
}

UClass* UClass::StaticClass()
{
	static UClass* ptr = nullptr;
	if (!ptr)
		ptr = UObject::FindClass("Class CoreUObject.Class");
	return ptr;
}

UClass* UFunction::StaticClass()
{
	static UClass* ptr = nullptr;
	if (!ptr)
		ptr = UObject::FindClass("Class CoreUObject.Function");
	return ptr;
}

//=============================================================================================================================================

FRotator::FRotator()
{
	Pitch = 0.0f;
	Yaw = 0.0f;
	Roll = 0.0f;
}

FRotator::FRotator(float pitch, float yaw, float roll)
{
	Pitch = pitch;
	Yaw = yaw;
	Roll = roll;
}

FRotator FRotator::operator +(float scalar) const
{
	return FRotator(Pitch + scalar, Yaw + scalar, Roll + scalar);
}

FRotator FRotator::operator +(const FRotator& other) const
{
	return FRotator(Pitch + other.Pitch, Yaw + other.Yaw, Roll + other.Roll);
}

FRotator FRotator::operator -(float scalar) const
{
	return FRotator(Pitch - scalar, Yaw - scalar, Roll - scalar);
}

FRotator FRotator::operator -(const FRotator& other) const
{
	return FRotator(Pitch - other.Pitch, Yaw - other.Yaw, Roll - other.Roll);
}

FRotator FRotator::operator *(float scalar) const
{
	return FRotator(Pitch * scalar, Yaw * scalar, Roll * scalar);
}

FRotator FRotator::operator *(const FRotator& other) const
{
	return FRotator(Pitch * other.Pitch, Yaw * other.Yaw, Roll * other.Roll);
}

FRotator FRotator::operator /(float scalar) const
{
	return FRotator(Pitch / scalar, Yaw / scalar, Roll / scalar);
}

FRotator FRotator::operator /(const FRotator& other) const
{
	return FRotator(Pitch / other.Pitch, Yaw / other.Yaw, Roll / other.Roll);
}

FRotator& FRotator::operator =(const FRotator& other)
{
	Pitch = other.Pitch; Yaw = other.Yaw; Roll = other.Roll; return *this;
}

FRotator& FRotator::operator +=(float scalar)
{
	Pitch += scalar;
	Yaw += scalar;
	Roll += scalar;
	return *this;
}

FRotator& FRotator::operator +=(const FRotator& other)
{
	Pitch += other.Pitch;
	Yaw += other.Yaw;
	Roll += other.Roll;
	return *this;
}

FRotator& FRotator::operator -=(float scalar)
{
	Pitch -= scalar;
	Yaw -= scalar;
	Roll -= scalar;
	return *this;
}

FRotator& FRotator::operator -=(const FRotator& other)
{
	Pitch -= other.Pitch;
	Yaw -= other.Yaw;
	Roll -= other.Roll;
	return *this;
}

FRotator& FRotator::operator *=(const FRotator& other)
{
	Pitch *= other.Pitch;
	Yaw *= other.Yaw;
	Roll *= other.Roll;
	return *this;
}

FRotator& FRotator::operator *=(const float other)
{
	Pitch *= other;
	Yaw *= other;
	Roll *= other;
	return *this;
}

FRotator& FRotator::operator /=(const FRotator& other)
{
	Pitch /= other.Pitch;
	Yaw /= other.Yaw;
	Roll /= other.Roll;
	return *this;
}

FRotator& FRotator::operator /=(const float other)
{
	Pitch /= other;
	Yaw /= other;
	Roll /= other;
	return *this;
}

float FRotator::Size() const
{
	return sqrt(Pitch * Pitch + Yaw * Yaw + Roll * Roll);
}

FRotator FRotator::Clamp() const
{
	FRotator r = { Pitch, Yaw, Roll };
	if (r.Yaw > 180.0f)
		r.Yaw -= 360.0f;
	else if (r.Yaw < -180.0f)
		r.Yaw += 360.0f;
	if (r.Pitch > 180.0f)
		r.Pitch -= 360.0f;
	else if (r.Pitch < -180.0f)
		r.Pitch += 360.0f;
	if (r.Pitch < -89.0f)
		r.Pitch = -89.0f;
	else if (r.Pitch > 89.0f)
		r.Pitch = 89.0f;
	r.Roll = 0.0f;
	return r;
}

FVector::FVector()
{
	X = 0;
	Y = 0;
	Z = 0;
}

FVector::FVector(float x, float y, float z)
{
	X = x;
	Y = y;
	Z = z;
}

FVector FVector::operator +(float scalar) const
{
	return FVector(X + scalar, Y + scalar, Z + scalar);
}

FVector FVector::operator +(const FVector& other) const
{
	return FVector(X + other.X, Y + other.Y, Z + other.Z);
}

FVector FVector::operator -(float scalar) const
{
	return FVector(X - scalar, Y - scalar, Z - scalar);
}

FVector FVector::operator -(const FVector& other) const
{
	return FVector(X - other.X, Y - other.Y, Z - other.Z);
}

FVector FVector::operator *(float scalar) const
{
	return FVector(X * scalar, Y * scalar, Z * scalar);
}

FVector FVector::operator *(const FVector& other) const
{
	return FVector(X * other.X, Y * other.Y, Z * other.Z);
}

FVector FVector::operator /(float scalar) const
{
	return FVector(X / scalar, Y / scalar, Z / scalar);
}

FVector FVector::operator /(const FVector& other) const
{
	return FVector(X / other.X, Y / other.Y, Z / other.Z);
}

FVector& FVector::operator =(const FVector& other)
{
	X = other.X;
	Y = other.Y;
	Z = other.Z;
	return *this;
}

FVector& FVector::operator +=(float scalar)
{
	X += scalar;
	Y += scalar;
	Z += scalar;
	return *this;
}

FVector& FVector::operator +=(const FVector& other)
{
	X += other.X;
	Y += other.Y;
	Z += other.Z;
	return *this;
}

FVector& FVector::operator -=(float scalar)
{
	X -= scalar;
	Y -= scalar;
	Z -= scalar;
	return *this;
}

FVector& FVector::operator -=(const FVector& other)
{
	X -= other.X;
	Y -= other.Y;
	Z -= other.Z;
	return *this;
}

FVector& FVector::operator *=(const FVector& other)
{
	X *= other.X;
	Y *= other.Y;
	Z *= other.Z;
	return *this;
}

FVector& FVector::operator *=(float scalar)
{
	X *= scalar;
	Y *= scalar;
	Z *= scalar;
	return *this;
}

bool FVector::IsValid()
{
	return X != 0.0f && Y != 0.0f;
}

void FVector::Normalize360(FVector& angle)
{
	const float maxX1 = 80.0f;

	while (angle.X > maxX1)
		angle.X -= 180.0f;
	while (angle.X < -maxX1)
		angle.X += 180.0f;
	while (angle.Y > 180.0f)
		angle.Y -= 360.0f;
	while (angle.Y < -180.0f)
		angle.Y += 360.0f;
}

float FVector::Dot(const FVector& b) const
{
	return (X * b.X) + (Y * b.Y) + (Z * b.Z);
}

float FVector::MagnitudeSqr() const
{
	return Dot(*this);
}

float FVector::Magnitude() const
{
	return sqrtf(MagnitudeSqr());
}

FVector FVector::Unit() const
{
	const float fMagnitude = Magnitude();
	return FVector(X / fMagnitude, Y / fMagnitude, Z / fMagnitude);
}

FRotator FVector::ToRotator() const
{
	static constexpr float PI = 3.14159265359f;
	// Pitch, Yaw, Roll
	return FRotator(asinf(Z / Magnitude()) * 180.0f / PI, atan2f(Y, X) * 180.0f / PI, 0.0f);
}

float FVector::Distance(const FVector& v) const
{
	return float(sqrtf(powf(v.X - X, 2.0f) + powf(v.Y - Y, 2.0f) + powf(v.Z - Z, 2.0f)));
}

float FVector::DistanceMeter(FVector& v) const
{
	return Distance(v) * 0.01f;
}

FVector2D::FVector2D()
{
	X = 0;
	Y = 0;
}

FVector2D::FVector2D(float x, float y)
{
	X = x;
	Y = y;
}

bool FVector2D::IsValid()
{
	return X != 0.0f && Y != 0.0f;
}

float FVector2D::Distance(FVector2D& v) const
{
	return powf(v.X - X, 2) + powf(v.Y - Y, 2);
}

FVector2D FVector2D::operator +(const FVector2D& other) const
{
	return FVector2D(X + other.X, Y + other.Y);
}

FVector2D FVector2D::operator +(float scalar) const
{
	return FVector2D(X + scalar, Y + scalar);
}

FVector2D FVector2D::operator -(float scalar) const
{
	return FVector2D(X - scalar, Y - scalar);
}

FVector2D FVector2D::operator -(const FVector2D& other) const
{
	return FVector2D(X - other.X, Y - other.Y);
}

FVector2D FVector2D::operator *(float scalar) const
{
	return FVector2D(X * scalar, Y * scalar);
}

FVector2D FVector2D::operator *(const FVector2D& other) const
{
	return FVector2D(X * other.X, Y * other.Y);
}

FVector2D FVector2D::operator /(float scalar) const
{
	return FVector2D(X / scalar, Y / scalar);
}

FVector2D FVector2D::operator /(const FVector2D& other) const
{
	return FVector2D(X / other.X, Y / other.Y);
}

FVector2D& FVector2D::operator =(const FVector2D& other)
{
	X = other.X;
	Y = other.Y;
	return *this;
}

FVector2D& FVector2D::operator +=(float scalar)
{
	X += scalar;
	Y += scalar;
	return *this;
}

FVector2D& FVector2D::operator +=(const FVector2D& other)
{
	X += other.X;
	Y += other.Y;
	return *this;
}

FVector2D& FVector2D::operator -=(float scalar)
{
	X -= scalar;
	Y -= scalar;
	return *this;
}

FVector2D& FVector2D::operator -=(const FVector2D& other)
{
	X -= other.X;
	Y -= other.Y;
	return *this;
}

FVector2D& FVector2D::operator *=(const FVector2D& other)
{
	X *= other.X;
	Y *= other.Y;
	return *this;
}

FVector2D& FVector2D::operator *=(float scalar)
{
	X *= scalar;
	Y *= scalar;
	return *this;
}

FLinearColor::FLinearColor()
{
	R = 0;
	G = 0;
	B = 0;
	A = 0;
}

FLinearColor::FLinearColor(float r, float g, float b, float a)
{
	R = r;
	G = g;
	B = b;
	A = a;
}

FMatrix FMatrix::operator *(const FMatrix& other) const
{
	FMatrix ret;

	ret.XPlane.X = XPlane.X * other.XPlane.X + XPlane.Y * other.YPlane.X + XPlane.Z * other.ZPlane.X + XPlane.W * other.WPlane.X;
	ret.XPlane.Y = XPlane.X * other.XPlane.Y + XPlane.Y * other.YPlane.Y + XPlane.Z * other.ZPlane.Y + XPlane.W * other.WPlane.Y;
	ret.XPlane.Z = XPlane.X * other.XPlane.Z + XPlane.Y * other.YPlane.Z + XPlane.Z * other.ZPlane.Z + XPlane.W * other.WPlane.Z;
	ret.XPlane.W = XPlane.X * other.XPlane.W + XPlane.Y * other.YPlane.W + XPlane.Z * other.ZPlane.W + XPlane.W * other.WPlane.W;

	ret.YPlane.X = YPlane.X * other.XPlane.X + YPlane.Y * other.YPlane.X + YPlane.Z * other.ZPlane.X + YPlane.W * other.WPlane.X;
	ret.YPlane.Y = YPlane.X * other.XPlane.Y + YPlane.Y * other.YPlane.Y + YPlane.Z * other.ZPlane.Y + YPlane.W * other.WPlane.Y;
	ret.YPlane.Z = YPlane.X * other.XPlane.Z + YPlane.Y * other.YPlane.Z + YPlane.Z * other.ZPlane.Z + YPlane.W * other.WPlane.Z;
	ret.YPlane.W = YPlane.X * other.XPlane.W + YPlane.Y * other.YPlane.W + YPlane.Z * other.ZPlane.W + YPlane.W * other.WPlane.W;

	ret.ZPlane.X = ZPlane.X * other.XPlane.X + ZPlane.Y * other.YPlane.X + ZPlane.Z * other.ZPlane.X + ZPlane.W * other.WPlane.X;
	ret.ZPlane.Y = ZPlane.X * other.XPlane.Y + ZPlane.Y * other.YPlane.Y + ZPlane.Z * other.ZPlane.Y + ZPlane.W * other.WPlane.Y;
	ret.ZPlane.Z = ZPlane.X * other.XPlane.Z + ZPlane.Y * other.YPlane.Z + ZPlane.Z * other.ZPlane.Z + ZPlane.W * other.WPlane.Z;
	ret.ZPlane.W = ZPlane.X * other.XPlane.W + ZPlane.Y * other.YPlane.W + ZPlane.Z * other.ZPlane.W + ZPlane.W * other.WPlane.W;

	ret.WPlane.X = WPlane.X * other.XPlane.X + WPlane.Y * other.YPlane.X + WPlane.Z * other.ZPlane.X + WPlane.W * other.WPlane.X;
	ret.WPlane.Y = WPlane.X * other.XPlane.Y + WPlane.Y * other.YPlane.Y + WPlane.Z * other.ZPlane.Y + WPlane.W * other.WPlane.Y;
	ret.WPlane.Z = WPlane.X * other.XPlane.Z + WPlane.Y * other.YPlane.Z + WPlane.Z * other.ZPlane.Z + WPlane.W * other.WPlane.Z;
	ret.WPlane.W = WPlane.X * other.XPlane.W + WPlane.Y * other.YPlane.W + WPlane.Z * other.ZPlane.W + WPlane.W * other.WPlane.W;

	return ret;
}

FMatrix FTransform::ToMatrixWithScale() const
{
	FMatrix OutMatrix;
	OutMatrix.WPlane.X = Translation.X;
	OutMatrix.WPlane.Y = Translation.Y;
	OutMatrix.WPlane.Z = Translation.Z;

	const float x2 = Rotation.X + Rotation.X;
	const float y2 = Rotation.Y + Rotation.Y;
	const float z2 = Rotation.Z + Rotation.Z;

	{
		const float xx2 = Rotation.X * x2;
		const float yy2 = Rotation.Y * y2;
		const float zz2 = Rotation.Z * z2;

		OutMatrix.XPlane.X = (1.0f - (yy2 + zz2)) * Scale3D.X;
		OutMatrix.YPlane.Y = (1.0f - (xx2 + zz2)) * Scale3D.Y;
		OutMatrix.ZPlane.Z = (1.0f - (xx2 + yy2)) * Scale3D.Z;
	}

	{
		const float yz2 = Rotation.Y * z2;
		const float wx2 = Rotation.W * x2;

		OutMatrix.ZPlane.Y = (yz2 - wx2) * Scale3D.Z;
		OutMatrix.YPlane.Z = (yz2 + wx2) * Scale3D.Y;
	}

	{
		const float xy2 = Rotation.X * y2;
		const float wz2 = Rotation.W * z2;

		OutMatrix.YPlane.X = (xy2 - wz2) * Scale3D.Y;
		OutMatrix.XPlane.Y = (xy2 + wz2) * Scale3D.X;
	}

	{
		const float xz2 = Rotation.X * z2;
		const float wy2 = Rotation.W * y2;

		OutMatrix.ZPlane.X = (xz2 + wy2) * Scale3D.Z;
		OutMatrix.XPlane.Z = (xz2 - wy2) * Scale3D.X;
	}

	OutMatrix.XPlane.W = 0.0f;
	OutMatrix.YPlane.W = 0.0f;
	OutMatrix.ZPlane.W = 0.0f;
	OutMatrix.WPlane.W = 1.0f;

	return OutMatrix;
}