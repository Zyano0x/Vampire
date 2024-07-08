#include "pch.h"

namespace Z
{
#pragma region GAME

	Game::Game()
	{
		InitSDK();

		p_UKSystemLib = reinterpret_cast<UKismetSystemLibrary*>(UKismetSystemLibrary::StaticClass());
		p_UKMathLib = reinterpret_cast<UKismetMathLibrary*>(UKismetMathLibrary::StaticClass());
	}

	Game::~Game() {}

	void Game::GetViewPointHook(ULocalPlayer* LocalPlayer, FMinimalViewInfo* OutViewInfo, EStereoscopicPass StereoPass)
	{
		g_Game->GetViewPointStub(LocalPlayer, OutViewInfo, StereoPass);

		if (Settings[AIM_MODE].Value.iValue == 1 && Settings[AIM_ENABLED].Value.bValue)
		{
			if (Engine::GetKeyPress(VK_LBUTTON, true))
			{
				OutViewInfo->Location = OriginalLocation;
				OutViewInfo->Rotation = OriginalRotation;
			}
		}
	}

	void Game::GetPlayerViewPointHook(APlayerController* PlayerController, FVector* Location, FVector* Rotation)
	{
		g_Game->GetPlayerViewPointStub(PlayerController, Location, Rotation);

		OriginalLocation = *Location;
		OriginalRotation = *Rotation;

		if (Settings[AIM_MODE].Value.iValue == 1 && Settings[AIM_ENABLED].Value.bValue)
		{
			if (Engine::GetKeyPress(VK_LBUTTON, true))
			{
				if ((!isnan(g_Game->BestTarget.X) || !isnan(g_Game->BestTarget.Y)) && g_Game->BestTarget.IsValid())
				{
					*Rotation = g_Game->BestTarget;
				}
			}
		}
	}

	// UWorld - 48 8B 1D ? ? ? ? 48 85 DB 74 ? 41 B0 | Instruction 3 | Add 7

	void Game::Initialize()
	{
		uint64_t GetViewPointAddr = Signature(std::string(skCrypt("4C 8B DC 49 89 5B ? 55 56 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 41 0F 29 73"))).GetPointer();
		uint64_t GetPlayerViewPointAddr = Signature(std::string(skCrypt("48 89 5C 24 ? 48 89 74 24 ? 55 41 56 41 57 48 8B EC 48 83 EC ? 48 8B F2"))).GetPointer();
		if (GetViewPointAddr && GetPlayerViewPointAddr)
		{
			Hooking::CreateHook(reinterpret_cast<LPVOID>(GetViewPointAddr), &GetViewPointHook, reinterpret_cast<LPVOID*>(&GetViewPointStub));
			Hooking::CreateHook(reinterpret_cast<LPVOID>(GetPlayerViewPointAddr), &GetPlayerViewPointHook, reinterpret_cast<LPVOID*>(&GetPlayerViewPointStub));
		}

		//g_Console->cLog(skCrypt("GObject: 0x%llX\nGNames: 0x%llX\nGWorld: 0x%llX\n"), Console::ECOLOR_DARK_GRAY, UObject::GObjects, FName::GNames, UWorld::GWorld);
	}

	void Game::Visual()
	{
		if (!Settings[ESP_ENABLED].Value.bValue)
			return;

		UWorld* GWorld = UWorld::GetWorld();
		if (!p_UKSystemLib->IsValid(GWorld))
			return;

		UGameInstance* OwningGameInstance = GWorld->GetOwningGameInstance();
		if (!p_UKSystemLib->IsValid(OwningGameInstance))
			return;

		ULocalPlayer* LocalPlayer = OwningGameInstance->GetLocalPlayers();
		if (!LocalPlayer)
			return;

		LocalPlayerController = LocalPlayer->GetPlayerController();
		if (!p_UKSystemLib->IsValid(LocalPlayerController))
			return;

		LocalCharacter = static_cast<ATigerCharacter*>(LocalPlayerController->GetAcknowledgedPawn());
		if (!p_UKSystemLib->IsValid(LocalCharacter))
			return;

		APlayerState* LocalPlayerState = LocalCharacter->GetPlayerState();
		if (!p_UKSystemLib->IsValid(LocalPlayerState))
			return;

		LocalPlayerController->GetViewportSize(&m_ScreenWidth, &m_ScreenHeight);

		std::vector<ATigerCharacter*> Players{};
		std::vector<ATigerItemWorldRepresentation*> Items{};
		int32_t Count = GWorld->GetPersistentLevel()->MaxPacket();
		TArray<AActor*> Actors = GWorld->GetPersistentLevel()->GetActors();
		for (int i = 0; i < Count; ++i)
		{
			AActor* Actor = Actors[i];
			if (!Actor)
				continue;

			FString ObjectName = p_UKSystemLib->GetObjectName(Actor);
			if (!ObjectName.IsValid())
				continue;

			if (wcsstr(ObjectName.wc_str(), skCrypt(L"TBP_ElysiumPlayer_C")) || wcsstr(ObjectName.wc_str(), skCrypt(L"TBP_Player_C")) ||
				wcsstr(ObjectName.wc_str(), skCrypt(L"TBP_NPC_Primogen_C")) || wcsstr(ObjectName.wc_str(), skCrypt(L"TBP_NPC_C")))
			{
				Players.push_back(static_cast<ATigerCharacter*>(Actor));
			}

			if (wcsstr(ObjectName.wc_str(), skCrypt(L"TBP_Item_C")))
			{
				Items.push_back(static_cast<ATigerItemWorldRepresentation*>(Actor));
			}

			UEngine::FreeMemory(reinterpret_cast<uint64_t>(ObjectName.wc_str()));
		}

		for (ATigerCharacter* Player : Players)
		{
			if (!p_UKSystemLib->IsValid(Player))
				continue;

			if (Player == LocalCharacter)
				continue;

			APlayerState* PlayerState = Player->GetPlayerState();
			if (!p_UKSystemLib->IsValid(PlayerState))
				continue;

			if (LocalPlayerState->IsOfSameTeam(PlayerState))
				m_IsTeammate = true;
			else
				m_IsTeammate = false;

			USkeletalMeshComponent* MeshComponent = Player->GetMesh();
			if (!p_UKSystemLib->IsValid(MeshComponent))
				continue;

			if (!m_IsTeammate && !Settings[ESP_ENEMY].Value.bValue)
				continue;

			// Don't draw the teammate if esp team is off
			if (m_IsTeammate && !Settings[ESP_FRIENDLY].Value.bValue)
				continue;

			if (!m_IsTeammate)
			{
				VEC4CPY(Settings[ESP_ENEMY_COLOR].Value.v4Value, m_Color);

				if (LocalPlayerController->LineOfSightTo(Player, { 0.0f, 0.0f, 0.0f }, false))
					VEC4CPY(Settings[ESP_VISIBLE_COLOR].Value.v4Value, m_Color);
			}
			else
				VEC4CPY(Settings[ESP_FRIENDLY_COLOR].Value.v4Value, m_Color);

			if (!Player->IsDead())
			{
				// m_Head = MeshComponent->GetBoneWorldPos(28);
				// m_Foot = MeshComponent->GetBoneWorldPos(0);
				m_Head = MeshComponent->GetSocketLocation(MeshComponent->GetBoneName(MeshComponent->GetBoneIndex(FName("Head"))));
				m_Foot = MeshComponent->GetSocketLocation(MeshComponent->GetBoneName(MeshComponent->GetBoneIndex(FName("Root"))));
				// m_Head = MeshComponent->GetSocketLocation(MeshComponent->GetBoneName(28));
				// m_Foot = MeshComponent->GetSocketLocation(MeshComponent->GetBoneName(0));

				FVector Location, Extend;
				Player->GetActorBounds(true, &Location, &Extend, false);

				FVector2D HeadPos, FootPos;
				if (LocalPlayerController->ProjectWorldLocationToScreen({ Location.X, Location.Y, Location.Z - Extend.Z }, &FootPos, false) &&
					LocalPlayerController->ProjectWorldLocationToScreen({ Location.X, Location.Y, Location.Z + Extend.Z }, &HeadPos, false))
				{
					const float m_Distance = Player->GetDistanceTo(LocalCharacter) / 100.0f;
					const float Height = abs(HeadPos.Y - FootPos.Y) / 1.5f;
					const float Width = Height * 0.6f;

					FVector2D Top = { HeadPos.X - Width * 0.5f, HeadPos.Y };
					FVector2D Bottom = { HeadPos.X + Width * 0.5f, FootPos.Y };

					if (Settings[ESP_NAME].Value.bValue)
					{
						if (Player->GetCharacterType() == ETigerCharacterType::PlayerBot)
						{
							Draw::DrawString(ImGui::GetIO().FontDefault, std::string(skCrypt("BOT")), (Top.X + Bottom.X) / 2, Top.Y - 20, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
						}
						else if (Player->GetCharacterType() == ETigerCharacterType::Npc)
						{
							Draw::DrawString(ImGui::GetIO().FontDefault, std::string(skCrypt("NPC")), (Top.X + Bottom.X) / 2, Top.Y - 20, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
						}
						else if (Player->GetCharacterType() == ETigerCharacterType::Player)
						{
							FString PlayerName = PlayerState->GetPlayerName();
							if (PlayerName.IsValid())
							{
								Draw::DrawString(ImGui::GetIO().FontDefault, PlayerName.ToString(), (Top.X + Bottom.X) / 2, Top.Y - 20, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
							}
						}
					}

					if (Settings[ESP_SKELETON].Value.bValue)
					{
						std::vector<std::pair<const char*, const char*>> SkeletonConnections = {
							{"neck_01", "Head"}, // NECK -> HEAD

							{"Spine_02", "neck_01"},
							{"Spine_01", "Spine_02"},
							{"spine_03", "Spine_01"}, // STOMACH -> CHEST
							{"pelvis", "spine_03"},

							{"clavicle_r", "Spine_02"},
							{"upperarm_r", "clavicle_r"},
							{"lowerarm_r", "upperarm_r"}, // RIGHT ELBOW
							{"hand_r", "lowerarm_r"},

							{"clavicle_l", "Spine_02"},
							{"upperarm_l", "clavicle_l"},
							{"lowerarm_l", "upperarm_l"}, // LEFT ELBOW
							{"hand_l", "lowerarm_l"},

							{"calf_r", "pelvis"},
							{"foot_r", "calf_r"},

							{"calf_l", "pelvis"},
							{"foot_l", "calf_l"},
						};

						FVector2D BoneScreen, PrevBoneScreen, HeadScreen;
						for (const std::pair<const char*, const char*>& Connection : SkeletonConnections)
						{
							auto BONE1 = Connection.first;
							auto BONE2 = Connection.second;

							FVector BoneLoc1 = MeshComponent->GetSocketLocation(MeshComponent->GetBoneName(MeshComponent->GetBoneIndex(FName(BONE1))));
							FVector BoneLoc2 = MeshComponent->GetSocketLocation(MeshComponent->GetBoneName(MeshComponent->GetBoneIndex(FName(BONE2))));

							LocalPlayerController->ProjectWorldLocationToScreen(BoneLoc1, &BoneScreen, false);
							LocalPlayerController->ProjectWorldLocationToScreen(BoneLoc2, &PrevBoneScreen, false);

							if (BoneScreen.IsValid() && PrevBoneScreen.IsValid())
								Draw::DrawLine(BoneScreen.X, BoneScreen.Y, PrevBoneScreen.X, PrevBoneScreen.Y, 1.6f, m_Color);
						}

						float HeadCircleRadius = CalculateHeadCircleRadius(m_Distance);
						LocalPlayerController->ProjectWorldLocationToScreen(m_Head, &HeadScreen, false);

						if (HeadScreen.IsValid())
							Draw::DrawCircleFilled(HeadScreen.X, HeadScreen.Y - 2.5f, HeadCircleRadius, m_Color);
					}

					if (Settings[ESP_SNAP_LINES].Value.bValue)
					{
						Draw::DrawLine(m_ScreenWidth / 2, m_ScreenHeight, FootPos.X, FootPos.Y, 1.5f, m_Color);
					}

					if (Settings[ESP_DISTANCE].Value.bValue)
					{
						Draw::DrawString(ImGui::GetIO().FontDefault, std::string(skCrypt("[")).append(std::to_string((int)m_Distance)).append(std::string(skCrypt(" M]"))), (Top.X + Bottom.X) / 2, Bottom.Y + 5, 15.f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
					}

					if (Settings[ESP_BOX].Value.iValue)
					{
						if (!Player->IsDowned())
						{
							switch (Settings[ESP_BOX].Value.iValue)
							{
							case 1:
								Draw::DrawBox(Top.X, Top.Y, Bottom.X, Bottom.Y, true, m_Color);
								break;
							case 2:
								Draw::DrawCornersBox(Top.X, Top.Y, Bottom.X, Bottom.Y, true, true, m_Color);
								break;
							}
						}
					}

					if (Settings[ESP_HEALTH].Value.bValue)
					{
						float Health = Player->GetCurrentHealth();
						float MaxHealth = Player->GetMaxHealth();
						float Percent = Health * 100.0f / MaxHealth;

						float HWidth = Width / 10;
						if (HWidth < 2.0f)
							HWidth = 2.0f;
						if (HWidth > 3.0f)
							HWidth = 3.0f;

						Draw::VerticalHealthBar(HeadPos.X - (Width / 2.0f) - 8.0f, HeadPos.Y, HWidth, FootPos.Y - HeadPos.Y, Percent, true);
					}

					if (Settings[ESP_ARMOR].Value.bValue)
					{
						float Shield = Player->GetCurrentShield();
						float MaxShield = Player->GetMaxShield();
						float Percent = Shield * 100.0f / MaxShield;

						if (!std::isnan(Percent) && !std::isinf(Percent))
						{
							float HWidth = Width / 10;
							if (HWidth < 2.0f)
								HWidth = 2.0f;
							if (HWidth > 3.0f)
								HWidth = 3.0f;

							Draw::VerticalHealthBar(HeadPos.X + (Width / 2.0f) + 8.0f, HeadPos.Y, HWidth, FootPos.Y - HeadPos.Y, Percent, true);
						}
					}

					if (Settings[ESP_GLOW].Value.bValue)
					{
						UTigerOutlineComponent* OutlineComponent = Player->GetOutlineComponent();
						if (!p_UKSystemLib->IsValid(OutlineComponent))
							continue;

						OutlineComponent->CurrentMode = ETigerOutlineMode::BloodHunted;
						OutlineComponent->ActivateOutline(ETigerOutlineMode::HeightenedSenses);
						OutlineComponent->ActivateOutline(ETigerOutlineMode::BloodHunted);
					}
				}
			}
		}

		for (ATigerItemWorldRepresentation* Item : Items)
		{
			if (!p_UKSystemLib->IsValid(Item))
				continue;

			UTigerItemAsset* ItemType = Item->GetItemType();
			if (!p_UKSystemLib->IsValid(ItemType))
				continue;

			FVector Location = Item->GetActorLocation();
			if (!Location.IsValid())
				continue;

			FVector2D Position;
			if (LocalPlayerController->ProjectWorldLocationToScreen(Location, &Position, false))
			{
				const float m_Distance = Item->GetDistanceTo(LocalCharacter) / 100.0f;
				uint8_t LootType = ItemType->GetItemType();
				uint8_t LootRarity = ItemType->GetItemRarity();
				std::string Rarity = GetItemRarity(LootRarity);
				std::string Name = ItemType->GetItemName().ToString();

				// g_Console->cLog(skCrypt("Name: %s - Type: %d - Rarity: %d\n"), Console::EColor_dark_white, Name.c_str(), LootType, LootRarity);

				switch (LootType)
				{
				case 0: // FIREARM
					if (Settings[ESP_FIREARM].Value.bValue && LootRarity >= Settings[ESP_LOOT].Value.iValue)
					{
						Draw::DrawString(ImGui::GetIO().FontDefault, Rarity.append(Name).append(std::string(skCrypt(" ["))).append(std::to_string((int)m_Distance)).append(std::string(skCrypt(" M]"))), Position.X, Position.Y, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
					}
					break;
				case 1: // MELEE
					if (Settings[ESP_MELEE].Value.bValue && LootRarity >= Settings[ESP_LOOT].Value.iValue)
					{
						Draw::DrawString(ImGui::GetIO().FontDefault, Rarity.append(Name).append(std::string(skCrypt(" ["))).append(std::to_string((int)m_Distance)).append(std::string(skCrypt(" M]"))), Position.X, Position.Y, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
					}
					break;
				case 2: // UNARMED
					if (Settings[ESP_UNARMED].Value.bValue)
					{
						Draw::DrawString(ImGui::GetIO().FontDefault, Rarity.append(Name).append(std::string(skCrypt(" ["))).append(std::to_string((int)m_Distance)).append(std::string(skCrypt(" M]"))), Position.X, Position.Y, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
					}
					break;
				case 3: // OUTFIT
					if (Settings[ESP_OUTFIT].Value.bValue)
					{
						Draw::DrawString(ImGui::GetIO().FontDefault, Rarity.append(Name).append(std::string(skCrypt(" ["))).append(std::to_string((int)m_Distance)).append(std::string(skCrypt(" M]"))), Position.X, Position.Y, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
					}
					break;
				case 4: // AMULET
					if (Settings[ESP_AMULET].Value.bValue)
					{
						Draw::DrawString(ImGui::GetIO().FontDefault, Rarity.append(Name).append(std::string(skCrypt(" ["))).append(std::to_string((int)m_Distance)).append(std::string(skCrypt(" M]"))), Position.X, Position.Y, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
					}
					break;
				case 5: // ARTIFACT
					if (Settings[ESP_ARTIFACT].Value.bValue)
					{
						Draw::DrawString(ImGui::GetIO().FontDefault, Rarity.append(Name).append(std::string(skCrypt(" ["))).append(std::to_string((int)m_Distance)).append(std::string(skCrypt(" M]"))), Position.X, Position.Y, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
					}
					break;
				case 6: // BODYARMOR
					if (Settings[ESP_BODYARMOR].Value.bValue)
					{
						Draw::DrawString(ImGui::GetIO().FontDefault, Rarity.append(Name).append(std::string(skCrypt(" ["))).append(std::to_string((int)m_Distance)).append(std::string(skCrypt(" M]"))), Position.X, Position.Y, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
					}
					break;
				case 7: // RELIC
					if (Settings[ESP_RELIC].Value.bValue)
					{
						Draw::DrawString(ImGui::GetIO().FontDefault, Rarity.append(Name).append(std::string(skCrypt(" ["))).append(std::to_string((int)m_Distance)).append(std::string(skCrypt(" M]"))), Position.X, Position.Y, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
					}
					break;
				case 8: // AMMO
					if (Settings[ESP_AMMO].Value.bValue)
					{
						Draw::DrawString(ImGui::GetIO().FontDefault, Rarity.append(Name).append(std::string(skCrypt(" ["))).append(std::to_string((int)m_Distance)).append(std::string(skCrypt(" M]"))), Position.X, Position.Y, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
					}
					break;
				case 9: // COMBINED AMMO
					if (Settings[ESP_COMBINED_AMMO].Value.bValue)
					{
						Draw::DrawString(ImGui::GetIO().FontDefault, Rarity.append(Name).append(std::string(skCrypt(" ["))).append(std::to_string((int)m_Distance)).append(std::string(skCrypt(" M]"))), Position.X, Position.Y, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
					}
					break;
				case 10: // CONSUMABLE
					if (Settings[ESP_CONSUMABLE].Value.bValue)
					{
						Draw::DrawString(ImGui::GetIO().FontDefault, Rarity.append(Name).append(std::string(skCrypt(" ["))).append(std::to_string((int)m_Distance)).append(std::string(skCrypt(" M]"))), Position.X, Position.Y, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
					}
					break;
				case 11: // MOD
					if (Settings[ESP_MOD].Value.bValue)
					{
						Draw::DrawString(ImGui::GetIO().FontDefault, Rarity.append(Name).append(std::string(skCrypt(" ["))).append(std::to_string((int)m_Distance)).append(std::string(skCrypt(" M]"))), Position.X, Position.Y, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
					}
					break;
				case 12: // CONSUMABLE
					if (Settings[ESP_CONSUMABLE].Value.bValue)
					{
						Draw::DrawString(ImGui::GetIO().FontDefault, Rarity.append(Name).append(std::string(skCrypt(" ["))).append(std::to_string((int)m_Distance)).append(std::string(skCrypt(" M]"))), Position.X, Position.Y, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
					}
					break;
				case 13: // CONSUMABLE
					if (Settings[ESP_CONSUMABLE].Value.bValue)
					{
						Draw::DrawString(ImGui::GetIO().FontDefault, Rarity.append(Name).append(std::string(skCrypt(" ["))).append(std::to_string((int)m_Distance)).append(std::string(skCrypt(" M]"))), Position.X, Position.Y, 15.0f, true, ImVec4(1.f, 1.f, 1.f, 1.f));
					}
					break;
				}
			}
		}
	}

	void Game::Aimbot()
	{
		if (!Settings[AIM_ENABLED].Value.bValue)
			return;

		if (Settings[FOV].Value.bValue)
			Draw::DrawCircle(m_ScreenWidth / 2, m_ScreenHeight / 2, Settings[RADIUS].Value.fValue, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

		if (Settings[AIM_MODE].Value.iValue == 0)
		{
			if (Engine::GetKeyPress(VK_LBUTTON, true))
			{
				ATigerCharacter* Target = GetBestPlayer();
				if (!p_UKSystemLib->IsValid(Target))
					return;

				FVector TargetLocation = GetAimWorldLocation(LocalPlayerController, Target);
				FRotator NewRotation = p_UKMathLib->FindLookAtRotation(LocalPlayerController->GetPlayerCameraManager()->GetCameraLocation(), TargetLocation);
				LocalPlayerController->SetControlRotation(NewRotation);

				//g_Console->cLog(skCrypt("%.3f | %.3f | %.3f\n"), Console::EColor_dark_green, TargetLocation.X, TargetLocation.Y, TargetLocation.Z);
			}
		}
		else if (Settings[AIM_MODE].Value.iValue == 1)
		{
			if (Engine::GetKeyPress(VK_LBUTTON, true))
			{
				ATigerCharacter* Target = GetBestPlayer();
				if (!p_UKSystemLib->IsValid(Target))
					return;

				FVector TargetLocation = GetAimWorldLocation(LocalPlayerController, Target);

				USceneComponent* LocalRootComponent = LocalCharacter->GetRootComponent();
				USceneComponent* TargetRootComponent = Target->GetRootComponent();
				if (!p_UKSystemLib->IsValid(LocalRootComponent) || !p_UKSystemLib->IsValid(TargetRootComponent))
					return;

				FVector LocalRelativeLocation = LocalRootComponent->GetRelativeLocation();
				FVector TargetRelativeLocation = TargetRootComponent->GetRelativeLocation();

				float Distance = LocalRelativeLocation.Distance(TargetRelativeLocation);

				APlayerCameraManager* CameraManager = LocalPlayerController->GetPlayerCameraManager();
				if (!p_UKSystemLib->IsValid(CameraManager))
					return;

				FCameraCacheEntry CameraCachePrivate = CameraManager->GetCameraCachePrivate();
				FVector Velocity = LocalRootComponent->GetComponentVelocity();
				FVector Predicted = AimbotPrediction(30000, -504, Distance, TargetLocation, Velocity);
				BestTarget = CalcAngle(CameraCachePrivate.POV.Location, Predicted);

				//g_Console->cLog(skCrypt("%.3f | %.3f | %.3f\n"), Console::EColor_dark_green, BestTarget.X, BestTarget.Y, BestTarget.Z);
			}
			else
			{
				BestTarget = FVector();
			}
		}
	}

	void Game::Misc()
	{
		UWorld* GWorld = UWorld::GetWorld();
		if (!p_UKSystemLib->IsValid(GWorld))
			return;

		UGameInstance* OwningGameInstance = GWorld->GetOwningGameInstance();
		if (!p_UKSystemLib->IsValid(OwningGameInstance))
			return;

		ULocalPlayer* LocalPlayer = OwningGameInstance->GetLocalPlayers();
		if (!LocalPlayer)
			return;

		APlayerController* LocalPlayerController = LocalPlayer->GetPlayerController();
		if (!p_UKSystemLib->IsValid(LocalPlayerController))
			return;

		ATigerCharacter* LocalCharacter = static_cast<ATigerCharacter*>(LocalPlayerController->GetAcknowledgedPawn());
		if (!p_UKSystemLib->IsValid(LocalCharacter))
			return;

		UTigerRangedWeaponComponent* RangedWeaponComponent = LocalCharacter->GetRangedWeaponComponent();
		if (!p_UKSystemLib->IsValid(RangedWeaponComponent))
			return;

		UTigerRangedWeapon* EquippedWeaponType = RangedWeaponComponent->GetEquippedWeaponType();
		if (!p_UKSystemLib->IsValid(EquippedWeaponType))
			return;

		std::optional<FTigerFireSettings> CurrentFireSettings = RangedWeaponComponent->GetCurrentFireSettings();
		if (!CurrentFireSettings.has_value())
			return;

		if (Settings[NO_RECOIL].Value.bValue)
		{
			EquippedWeaponType->VerticalRecoilCurve = nullptr;
			EquippedWeaponType->HorizontalRecoilCurve = nullptr;

			CurrentFireSettings->VerticalRecoilAmount = 0.0f;
			CurrentFireSettings->HorizontalRecoilAmount = 0.0f;
		}

		if (Settings[NO_SPREAD].Value.bValue)
		{
			CurrentFireSettings->StartingSpread = 0.0f;
			CurrentFireSettings->SpreadIncreaseCurve = nullptr;
		}
	}

	float Game::CalculateHeadCircleRadius(float Distance)
	{
		const float MinDistance = 0.0f;
		const float MaxDistance = 100.0f;
		const float MinRadius = 0.5f;
		const float MaxRadius = 6.0f;

		if (Distance > MaxDistance)
		{
			return MinRadius;
		}
		else if (Distance < MinDistance)
		{
			return MaxRadius;
		}
		else
		{
			// Linear interpolation across the entire range
			return MaxRadius + (MinRadius - MaxRadius) * (Distance - MinDistance) / (MaxDistance - MinDistance);
		}
	}

	std::string Game::GetItemRarity(uint8_t Rarity)
	{
		switch (Rarity)
		{
		case 0:
			return std::string(skCrypt("[CM]"));
		case 1:
			return std::string(skCrypt("[UC]"));
		case 2:
			return std::string(skCrypt("[RR]"));
		case 3:
			return std::string(skCrypt("[EP]"));
		case 4:
			return std::string(skCrypt("[LG]"));
		default:
			return std::string(skCrypt("[UK]"));
		}
	}

	ATigerCharacter* Game::GetBestPlayer()
	{
		UWorld* GWorld = UWorld::GetWorld();
		if (!p_UKSystemLib->IsValid(GWorld))
			return nullptr;

		APlayerState* LocalPlayerState = LocalCharacter->GetPlayerState();
		if (!p_UKSystemLib->IsValid(LocalPlayerState))
			return nullptr;

		APlayerCameraManager* LocalPlayerCamera = LocalPlayerController->GetPlayerCameraManager();
		if (!p_UKSystemLib->IsValid(LocalPlayerCamera))
			return nullptr;

		float MinDistance = 133713371337.0f;
		ATigerCharacter* Out = nullptr;

		int Count = GWorld->GetPersistentLevel()->MaxPacket();
		for (int i = 0; i < Count; ++i)
		{
			AActor* Actor = GWorld->GetPersistentLevel()->GetInstigator(i);
			if (!Actor)
				continue;

			FString ObjectName = p_UKSystemLib->GetObjectName(Actor);
			if (!ObjectName.IsValid())
				continue;

			if (wcsstr(ObjectName.wc_str(), skCrypt(L"TBP_ElysiumPlayer_C")) || wcsstr(ObjectName.wc_str(), skCrypt(L"TBP_Player_C_")) ||
				wcsstr(ObjectName.wc_str(), skCrypt(L"TBP_NPC_Primogen_C")) || wcsstr(ObjectName.wc_str(), skCrypt(L"TBP_NPC_C_")))
			{
				ATigerCharacter* Player = static_cast<ATigerCharacter*>(Actor);
				if (p_UKSystemLib->IsValid(Player))
				{
					if (Player != LocalCharacter)
					{
						APlayerState* PlayerState = Player->GetPlayerState();
						if (!p_UKSystemLib->IsValid(PlayerState))
							continue;

						if (!Player->IsDead())
						{
							if (Settings[IGNORE_DOWNED].Value.bValue && Player->IsDowned())
								continue;

							if (!LocalPlayerState->IsOfSameTeam(PlayerState))
							{
								if (Settings[VISIBLE_CHECK].Value.bValue)
								{
									if (LocalPlayerController->LineOfSightTo(Player, { 0.0f ,0.0f, 0.0f }, false))
									{
										FVector TargetLocation = GetAimWorldLocation(LocalPlayerController, Player);

										FVector2D Pos;
										if (!LocalPlayerController->ProjectWorldLocationToScreen(TargetLocation, &Pos, false))
											return nullptr;

										float XC = Pos.X - m_ScreenWidth / 2;
										float YC = Pos.Y - m_ScreenHeight / 2;
										float Distance = sqrtf((XC * XC) + (YC * YC));

										if (Distance <= MinDistance && Distance < Settings[RADIUS].Value.fValue)
										{
											MinDistance = Distance;
											Out = Player;
										}
									}
								}
								else
								{
									FVector TargetLocation = GetAimWorldLocation(LocalPlayerController, Player);

									FVector2D Pos;
									if (!LocalPlayerController->ProjectWorldLocationToScreen(TargetLocation, &Pos, false))
										return nullptr;

									float XC = Pos.X - m_ScreenWidth / 2;
									float YC = Pos.Y - m_ScreenHeight / 2;
									float Distance = sqrtf((XC * XC) + (YC * YC));

									if (Distance <= MinDistance && Distance < Settings[RADIUS].Value.fValue)
									{
										MinDistance = Distance;
										Out = Player;
									};
								}
							}
						}
					}
				}
			}
		}
		return Out;
	}

	FVector Game::GetAimWorldLocation(APlayerController* LocalController, ATigerCharacter* Player)
	{
		if (!p_UKSystemLib->IsValid(LocalController) || !p_UKSystemLib->IsValid(Player))
			return FVector();

		auto PlayerMesh = Player->GetMesh();
		if (!p_UKSystemLib->IsValid(PlayerMesh))
			return FVector();

		if (Settings[AIM_BONE].Value.iValue == 0)
			return PlayerMesh->GetSocketLocation(PlayerMesh->GetBoneName(PlayerMesh->GetBoneIndex(FName("Head"))));
		else if (Settings[AIM_BONE].Value.iValue == 1)
			return PlayerMesh->GetSocketLocation(PlayerMesh->GetBoneName(PlayerMesh->GetBoneIndex(FName("neck_01"))));

		return FVector();
	}

	FVector Game::AimbotPrediction(float BulletVelocity, float BulletGravity, float TargetDistance, FVector TargetPosition, FVector TargetVelocity)
	{
		FVector Recalculated = TargetPosition;
		float Gravity = fabs(BulletGravity);
		float Time = TargetDistance / fabs(BulletVelocity);
		float BulletDrop = Gravity * Time * Time;
		Recalculated.Z += BulletDrop;
		Recalculated.X += Time * (TargetVelocity.X);
		Recalculated.Y += Time * (TargetVelocity.Y);
		Recalculated.Z += Time * (TargetVelocity.Z);
		return Recalculated;
	}

	FVector Game::CalcAngle(const FVector& Src, const FVector& Dst)
	{
		FVector RetVector = { 0,0,0 };
		FVector VectorPos = Dst - Src;
		float Distance = VectorPos.Magnitude();
		RetVector.X = -(((float)acosf((VectorPos.Z / Distance)) * (float)(180.0f / M_PI)) - 90.f);
		RetVector.Y = (float)atan2f((VectorPos.Y), (VectorPos.X)) * (float)(180.0f / M_PI);
		return RetVector;
	}

#pragma endregion
}