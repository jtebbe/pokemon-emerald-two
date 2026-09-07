#define LOCALID_REALGAM_DESERT_DRAGON_BC_BALL 1

REALGAM_DESERT_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, REALGAM_DESERT_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, REALGAM_DESERT_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, REALGAM_DESERT_OnFrame
	.byte 0

REALGAM_DESERT_OnLoad:
	call_if_set FLAG_THIRD_CACTUS_GONE, REALGAM_DESERT_EventScript_RemoveThirdCactusMetatiles
	end

REALGAM_DESERT_OnTransition:
	setflag FLAG_TEMP_1
	setvar VAR_TEMP_1, 0
	end

REALGAM_DESERT_EventScript_RemoveThirdCactusMetatiles::
	setmetatile 22, 3, 0x000, FALSE
	setmetatile 23, 3, 0x000, FALSE
	setmetatile 22, 4, 0x000, FALSE
	setmetatile 23, 4, 0x000, FALSE
	return

REALGAM_DESERT_OnFrame:
	map_script_2 VAR_TEMP_1, TRUE, REALGAM_DESERT_EventScript_DragonBingoCardDrop
	.2byte 0

REALGAM_DESERT_EventScript_DragonBingoCardDrop::
	setvar VAR_TEMP_1, 0
	goto_if_set FLAG_REALGAM_DESERT_DRAGON_BC_GIFT, REALGAM_DESERT_EventScript_DragonBingoCardDropEnd
	lockall
	setobjectxy LOCALID_REALGAM_DESERT_DRAGON_BC_BALL, 19, 18
	clearflag FLAG_TEMP_1
	addobject LOCALID_REALGAM_DESERT_DRAGON_BC_BALL
	playse SE_FALL
	applymovement LOCALID_REALGAM_DESERT_DRAGON_BC_BALL, REALGAM_DESERT_Movement_DragonBingoCardBallFall
	waitmovement 0
	setobjectxy LOCALID_REALGAM_DESERT_DRAGON_BC_BALL, 19, 24
	delay 8
	removeobject LOCALID_REALGAM_DESERT_DRAGON_BC_BALL
	giveitem ITEM_DRAGON_BINGO_CARD
	goto_if_eq VAR_RESULT, FALSE, REALGAM_DESERT_EventScript_DragonBingoCardDropRelease
	setflag FLAG_REALGAM_DESERT_DRAGON_BC_GIFT
REALGAM_DESERT_EventScript_DragonBingoCardDropRelease:
	releaseall
REALGAM_DESERT_EventScript_DragonBingoCardDropEnd:
	end

REALGAM_DESERT_Movement_DragonBingoCardBallFall:
	set_fixed_priority
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	step_end

REALGAM_DESERT_EventScript_ThirdCactus::
	goto_if_set FLAG_THIRD_CACTUS_GONE, REALGAM_DESERT_EventScript_ThirdCactusEnd
	goto_if_unset FLAG_REALGAM_HUB_SIDNEY_PASSWORD, REALGAM_DESERT_EventScript_ThirdCactusEnd
	lockall
	msgbox REALGAM_DESERT_Text_ThirdCactusPasswordSetup, MSGBOX_DEFAULT
	msgbox REALGAM_DESERT_Text_ThirdCactusPassword, MSGBOX_DEFAULT
	msgbox REALGAM_DESERT_Text_ThirdCactusLaughs, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	call REALGAM_DESERT_EventScript_RemoveThirdCactusMetatiles
	special DrawWholeMapView
	delay 60
	playse SE_FLEE
	waitse
	setflag FLAG_THIRD_CACTUS_GONE
	fadescreen FADE_FROM_BLACK
	releaseall
REALGAM_DESERT_EventScript_ThirdCactusEnd:
	end

REALGAM_DESERT_EventScript_PsychicM::
	lock
	faceplayer
	specialvar VAR_RESULT, BattleBingoCountClearedBoards
	goto_if_ge VAR_RESULT, 3, REALGAM_DESERT_EventScript_PsychicMDragonHint
	msgbox REALGAM_DESERT_Text_PsychicMSecrets, MSGBOX_DEFAULT
	msgbox REALGAM_DESERT_Text_PsychicMLostCardTrial, MSGBOX_DEFAULT
	release
	end

REALGAM_DESERT_EventScript_PsychicMDragonHint::
	msgbox REALGAM_DESERT_Text_PsychicMEnthusiast, MSGBOX_DEFAULT
	msgbox REALGAM_DESERT_Text_PsychicMDragonRumor, MSGBOX_DEFAULT
	release
	end

REALGAM_DESERT_Text_PsychicMSecrets:
	.string "The desert still hides many of cipher's\n"
	.string "secrets.\p"
	.string "Battle Bingo was once used for\n"
	.string "simulating how shadow pokemon fought,\l"
	.string "you know.$"

REALGAM_DESERT_Text_PsychicMLostCardTrial:
	.string "That reminds me... there was a tale about\n"
	.string "a certain lost bingo card that was\l"
	.string "passed down.\p"
	.string "If you can successfully clear three or\n"
	.string "more bingo cards, maybe I'll tell you\l"
	.string "about it.\p"
	.string "I just want to see if you're serious\n"
	.string "about it.$"

REALGAM_DESERT_Text_PsychicMEnthusiast:
	.string "You... You are a true battle bingo\n"
	.string "enthusiast, aren't you?\p"
	.string "Very well, listen closely.$"

REALGAM_DESERT_Text_PsychicMDragonRumor:
	.string "There are rumors of a dragon that flies\n"
	.string "over the desert, mourning its comrades\l"
	.string "taken and disposed of by cipher's\l"
	.string "experimental team.\p"
	.string "Should you, too, revere those fallen\n"
	.string "in silence, it may reward your empathy.$"

REALGAM_DESERT_Text_ThirdCactusPasswordSetup:
	.string "You say the password the sketchy guy\n"
	.string "told you:$"

REALGAM_DESERT_Text_ThirdCactusPassword:
	.string "Ooo, you touch my tralala.$"

REALGAM_DESERT_Text_ThirdCactusLaughs:
	.string "The cactus jiggles as though it's\n"
	.string "laughing, then...$"
