Agartha_Paldea_MapScripts::
	.byte 0

Agartha_Paldea_EventScript_Miraidon::
	lockall
	faceplayer
	playmoncry SPECIES_MIRAIDON, CRY_MODE_ENCOUNTER
	msgbox Agartha_Paldea_Text_Miraidon, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_MIRAIDON, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Paldea_EventScript_MiraidonWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_MIRAIDON
	removeobject LOCALID_MIRAIDON
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Paldea_EventScript_MiraidonWaits::
	playmoncry SPECIES_MIRAIDON, CRY_MODE_ENCOUNTER
	msgbox Agartha_Paldea_Text_Miraidon, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Paldea_Text_Miraidon:
	.string "Mi...$"

Agartha_Paldea_EventScript_Koraidon::
	lockall
	faceplayer
	playmoncry SPECIES_KORAIDON, CRY_MODE_ENCOUNTER
	msgbox Agartha_Paldea_Text_Koraidon, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_KORAIDON, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Paldea_EventScript_KoraidonWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_KORAIDON
	removeobject LOCALID_KORAIDON
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Paldea_EventScript_KoraidonWaits::
	playmoncry SPECIES_KORAIDON, CRY_MODE_ENCOUNTER
	msgbox Agartha_Paldea_Text_Koraidon, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Paldea_Text_Koraidon:
	.string "Ko...$"

Agartha_Paldea_EventScript_Terapagos::
	lockall
	faceplayer
	playmoncry SPECIES_TERAPAGOS, CRY_MODE_ENCOUNTER
	msgbox Agartha_Paldea_Text_Terapagos, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_TERAPAGOS, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Paldea_EventScript_TerapagosWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_TERAPAGOS
	removeobject LOCALID_TERAPAGOS
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Paldea_EventScript_TerapagosWaits::
	playmoncry SPECIES_TERAPAGOS, CRY_MODE_ENCOUNTER
	msgbox Agartha_Paldea_Text_Terapagos, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Paldea_Text_Terapagos:
	.string "Te...$"

Agartha_Paldea_EventScript_Agarthan::
	lockall
	faceplayer
	msgbox Agartha_Paldea_Text_AgarthanIntro, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Agartha_Paldea_EventScript_AgarthanDenied
	goto_if_eq VAR_RESULT, YES, Agartha_Paldea_EventScript_AgarthanAccepted

Agartha_Paldea_EventScript_AgarthanDenied::
	msgbox Agartha_Paldea_Text_AgarthanDenied, MSGBOX_DEFAULT
	releaseall
	end

Agartha_Paldea_EventScript_AgarthanAccepted::
	msgbox Agartha_Paldea_Text_AgarthanAccepted, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_DANI_REMATCH, Agartha_Paldea_Text_AgarthanDefeat
	msgbox Agartha_Paldea_Text_AgarthanPostBattle, MSGBOX_DEFAULT
	releaseall
	end

Agartha_Paldea_Text_AgarthanIntro:
	.string "Hmm? Oh?\n"
	.string "You're here, that's new\p"
	.string "Oh? You want to fight again?$"

Agartha_Paldea_Text_AgarthanAccepted:
	.string "Yes, yes, one moment... Ok!$"

Agartha_Paldea_Text_AgarthanDenied:
	.string "Good, I needed the time to do\n"
	.string "more work. I'll just be here.$"

Agartha_Paldea_Text_AgarthanDefeat:
	.string "Oof, good excuse to return\n"
	.string "to my work.$"


Agartha_Paldea_Text_AgarthanPostBattle:
	.string "Well, you're certainly still\n"
	.string "strong. That much is certain.\l"
	.string "Go on, get moving.\p"
	.string "There's plenty to do and I am\n"
	.string "but a humble scribe.$"
