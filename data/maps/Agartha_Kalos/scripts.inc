Agartha_Kalos_MapScripts::
	.byte 0

Agartha_Kalos_EventScript_Zygarde::
	lockall
	faceplayer
	playmoncry SPECIES_ZYGARDE, CRY_MODE_ENCOUNTER
	msgbox Agartha_Kalos_Text_Zygarde, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_ZYGARDE, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Kalos_EventScript_ZygardeWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_ZYGARDE
	removeobject LOCALID_ZYGARDE
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Kalos_EventScript_ZygardeWaits::
	playmoncry SPECIES_ZYGARDE, CRY_MODE_ENCOUNTER
	msgbox Agartha_Kalos_Text_Zygarde, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Kalos_Text_Zygarde:
	.string "Zyyy...$"

Agartha_Kalos_EventScript_Xerneas::
	lockall
	faceplayer
	playmoncry SPECIES_XERNEAS, CRY_MODE_ENCOUNTER
	msgbox Agartha_Kalos_Text_Xerneas, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_XERNEAS, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Kalos_EventScript_XerneasWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_XERNEAS
	removeobject LOCALID_XERNEAS
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Kalos_EventScript_XerneasWaits::
	playmoncry SPECIES_XERNEAS, CRY_MODE_ENCOUNTER
	msgbox Agartha_Kalos_Text_Xerneas, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Kalos_Text_Xerneas:
	.string "Xe...$"

Agartha_Kalos_EventScript_Yveltal::
	lockall
	faceplayer
	playmoncry SPECIES_YVELTAL, CRY_MODE_ENCOUNTER
	msgbox Agartha_Kalos_Text_Yveltal, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_YVELTAL, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Kalos_EventScript_YveltalWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_YVELTAL
	removeobject LOCALID_YVELTAL
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Kalos_EventScript_YveltalWaits::
	playmoncry SPECIES_YVELTAL, CRY_MODE_ENCOUNTER
	msgbox Agartha_Kalos_Text_Yveltal, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Kalos_Text_Yveltal:
	.string "Yyy...$"

Agartha_Kalos_EventScript_Agarthan::
	lockall
	faceplayer
	msgbox Agartha_Kalos_Text_AgarthanIntro, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Agartha_Kalos_EventScript_AgarthanDenied
	goto_if_eq VAR_RESULT, YES, Agartha_Kalos_EventScript_AgarthanAccepted

Agartha_Kalos_EventScript_AgarthanDenied::
	msgbox Agartha_Kalos_Text_AgarthanDenied, MSGBOX_DEFAULT
	releaseall
	end

Agartha_Kalos_EventScript_AgarthanAccepted::
	msgbox Agartha_Kalos_Text_AgarthanAccepted, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_JAKE_REMATCH, Agartha_Kalos_Text_AgarthanDefeat
	msgbox Agartha_Kalos_Text_AgarthanPostBattle, MSGBOX_DEFAULT
	releaseall
	end

Agartha_Kalos_Text_AgarthanIntro:
	.string "You?!\n"
	.string "I thought you were weak!.\p"
	.string "Are you really strong enough?$"

Agartha_Kalos_Text_AgarthanAccepted:
	.string "Then prove it! Show me!$"

Agartha_Kalos_Text_AgarthanDenied:
	.string "That's what I thought. Scurry\n"
	.string "home, kid.$"

Agartha_Kalos_Text_AgarthanDefeat:
	.string "Dammit! You're too tough!$"

Agartha_Kalos_Text_AgarthanPostBattle:
	.string "One day I'll gather your power\n"
	.string "too, just you wait. Until then,\l"
	.string "I'll be waiting!$"
