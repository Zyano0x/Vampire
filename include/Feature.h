#pragma once

namespace Z
{
	class Game
	{
	public:
		explicit Game();
		~Game() noexcept;

	public:
		void Initialize();
		static void Visual();
		static void Aimbot();
		static void Misc();

		static float CalculateHeadCircleRadius(float Distance);
		static std::string GetItemRarity(uint8_t Rarity);
		static ATigerCharacter* GetBestPlayer();
		static FVector GetAimWorldLocation(APlayerController* LocalController, ATigerCharacter* Player);
		static FVector AimbotPrediction(float BulletVelocity, float BulletGravity, float TargetDistance, FVector TargetPosition, FVector TargetVelocity);
		static FVector CalcAngle(const FVector& Src, const FVector& Dst);

	private:
		static inline bool m_IsTeammate = false;
		static inline int m_ScreenWidth = 0;
		static inline int m_ScreenHeight = 0;
		static inline ImVec4 m_Color{};
		static inline FVector BestTarget{};
		static inline FVector OriginalLocation{ 0.0f, 0.0f, 0.0f };
		static inline FVector OriginalRotation{ 0.0f, 0.0f, 0.0f };
		static inline FVector m_Head{};
		static inline FVector m_Foot{};
		static inline UKismetSystemLibrary* p_UKSystemLib = nullptr;
		static inline UKismetMathLibrary* p_UKMathLib = nullptr;
		static inline APlayerController* LocalPlayerController = nullptr;
		static inline ATigerCharacter* LocalCharacter = nullptr;

	private:
		typedef void(*GetViewPoint)(ULocalPlayer*, FMinimalViewInfo*, EStereoscopicPass);
		typedef void(*GetPlayerViewPoint)(APlayerController*, FVector*, FVector*);

		GetViewPoint GetViewPointStub = 0;
		GetPlayerViewPoint GetPlayerViewPointStub = 0;

	private:
		static void GetViewPointHook(ULocalPlayer* LocalPlayer, FMinimalViewInfo* OutViewInfo, EStereoscopicPass StereoPass);
		static void GetPlayerViewPointHook(APlayerController* PlayerController, FVector* Location, FVector* Rotation);
	};
	inline std::unique_ptr<Game> g_Game;
}