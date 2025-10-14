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

Agartha_Kanto_EventScript_Agarthan::
	lockall
	faceplayer
	msgbox Agartha_Kanto_Text_AgarthanIntro, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Agartha_Kanto_EventScript_AgarthanDenied
	goto_if_eq VAR_RESULT, YES, Agartha_Kanto_EventScript_AgarthanAccepted

Agartha_Kanto_EventScript_AgarthanDenied::
	msgbox Agartha_Kanto_Text_AgarthanDenied, MSGBOX_DEFAULT
	releaseall
	end

Agartha_Kanto_EventScript_AgarthanAccepted::
	msgbox Agartha_Kanto_Text_AgarthanAccepted, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_EMEKA_REMATCH, Agartha_Kanto_Text_AgarthanDefeat
	msgbox Agartha_Kanto_Text_AgarthanPostBattle, MSGBOX_DEFAULT
	releaseall
	end

Agartha_Kanto_Text_AgarthanIntro:
	.string "Oho? So you made it, as I\n"
	.string "foresaw. Welcome!\p"
	.string "Would you like to test your\n"
	.string "mettle against my full strength?$"

Agartha_Kanto_Text_AgarthanAccepted:
	.string "Hohoho, here we come!$"

Agartha_Kanto_Text_AgarthanDenied:
	.string "A shame, but I know you'll be\n"
	.string "back.$"

Agartha_Kanto_Text_AgarthanDefeat:
	.string "Well, that was expected!\n"
	.string "Congratulations!$"

Agartha_Kanto_Text_AgarthanPostBattle:
	.string "I'm sure with the help of the\n"
	.string "stronger pokemon in Agartha, you\l"
	.string "can take down any challenger.\p"
	.string "Treat them well, and they'll do\n"
	.string "the same to you!$"
