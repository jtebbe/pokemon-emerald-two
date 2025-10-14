Agartha_Unova_MapScripts::
	.byte 0

Agartha_Unova_EventScript_Kyurem::
	lockall
	faceplayer
	playmoncry SPECIES_KYUREM, CRY_MODE_ENCOUNTER
	msgbox Agartha_Unova_Text_Kyurem, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_KYUREM, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Unova_EventScript_KyuremWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_KYUREM
	removeobject LOCALID_KYUREM
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Unova_EventScript_KyuremWaits::
	playmoncry SPECIES_KYUREM, CRY_MODE_ENCOUNTER
	msgbox Agartha_Unova_Text_Kyurem, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Unova_Text_Kyurem:
	.string "Kyyy...$"

Agartha_Unova_EventScript_Zekrom::
	lockall
	faceplayer
	playmoncry SPECIES_ZEKROM, CRY_MODE_ENCOUNTER
	msgbox Agartha_Unova_Text_Zekrom, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_ZEKROM, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Unova_EventScript_ZekromWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_ZEKROM
	removeobject LOCALID_ZEKROM
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Unova_EventScript_ZekromWaits::
	playmoncry SPECIES_ZEKROM, CRY_MODE_ENCOUNTER
	msgbox Agartha_Unova_Text_Zekrom, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Unova_Text_Zekrom:
	.string "Zeee...$"

Agartha_Unova_EventScript_Reshiram::
	lockall
	faceplayer
	playmoncry SPECIES_RESHIRAM, CRY_MODE_ENCOUNTER
	msgbox Agartha_Unova_Text_Reshiram, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_RESHIRAM, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Unova_EventScript_ReshiramWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_RESHIRAM
	removeobject LOCALID_RESHIRAM
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Unova_EventScript_ReshiramWaits::
	playmoncry SPECIES_RESHIRAM, CRY_MODE_ENCOUNTER
	msgbox Agartha_Unova_Text_Reshiram, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Unova_Text_Reshiram:
	.string "Ri...$"

Agartha_Unova_EventScript_Agarthan::
	lockall
	faceplayer
	msgbox Agartha_Unova_Text_AgarthanIntro, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Agartha_Unova_EventScript_AgarthanDenied
	goto_if_eq VAR_RESULT, YES, Agartha_Unova_EventScript_AgarthanAccepted

Agartha_Unova_EventScript_AgarthanDenied::
	msgbox Agartha_Unova_Text_AgarthanDenied, MSGBOX_DEFAULT
	releaseall
	end

Agartha_Unova_EventScript_AgarthanAccepted::
	msgbox Agartha_Unova_Text_AgarthanAccepted, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_NIKKI_REMATCH, Agartha_Unova_Text_AgarthanDefeat
	msgbox Agartha_Unova_Text_AgarthanPostBattle, MSGBOX_DEFAULT
	releaseall
	end

Agartha_Unova_Text_AgarthanIntro:
	.string "Well, well, well! You made it!\n"
	.string "I'm ecstatic to see you!.\p"
	.string "Ready for a rematch?$"

Agartha_Unova_Text_AgarthanAccepted:
	.string "Hit me with all you've got!$"

Agartha_Unova_Text_AgarthanDenied:
	.string "Unfortunate! I'll be here if\n"
	.string "you change your mind.$"

Agartha_Unova_Text_AgarthanDefeat:
	.string "Well done! Really well done!$"

Agartha_Unova_Text_AgarthanPostBattle:
	.string "You beat me fair and square a\n"
	.string "second time? You're the real\l"
	.string "deal! Seriously!\p"
	.string "I and those beyond the veil\n"
	.string "continue watching your journey\l"
	.string "to see how far you go!$"
