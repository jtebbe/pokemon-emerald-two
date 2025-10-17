Agartha_Alola_MapScripts::
	.byte 0

Agartha_Alola_EventScript_Cosmog1::
	lockall
	faceplayer
	playmoncry SPECIES_COSMOG, CRY_MODE_ENCOUNTER
	msgbox Agartha_Alola_Text_Cosmog1, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_COSMOG, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Alola_EventScript_Cosmog1Waits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_COSMOG_1
	removeobject LOCALID_COSMOG_1
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Alola_EventScript_Cosmog1Waits::
	playmoncry SPECIES_COSMOG, CRY_MODE_ENCOUNTER
	msgbox Agartha_Alola_Text_Cosmog1, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Alola_Text_Cosmog1:
	.string "Moo...$"

Agartha_Alola_EventScript_Cosmog2::
	lockall
	faceplayer
	playmoncry SPECIES_COSMOG, CRY_MODE_ENCOUNTER
	msgbox Agartha_Alola_Text_Cosmog2, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_COSMOG, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Alola_EventScript_Cosmog2Waits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_COSMOG_2
	removeobject LOCALID_COSMOG_2
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Alola_EventScript_Cosmog2Waits::
	playmoncry SPECIES_COSMOG, CRY_MODE_ENCOUNTER
	msgbox Agartha_Alola_Text_Cosmog2, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Alola_Text_Cosmog2:
	.string "Cos...$"

Agartha_Alola_EventScript_Necrozma::
	lockall
	faceplayer
	playmoncry SPECIES_NECROZMA, CRY_MODE_ENCOUNTER
	msgbox Agartha_Alola_Text_Necrozma, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	setwildbattle SPECIES_NECROZMA, 70
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Agartha_Alola_EventScript_NecrozmaWaits
	fadescreen FADE_TO_BLACK
	delay 30
	setflag FLAG_CAUGHT_NECROZMA
	removeobject LOCALID_NECROZMA
	fadescreen FADE_FROM_BLACK
	releaseall
	end

Agartha_Alola_EventScript_NecrozmaWaits::
	playmoncry SPECIES_NECROZMA, CRY_MODE_ENCOUNTER
	msgbox Agartha_Alola_Text_Necrozma, MSGBOX_AUTOCLOSE
	delay 40
	waitmoncry
	releaseall
	end

Agartha_Alola_Text_Necrozma:
	.string "Otherworldly noises$"

Agartha_Alola_EventScript_Agarthan::
	lockall
	faceplayer
	msgbox Agartha_Alola_Text_AgarthanIntro, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Agartha_Alola_EventScript_AgarthanDenied
	goto_if_eq VAR_RESULT, YES, Agartha_Alola_EventScript_AgarthanAccepted

Agartha_Alola_EventScript_AgarthanDenied::
	msgbox Agartha_Alola_Text_AgarthanDenied, MSGBOX_DEFAULT
	releaseall
	end

Agartha_Alola_EventScript_AgarthanAccepted::
	msgbox Agartha_Alola_Text_AgarthanAccepted, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_MADDY_REMATCH, Agartha_Alola_Text_AgarthanDefeat
	msgbox Agartha_Alola_Text_AgarthanPostBattle, MSGBOX_DEFAULT
	releaseall
	end

Agartha_Alola_Text_AgarthanIntro:
	.string "Heya! You made it, huh?\n"
	.string "Great work! But now...\p"
	.string "You wanna see my full strength?$"

Agartha_Alola_Text_AgarthanAccepted:
	.string "Let's go!$"

Agartha_Alola_Text_AgarthanDenied:
	.string "You're not chicken are you?\n"
	.string "That sure would be a shame!$"

Agartha_Alola_Text_AgarthanDefeat:
	.string "Whew! What a workout!$"

Agartha_Alola_Text_AgarthanPostBattle:
	.string "Well played, you're one real\n"
	.string "trainer! Still, there's more\l"
	.string "for you to see and do.\p"
	.string "Go and see for yourself,\n"
	.string "I'll just be here!$"
