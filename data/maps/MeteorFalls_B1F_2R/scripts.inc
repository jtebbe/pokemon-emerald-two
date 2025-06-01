MeteorFalls_B1F_2R_MapScripts::
	.byte 0

MeteorFalls_B1F_2R_EventScript_Nico::
	lockall
	faceplayer
	msgbox MeteorFalls_B1F_2R_Text_NicoIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_NICO, MeteorFalls_B1F_2R_Text_NicoDefeat
	msgbox MeteorFalls_B1F_2R_Text_NicoPostDefeat, MSGBOX_DEFAULT
	giveitem ITEM_ASSAULT_VEST
	msgbox MeteorFalls_B1F_2R_Text_NicoGoodbye, MSGBOX_DEFAULT
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_NICO
	fadescreen FADE_FROM_BLACK
	setflag FLAG_AGARTHAN_2_DEFEATED
	clearflag FLAG_SHOW_HOUSE_AGARTHAN_2
	releaseall
	end

MeteorFalls_B1F_2R_Text_NicoIntro:
	.string "So yet again, the king proves me\n"
	.string "wrong. You've seen much success.\p"
	.string "It makes me wonder, though. How\n"
	.string "far do you think you can make it?\p"
	.string "You've picked quite the rough\n"
	.string "battle, I'm afraid. I am second\l"
	.string "in strength only to my liege.\p"
	.string "Steel yourself, or you'll find\n"
	.string "peace buried below like my prey!$"

MeteorFalls_B1F_2R_Text_NicoDefeat:
	.string "Yeurgh... So I was the prey...$"

MeteorFalls_B1F_2R_Text_NicoPostDefeat:
	.string "I shudder to think of the monster\n"
	.string "you'll once you reach your\l"
	.string "full potential.\p"
	.string "I will return to the kingdom, and\n"
	.string "I await our fated rematch then.\p"
	.string "But before that, defeat the rest,\n"
	.string "and challenge our lord.\p"
	.string "I hope this provides you some\n"
	.string "assistance in that endeavor.$"

MeteorFalls_B1F_2R_Text_NicoGoodbye:
	.string "May we meet again, trainer.$"



