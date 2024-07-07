#include "pch.h"

#pragma warning(disable : 4996)

Config Settings[CONFIGS_];
char   szSettingsFile[MAX_PATH];

Config BotAddConfig(const std::string pszAppName, const std::string pszKeyName, const bool bDefault);
Config BotAddConfig(const std::string pszAppName, const std::string pszKeyName, const int iDefault, const int iMin, const int iMax);
Config BotAddConfig(const std::string pszAppName, const std::string pszKeyName, const float fDefault, const float fMin, const float fMax);
Config BotAddConfig(const std::string pszAppName, const std::string pszKeyName, const ImVec4 v4Default);

// For some reason this function won't work if i use "const std::string" instead of "const char*", 
// Because: it redirects to the first function (with const bool bDefault) if i use "const std::string"
Config BotAddConfig(const std::string pszAppName, const std::string pszKeyName, const char* pszDefault);
Config BotAddConfig(const std::string pszAppName, const std::string pszKeyName, const ImVec2 v2Default);

//========================================================================

void BotInitSettings()
{
	Settings[AIM_ENABLED] = BotAddConfig("Aimbot Settings", "Aimbot_Enabled", false);
	Settings[AIM_KEY] = BotAddConfig("Aimbot Settings", "Aimbot_Key", 0, 0, 4); // 5: 0, 1, 2, 3, 4
	Settings[AIM_MODE] = BotAddConfig("Aimbot Settings", "Aimbot_Mode", 1, 0, 1); // Normal, Silent.	
	Settings[AIM_BONE] = BotAddConfig("Aimbot Settings", "Aimbot_Bone", 0, 0, 2); // Head, Body
	Settings[AIM_SMOOTH] = BotAddConfig("Aimbot Settings", "Aimbot_Smooth", 5.0f, 0.0f, 30.0f);
	Settings[FOV] = BotAddConfig("Aimbot Settings", "Aimbot_Fov", false);
	Settings[RADIUS] = BotAddConfig("Aimbot Settings", "Aimbot_Radius", 180.0f, 0.0f, 360.0f);
	Settings[VISIBLE_CHECK] = BotAddConfig("Aimbot Settings", "Aimbot_VisibleCheck", false);
	Settings[IGNORE_DOWNED] = BotAddConfig("Aimbot Settings", "Aimbot_IgnoreDowned", false);

	//========================================================================

	Settings[ESP_ENABLED] = BotAddConfig("Esp Settings", "Esp_Enabled", false);
	Settings[ESP_ENEMY] = BotAddConfig("Esp Settings", "Esp_Enemies", false);
	Settings[ESP_ENEMY_COLOR] = BotAddConfig("Esp Settings", "Esp_EnemyColor", ImVec4(1.00f, 1.00f, 0.64f, 1.00f));
	Settings[ESP_VISIBLE_COLOR] = BotAddConfig("Esp Settings", "Esp_VisibleColor", ImVec4(0.38f, 0.03f, 0.77f, 1.00f));
	Settings[ESP_STEALTH_COLOR] = BotAddConfig("Esp Settings", "Esp_StealthColor", ImVec4(0.00f, 0.00f, 0.00f, 1.00f));
	Settings[ESP_FRIENDLY] = BotAddConfig("Esp Settings", "Esp_Friendlies", false);
	Settings[ESP_FRIENDLY_COLOR] = BotAddConfig("Esp Settings", "Esp_FriendlyColor", ImVec4(0.08f, 0.64f, 0.84f, 1.00f));
	Settings[ESP_BOX] = BotAddConfig("Esp Settings", "Esp_Box", 0, 0, 2); // Off, 2D-F, Corn-F
	Settings[ESP_SNAP_LINES] = BotAddConfig("Esp Settings", "Esp_SnapLines", false);
	Settings[ESP_NAME] = BotAddConfig("Esp Settings", "Esp_Name", false);
	Settings[ESP_SKELETON] = BotAddConfig("Esp Settings", "Esp_Skeleton", false);
	Settings[ESP_DISTANCE] = BotAddConfig("Esp Settings", "Esp_Distance", false);
	Settings[ESP_HEALTH] = BotAddConfig("Esp Settings", "Esp_Health", false);
	Settings[ESP_ARMOR] = BotAddConfig("Esp Settings", "Esp_Armor", false);
	Settings[ESP_GLOW] = BotAddConfig("Esp Settings", "Esp_Glow", false);
	Settings[ESP_LOOT] = BotAddConfig("Esp Settings", "Esp_Loot", 0, 0, 4);
	Settings[ESP_FIREARM] = BotAddConfig("Esp Settings", "Esp_Firearm", false);
	Settings[ESP_MELEE] = BotAddConfig("Esp Settings", "Esp_Melee", false);
	Settings[ESP_UNARMED] = BotAddConfig("Esp Settings", "Esp_Unarmed", false);
	Settings[ESP_OUTFIT] = BotAddConfig("Esp Settings", "Esp_Outfit", false);
	Settings[ESP_AMULET] = BotAddConfig("Esp Settings", "Esp_Amulet", false);
	Settings[ESP_ARTIFACT] = BotAddConfig("Esp Settings", "Esp_Artifact", false);
	Settings[ESP_BODYARMOR] = BotAddConfig("Esp Settings", "Esp_BodyArmor", false);
	Settings[ESP_RELIC] = BotAddConfig("Esp Settings", "Esp_Relic", false);
	Settings[ESP_AMMO] = BotAddConfig("Esp Settings", "Esp_Ammo", false);
	Settings[ESP_COMBINED_AMMO] = BotAddConfig("Esp Settings", "Esp_CombinedAmmo", false);
	Settings[ESP_MOD] = BotAddConfig("Esp Settings", "Esp_Mod", false);
	Settings[ESP_COLLECTIBLE] = BotAddConfig("Esp Settings", "Esp_Collectible", false);
	Settings[ESP_KEY] = BotAddConfig("Esp Settings", "Esp_Key", false);
	Settings[ESP_COMPASS_RADAR] = BotAddConfig("Esp Settings", "Esp_Compass", false);
	Settings[ESP_COMPASS_RADAR_RADIUS] = BotAddConfig("Esp Settings", "Esp_CompassRadius", 0.0f, 0.0f, 100.0f);

	//========================================================================	

	Settings[NO_RECOIL] = BotAddConfig("Misc Settings", "Misc_NoRecoil", false);
	Settings[NO_SPREAD] = BotAddConfig("Misc Settings", "Misc_NoSpread", false);
	Settings[FREE_CAM] = BotAddConfig("Misc Settings", "Misc_FreeCam", false);
	Settings[FOV_CHANGER] = BotAddConfig("Misc Settings", "Misc_FovChanger", false);
	Settings[FOV_AMOUNT] = BotAddConfig("Misc Settings", "Misc_FovAmount", 0, 0, 200);
}

bool BotLoadSettings()
{
	sprintf_s<MAX_PATH>(szSettingsFile, "%s%s", Z::g_D3D11Window->s_szDllDirectory, SETTINGS_FILE_NAME);
	FILE* Settings_File = fopen(szSettingsFile, "r"); //"r" Opens a file for reading. The file must exist.
	if (!Settings_File)
	{
		//First time writing to file
		Settings_File = fopen(szSettingsFile, "w+"); //"w+" Creates an empty file for both reading and writing.
		if (!Settings_File)
		{
			return false;
		}

		for (size_t i = 0; i < CONFIGS_; i++)
		{
			switch (Settings[i].eType)
			{
			case SETTING_BOOL:
				Settings[i].szDefault = (Settings[i].Value.bValue) ? "on" : "off";
				break;

			case SETTING_INT:
				LIMIT_VALUE(Settings[i].Value.iValue, Settings[i].Value.iMin, Settings[i].Value.iMax);
				Settings[i].szDefault = std::to_string(Settings[i].Value.iValue);
				break;

			case SETTING_FLOAT:
				LIMIT_VALUE(Settings[i].Value.fValue, Settings[i].Value.fMin, Settings[i].Value.fMax);
				Settings[i].szDefault = STREAM(std::fixed << std::setprecision(3) << Settings[i].Value.fValue);
				break;

			case SETTING_VEC4:
				Settings[i].szDefault = STREAM(
					std::fixed << std::setprecision(3) << Settings[i].Value.v4Value.x << " " <<
					std::fixed << std::setprecision(3) << Settings[i].Value.v4Value.y << " " <<
					std::fixed << std::setprecision(3) << Settings[i].Value.v4Value.z << " " <<
					std::fixed << std::setprecision(3) << Settings[i].Value.v4Value.w
				);
				break;

			case SETTING_VEC2:
				Settings[i].szDefault = STREAM(
					std::fixed << std::setprecision(3) << Settings[i].Value.v2Value.x << "," <<
					std::fixed << std::setprecision(3) << Settings[i].Value.v2Value.y
				);
				break;

			case SETTING_STRING:
				Settings[i].szDefault = Settings[i].Value.szValue;
			}

			WritePrivateProfileStringA(Settings[i].pszAppName.c_str(), Settings[i].pszKeyName.c_str(), Settings[i].szDefault.c_str(), szSettingsFile);
		}

		fclose(Settings_File);
		return true;
	}

	// Loading 
	char* temp = new char[1024];
	if (temp == nullptr)
	{
		fclose(Settings_File);
		return false;
	}

	for (size_t i = 0; i < CONFIGS_; i++)
	{
		GetPrivateProfileStringA(Settings[i].pszAppName.c_str(),
			Settings[i].pszKeyName.c_str(),
			Settings[i].szDefault.c_str(),
			temp,
			1024,
			szSettingsFile);

		switch (Settings[i].eType)
		{
		case SETTING_BOOL:
			Settings[i].Value.bValue = (_stricmp(temp, "on") == 0);
			break;

		case SETTING_INT:
			Settings[i].Value.iValue = atoi(temp);
			LIMIT_VALUE(Settings[i].Value.iValue, Settings[i].Value.iMin, Settings[i].Value.iMax);
			break;

		case SETTING_FLOAT:
			Settings[i].Value.fValue = static_cast<float>(atof(temp));
			LIMIT_VALUE(Settings[i].Value.fValue, Settings[i].Value.fMin, Settings[i].Value.fMax);
			break;

		case SETTING_VEC4:
			sscanf_s(temp, "%f %f %f %f",
				&Settings[i].Value.v4Value.x,
				&Settings[i].Value.v4Value.y,
				&Settings[i].Value.v4Value.z,
				&Settings[i].Value.v4Value.w);
			break;

		case SETTING_VEC2:
			sscanf_s(temp, "%f,%f",
				&Settings[i].Value.v2Value.x,
				&Settings[i].Value.v2Value.y);
			break;

		case SETTING_STRING:
			Settings[i].Value.szValue = temp;
		}
	}

	delete[] temp;
	fclose(Settings_File);
	return true;
}

bool BotSaveSettings()
{
	FILE* fp = fopen(szSettingsFile, "a");
	if (!fp)
	{
		return false;
	}

	for (size_t i = 0; i < CONFIGS_; i++)
	{
		switch (Settings[i].eType)
		{
		case SETTING_BOOL:
			Settings[i].szDefault = (Settings[i].Value.bValue) ? "on" : "off";
			break;

		case SETTING_INT:
			LIMIT_VALUE(Settings[i].Value.iValue, Settings[i].Value.iMin, Settings[i].Value.iMax);
			Settings[i].szDefault = std::to_string(Settings[i].Value.iValue);
			break;

		case SETTING_FLOAT:
			LIMIT_VALUE(Settings[i].Value.fValue, Settings[i].Value.fMin, Settings[i].Value.fMax);
			Settings[i].szDefault = STREAM(std::fixed << std::setprecision(3) << Settings[i].Value.fValue);
			break;

		case SETTING_VEC4:
			Settings[i].szDefault = STREAM(
				std::fixed << std::setprecision(3) << Settings[i].Value.v4Value.x << " " <<
				std::fixed << std::setprecision(3) << Settings[i].Value.v4Value.y << " " <<
				std::fixed << std::setprecision(3) << Settings[i].Value.v4Value.z << " " <<
				std::fixed << std::setprecision(3) << Settings[i].Value.v4Value.w
			);
			break;

		case SETTING_VEC2:
			Settings[i].szDefault = STREAM(
				std::fixed << std::setprecision(3) << Settings[i].Value.v2Value.x << "," <<
				std::fixed << std::setprecision(3) << Settings[i].Value.v2Value.y
			);
			break;

		case SETTING_STRING:
			Settings[i].szDefault = Settings[i].Value.szValue;
		}

		if (WritePrivateProfileStringA(Settings[i].pszAppName.c_str(), Settings[i].pszKeyName.c_str(), Settings[i].szDefault.c_str(), szSettingsFile))
			continue;
		else
			return false;
	}

	fclose(fp);
	return true;
}

Config BotAddConfig(const std::string pszAppName, const std::string pszKeyName, const bool bDefault)
{
	Value_t val;

	val.bValue = bDefault;

	return Config(SETTING_BOOL, pszAppName, pszKeyName, (val.bValue) ? "on" : "off", val);
}

Config BotAddConfig(const std::string pszAppName, const std::string pszKeyName, const int iDefault, const int iMin, const int iMax)
{
	Value_t val;

	val.iValue = iDefault;
	val.iMin = iMin;
	val.iMax = iMax;

	return Config(SETTING_INT, pszAppName, pszKeyName, std::to_string(val.iValue), val);
}

Config BotAddConfig(const std::string pszAppName, const std::string pszKeyName, const float fDefault, const float fMin, const float fMax)
{
	Value_t val;

	val.fValue = fDefault;
	val.fMin = fMin;
	val.fMax = fMax;

	return Config(SETTING_FLOAT, pszAppName, pszKeyName, STREAM(std::fixed << std::setprecision(3) << val.fValue), val);
}

Config BotAddConfig(const std::string pszAppName, const std::string pszKeyName, const ImVec4 v4Default)
{
	Value_t val;

	val.v4Value = v4Default;

	return Config(SETTING_VEC4, pszAppName, pszKeyName,

		STREAM(
			std::fixed << std::setprecision(3) << val.v4Value.x << " " <<
			std::fixed << std::setprecision(3) << val.v4Value.y << " " <<
			std::fixed << std::setprecision(3) << val.v4Value.z << " " <<
			std::fixed << std::setprecision(3) << val.v4Value.w
		), val);
}

Config BotAddConfig(const std::string pszAppName, const std::string pszKeyName, const char* pszDefault)
{
	Value_t val;

	val.szValue = pszDefault;

	return Config(SETTING_STRING, pszAppName, pszKeyName, val.szValue, val);
}

Config BotAddConfig(const std::string pszAppName, const std::string pszKeyName, const ImVec2 v2Default)
{
	Value_t val;

	val.v2Value = v2Default;

	return Config(SETTING_VEC2, pszAppName, pszKeyName,

		STREAM(
			std::fixed << std::setprecision(3) << val.v2Value.x << "," <<
			std::fixed << std::setprecision(3) << val.v2Value.y
		), val);
}