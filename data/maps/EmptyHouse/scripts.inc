EmptyHouse_MapScripts::
	.byte 0

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
	msgbox EmptyHouse_Text_9thSign, MSGBOX_SIGN
	call_if_unset FLAG_AGARTHANS_UNLOCKED, EmptyHouse_EventScript_UnlockAgartha
	end

EmptyHouse_Text_9thSign:
	.string "The King awaits those who would\n"
	.string "transgress upon all of Agartha.$"

EmptyHouse_EventScript_UnlockAgartha::
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_1
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_2
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_3
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_4
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_5
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_6
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_7
	clearflag FLAG_HIDE_OVERWORLD_AGARTHAN_8
	setvar VAR_0x8004, 2  @ vertical pan
	setvar VAR_0x8005, 2  @ horizontal pan
	setvar VAR_0x8006, 8  @ num shakes
	setvar VAR_0x8007, 5  @ shake delay
	special ShakeCamera
	waitstate
	delay 30
	end
