#pragma once

#include "UECore.h"

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

bool InitSDK(uintptr_t gObjectsOffset, uintptr_t gNamesOffset, uintptr_t gWorldOffset);
bool InitSDK();

enum class ETigerOutlineMode : uint8_t
{
	HeightenedSenses = 0,
	Lure = 1,
	EnemyPlayer = 2,
	ScoutingFamiliarsGroupMember = 3,
	ScoutingFamiliars = 4,
	BloodHuntedHSReveal = 5,
	BloodHunted = 6,
	CapturingHaven = 7,
	CharmingUs = 8,
	GroupMember = 9,
	ExtendedGroupMember = 10,
	LocalPlayer = 11,
	Count = 12,
	None = 13,
	MAX = 14
};

enum class ETigerCharacterType : uint8_t
{
	Player = 0,
	Npc = 1,
	TutorialTrainer = 2,
	PlayerBot = 3,
	TargetDummy = 4,
	Count = 5,
	ETigerCharacterType_MAX = 6,
};

enum class ETigerInventoryItemType : uint8_t
{
	TigerInventoryItem_Firearm = 0,
	TigerInventoryItem_Melee = 1,
	TigerInventoryItem_Unarmed = 2,
	TigerInventoryItem_Outfit = 3,
	TigerInventoryItem_Amulet = 4,
	TigerInventoryItem_Artifact = 5,
	TigerInventoryItem_BodyArmor = 6,
	TigerInventoryItem_Relic = 7,
	TigerInventoryItem_Ammo = 8,
	TigerInventoryItem_CombinedAmmo = 9,
	TigerInventoryItem_Consumable = 10,
	TigerInventoryItem_Mod = 11,
	TigerInventoryItem_Collectible = 12,
	TigerInventoryItem_Key = 13,
	TigerInventoryItem_Size = 14,
	TigerInventoryItem_MAX = 15
};

enum class ETigerItemRarity : uint8_t
{
	TigerItemRarity_Common = 0,
	TigerItemRarity_Uncommon = 1,
	TigerItemRarity_Rare = 2,
	TigerItemRarity_Epic = 3,
	TigerItemRarity_Legendary = 4,
	TigerItemRarity_Size = 5,
	TigerItemRarity_MAX = 6,
};

enum class ETigerWeaponFireMode : uint8_t
{
	Single = 0,
	Burst = 1,
	Automatic = 2,
	Count = 3,
	MAX = 4
};

enum class ETigerSpreadIncreaseMode : uint8_t
{
	Add = 0,
	Multiply = 1,
	MAX = 2
};

enum class EStereoscopicPass
{
	eSSP_FULL,
	eSSP_LEFT_EYE,
	eSSP_RIGHT_EYE,
	eSSP_LEFT_EYE_SIDE,
	eSSP_RIGHT_EYE_SIDE,
};

enum class EInputEvent
{
	IE_Pressed = 0,
	IE_Released = 1,
	IE_Repeat = 2,
	IE_DoubleClick = 3,
	IE_Axis = 4,
	IE_MAX = 5,
};

struct FMinimalViewInfo
{
public:
	struct FVector                                             Location;                                                // 0x0000(0x000C) Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	struct FVector											   Rotation;                                                // 0x000C(0x000C) Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic
	float                                                      FOV;                                                     // 0x0018(0x0004) Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      DesiredFOV;                                              // 0x001C(0x0004) ZeroConstructor, Transient, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      OrthoWidth;                                              // 0x0020(0x0004) Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      OrthoNearClipPlane;                                      // 0x0024(0x0004) Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, Interp, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      OrthoFarClipPlane;                                       // 0x0028(0x0004) Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, Interp, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      AspectRatio;                                             // 0x002C(0x0004) Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	bool                                                       bConstrainAspectRatio : 1;                               // 0x0030(0x0001) BIT_FIELD Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	bool                                                       bUseFieldOfViewForLOD : 1;                               // 0x0030(0x0001) BIT_FIELD Edit, BlueprintVisible, NoDestructor, AdvancedDisplay, HasGetValueTypeHash, NativeAccessSpecifierPublic
	unsigned char                                              UnknownData_VOZJ[0x3];                                   // 0x0031(0x0003) MISSED OFFSET (FIX SPACE BETWEEN PREVIOUS PROPERTY)
	unsigned char                                              UnknownData_VOZK[0x1];                                   // 0x0034(0x0001) Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	unsigned char                                              UnknownData_MA8F[0x3];                                   // 0x0035(0x0003) MISSED OFFSET (FIX SPACE BETWEEN PREVIOUS PROPERTY)
	float                                                      PostProcessBlendWeight;                                  // 0x0038(0x0004) BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	unsigned char                                              UnknownData_308J[0x4];                                   // 0x003C(0x0004) MISSED OFFSET (FIX SPACE BETWEEN PREVIOUS PROPERTY)
	unsigned char                                              UnknownData_308K[0x0550];                                // 0x0040(0x0550) BlueprintVisible, NativeAccessSpecifierPublic
	struct FVector2D                                           OffCenterProjectionOffset;                               // 0x0590(0x0008) Edit, BlueprintVisible, ZeroConstructor, DisableEditOnTemplate, Transient, EditConst, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	unsigned char                                              UnknownData_DENC[0x48];                                  // 0x0598(0x0048) MISSED OFFSET (PADDING)
};

struct FTigerSpreadModifier
{
public:
	ETigerSpreadIncreaseMode                                   Mode;                                                    // 0x0000(0x0001) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	unsigned char                                              UnknownData_VUHK[0x3];                                   // 0x0001(0x0003) MISSED OFFSET (FIX SPACE BETWEEN PREVIOUS PROPERTY)
	float                                                      Speed;                                                   // 0x0004(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      Amount;                                                  // 0x0008(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
};

struct FTigerFireSettings
{
public:
	float                                                      BulletDamage;                                            // 0x0000(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	ETigerWeaponFireMode                                       FireMode;                                                // 0x0004(0x0001) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	unsigned char                                              UnknownData_9TY8[0x3];                                   // 0x0005(0x0003) MISSED OFFSET (FIX SPACE BETWEEN PREVIOUS PROPERTY)
	int32_t                                                    BurstCount;                                              // 0x0008(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      ShotsFiredPerSecondInBurst;                              // 0x000C(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	int32_t                                                    BulletsFiredSimultaneously;                              // 0x0010(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      ShotsFiredPerSecond;                                     // 0x0014(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	class UClass* AreaEffect;                                              // 0x0018(0x0008) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      FireStickyTime;                                          // 0x0020(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      VerticalRecoilAmount;                                    // 0x0024(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      HorizontalRecoilAmount;                                  // 0x0028(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      StartingSpread;                                          // 0x002C(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	struct FTigerSpreadModifier                                SpreadModifiers[0x7];                                    // 0x0030(0x0054) Edit, NoDestructor, NativeAccessSpecifierPublic
	float                                                      TimeUntilSpreadDecreases;                                // 0x0084(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      BaseMovementSpeedMultiplier;                             // 0x0088(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      AimingMovementSpeedMultiplier;                           // 0x008C(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      HipFireMovementSpeedMultiplier;                          // 0x0090(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	unsigned char                                              UnknownData_136W[0x4];                                   // 0x0094(0x0004) MISSED OFFSET (FIX SPACE BETWEEN PREVIOUS PROPERTY)
	class UCurveFloat* SpreadIncreaseCurve;                                     // 0x0098(0x0008) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	class UCurveFloat* SpreadDecreaseCurve;                                     // 0x00A0(0x0008) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	class UCurveFloat* FireRateIncreaseCurve;                                   // 0x00A8(0x0008) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	class UCurveFloat* FireRateDecreaseCurve;                                   // 0x00B0(0x0008) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	float                                                      MasqueradeNoiseEventRadius;                              // 0x00B8(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	bool                                                       ReverseSpreadChange;                                     // 0x00BC(0x0001) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	bool                                                       bPierceCivilians;                                        // 0x00BD(0x0001) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	bool                                                       bUsesScope;                                              // 0x00BE(0x0001) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	unsigned char                                              UnknownData_1BZ4[0x1];                                   // 0x00BF(0x0001) MISSED OFFSET (FIX SPACE BETWEEN PREVIOUS PROPERTY)
	int32_t                                                    ResistanceStart;                                         // 0x00C0(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	int32_t                                                    ResistanceEnd;                                           // 0x00C4(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	int32_t                                                    ResistanceStrength;                                      // 0x00C8(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	int32_t                                                    TriggerVibrationFeedbackStrength;                        // 0x00CC(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	int32_t                                                    ADSResistanceStart;                                      // 0x00D0(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	int32_t                                                    ADSResistanceStrength;                                   // 0x00D4(0x0004) Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
};

struct FCameraCacheEntry
{
public:
	float                                                      Timestamp;                                               // 0x0000(0x0004) ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
	unsigned char                                              UnknownData_L39P[0xC];                                   // 0x0004(0x000C) MISSED OFFSET (FIX SPACE BETWEEN PREVIOUS PROPERTY)
	struct FMinimalViewInfo                                    POV;                                                     // 0x0010(0x05E0) NativeAccessSpecifierPublic
};

class UCurveFloat : public UObject
{
public:
	union
	{
		DEFINE_MEMBER_N(BOOL, bIsEventCurve, 0xB0);
	};
};

class UTigerRangedWeapon : public UObject
{
public:
	union
	{
		DEFINE_MEMBER_N(UCurveFloat*, VerticalRecoilCurve, 0x108);
		DEFINE_MEMBER_N(UCurveFloat*, HorizontalRecoilCurve, 0x110);
		DEFINE_MEMBER_N(UCurveFloat*, VerticalRecoilMultiplierCurveOverRecoil, 0x118);
		DEFINE_MEMBER_N(UCurveFloat*, HorizontalRecoilMultiplierCurveOverRecoil, 0x120);
	};
};

class UTigerRangedWeaponComponent : public UObject
{
public:
	union
	{
		DEFINE_MEMBER_N(UTigerRangedWeapon*, EquippedWeaponType, 0x170);
	};

public:
	UTigerRangedWeapon* GetEquippedWeaponType();
	std::optional<FTigerFireSettings> GetCurrentFireSettings();
};

class USceneComponent : public UObject
{
public:
	union
	{
		DEFINE_MEMBER_N(FVector, RelativeLocation, 0x11C);
		DEFINE_MEMBER_N(FVector, ComponentVelocity, 0x140);
	};
public:
	FVector GetSocketLocation(const class FName& InSocketName);
	FVector GetRelativeLocation();
	FVector GetComponentVelocity();
};

class UMeshComponent : public USceneComponent
{
public:
	union
	{
		DEFINE_MEMBER_N(INT, MinDrawDistance, 0x200);
		DEFINE_MEMBER_N(INT, LDMaxDrawDistance, 0x204);
	};
};

class USkinnedMeshComponent : public UMeshComponent
{
public:
	FName GetBoneName(int32_t BoneIndex);
	int32_t GetBoneIndex(const class FName& BoneName);
};

class USkeletalMeshComponent : public USkinnedMeshComponent
{
public:
	FVector GetBoneWorldPos(const int32_t& BoneIndex) const;
};

class UTigerOutlineComponent : public UObject
{
public:
	union
	{
		DEFINE_MEMBER_N(ETigerOutlineMode, CurrentMode, 0xB0);
	};

public:
	void DeactivateOutline(ETigerOutlineMode InMode);
	void ActivateOutline(ETigerOutlineMode InMode);
};

class AActor : public UObject
{
public:
	void GetActorBounds(bool bOnlyCollidingComponents, struct FVector* Origin, struct FVector* BoxExtent, bool bIncludeFromChildActors);
	float GetDistanceTo(class AActor* OtherActor);
	FVector GetActorLocation();
	FRotator GetActorRotation();
	USceneComponent* GetRootComponent();
};

class UTigerItemAsset : public UObject
{
public:
	union
	{
		DEFINE_MEMBER_N(ETigerInventoryItemType, ItemType, 0x40);
		DEFINE_MEMBER_N(ETigerItemRarity, ItemRarity, 0x41);
		DEFINE_MEMBER_N(FText, Name, 0x48);
	};

public:
	uint8_t GetItemType();
	uint8_t GetItemRarity();
	FText GetItemName();
};

class ATigerItemWorldRepresentation : public AActor
{
public:
	union
	{
		DEFINE_MEMBER_N(UTigerItemAsset*, ItemType, 0x240);
	};

public:
	UTigerItemAsset* GetItemType();
};

class APlayerState : public AActor
{
public:
	FString GetPlayerName();
	bool IsOfSameTeam(APlayerState* OtherState);
};

class APlayerCameraManager : public AActor
{
public:
	union
	{
		DEFINE_MEMBER_N(FCameraCacheEntry, CameraCachePrivate, 0x1AB0);
	};
public:
	FVector GetCameraLocation();
	FRotator GetCameraRotation();
	FCameraCacheEntry GetCameraCachePrivate();
};

class APawn : public AActor
{
public:
	union
	{
		DEFINE_MEMBER_N(APlayerState*, PlayerState, 0x248);
	};
};

class ACharacter : public APawn
{
public:
	union
	{
		DEFINE_MEMBER_N(USkeletalMeshComponent*, Mesh, 0x288);
	};
};

class ATigerCharacter : public ACharacter
{
public:
	union
	{
		DEFINE_MEMBER_N(UTigerRangedWeaponComponent*, RangedWeaponComponent, 0x4C8);
		DEFINE_MEMBER_N(UTigerOutlineComponent*, OutlineComponent, 0x4F0);
		DEFINE_MEMBER_N(ETigerCharacterType, CharacterType, 0x7A6);
	};

public:
	APlayerState* GetPlayerState();
	USkeletalMeshComponent* GetMesh();
	UTigerOutlineComponent* GetOutlineComponent();
	UTigerRangedWeaponComponent* GetRangedWeaponComponent();
	ETigerCharacterType GetCharacterType();

public:
	float GetCurrentHealth();
	float GetCurrentShield();
	float GetMaxHealth();
	float GetMaxShield();
	bool IsDead();
	bool IsDowned();
};

class APlayerController : public AActor
{
public:
	union
	{
		DEFINE_MEMBER_N(APawn*, AcknowledgedPawn, 0x2A8);
		DEFINE_MEMBER_N(APlayerCameraManager*, PlayerCameraManager, 0x2C0);
	};

public:
	ATigerCharacter* GetAcknowledgedPawn();
	APlayerCameraManager* GetPlayerCameraManager();

public:
	void SetControlRotation(const struct FRotator& NewRotation);
	void GetViewportSize(int32_t* SizeX, int32_t* SizeY);
	bool LineOfSightTo(class AActor* Other, const struct FVector& ViewPoint, bool bAlternateChecks = false);
	bool ProjectWorldLocationToScreen(const struct FVector& WorldLocation, struct FVector2D* ScreenLocation, bool bPlayerViewportRelative);
	bool IsInputKeyDown(const struct FKey& Key);
};

class ULocalPlayer : public UObject
{
public:
	union
	{
		DEFINE_MEMBER_N(APlayerController*, PlayerController, 0x30);
	};

public:
	APlayerController* GetPlayerController();
};

class UGameInstance : public UObject
{
public:
	union
	{
		DEFINE_MEMBER_N(TArray<class ULocalPlayer*>, LocalPlayers, 0x38);
	};

public:
	ULocalPlayer* GetLocalPlayers();
};

class ULevel : public UObject
{
public:
	ATigerCharacter* GetInstigator(int Index);
	TArray<AActor*> GetActors();
	int32_t MaxPacket();
};

class UEngine : public UObject
{
public:
	static void FreeMemory(uint64_t AllocatedMemory);
};

class UWorld : public UObject
{
public:
	static inline class UWorld** GWorld = nullptr;

public:
	union
	{
		DEFINE_MEMBER_N(ULevel*, PersistentLevel, 0x30);
		DEFINE_MEMBER_N(UGameInstance*, OwningGameInstance, 0x180);
	};

public:
	ULevel* GetPersistentLevel();
	UGameInstance* GetOwningGameInstance();

public:
	static UWorld* GetWorld();
};

class UKismetSystemLibrary : public UObject
{
public:
	bool IsValid(class UObject* Object);
	FString GetObjectName(class UObject* Object);

public:
	static inline UClass* StaticClass()
	{
		static UClass* ptr = nullptr;
		if (!ptr)
			ptr = UObject::FindClass("Class Engine.KismetSystemLibrary");
		return ptr;
	}
};

class UKismetMathLibrary : public UObject
{
public:
	float Vector_DistanceSquared(const struct FVector& V1, const struct FVector& V2);
	float Vector_Distance(const struct FVector& v1, const struct FVector& v2);
	FRotator FindLookAtRotation(const struct FVector& Start, const struct FVector& Target);
	FRotator Conv_VectorToRotator(const struct FVector& InVec);
	FVector Conv_RotatorToVector(const struct FRotator& InRot);

public:
	static inline UClass* StaticClass()
	{
		static UClass* ptr = nullptr;
		if (!ptr)
			ptr = UObject::FindClass("Class Engine.KismetMathLibrary");
		return ptr;
	}
};