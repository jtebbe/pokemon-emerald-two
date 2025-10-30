SealedChamber_RegigigasRoom_MapScripts::
	.byte 0

SealedChamber_RegigigasRoom_EventScript_Regigigas::
	lockall
	faceplayer
	playmoncry SPECIES_REGIGIGAS, CRY_MODE_ENCOUNTER
	msgbox SealedChamber_RegigigasRoom_Text_Regigigas, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_REGIGIGAS, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, SealedChamber_RegigigasRoom_EventScript_RegigigasWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_REGIGIGAS
	removeobject LOCALID_REGIGIGAS
	fadescreen FADE_FROM_BLACK
	releaseall
	end

SealedChamber_RegigigasRoom_EventScript_RegigigasWaits::
	playmoncry SPECIES_REGIGIGAS, CRY_MODE_ENCOUNTER
	msgbox SealedChamber_RegigigasRoom_Text_Regigigas, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

SealedChamber_RegigigasRoom_Text_Regigigas:
	.string "Gigi...$"
