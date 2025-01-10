SlateportCity_House_MapScripts::
	.byte 0

SlateportCity_House_EventScript_PokefanM::
	msgbox SlateportCity_House_Text_NatureToDoWithStatGains, MSGBOX_NPC
	end

SlateportCity_House_EventScript_Girl::
	goto_if_set FLAG_DEFEATED_MUSCLE_BAND_LADY, SlateportCity_House_EventScript_GirlDefeat
	msgbox SlateportCity_House_Text_MustBeGoingToBattleTent, MSGBOX_NPC
	msgbox SlateportCity_House_Text_Question, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, SlateportCity_House_EventScript_BattleGirl
	msgbox SlateportCity_House_Text_OkayComeAgain, MSGBOX_NPC
	end

SlateportCity_House_EventScript_BattleGirl::
	msgbox SlateportCity_House_Text_OkayLetsDoIt, MSGBOX_NPC
	trainerbattle_no_intro TRAINER_BRIDGETTE, SlateportCity_House_Text_BattleDefeat
	setflag FLAG_DEFEATED_MUSCLE_BAND_LADY
	msgbox SlateportCity_House_Text_WhewWhatABattle, MSGBOX_NPC
	giveitem ITEM_MUSCLE_BAND
	goto SlateportCity_House_EventScript_GirlDefeat

SlateportCity_House_EventScript_GirlDefeat::
	msgbox SlateportCity_House_Text_Done, MSGBOX_NPC
	end


SlateportCity_House_Text_NatureToDoWithStatGains:
	.string "My POKéMON has a HASTY nature.\p"
	.string "It has higher SPEED compared to\n"
	.string "my other POKéMON.\p"
	.string "Maybe their nature has something to\n"
	.string "do with the stat gains of POKéMON.$"

SlateportCity_House_Text_MustBeGoingToBattleTent:
	.string "You're a TRAINER, aren't you?\p"
	.string "Since you came to SLATEPORT, you\n"
	.string "must be going to MAUVILLE CITY.p"
	.string "Hmm... Tell you what, if you beat\n"
	.string "me in battle, I'll give you some-\l"
	.string "thing that'll help you.\p"

SlateportCity_House_Text_Question:
	.string "What do you say?$"

SlateportCity_House_Text_OkayComeAgain:
	.string "You sure? Well, I'll be here!$"

SlateportCity_House_Text_OkayLetsDoIt:
	.string "Alright! Let's get it on!$"

SlateportCity_House_Text_BattleDefeat:
	.string "Whew! You're the real deal!$"

SlateportCity_House_Text_WhewWhatABattle:
	.string "You're fantastic! You'll go far!\n"
	.string "Take this and press on!$"
 
SlateportCity_House_Text_Done:
	.string "The MUSCLE BAND allows your\n"
	.string "POKéMON to deal more physical\l"
	.string "Damage. Use it wisely!$"
