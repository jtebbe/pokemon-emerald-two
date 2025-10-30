Agartha_Galar_MapScripts::
	.byte 0

Agartha_Galar_EventScript_Eternatus::
	lockall
	faceplayer
	playmoncry SPECIES_ETERNATUS, CRY_MODE_ENCOUNTER
	msgbox Agartha_Galar_Text_Eternatus, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_ETERNATUS, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Galar_EventScript_EternatusWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_ETERNATUS
	removeobject LOCALID_ETERNATUS
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Galar_EventScript_EternatusWaits::
	playmoncry SPECIES_ETERNATUS, CRY_MODE_ENCOUNTER
	msgbox Agartha_Galar_Text_Eternatus, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Galar_Text_Eternatus:
	.string "...$"

Agartha_Galar_EventScript_Zamazenta::
	lockall
	faceplayer
	playmoncry SPECIES_ZAMAZENTA, CRY_MODE_ENCOUNTER
	msgbox Agartha_Galar_Text_Zamazenta, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_ZAMAZENTA, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Galar_EventScript_ZamazentaWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_ZAMAZENTA
	removeobject LOCALID_ZAMAZENTA
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Galar_EventScript_ZamazentaWaits::
	playmoncry SPECIES_ZAMAZENTA, CRY_MODE_ENCOUNTER
	msgbox Agartha_Galar_Text_Zamazenta, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Galar_Text_Zamazenta:
	.string "Zen...$"

Agartha_Galar_EventScript_Zacian::
	lockall
	faceplayer
	playmoncry SPECIES_ZACIAN, CRY_MODE_ENCOUNTER
	msgbox Agartha_Galar_Text_Zacian, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_ZACIAN, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Galar_EventScript_ZacianWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_ZACIAN
	removeobject LOCALID_ZACIAN
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Galar_EventScript_ZacianWaits::
	playmoncry SPECIES_ZACIAN, CRY_MODE_ENCOUNTER
	msgbox Agartha_Galar_Text_Zacian, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Galar_Text_Zacian:
	.string "Za...$"

Agartha_Galar_EventScript_Spectrier::
	lockall
	faceplayer
	playmoncry SPECIES_SPECTRIER, CRY_MODE_ENCOUNTER
	msgbox Agartha_Galar_Text_Spectrier, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_SPECTRIER, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Galar_EventScript_SpectrierWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_SPECTRIER
	removeobject LOCALID_SPECTRIER
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Galar_EventScript_SpectrierWaits::
	playmoncry SPECIES_SPECTRIER, CRY_MODE_ENCOUNTER
	msgbox Agartha_Galar_Text_Spectrier, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Galar_Text_Spectrier:
	.string "Tri...$"

Agartha_Galar_EventScript_Calyrex::
	lockall
	faceplayer
	playmoncry SPECIES_CALYREX, CRY_MODE_ENCOUNTER
	msgbox Agartha_Galar_Text_Calyrex, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_CALYREX, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Galar_EventScript_CalyrexWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_CALYREX
	removeobject LOCALID_CALYREX
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Galar_EventScript_CalyrexWaits::
	playmoncry SPECIES_CALYREX, CRY_MODE_ENCOUNTER
	msgbox Agartha_Galar_Text_Calyrex, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Galar_Text_Calyrex:
	.string "Fight me...$"

Agartha_Galar_EventScript_Glastrier::
	lockall
	faceplayer
	playmoncry SPECIES_GLASTRIER, CRY_MODE_ENCOUNTER
	msgbox Agartha_Galar_Text_Glastrier, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_GLASTRIER, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Galar_EventScript_GlastrierWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_GLASTRIER
	removeobject LOCALID_GLASTRIER
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Galar_EventScript_GlastrierWaits::
	playmoncry SPECIES_GLASTRIER, CRY_MODE_ENCOUNTER
	msgbox Agartha_Galar_Text_Glastrier, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Galar_Text_Glastrier:
	.string "Gla...$"

Agartha_Galar_EventScript_Agarthan::
	lockall
	faceplayer
	msgbox Agartha_Galar_Text_AgarthanIntro, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Agartha_Galar_EventScript_AgarthanDenied
	goto_if_eq VAR_RESULT, YES, Agartha_Galar_EventScript_AgarthanAccepted

Agartha_Galar_EventScript_AgarthanDenied::
	msgbox Agartha_Galar_Text_AgarthanDenied, MSGBOX_DEFAULT
	releaseall
	end

Agartha_Galar_EventScript_AgarthanAccepted::
	msgbox Agartha_Galar_Text_AgarthanAccepted, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_SEB_REMATCH, Agartha_Galar_Text_AgarthanDefeat
	msgbox Agartha_Galar_Text_AgarthanPostBattle, MSGBOX_DEFAULT
	releaseall
	end

Agartha_Galar_Text_AgarthanIntro:
	.string "Hehehe! My taunting worked!\n"
	.string "You're here!.\p"
	.string "Shall we dance once more?$"

Agartha_Galar_Text_AgarthanAccepted:
	.string "Don't bore me now!$"

Agartha_Galar_Text_AgarthanDenied:
	.string "Aww, you're no fun! Come back\n"
	.string " if you change your mind.$"

Agartha_Galar_Text_AgarthanDefeat:
	.string "What a fun game!$"

Agartha_Galar_Text_AgarthanPostBattle:
	.string "Well well well, you really kicked\n"
	.string "my rear once more. Congrats!\l"
	.string "You've earned your praises.\p"
	.string "Enjoy the fruits of this \n"
	.string "orchard and continue to reach\l"
	.string "for the top!$"
