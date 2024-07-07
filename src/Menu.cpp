#include "pch.h"

namespace Z
{
	void Menu::Dark()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
		colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

		// Border
		colors[ImGuiCol_Border] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
		colors[ImGuiCol_BorderShadow] = ImVec4{ 0.0f, 0.0f, 0.0f, 0.24f };

		// Text
		colors[ImGuiCol_Text] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
		colors[ImGuiCol_TextDisabled] = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
		colors[ImGuiCol_CheckMark] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };

		// Popups
		colors[ImGuiCol_PopupBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 0.92f };

		// Slider
		colors[ImGuiCol_SliderGrab] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.54f };
		colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.54f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.2f, 0.22f, 0.27f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

		// Scrollbar
		colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
		colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };

		// Seperator
		colors[ImGuiCol_Separator] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };
		colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };
		colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 1.0f };

		// Resize Grip
		colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
		colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.29f };
		colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 0.29f };

		// Docking
		colors[ImGuiCol_DockingPreview] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };

		auto& style = ImGui::GetStyle();
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.TabRounding = 4;
		style.ScrollbarRounding = 9;
		style.WindowRounding = 0;
		style.GrabRounding = 3;
		style.FrameRounding = 3;
		style.PopupRounding = 4;
		style.ChildRounding = 0;
		style.GrabMinSize = 8;
		style.FrameBorderSize = 1;
		style.WindowBorderSize = 1;
		style.TabBorderSize = 1;
		style.TabRounding = 2;
	}

	void Menu::Draw()
	{
		if (g_Engine->m_IsShowMenu)
		{
			MainMenu();
		}
	}

	void Menu::MainMenu()
	{
		ImGui::SetNextWindowSize(ImVec2(450.0f, 450.0f));
		ImGui::Begin("WTF", &g_Engine->m_IsShowMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
		{
			if (ImGui::BeginTabBar("TAB"))
			{
				if (ImGui::BeginTabItem("AIMBOT"))
				{
					ImGui::BeginChild("Aimbot", ImVec2(0, 0), ImGuiChildFlags_Border);
					{
						ImGui::Checkbox("Enabled", &Settings[AIM_ENABLED].Value.bValue);
						ImGui::Spacing();

						if (!Settings[AIM_ENABLED].Value.bValue)
						{
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
						}
						{
							ImGui::Checkbox("Draw FOV", &Settings[FOV].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Visible Check", &Settings[VISIBLE_CHECK].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Ignore Downed", &Settings[IGNORE_DOWNED].Value.bValue);
							ImGui::Spacing();

							ImGui::PushItemWidth(150.0f);
							static const char* AimMode[] = { "Normal", "Silent" };
							ImGui::Combo("Mode", &Settings[AIM_MODE].Value.iValue, AimMode, IM_ARRAYSIZE(AimMode));
							ImGui::Spacing();

							static const char* AimBone[] = { "Head", "Body", "Randomize" };
							ImGui::Combo("Bone Target", &Settings[AIM_BONE].Value.iValue, AimBone, IM_ARRAYSIZE(AimBone));
							ImGui::Spacing();
							ImGui::SliderFloat("FOV Radius", &Settings[RADIUS].Value.fValue, Settings[RADIUS].Value.fMin, Settings[RADIUS].Value.fMax);
							ImGui::Spacing();
							ImGui::SliderFloat("Smooth", &Settings[AIM_SMOOTH].Value.fValue, Settings[AIM_SMOOTH].Value.fMin, Settings[AIM_SMOOTH].Value.fMax);
							ImGui::PopItemWidth();
						}
						if (!Settings[AIM_ENABLED].Value.bValue)
						{
							ImGui::PopItemFlag();
							ImGui::PopStyleVar();
						}
					}
					ImGui::EndChild();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("VISUAL"))
				{
					ImGui::BeginChild("ESP", ImVec2(ImGui::GetContentRegionAvail().x / 2, 0), ImGuiChildFlags_Border);
					{
						ImGui::Checkbox("Enabled", &Settings[ESP_ENABLED].Value.bValue);
						ImGui::Spacing();

						if (!Settings[ESP_ENABLED].Value.bValue)
						{
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
						}
						{
							ImGui::Checkbox("Enemy", &Settings[ESP_ENEMY].Value.bValue);
							ImGui::SameLine();
							ImGui::SetCursorPosX(140.0f);
							ImGui::ColorEdit4("Enemy Color", (float*)&Settings[ESP_ENEMY_COLOR].Value.v4Value, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
							ImGui::SameLine();
							ImGui::ColorEdit4("Visible Color", (float*)&Settings[ESP_VISIBLE_COLOR].Value.v4Value, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
							ImGui::Spacing();

							ImGui::Checkbox("Team", &Settings[ESP_FRIENDLY].Value.bValue);
							ImGui::SameLine();
							ImGui::SetCursorPosX(140.0f);
							ImGui::ColorEdit4("Team Color", (float*)&Settings[ESP_FRIENDLY_COLOR].Value.v4Value, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
							ImGui::Spacing();

							ImGui::Checkbox("Name", &Settings[ESP_NAME].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Skeleton", &Settings[ESP_SKELETON].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Snaplines", &Settings[ESP_SNAP_LINES].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Distance", &Settings[ESP_DISTANCE].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Health Bar", &Settings[ESP_HEALTH].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Armor Bar", &Settings[ESP_ARMOR].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Glow", &Settings[ESP_GLOW].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Compass", &Settings[ESP_COMPASS_RADAR].Value.bValue);
							ImGui::Spacing();

							ImGui::PushItemWidth(150.0f);
							static const char* Box[] = { "Off", "2D Filled", "Corner Filled" };
							ImGui::Combo("Box", &Settings[ESP_BOX].Value.iValue, Box, IM_ARRAYSIZE(Box));
							ImGui::Spacing();

							static const char* Loot[] = { "Common", "Uncommon", "Rare", "Epic", "Legendary" };
							ImGui::Combo("Loot", &Settings[ESP_LOOT].Value.iValue, Loot, IM_ARRAYSIZE(Loot));
							ImGui::Spacing();
							ImGui::PopItemWidth();
						}
						if (!Settings[ESP_ENABLED].Value.bValue)
						{
							ImGui::PopItemFlag();
							ImGui::PopStyleVar();
						}
					}
					ImGui::EndChild();

					ImGui::SameLine();

					ImGui::BeginChild("Loot", ImVec2(0, 0), ImGuiChildFlags_Border);
					{
						if (!Settings[ESP_ENABLED].Value.bValue)
						{
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
						}
						{
							ImGui::Checkbox("Firearm", &Settings[ESP_FIREARM].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Melee", &Settings[ESP_MELEE].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Unarmed", &Settings[ESP_UNARMED].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Outfit", &Settings[ESP_OUTFIT].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Amulet", &Settings[ESP_AMULET].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Artifact", &Settings[ESP_ARTIFACT].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Armor", &Settings[ESP_BODYARMOR].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Relic", &Settings[ESP_RELIC].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Ammo", &Settings[ESP_AMMO].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Combined Ammo", &Settings[ESP_COMBINED_AMMO].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Consumable", &Settings[ESP_CONSUMABLE].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Mod", &Settings[ESP_MOD].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Collectible", &Settings[ESP_COLLECTIBLE].Value.bValue);
							ImGui::Spacing();
							ImGui::Checkbox("Key", &Settings[ESP_KEY].Value.bValue);
							ImGui::Spacing();
						}
						if (!Settings[ESP_ENABLED].Value.bValue)
						{
							ImGui::PopItemFlag();
							ImGui::PopStyleVar();
						}
					}
					ImGui::EndChild();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("MISC"))
				{
					ImGui::BeginChild("Misc", ImVec2(0, 0), ImGuiChildFlags_Border);
					{
						ImGui::Checkbox("No Recoil", &Settings[NO_RECOIL].Value.bValue);
						ImGui::Spacing();
						ImGui::Checkbox("No Spread", &Settings[NO_SPREAD].Value.bValue);
						ImGui::Spacing();
						ImGui::Checkbox("Free Cam", &Settings[FREE_CAM].Value.bValue);
						ImGui::Spacing();
						ImGui::Checkbox("FOV Changer", &Settings[FOV_CHANGER].Value.bValue);

						if (!Settings[FOV_CHANGER].Value.bValue)
						{
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
						}
						{
							ImGui::PushItemWidth(150.0f);
							ImGui::SliderFloat("FOV", &Settings[FOV_AMOUNT].Value.fValue, Settings[FOV_AMOUNT].Value.fMin, Settings[FOV_AMOUNT].Value.fMax);
							ImGui::PopItemWidth();
						}
						if (!Settings[FOV_CHANGER].Value.bValue)
						{
							ImGui::PopItemFlag();
							ImGui::PopStyleVar();
						}
					}
					ImGui::EndChild();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("CONFIG"))
				{
					ImGui::BeginChild("Config", ImVec2(0, 0), ImGuiChildFlags_Border);
					{
						if (ImGui::Button("Load", ImVec2(90, 30)))
						{
							if (BotLoadSettings())
							{
								ImGui::OpenPopup("Settings Loaded");
							}
							else
							{
								ImGui::OpenPopup("Loading Failed");
							}
						}

						ImGui::SameLine();

						if (ImGui::Button("Save", ImVec2(90, 30)))
						{
							if (BotSaveSettings())
							{
								ImGui::OpenPopup("Settings Saved");
							}
							else
							{
								ImGui::OpenPopup("Saving Failed");
							}
						}

						if (ImGui::BeginPopupModal("Settings Loaded", 0, ImGuiWindowFlags_NoResize))
						{
							ImGui::Text("The settings have been loaded");
							ImGui::Separator();
							if (ImGui::Button("OK", ImVec2(70, 0))) { ImGui::CloseCurrentPopup(); }
							ImGui::EndPopup();
						}

						if (ImGui::BeginPopupModal("Settings Saved", 0, ImGuiWindowFlags_NoResize))
						{
							ImGui::Text("The settings have been saved");
							ImGui::Separator();
							if (ImGui::Button("OK", ImVec2(70, 0))) { ImGui::CloseCurrentPopup(); }
							ImGui::EndPopup();
						}

						if (ImGui::BeginPopupModal("Loading Failed", 0, ImGuiWindowFlags_NoResize))
						{
							ImGui::Text("Failed to load the settings");
							ImGui::Separator();
							if (ImGui::Button("OK", ImVec2(70, 0))) { ImGui::CloseCurrentPopup(); }
							ImGui::EndPopup();
						}

						if (ImGui::BeginPopupModal("Saving Failed", 0, ImGuiWindowFlags_NoResize))
						{
							ImGui::Text("Failed to save the settings");
							ImGui::Separator();
							if (ImGui::Button("OK", ImVec2(70, 0))) { ImGui::CloseCurrentPopup(); }
							ImGui::EndPopup();
						}
					}
					ImGui::EndChild();
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
		}
		ImGui::End();
	}
}