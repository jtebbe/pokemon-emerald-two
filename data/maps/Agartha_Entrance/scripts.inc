Agartha_Entrance_MapScripts::
	.byte 0

Agartha_Entrance_EventScript_Nadroj::
	lockall
	msgbox Agartha_Entrance_Text_DotDotDot, MSGBOX_DEFAULT
	applymovement LOCALID_NADROJ, Common_Movement_FacePlayer
	waitmovement 0
	msgbox Agartha_Entrance_Text_YouveDefeatedTheOthers, MSGBOX_DEFAULT
	setvar VAR_0x8004, 1  @ vertical pan
	setvar VAR_0x8005, 1  @ horizontal pan
	setvar VAR_0x8006, 1  @ num shakes
	setvar VAR_0x8007, 5  @ shake delay
	special ShakeCamera
	waitstate
	delay 4
	msgbox Agartha_Entrance_Text_Fate, MSGBOX_DEFAULT
	setvar VAR_0x8004, 1  @ vertical pan
	setvar VAR_0x8005, 1  @ horizontal pan
	setvar VAR_0x8006, 2  @ num shakes
	setvar VAR_0x8007, 5  @ shake delay
	special ShakeCamera
	waitstate
	delay 4
	msgbox Agartha_Entrance_Text_YourMettle, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_NADROJ, Agartha_Entrance_Text_WellDone
	msgbox Agartha_Entrance_Text_MyDefeat, MSGBOX_DEFAULT
	giveitem ITEM_ASSAULT_VEST
	msgbox Agartha_Entrance_Text_ItIsDone, MSGBOX_DEFAULT
	giveitem ITEM_EON_TICKET
	giveitem ITEM_MYSTIC_TICKET
	giveitem ITEM_AURORA_TICKET
	giveitem ITEM_OLD_SEA_MAP
	setflag FLAG_ENABLE_SHIP_BIRTH_ISLAND
	setflag FLAG_ENABLE_SHIP_FARAWAY_ISLAND
	setflag FLAG_ENABLE_SHIP_NAVEL_ROCK
	setflag FLAG_ENABLE_SHIP_SOUTHERN_ISLAND
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_NADROJ
	fadescreen FADE_FROM_BLACK
	setflag FLAG_AGARTHAN_KING_DEFEATED
	releaseall
	end
	
Agartha_Entrance_Text_DotDotDot:
	.string "...\n"
	.string ".....\p"
	.string "So, a challenger once again sets\n"
	.string "foot upon the doorstep of Agartha.\p"
	.string "Claiming to be ready for a reward\n"
	.string "they believe themselves to be worthy\l"
	.string "of...$"

Agartha_Entrance_Text_YouveDefeatedTheOthers:
	.string "You've defeated my friends...\n"
	.string "but now you face me.\l"
	.string "I am your final obstacle.$"

Agartha_Entrance_Text_Fate:
	.string "Whether you are fated to enter\n"
	.string "matters little to me.$"

Agartha_Entrance_Text_YourMettle:
	.string "Your mettle, prove it to me!\p"
	.string "Defeat me, and the lands of\n"
	.string "Agartha shall open to you!$"

Agartha_Entrance_Text_WellDone:
	.string "... ha...\n"
	.string "... haha, well done.$"

Agartha_Entrance_Text_MyDefeat:
	.string "It's my loss. You've exceeded\n"
	.string "every possible expectation.\p"
	.string "Here, you deserve this.$"

Agartha_Entrance_Text_ItIsDone:
	.string "Well... I can see my old friend\p"
	.string "was right. You have potential\n"
	.string "enough to rival the greatest of\l"
	.string "trainers, to be sure.\p"
	.string "And so, Agartha opens her arms\n"
	.string "and welcomes you.\p"
	.string "And should you seek me out once\n"
	.string "more, my partners and I...\p"
	.string "We'll fight you with our true\n"
	.string "strength. But until then...\p"
	.string "Take these. They're yours.\n"
	.string "Goodbye, {PLAYER}.$"
