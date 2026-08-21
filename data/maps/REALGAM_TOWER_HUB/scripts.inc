REALGAM_TOWER_HUB_MapScripts::
	.byte 0

REALGAM_TOWER_HUB_EventScript_BattleBingoAttendant::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_FWG_BINGO_CARD, REALGAM_TOWER_HUB_EventScript_BattleBingoPrompt
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoIntro, MSGBOX_DEFAULT
	giveitem ITEM_FWG_BINGO_CARD
	goto_if_eq VAR_RESULT, FALSE, REALGAM_TOWER_HUB_EventScript_BattleBingoIntroEnd
	setflag FLAG_RECEIVED_FWG_BINGO_CARD
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoComeBack, MSGBOX_DEFAULT
REALGAM_TOWER_HUB_EventScript_BattleBingoIntroEnd::
	release
	end

REALGAM_TOWER_HUB_EventScript_BattleBingoPrompt::
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoWelcome, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, REALGAM_TOWER_HUB_EventScript_StartBattleBingo
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoDecline, MSGBOX_DEFAULT
	release
	end

REALGAM_TOWER_HUB_EventScript_StartBattleBingo::
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoFireBoard, MSGBOX_DEFAULT
	special ShowBattleBingoStarterSelect
	special ShowBattleBingoBoard
	specialvar VAR_RESULT, BattleBingoGetLastResult
	goto_if_eq VAR_RESULT, BATTLE_BINGO_RESULT_PRIZE, REALGAM_TOWER_HUB_EventScript_BattleBingoPrize
	goto_if_eq VAR_RESULT, BATTLE_BINGO_RESULT_COMPLETE, REALGAM_TOWER_HUB_EventScript_BattleBingoPrize
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoLossTryAgain, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, REALGAM_TOWER_HUB_EventScript_StartBattleBingo
	release
	end

REALGAM_TOWER_HUB_EventScript_BattleBingoPrize::
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoPrize, MSGBOX_DEFAULT
	specialvar VAR_RESULT, BattleBingoGetLastPrizeMoney
	buffernumberstring STR_VAR_1, VAR_RESULT
	playfanfare MUS_OBTAIN_ITEM
	message REALGAM_TOWER_HUB_Text_BattleBingoReceivedPrizeMoney
	waitmessage
	waitfanfare
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoPlayAgain, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, REALGAM_TOWER_HUB_EventScript_StartBattleBingo
	release
	end

REALGAM_TOWER_HUB_Text_BattleBingoIntro:
	.string "Hey There!\n"
	.string "Welcome to Realgam Tower!\p"
	.string "Our colosseum is currently\n"
	.string "undergoing maintenance due to a\l"
	.string "helicopter crash, but there's\l"
	.string "good news!\p"
	.string "We recently spent some time\n"
	.string "overhauling our battle bingo\l"
	.string "cards, and our simulator is\l"
	.string "still up and running.\p"
	.string "You should give it a try!\n"
	.string "You can win a lot of money if\l"
	.string "you strategize correctly.\p"
	.string "In fact, here, take this,\n"
	.string "it's on the house!$"

REALGAM_TOWER_HUB_Text_BattleBingoComeBack:
	.string "If you want to give it a try,\n"
	.string "just come talk to me and we'll\l"
	.string "get you set up!$"

REALGAM_TOWER_HUB_Text_BattleBingoWelcome:
	.string "Welcome to BATTLE BINGO!\p"
	.string "Would you like to play a card?$"

REALGAM_TOWER_HUB_Text_BattleBingoDecline:
	.string "No worries. Come back whenever\n"
	.string "you want to play.$"

REALGAM_TOWER_HUB_Text_BattleBingoFireBoard:
	.string "For now, I'll set up the FWG BOARD.$"

REALGAM_TOWER_HUB_Text_BattleBingoLossTryAgain:
	.string "Nice try! You'll get 'em next time.\p"
	.string "Would you like to try again?$"

REALGAM_TOWER_HUB_Text_BattleBingoPrize:
	.string "Congratulations! Here's your prize\n"
	.string "money based on your efforts.$"

REALGAM_TOWER_HUB_Text_BattleBingoReceivedPrizeMoney:
	.string "{PLAYER} received ¥{STR_VAR_1}!$"

REALGAM_TOWER_HUB_Text_BattleBingoPlayAgain:
	.string "Would you like to play another\n"
	.string "round of BATTLE BINGO?$"
