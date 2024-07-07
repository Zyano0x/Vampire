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
		void Visual();
		void Aimbot();
		void Misc();

		float CalculateHeadCircleRadius(float Distance);
		std::string GetItemRarity(uint8_t Rarity);
		ATigerCharacter* GetBestPlayer();
		FVector GetAimWorldLocation(APlayerController* LocalController, ATigerCharacter* Player);
		FVector AimbotPrediction(float BulletVelocity, float BulletGravity, float TargetDistance, FVector TargetPosition, FVector TargetVelocity);
		FVector CalcAngle(const FVector& Src, const FVector& Dst);

	private:
		bool m_IsTeammate = false;
		int m_ScreenWidth = 0;
		int m_ScreenHeight = 0;
		ImVec4 m_Color{};
		FVector BestTarget{};
		FVector OriginalLocation{ 0.0f, 0.0f, 0.0f };
		FVector OriginalRotation{ 0.0f, 0.0f, 0.0f };
		FVector m_Head{};
		FVector m_Foot{};
		UKismetSystemLibrary* p_UKSystemLib = nullptr;
		UKismetMathLibrary* p_UKMathLib = nullptr;
		APlayerController* LocalPlayerController = nullptr;
		ATigerCharacter* LocalCharacter = nullptr;

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