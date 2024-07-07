#include "pch.h"

bool InitSDK(uintptr_t gObjectsOffset, uintptr_t gNamesOffset, uintptr_t gWorldOffset)
{
	auto mBaseAddress = reinterpret_cast<uintptr_t>(GetModuleHandleW(skCrypt(L"Tiger-Win64-Shipping.exe")));
	if (!mBaseAddress)
		return false;

	UObject::GObjects = reinterpret_cast<TUObjectArray*>(mBaseAddress + gObjectsOffset);
	FName::GNames = reinterpret_cast<FNamePool*>(mBaseAddress + gNamesOffset);
	UWorld::GWorld = reinterpret_cast<UWorld**>(mBaseAddress + gWorldOffset);
	//UWorld::GWorld = reinterpret_cast<UWorld**>(Signature(std::string(skCrypt("48 8B 1D ? ? ? ? 48 85 DB 74 ? 41 B0"))).Instruction(3).Add(7).GetPointer());

	return true;
}

bool InitSDK()
{
	return InitSDK(Offsets::GObjects, Offsets::GNames, Offsets::GWorld);
}

FVector USceneComponent::GetSocketLocation(const class FName& InSocketName)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.SceneComponent.GetSocketLocation");

	struct USceneComponent_GetSocketLocation_Params
	{
	public:
		class FName                                                InSocketName;                                            // 0x0000(0x0008)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		struct FVector                                             ReturnValue;                                             // 0x0008(0x000C)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	USceneComponent_GetSocketLocation_Params params{};
	params.InSocketName = InSocketName;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

FVector USceneComponent::GetRelativeLocation()
{
	if (!this)
		return FVector();

	return *reinterpret_cast<FVector*>(reinterpret_cast<uint64_t>(this) + 0x11C);
}

FVector USceneComponent::GetComponentVelocity()
{
	if (!this)
		return FVector();

	return *reinterpret_cast<FVector*>(reinterpret_cast<uint64_t>(this) + 0x140);
}

FName USkinnedMeshComponent::GetBoneName(int32_t BoneIndex)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.SkinnedMeshComponent.GetBoneName");

	struct USkinnedMeshComponent_GetBoneName_Params
	{
	public:
		int32_t                                                    BoneIndex;                                               // 0x0000(0x0004)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		class FName                                                ReturnValue;                                             // 0x0004(0x0008)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	USkinnedMeshComponent_GetBoneName_Params params{};
	params.BoneIndex = BoneIndex;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

int32_t USkinnedMeshComponent::GetBoneIndex(const class FName& BoneName)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.SkinnedMeshComponent.GetBoneIndex");

	struct USkinnedMeshComponent_GetBoneIndex_Params
	{
	public:
		class FName                                                BoneName;                                                // 0x0000(0x0008)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		int32_t                                                    ReturnValue;                                             // 0x0008(0x0004)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	USkinnedMeshComponent_GetBoneIndex_Params params{};
	params.BoneName = BoneName;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

FVector USkeletalMeshComponent::GetBoneWorldPos(const int32_t& BoneIndex) const
{
	if (GET_BONE_MATRIX_OFFSET > 0x00)
	{
		typedef void(*GetBoneMatrixType)(const USkinnedMeshComponent*, FMatrix&, int);
		static GetBoneMatrixType GetBoneMatrixPtr = nullptr;
		if (!GetBoneMatrixPtr)
			GetBoneMatrixPtr = reinterpret_cast<GetBoneMatrixType>((uintptr_t)GetModuleHandle(nullptr) + GET_BONE_MATRIX_OFFSET);

		FMatrix matrix;
		GetBoneMatrixPtr(this, matrix, BoneIndex);

		return matrix.WPlane;
	}

	FVector worldPos{};

	/*
	 * How to find the `CachedBoneSpaceTransforms` field:
	 * - Locate the `USkinnedMeshComponent` class in the `Engine` package (Engine_USkinnedMeshComponent.h).
	 * - Find the `MasterPoseComponent` field.
	 * - Find the `PhysicsAssetOverride` field.
	 * - Between these two fields, locate a field with a name starting with `UnknownData_` and of type `uint8_t` with a size of at least 0x20 bytes.
	 * - Within that field, there are two `BasicTypes::TArray<FTransform>` fields; one of them is `CachedBoneSpaceTransforms`.
	 *
	 * How to find the `ComponentToWorld` field:
	 * - Locate the `USceneComponent` class in the `Engine` package (Engine_USceneComponent.h).
	 * - Starting from offset `0x190`, find a field with a name starting with `UnknownData_` and of type `uint8_t` that has at least the size of `FTransform`.
	 *   - This field is typically either the last field in the `USceneComponent` class or the first field in the `UPrimitiveComponent` class (Engine_UPrimitiveComponent.h).
	 * - Search for the `FTransform` where `Scale3D` equals `FVector(1, 1, 1)`.
	 */

	int32_t componentSpaceTransformsOffset = /* 0x400 */ 0x740;
	int32_t componentToWorldOffset = /* 0x190 || 0x1C0 || 0x1D0 || 0x250 */ 0x1C0;

	uintptr_t thiz = reinterpret_cast<uintptr_t>(this);
	const auto& componentSpaceTransforms = *reinterpret_cast<TArray<FTransform>*>(thiz + componentSpaceTransformsOffset);
	if (BoneIndex >= componentSpaceTransforms.Count())
		return worldPos;

	FMatrix targetBoneMatrix = componentSpaceTransforms[BoneIndex].ToMatrixWithScale();
	FMatrix componentToWorldMatrix = (*reinterpret_cast<FTransform*>(thiz + componentToWorldOffset)).ToMatrixWithScale();
	FMatrix newMatrix = targetBoneMatrix * componentToWorldMatrix;

	worldPos = { newMatrix.WPlane.X, newMatrix.WPlane.Y, newMatrix.WPlane.Z };
	return worldPos;
}

void UTigerOutlineComponent::DeactivateOutline(ETigerOutlineMode InMode)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Tiger.TigerOutlineComponent.DeactivateOutline");

	struct UTigerOutlineComponent_DeactivateOutline_Params
	{
	public:
		ETigerOutlineMode                                          InMode;                                                  // 0x0000(0x0001)  (ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	UTigerOutlineComponent_DeactivateOutline_Params params{};
	params.InMode = InMode;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;
}

void UTigerOutlineComponent::ActivateOutline(ETigerOutlineMode InMode)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Tiger.TigerOutlineComponent.ActivateOutline");

	struct UTigerOutlineComponent_ActivateOutline_Params
	{
	public:
		ETigerOutlineMode                                          InMode;                                                  // 0x0000(0x0001)  (ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	UTigerOutlineComponent_ActivateOutline_Params params{};
	params.InMode = InMode;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;
}

void AActor::GetActorBounds(bool bOnlyCollidingComponents, struct FVector* Origin, struct FVector* BoxExtent, bool bIncludeFromChildActors)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.Actor.GetActorBounds");

	struct AActor_GetActorBounds_Params
	{
	public:
		bool                                                       bOnlyCollidingComponents;                                // 0x0000(0x0001)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		unsigned char                                              UnknownData_LVOC[0x3];                                   // 0x0001(0x0003) MISSED OFFSET (FIX SPACE BETWEEN PREVIOUS PROPERTY) ()
		struct FVector                                             Origin;                                                  // 0x0004(0x000C)  (Parm, OutParm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		struct FVector                                             BoxExtent;                                               // 0x0010(0x000C)  (Parm, OutParm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		bool                                                       bIncludeFromChildActors;                                 // 0x001C(0x0001)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	AActor_GetActorBounds_Params params{};
	params.bOnlyCollidingComponents = bOnlyCollidingComponents;
	params.bIncludeFromChildActors = bIncludeFromChildActors;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	if (Origin != nullptr)
		*Origin = params.Origin;
	if (BoxExtent != nullptr)
		*BoxExtent = params.BoxExtent;
}

float AActor::GetDistanceTo(class AActor* OtherActor)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.Actor.GetDistanceTo");

	struct AActor_GetDistanceTo_Params
	{
	public:
		class AActor* OtherActor;                                      // 0x0000(0x0008)  (ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		float ReturnValue;                                             // 0x0008(0x0004)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	AActor_GetDistanceTo_Params params{};
	params.OtherActor = OtherActor;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

FVector AActor::GetActorLocation()
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.Actor.K2_GetActorLocation");

	struct AActor_K2_GetActorLocation_Params
	{
	public:
		struct FVector ReturnValue;                                             // 0x0000(0x000C)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	AActor_K2_GetActorLocation_Params params{};

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

FRotator AActor::GetActorRotation()
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.Actor.K2_GetActorRotation");

	struct AActor_K2_GetActorRotation_Params
	{
	public:
		struct FRotator ReturnValue;                                             // 0x0000(0x000C)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
	};

	AActor_K2_GetActorRotation_Params params{};

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

USceneComponent* AActor::GetRootComponent()
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.Actor.K2_GetRootComponent");

	struct AActor_K2_GetRootComponent_Params
	{
	public:
		class USceneComponent* ReturnValue;                                             // 0x0000(0x0008)  (ExportObject, Parm, OutParm, ZeroConstructor, ReturnParm, InstancedReference, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	AActor_K2_GetRootComponent_Params params{};

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

uint8_t UTigerItemAsset::GetItemType()
{
	if (!this)
		return -1;

	return *reinterpret_cast<uint8_t*>(reinterpret_cast<uint64_t>(this) + 0x40);
}

uint8_t UTigerItemAsset::GetItemRarity()
{
	if (!this)
		return -1;

	return *reinterpret_cast<uint8_t*>(reinterpret_cast<uint64_t>(this) + 0x41);
}

FText UTigerItemAsset::GetItemName()
{
	if (!this)
		return FText();

	return *reinterpret_cast<FText*>(reinterpret_cast<uint64_t>(this) + 0x48);
}

UTigerItemAsset* ATigerItemWorldRepresentation::GetItemType()
{
	if (!this)
		return nullptr;

	return *reinterpret_cast<UTigerItemAsset**>(reinterpret_cast<uint64_t>(this) + 0x240);
}

FString APlayerState::GetPlayerName()
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.PlayerState.GetPlayerName");

	struct APlayerState_GetPlayerName_Params
	{
	public:
		class FString ReturnValue;                                             // 0x0000(0x0010)  (Parm, OutParm, ZeroConstructor, ReturnParm, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	APlayerState_GetPlayerName_Params params{};

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

bool APlayerState::IsOfSameTeam(APlayerState* OtherState)
{
	static uintptr_t IsOfSameTeamAddress = NULL;
	if (IsOfSameTeamAddress == NULL)
	{
		IsOfSameTeamAddress = Signature(std::string(skCrypt("40 53 48 83 EC ? 48 8B DA 48 85 D2 75 ? 32 C0"))).GetPointer();

		if (IsOfSameTeamAddress == NULL)
			return false;
	}

	return reinterpret_cast<bool(__fastcall*)(APlayerState*, APlayerState*)>(IsOfSameTeamAddress)(this, OtherState);
}

ATigerCharacter* APlayerController::GetAcknowledgedPawn()
{
	if (!this)
		return nullptr;

	return *reinterpret_cast<ATigerCharacter**>(reinterpret_cast<uint64_t>(this) + 0x2A8);
}

APlayerCameraManager* APlayerController::GetPlayerCameraManager()
{
	if (!this)
		return nullptr;

	return *reinterpret_cast<APlayerCameraManager**>(reinterpret_cast<uint64_t>(this) + 0x2C0);
}

void APlayerController::SetControlRotation(const FRotator& NewRotation)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.Controller.SetControlRotation");

	struct AController_SetControlRotation_Params
	{
	public:
		struct FRotator                                            NewRotation;                                             // 0x0000(0x000C)  (ConstParm, Parm, OutParm, ZeroConstructor, ReferenceParm, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
	};

	AController_SetControlRotation_Params params{};
	params.NewRotation = NewRotation;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;
}

void APlayerController::GetViewportSize(int32_t* SizeX, int32_t* SizeY)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.PlayerController.GetViewportSize");

	struct APlayerController_GetViewportSize_Params
	{
	public:
		int32_t                                                    SizeX;                                                   // 0x0000(0x0004)  (Parm, OutParm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		int32_t                                                    SizeY;                                                   // 0x0004(0x0004)  (Parm, OutParm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	APlayerController_GetViewportSize_Params params{};

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	if (SizeX != nullptr)
		*SizeX = params.SizeX;
	if (SizeY != nullptr)
		*SizeY = params.SizeY;
}

bool APlayerController::LineOfSightTo(AActor* Other, const FVector& ViewPoint, bool bAlternateChecks)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.Controller.LineOfSightTo");

	struct AController_LineOfSightTo_Params
	{
	public:
		class AActor* Other;                                          // 0x0000(0x0008)  (ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		struct FVector ViewPoint;                                     // 0x0008(0x000C)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		bool bAlternateChecks;                                        // 0x0014(0x0001)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		bool ReturnValue;                                             // 0x0015(0x0001)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	AController_LineOfSightTo_Params params{};
	params.Other = Other;
	params.ViewPoint = ViewPoint;
	params.bAlternateChecks = bAlternateChecks;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

bool APlayerController::ProjectWorldLocationToScreen(const struct FVector& WorldLocation, struct FVector2D* ScreenLocation, bool bPlayerViewportRelative)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.PlayerController.ProjectWorldLocationToScreen");

	struct APlayerController_ProjectWorldLocationToScreen_Params
	{
	public:
		struct FVector                                             WorldLocation;                                           // 0x0000(0x000C)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		struct FVector2D                                           ScreenLocation;                                          // 0x000C(0x0008)  (Parm, OutParm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		bool                                                       bPlayerViewportRelative;                                 // 0x0014(0x0001)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		bool                                                       ReturnValue;                                             // 0x0015(0x0001)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	APlayerController_ProjectWorldLocationToScreen_Params params{};
	params.WorldLocation = WorldLocation;
	params.bPlayerViewportRelative = bPlayerViewportRelative;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	if (ScreenLocation != nullptr)
		*ScreenLocation = params.ScreenLocation;

	return params.ReturnValue;
}

bool APlayerController::IsInputKeyDown(const FKey& Key)
{
	// GetVFunction<bool(*)(APlayerController*, FKey, EInputEvent, float, bool)>(this, Index)(this, FKey("LeftMouseButton"), EInputEvent::IE_Pressed, 1.0f, false);
	return false;
}

FVector APlayerCameraManager::GetCameraLocation()
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.PlayerCameraManager.GetCameraLocation");

	struct APlayerCameraManager_GetCameraLocation_Params
	{
	public:
		struct FVector ReturnValue;                                             // 0x0000(0x000C)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	APlayerCameraManager_GetCameraLocation_Params params{};

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

FRotator APlayerCameraManager::GetCameraRotation()
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.PlayerCameraManager.GetCameraRotation");

	struct APlayerCameraManager_GetCameraRotation_Params
	{
	public:
		struct FRotator ReturnValue;                                             // 0x0000(0x000C)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
	};

	APlayerCameraManager_GetCameraRotation_Params params{};

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

FCameraCacheEntry APlayerCameraManager::GetCameraCachePrivate()
{
	if (!this)
		return FCameraCacheEntry();

	return *reinterpret_cast<FCameraCacheEntry*>(reinterpret_cast<uint64_t>(this) + 0x1AB0);
}

APlayerState* ATigerCharacter::GetPlayerState()
{
	if (!this)
		return nullptr;

	return *reinterpret_cast<APlayerState**>(reinterpret_cast<uint64_t>(this) + 0x248);
}

USkeletalMeshComponent* ATigerCharacter::GetMesh()
{
	if (!this)
		return nullptr;

	return *reinterpret_cast<USkeletalMeshComponent**>(reinterpret_cast<uint64_t>(this) + 0x288);
}

UTigerOutlineComponent* ATigerCharacter::GetOutlineComponent()
{
	if (!this)
		return nullptr;

	return *reinterpret_cast<UTigerOutlineComponent**>(reinterpret_cast<uint64_t>(this) + 0x4F0);
}

UTigerRangedWeapon* UTigerRangedWeaponComponent::GetEquippedWeaponType()
{
	if (!this)
		return nullptr;

	return *reinterpret_cast<UTigerRangedWeapon**>(reinterpret_cast<uint64_t>(this) + 0x170);
}

std::optional<FTigerFireSettings> UTigerRangedWeaponComponent::GetCurrentFireSettings()
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Tiger.TigerRangedWeaponComponent.GetCurrentFireSettings");

	struct UTigerRangedWeaponComponent_GetCurrentFireSettings_Params
	{
	public:
		struct FTigerFireSettings                                  ReturnValue;                                             // 0x0000(0x00D8)  (ConstParm, Parm, OutParm, ReturnParm, ReferenceParm, NoDestructor, NativeAccessSpecifierPublic)
	};

	UTigerRangedWeaponComponent_GetCurrentFireSettings_Params params{};

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

UTigerRangedWeaponComponent* ATigerCharacter::GetRangedWeaponComponent()
{
	if (!this)
		return nullptr;

	return *reinterpret_cast<UTigerRangedWeaponComponent**>(reinterpret_cast<uint64_t>(this) + 0x4C8);
}

ETigerCharacterType ATigerCharacter::GetCharacterType()
{
	if (!this)
		return ETigerCharacterType::ETigerCharacterType_MAX;

	return *reinterpret_cast<ETigerCharacterType*>(reinterpret_cast<uint64_t>(this) + 0x7A6);
}

float ATigerCharacter::GetCurrentHealth()
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Tiger.TigerCharacter.GetCurrentHealth");

	struct ATigerCharacter_GetCurrentHealth_Params
	{
	public:
		float ReturnValue;                                             // 0x0000(0x0004)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	ATigerCharacter_GetCurrentHealth_Params params{};

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

float ATigerCharacter::GetCurrentShield()
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Tiger.TigerCharacter.GetCurrentShield");

	struct ATigerCharacter_GetCurrentShield_Params
	{
	public:
		float ReturnValue;                                             // 0x0000(0x0004)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	ATigerCharacter_GetCurrentShield_Params params{};

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

float ATigerCharacter::GetMaxHealth()
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Tiger.TigerCharacter.GetMaxHealth");

	struct ATigerCharacter_GetMaxHealth_Params
	{
	public:
		float ReturnValue;                                             // 0x0000(0x0004)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	ATigerCharacter_GetMaxHealth_Params params{};

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

float ATigerCharacter::GetMaxShield()
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Tiger.TigerCharacter.GetMaxShield");

	struct ATigerCharacter_GetMaxShield_Params
	{
	public:
		float ReturnValue;                                             // 0x0000(0x0004)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	ATigerCharacter_GetMaxShield_Params params{};

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

bool ATigerCharacter::IsDead()
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Tiger.TigerCharacter.IsDead");

	struct ATigerCharacter_IsDead_Params
	{
	public:
		bool ReturnValue;                                             // 0x0000(0x0001)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	ATigerCharacter_IsDead_Params params{};

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

bool ATigerCharacter::IsDowned()
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Tiger.TigerCharacter.IsDowned");

	struct ATigerCharacter_IsDowned_Params
	{
	public:
		bool ReturnValue;                                             // 0x0000(0x0001)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	ATigerCharacter_IsDowned_Params params{};

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

APlayerController* ULocalPlayer::GetPlayerController()
{
	if (!this)
		return nullptr;

	return *reinterpret_cast<APlayerController**>(reinterpret_cast<uint64_t>(this) + 0x30);
}

ULocalPlayer* UGameInstance::GetLocalPlayers()
{
	if (!this)
		return nullptr;

	uintptr_t LocalPlayerArray = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uint64_t>(this) + 0x38);
	if (!LocalPlayerArray)
		return nullptr;

	return *reinterpret_cast<ULocalPlayer**>(LocalPlayerArray);
}

ATigerCharacter* ULevel::GetInstigator(int Index)
{
	if (!this)
		return nullptr;

	uint64_t Actor = *reinterpret_cast<uint64_t*>(reinterpret_cast<uint64_t>(this) + 0x98);
	if (!Actor)
		return nullptr;

	uint64_t Pawn = *reinterpret_cast<uint64_t*>(Actor + Index * sizeof(uint64_t));
	if (Pawn == 0x00 || !Pawn)
		return nullptr;

	return reinterpret_cast<ATigerCharacter*>(Pawn);
}

TArray<AActor*> ULevel::GetActors()
{
	if (!this)
		return TArray<AActor*>();

	return *reinterpret_cast<TArray<AActor*>*>(reinterpret_cast<uint64_t>(this) + 0x98);
}

int32_t ULevel::MaxPacket()
{
	if (!this)
		return 0;

	return *reinterpret_cast<int32_t*>(reinterpret_cast<uint64_t>(this) + 0xA0);
}

void UEngine::FreeMemory(uint64_t AllocatedMemory)
{
	static uintptr_t FreeMemoryAddress = 0;
	if (!FreeMemoryAddress)
	{
		FreeMemoryAddress = Signature("E8 ? ? ? ? 48 8B 5C 24 ? 48 8B C7 48 83 C4 20").Instruction(1).Add(5).GetPointer();
		if (!FreeMemoryAddress)
			return;
	}

	reinterpret_cast<void(__fastcall*)(uintptr_t)>(FreeMemoryAddress)(AllocatedMemory);
}

ULevel* UWorld::GetPersistentLevel()
{
	if (!this)
		return nullptr;

	return *reinterpret_cast<ULevel**>(reinterpret_cast<uint64_t>(this) + 0x30);
}

UGameInstance* UWorld::GetOwningGameInstance()
{
	if (!this)
		return nullptr;

	return *reinterpret_cast<UGameInstance**>(reinterpret_cast<uint64_t>(this) + 0x180);
}

UWorld* UWorld::GetWorld()
{
	if (!GWorld)
		return nullptr;
	else
		return *GWorld;
}

bool UKismetSystemLibrary::IsValid(class UObject* Object)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.KismetSystemLibrary.IsValid");

	struct UKismetSystemLibrary_IsValid_Params
	{
	public:
		class UObject* Object;                                        // 0x0000(0x0008)  (ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		bool ReturnValue;                                             // 0x0008(0x0001)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	UKismetSystemLibrary_IsValid_Params params{};
	params.Object = Object;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

FString UKismetSystemLibrary::GetObjectName(class UObject* Object)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.KismetSystemLibrary.GetObjectName");

	struct UKismetSystemLibrary_GetObjectName_Params
	{
	public:
		class UObject* Object;                                                 // 0x0000(0x0008)  (ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		class FString ReturnValue;                                             // 0x0008(0x0010)  (Parm, OutParm, ZeroConstructor, ReturnParm, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	UKismetSystemLibrary_GetObjectName_Params params{};
	params.Object = Object;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

float UKismetMathLibrary::Vector_DistanceSquared(const struct FVector& V1, const struct FVector& V2)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.KismetMathLibrary.Vector_DistanceSquared");

	struct UKismetMathLibrary_Vector_DistanceSquared_Params
	{
	public:
		struct FVector                                             v1;                                                      // 0x0000(0x000C)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		struct FVector                                             v2;                                                      // 0x000C(0x000C)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		float                                                      ReturnValue;                                             // 0x0018(0x0004)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	UKismetMathLibrary_Vector_DistanceSquared_Params params{};
	params.v1 = V1;
	params.v2 = V2;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

float UKismetMathLibrary::Vector_Distance(const FVector& v1, const FVector& v2)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.KismetMathLibrary.Vector_Distance");

	struct UKismetMathLibrary_Vector_Distance_Params
	{
	public:
		struct FVector                                             v1;                                                      // 0x0000(0x000C)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		struct FVector                                             v2;                                                      // 0x000C(0x000C)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		float                                                      ReturnValue;                                             // 0x0018(0x0004)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	UKismetMathLibrary_Vector_Distance_Params params{};
	params.v1 = v1;
	params.v2 = v2;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

FRotator UKismetMathLibrary::FindLookAtRotation(const FVector& Start, const FVector& Target)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.KismetMathLibrary.FindLookAtRotation");

	struct UKismetMathLibrary_FindLookAtRotation_Params
	{
	public:
		struct FVector                                             Start;                                                   // 0x0000(0x000C)  (ConstParm, Parm, OutParm, ZeroConstructor, ReferenceParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		struct FVector                                             Target;                                                  // 0x000C(0x000C)  (ConstParm, Parm, OutParm, ZeroConstructor, ReferenceParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		struct FRotator                                            ReturnValue;                                             // 0x0018(0x000C)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
	};

	UKismetMathLibrary_FindLookAtRotation_Params params{};
	params.Start = Start;
	params.Target = Target;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

FRotator UKismetMathLibrary::Conv_VectorToRotator(const FVector& InVec)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.KismetMathLibrary.Conv_VectorToRotator");

	struct UKismetMathLibrary_Conv_VectorToRotator_Params
	{
	public:
		struct FVector                                             InVec;                                                   // 0x0000(0x000C)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		struct FRotator                                            ReturnValue;                                             // 0x000C(0x000C)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
	};

	UKismetMathLibrary_Conv_VectorToRotator_Params params{};
	params.InVec = InVec;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}

FVector UKismetMathLibrary::Conv_RotatorToVector(const FRotator& InRot)
{
	static UFunction* fn = nullptr;
	if (!fn)
		fn = UObject::FindObject<UFunction>("Function Engine.KismetMathLibrary.Conv_RotatorToVector");

	struct UKismetMathLibrary_Conv_RotatorToVector_Params
	{
	public:
		struct FRotator                                            InRot;                                                   // 0x0000(0x000C)  (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
		struct FVector                                             ReturnValue;                                             // 0x000C(0x000C)  (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	UKismetMathLibrary_Conv_RotatorToVector_Params params{};
	params.InRot = InRot;

	auto flags = fn->FunctionFlags;
	UObject::ProcessEvent(fn, &params);
	fn->FunctionFlags = flags;

	return params.ReturnValue;
}