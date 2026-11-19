"GameMenu"
{
	"1"
	{
		"label" "#GameUI_GameMenu_ResumeGame"
		"command" "ResumeGame"
		"InGameOrder" "10"
		"OnlyInGame" "1"
	}
	"2"
	{
		"label" "#GameUI_GameMenu_NewGame"
		"command" "OpenNewGameDialog"
		"InGameOrder" "40"
		"notmulti" "1"
	}
	"3"
	{
		"label" "Create Server"
		"command" "OpenCreateMultiplayerGameDialog"
		"InGameOrder" "115"
		"OnlyInMenu" "1"
	}
	"4"
	{
		"label" "Find Server"
		"command" "OpenServerBrowser"
		"InGameOrder" "116"
		"OnlyInMenu" "1"
	}
	"5" [$WIN32]
	{
		"label" "#GameUI_GameMenu_BonusMaps"
		"command" "OpenBonusMapsDialog"
		"InGameOrder" "50"
		"notmulti" "1"
	}
	"6"
	{
		"label" "#GameUI_GameMenu_LoadGame"
		"command" "OpenLoadGameDialog"
		"InGameOrder" "30"
		"notmulti" "1"
	}
	"7"
	{
		"label" "#GameUI_GameMenu_SaveGame"
		"command" "OpenSaveGameDialog"
		"InGameOrder" "20"
		"notmulti" "1"
		"OnlyInGame" "1"
	}
	//"8"
	//{
	//	"label" "#GameUI_LoadCommentary"
	//	"command" "OpenLoadSingleplayerCommentaryDialog"
	//	"InGameOrder" "60"
	//}
	//"9"
	//{
	//	"label" "#GameUI_GameMenu_Achievements"
	//	"command" "OpenAchievementsDialog"
	//	"InGameOrder" "70"
	//}
	"8"
	{
		"label" "#GameUI_GameMenu_MainMenu"
		"command" "MainMenu"
		"InGameOrder" "100"
		"OnlyInGame" "1"
	}
	"9"
	{
		"label" "#GameUI_Controller"
		"command" "OpenControllerDialog"
		"InGameOrder" "80"
		"ConsoleOnly" "1"
	}
	"10"
	{
		"label" "#GameUI_GameMenu_Options"
		"command" "OpenOptionsDialog"
		"InGameOrder" "90"
	}
	"11"
	{
		"label" "Credits"
		"command" "OpenCredits"
		"InGameOrder" "120"
		"OnlyInMenu" "1"
	}
	"12"
	{
		"label" "#GameUI_GameMenu_Quit"
		"command" "Quit"
		"InGameOrder" "110"
	}
}
