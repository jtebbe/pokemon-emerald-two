.set LOCALID_AGARTHAN1, 1
.set LOCALID_AGARTHAN2, 2
.set LOCALID_AGARTHAN3, 3
.set LOCALID_AGARTHAN4, 4
.set LOCALID_AGARTHAN5, 5
.set LOCALID_AGARTHAN6, 6
.set LOCALID_AGARTHAN7, 7
.set LOCALID_AGARTHAN8, 8

EmptyHouse_MapScripts::
	.byte 0

EmptyHouse_EventScript_1stAgarthan::
	msgbox EmptyHouse_Text_Agarthan1, MSGBOX_NPC
	end

EmptyHouse_EventScript_2ndAgarthan::
	msgbox EmptyHouse_Text_Agarthan2, MSGBOX_NPC
	end

EmptyHouse_EventScript_3rdAgarthan::
	msgbox EmptyHouse_Text_Agarthan3, MSGBOX_NPC
	end

EmptyHouse_EventScript_4thAgarthan::
	msgbox EmptyHouse_Text_Agarthan4, MSGBOX_NPC
	end

EmptyHouse_EventScript_5thAgarthan::
	msgbox EmptyHouse_Text_Agarthan5, MSGBOX_NPC
	end

EmptyHouse_EventScript_6thAgarthan::
	msgbox EmptyHouse_Text_Agarthan6, MSGBOX_NPC
	end

EmptyHouse_EventScript_7thAgarthan::
	msgbox EmptyHouse_Text_Agarthan7, MSGBOX_NPC
	end

EmptyHouse_EventScript_8thAgarthan::
	msgbox EmptyHouse_Text_Agarthan8, MSGBOX_NPC
	end

EmptyHouse_EventScript_1stSign::
	msgbox EmptyHouse_Text_1stSign, MSGBOX_SIGN
	end

EmptyHouse_Text_1stSign:
	.string "The Occultist stirs within a silent\n"
	.string "hill, gorging upon damned souls.$"

EmptyHouse_EventScript_2ndSign::
	msgbox EmptyHouse_Text_2ndSign, MSGBOX_SIGN
	end

EmptyHouse_Text_2ndSign:
	.string "The Slayer lay deep within the\n"
	.string "dragon's nest, seeking prey.$"

EmptyHouse_EventScript_3rdSign::
	msgbox EmptyHouse_Text_3rdSign, MSGBOX_SIGN
	end

EmptyHouse_Text_3rdSign:
	.string "The Jester stays near, bringing\n"
	.string "terror in a place of nostalgia.$"

EmptyHouse_EventScript_4thSign::
	msgbox EmptyHouse_Text_4thSign, MSGBOX_SIGN
	end

EmptyHouse_Text_4thSign:
	.string "The Doctor takes what is needed,\n"
	.string "creating twisted medication.$"

EmptyHouse_EventScript_5thSign::
	msgbox EmptyHouse_Text_5thSign, MSGBOX_SIGN
	end

EmptyHouse_Text_5thSign:
	.string "The Scribe learns from history,\n"
	.string "yearning to make Agartha golden.$"

EmptyHouse_EventScript_6thSign::
	msgbox EmptyHouse_Text_6thSign, MSGBOX_SIGN
	end

EmptyHouse_Text_6thSign:
	.string "The Scout hides in a jungle,\n"
	.string "assassinating great threats.$"

EmptyHouse_EventScript_7thSign::
	msgbox EmptyHouse_Text_7thSign, MSGBOX_SIGN
	end

EmptyHouse_Text_7thSign:
	.string "The Sage seeks absurd power,\n"
	.string "absorbing spacial strength.$"

EmptyHouse_EventScript_8thSign::
	msgbox EmptyHouse_Text_8thSign, MSGBOX_SIGN
	end

EmptyHouse_Text_8thSign:
	.string "The Pyschic scours possibility,\n"
	.string "near sources of great energy.$"

EmptyHouse_EventScript_9thSign::
	call_if_unset FLAG_AGARTHANS_UNLOCKED, EmptyHouse_EventScript_UnlockAgartha
	call_if_unset FLAG_ALL_AGARTHANS_DEFEATED, EmptyHouse_EventScript_1stCheck
	msgbox EmptyHouse_Text_9thSign, MSGBOX_SIGN
	end

EmptyHouse_EventScript_1stCheck::
	call_if_set FLAG_AGARTHAN_1_DEFEATED, EmptyHouse_EventScript_2ndCheck
	msgbox EmptyHouse_Text_1stCheck, MSGBOX_DEFAULT
	end

EmptyHouse_EventScript_2ndCheck::
	call_if_set FLAG_AGARTHAN_2_DEFEATED, EmptyHouse_EventScript_3rdCheck
	msgbox EmptyHouse_Text_2ndCheck, MSGBOX_DEFAULT
	end

EmptyHouse_EventScript_3rdCheck::
	call_if_set FLAG_AGARTHAN_3_DEFEATED, EmptyHouse_EventScript_4thCheck
	msgbox EmptyHouse_Text_3rdCheck, MSGBOX_DEFAULT
	end

EmptyHouse_EventScript_4thCheck::
	call_if_set FLAG_AGARTHAN_4_DEFEATED, EmptyHouse_EventScript_5thCheck
	msgbox EmptyHouse_Text_4thCheck, MSGBOX_DEFAULT
	end

EmptyHouse_EventScript_5thCheck::
	call_if_set FLAG_AGARTHAN_5_DEFEATED, EmptyHouse_EventScript_6thCheck
	msgbox EmptyHouse_Text_5thCheck, MSGBOX_DEFAULT
	end

EmptyHouse_EventScript_6thCheck::
	call_if_set FLAG_AGARTHAN_6_DEFEATED, EmptyHouse_EventScript_7thCheck
	msgbox EmptyHouse_Text_6thCheck, MSGBOX_DEFAULT
	end

EmptyHouse_EventScript_7thCheck::
	call_if_set FLAG_AGARTHAN_7_DEFEATED, EmptyHouse_EventScript_8thCheck
	msgbox EmptyHouse_Text_7thCheck, MSGBOX_DEFAULT
	end

EmptyHouse_EventScript_8thCheck::
	call_if_set FLAG_AGARTHAN_8_DEFEATED, EmptyHouse_EventScript_FinalCheck
	msgbox EmptyHouse_Text_8thCheck, MSGBOX_DEFAULT
	end

EmptyHouse_EventScript_FinalCheck::
	call_if_unset FLAG_ALL_AGARTHANS_DEFEATED, EmptyHouse_EventScript_UnlockAgarthaLand
	end

EmptyHouse_EventScript_UnlockAgarthaLand::
	setflag FLAG_ALL_AGARTHANS_DEFEATED
	setvar VAR_0x8004, 2  @ vertical pan
	setvar VAR_0x8005, 2  @ horizontal pan
	setvar VAR_0x8006, 8  @ num shakes
	setvar VAR_0x8007, 5  @ shake delay
	special ShakeCamera
	waitstate
	delay 30
	msgbox EmptyHouse_Text_AgarthansUnison
	fadescreen FADE_TO_BLACK
	delay 12
	clearflag FLAG_AGARTHAN_ITEM
	removeobject LOCALID_AGARTHAN1
	removeobject LOCALID_AGARTHAN2
	removeobject LOCALID_AGARTHAN3
	removeobject LOCALID_AGARTHAN4
	removeobject LOCALID_AGARTHAN5
	removeobject LOCALID_AGARTHAN6
	removeobject LOCALID_AGARTHAN7
	removeobject LOCALID_AGARTHAN8
	fadescreen FADE_FROM_BLACK
	end

EmptyHouse_Text_AgarthansUnison:
	.string "AGARTHANS: And so, the kingdom\n"
	.string "opens itself to you;\p"
	.string "And yet, one more battle remains.\p"
	.string "Go to him, beneath the lands\n"
	.string "where your world was birthed.\p"
	.string "He awaits you, your final\n"
	.string "challenge.$"

EmptyHouse_Text_9thSign:
	.string "The King waits where the world\n"
	.string "began, judging those who deem\l"
	.string "themselves worthy of Agartha.$"

EmptyHouse_Text_1stCheck:
	.string "The Occultist awaits you...$"

EmptyHouse_Text_2ndCheck:
	.string "The Slayer awaits you...$"

EmptyHouse_Text_3rdCheck:
	.string "The Jester awaits you...$"

EmptyHouse_Text_4thCheck:
	.string "The Doctor awaits you...$"

EmptyHouse_Text_5thCheck:
	.string "The Scribe awaits you...$"

EmptyHouse_Text_6thCheck:
	.string "The Scout awaits you...$"

EmptyHouse_Text_7thCheck:
	.string "The Sage awaits you...$"

EmptyHouse_Text_8thCheck:
	.string "The Psychic awaits you...$"

EmptyHouse_EventScript_UnlockAgartha::
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_1
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_2
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_3
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_4
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_5
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_6
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_7
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_8
	setflag FLAG_AGARTHANS_UNLOCKED
	setvar VAR_0x8004, 2  @ vertical pan
	setvar VAR_0x8005, 2  @ horizontal pan
	setvar VAR_0x8006, 8  @ num shakes
	setvar VAR_0x8007, 5  @ shake delay
	special ShakeCamera
	waitstate
	delay 30
	return

EmptyHouse_Text_Agarthan1:
	.string "The spirits grow restless. You're\n"
	.string "causing quite the stir, you know?$"

EmptyHouse_Text_Agarthan2:
	.string "The spirits grow restless. You're\n"
	.string "causing quite the stir, you know?$"

EmptyHouse_Text_Agarthan3:
	.string "The spirits grow restless. You're\n"
	.string "causing quite the stir, you know?$"

EmptyHouse_Text_Agarthan4:
	.string "The spirits grow restless. You're\n"
	.string "causing quite the stir, you know?$"

EmptyHouse_Text_Agarthan5:
	.string "The spirits grow restless. You're\n"
	.string "causing quite the stir, you know?$"

EmptyHouse_Text_Agarthan6:
	.string "The spirits grow restless. You're\n"
	.string "causing quite the stir, you know?$"

EmptyHouse_Text_Agarthan7:
	.string "The spirits grow restless. You're\n"
	.string "causing quite the stir, you know?$"

EmptyHouse_Text_Agarthan8:
	.string "The spirits grow restless. You're\n"
	.string "causing quite the stir, you know?$"
