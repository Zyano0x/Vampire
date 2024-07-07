#pragma once

#define SETTINGS_FILE_NAME "rc_config.ini"
#define STREAM(s) (((std::stringstream&)(std::stringstream() << s)).str())
#define LIMIT_VALUE(Val,Min,Max) \
		if( (Val) > (Max) ) (Val) = (Max); \
		else if( (Val) < (Min) ) (Val) = (Min);

typedef enum
{
	SETTING_BOOL,
	SETTING_INT,
	SETTING_FLOAT,
	SETTING_VEC4,
	SETTING_STRING,
	SETTING_VEC2,
	SETTING_NONE
} SettingType_t;

typedef enum
{
	// AIMBOT	
	AIM_ENABLED,
	AIM_KEY,
	AIM_MODE,
	AIM_BONE,
	AIM_SMOOTH,
	FOV,
	RADIUS,
	VISIBLE_CHECK,
	IGNORE_DOWNED,

	// ESP	
	ESP_ENABLED,
	ESP_ENEMY,
	ESP_ENEMY_COLOR,
	ESP_VISIBLE_COLOR,
	ESP_STEALTH_COLOR,
	ESP_FRIENDLY,
	ESP_FRIENDLY_COLOR,
	ESP_NAME,
	ESP_SKELETON,
	ESP_DISTANCE,
	ESP_BOX,
	ESP_SNAP_LINES,
	ESP_HEALTH,
	ESP_ARMOR,
	ESP_GLOW,
	ESP_LOOT,
	ESP_FIREARM,
	ESP_MELEE,
	ESP_UNARMED,
	ESP_OUTFIT,
	ESP_AMULET,
	ESP_ARTIFACT,
	ESP_BODYARMOR,
	ESP_RELIC,
	ESP_AMMO,
	ESP_COMBINED_AMMO,
	ESP_CONSUMABLE,
	ESP_MOD,
	ESP_COLLECTIBLE,
	ESP_KEY,
	ESP_COMPASS_RADAR,
	ESP_COMPASS_RADAR_RADIUS,

	// MISC	
	NO_RECOIL,
	NO_SPREAD,
	FREE_CAM,
	FOV_CHANGER,
	FOV_AMOUNT,

	CONFIGS_

} Configs_t;

typedef struct V
{
	int iValue;
	int	iMin;
	int	iMax;

	float fValue;
	float fMin;
	float fMax;

	bool bValue;

	ImVec4 v4Value;
	ImVec2 v2Value;

	std::string	szValue;

	~V() {}

	V()
	{
		iValue = 0;
		iMin = 0;
		iMax = 0;

		fValue = 0.0f;
		fMin = 0.0f;
		fMax = 0.0f;

		bValue = false;

		v4Value = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		v2Value = ImVec2(0.0f, 0.0f);
	}
} Value_t;

class Config
{
public:
	SettingType_t eType;
	Value_t Value;

	std::string	pszAppName;
	std::string	pszKeyName;
	std::string	szDefault;

	Config() = default;

	Config(SettingType_t eType, std::string pszAppName, std::string pszKeyName, std::string szDefault, Value_t val)
	{
		this->eType = eType;

		this->pszAppName = pszAppName;
		this->pszKeyName = pszKeyName;
		this->szDefault = szDefault;

		this->Value.iValue = val.iValue;
		this->Value.iMin = val.iMin;
		this->Value.iMax = val.iMax;
		this->Value.fValue = val.fValue;
		this->Value.fMin = val.fMin;
		this->Value.fMax = val.fMax;
		this->Value.bValue = val.bValue;
		this->Value.v4Value = ImVec4(val.v4Value.x, val.v4Value.y, val.v4Value.z, val.v4Value.w);
		this->Value.v2Value = ImVec2(val.v2Value.x, val.v2Value.y);
		this->Value.szValue = val.szValue;
	}
};

//========================================================================

extern Config Settings[CONFIGS_];
extern char	  szSettingsFile[MAX_PATH];

void BotInitSettings();
bool BotLoadSettings();
bool BotSaveSettings();