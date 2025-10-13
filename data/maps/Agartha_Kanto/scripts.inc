Agartha_Kanto_MapScripts::
	.byte 0

Agartha_Kanto_EventScript_Mewtwo::
	lockall
	faceplayer
	playmoncry SPECIES_MEWTWO, CRY_MODE_ENCOUNTER
	msgbox Agartha_Kanto_Text_Mewtwo, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_MEWTWO, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Kanto_EventScript_MewtwoWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_MEWTWO
	removeobject LOCALID_MEWTWO
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Kanto_EventScript_MewtwoWaits::
	playmoncry SPECIES_MEWTWO, CRY_MODE_ENCOUNTER
	msgbox Agartha_Kanto_Text_Mewtwo, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Kanto_Text_Mewtwo:
	.string "Mew...$"
