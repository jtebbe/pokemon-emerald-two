NavelRock_Fork_MapScripts::
	.byte 0

NavelRock_Fork_EventScript_Agarthan::
	lockall
	faceplayer
	msgbox NavelRock_Fork_Text_AgarthanIntro, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, NavelRock_Fork_EventScript_AgarthanDenied
	goto_if_eq VAR_RESULT, YES, NavelRock_Fork_EventScript_AgarthanAccepted

NavelRock_Fork_EventScript_AgarthanDenied::
	msgbox NavelRock_Fork_Text_AgarthanDenied, MSGBOX_DEFAULT
	releaseall
	end

NavelRock_Fork_EventScript_AgarthanAccepted::
	msgbox NavelRock_Fork_Text_AgarthanAccepted, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_CARTER_REMATCH, NavelRock_Fork_Text_AgarthanDefeat
	msgbox NavelRock_Fork_Text_AgarthanPostBattle, MSGBOX_DEFAULT
	releaseall
	end

NavelRock_Fork_Text_AgarthanIntro:
	.string "Hmph. You're here. That means...\n"
	.string "You're strong.\p"
	.string "Shall we see HOW strong?$"

NavelRock_Fork_Text_AgarthanAccepted:
	.string "Come then, show me!$"

NavelRock_Fork_Text_AgarthanDenied:
	.string "Not strong enough, evidently.$"

NavelRock_Fork_Text_AgarthanDefeat:
	.string "Absurd... Ridiculous!$"

NavelRock_Fork_Text_AgarthanPostBattle:
	.string "Well, there it is. The pinnacle.\n"
	.string "You're strong enough to beat the\l"
	.string "elite four once mor, and...\p"
	.string ".... maybe, MAYBE, take on the\n"
	.string "frontier. Do it, I'm curious.\l"
	.string "How strong can one become?$"
