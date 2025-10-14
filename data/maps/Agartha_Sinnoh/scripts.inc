Agartha_Sinnoh_MapScripts::
	.byte 0

Agartha_Sinnoh_EventScript_Arceus::
	lockall
	faceplayer
	playmoncry SPECIES_ARCEUS, CRY_MODE_ENCOUNTER
	msgbox Agartha_Sinnoh_Text_Arceus, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_ARCEUS, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Sinnoh_EventScript_ArceusWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_ARCEUS
	removeobject LOCALID_ARCEUS
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Sinnoh_EventScript_ArceusWaits::
	playmoncry SPECIES_ARCEUS, CRY_MODE_ENCOUNTER
	msgbox Agartha_Sinnoh_Text_Arceus, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Sinnoh_Text_Arceus:
	.string "...$"

Agartha_Sinnoh_EventScript_Giratina::
	lockall
	faceplayer
	playmoncry SPECIES_GIRATINA, CRY_MODE_ENCOUNTER
	msgbox Agartha_Sinnoh_Text_Giratina, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_GIRATINA, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Sinnoh_EventScript_GiratinaWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_GIRATINA
	removeobject LOCALID_GIRATINA
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Sinnoh_EventScript_GiratinaWaits::
	playmoncry SPECIES_GIRATINA, CRY_MODE_ENCOUNTER
	msgbox Agartha_Sinnoh_Text_Giratina, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Sinnoh_Text_Giratina:
	.string "Girrr...$"

Agartha_Sinnoh_EventScript_Dialga::
	lockall
	faceplayer
	playmoncry SPECIES_DIALGA, CRY_MODE_ENCOUNTER
	msgbox Agartha_Sinnoh_Text_Dialga, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_DIALGA, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Sinnoh_EventScript_DialgaWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_DIALGA
	removeobject LOCALID_DIALGA
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Sinnoh_EventScript_DialgaWaits::
	playmoncry SPECIES_DIALGA, CRY_MODE_ENCOUNTER
	msgbox Agartha_Sinnoh_Text_Dialga, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Sinnoh_Text_Dialga:
	.string "Di...$"

Agartha_Sinnoh_EventScript_Palkia::
	lockall
	faceplayer
	playmoncry SPECIES_PALKIA, CRY_MODE_ENCOUNTER
	msgbox Agartha_Sinnoh_Text_Palkia, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_PALKIA, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Sinnoh_EventScript_PalkiaWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_PALKIA
	removeobject LOCALID_PALKIA
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Sinnoh_EventScript_PalkiaWaits::
	playmoncry SPECIES_PALKIA, CRY_MODE_ENCOUNTER
	msgbox Agartha_Sinnoh_Text_Palkia, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Sinnoh_Text_Palkia:
	.string "Ki...$"

Agartha_Sinnoh_EventScript_Agarthan::
	lockall
	faceplayer
	msgbox Agartha_Sinnoh_Text_AgarthanIntro, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Agartha_Sinnoh_EventScript_AgarthanDenied
	goto_if_eq VAR_RESULT, YES, Agartha_Sinnoh_EventScript_AgarthanAccepted

Agartha_Sinnoh_EventScript_AgarthanDenied::
	msgbox Agartha_Sinnoh_Text_AgarthanDenied, MSGBOX_DEFAULT
	releaseall
	end

Agartha_Sinnoh_EventScript_AgarthanAccepted::
	msgbox Agartha_Sinnoh_Text_AgarthanAccepted, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_JORDAN_REMATCH, Agartha_Sinnoh_Text_AgarthanDefeat
	msgbox Agartha_Sinnoh_Text_AgarthanPostBattle, MSGBOX_DEFAULT
	releaseall
	end

Agartha_Sinnoh_Text_AgarthanIntro:
	.string "So, you've returned. Not a moment\n"
	.string "too soon, either.\p"
	.string "Are you ready?$"

Agartha_Sinnoh_Text_AgarthanAccepted:
	.string "Hit me with all you've got!$"

Agartha_Sinnoh_Text_AgarthanDenied:
	.string "I expected better from you.\n"
	.string "Truly.$"

Agartha_Sinnoh_Text_AgarthanDefeat:
	.string "Your victory... is truly\n"
	.string "deserved. Well done.$"

Agartha_Sinnoh_Text_AgarthanPostBattle:
	.string "Defeat still stings, even after\n"
	.string "all these years. Regardless, you\l"
	.string "have proven your strength.\p"
	.string "Continue to enjoy the fruits of\n"
	.string "Agartha, and strengthen more.$"
