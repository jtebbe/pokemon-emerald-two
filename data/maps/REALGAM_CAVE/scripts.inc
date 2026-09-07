REALGAM_CAVE_MapScripts::
	.byte 0

REALGAM_CAVE_EventScript_Rocker::
	lock
	faceplayer
	goto_if_set FLAG_REALGAM_CAVE_ROCKER_INTRO, REALGAM_CAVE_EventScript_RockerShop
	msgbox REALGAM_CAVE_Text_RockerIntro, MSGBOX_DEFAULT
	msgbox REALGAM_CAVE_Text_RockerExplains, MSGBOX_DEFAULT
	setflag FLAG_REALGAM_CAVE_ROCKER_INTRO

REALGAM_CAVE_EventScript_RockerShop::
	message REALGAM_CAVE_Text_RockerShop
	waitmessage
	special SetNextPokemartBingoCheatShop
	pokemart REALGAM_CAVE_Pokemart_BingoCheats
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
REALGAM_CAVE_Pokemart_BingoCheats:
	.2byte ITEM_HEAVY_DUTY_BOOTS
	.2byte ITEM_CLEAR_AMULET
	.2byte ITEM_BRIGHT_POWDER
	.2byte ITEM_SITRUS_BERRY
	.2byte ITEM_QUICK_CLAW
	.2byte ITEM_FOCUS_SASH
	.2byte ITEM_LIFE_ORB
	.2byte ITEM_LEFTOVERS
	.2byte ITEM_EXPERT_BELT
	.2byte ITEM_ASSAULT_VEST
	.2byte ITEM_FULL_RESTORE
	.2byte ITEM_CHOICE_BAND
	.2byte ITEM_CHOICE_SPECS
	.2byte ITEM_CHOICE_SCARF
	.2byte ITEM_MAX_REVIVE
	pokemartlistend

REALGAM_CAVE_Text_RockerIntro:
	.string "Heh, you're a new face. Looking to\n"
	.string "cheat the system a bit?$"

REALGAM_CAVE_Text_RockerExplains:
	.string "I can inject some assistance into\n"
	.string "the bingo boards you play if you find\n"
	.string "them too hard.\p"
	.string "But of course, it will cost you.$"

REALGAM_CAVE_Text_RockerShop:
	.string "Whatcha' lookin' to cheat in?$"
