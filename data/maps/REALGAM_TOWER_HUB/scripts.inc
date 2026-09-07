REALGAM_TOWER_HUB_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, REALGAM_TOWER_HUB_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, REALGAM_TOWER_HUB_OnFrame
	.byte 0

REALGAM_TOWER_HUB_OnTransition:
	setvar VAR_TEMP_1, 0
	goto_if_set FLAG_REALGAM_HUB_CIPHER_CREDITS_PENDING, REALGAM_TOWER_HUB_EventScript_QueueCipherCreditsReturn
	end

REALGAM_TOWER_HUB_EventScript_QueueCipherCreditsReturn::
	setvar VAR_TEMP_1, 1
	end

REALGAM_TOWER_HUB_OnFrame:
	map_script_2 VAR_TEMP_1, 1, REALGAM_TOWER_HUB_EventScript_CipherCreditsReturn
	.2byte 0

REALGAM_TOWER_HUB_EventScript_CipherCreditsReturn::
	lockall
	setvar VAR_TEMP_1, 0
	clearflag FLAG_REALGAM_HUB_CIPHER_CREDITS_PENDING
	turnobject LOCALID_PLAYER, DIR_NORTH
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoCipherCongrats, MSGBOX_DEFAULT
	releaseall
	end

REALGAM_TOWER_HUB_EventScript_BattleBingoAttendant::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_FWG_BINGO_CARD, REALGAM_TOWER_HUB_EventScript_BattleBingoCheckCipherReward
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoIntro, MSGBOX_DEFAULT
	giveitem ITEM_FWG_BINGO_CARD
	goto_if_eq VAR_RESULT, FALSE, REALGAM_TOWER_HUB_EventScript_BattleBingoIntroEnd
	setflag FLAG_RECEIVED_FWG_BINGO_CARD
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoComeBack, MSGBOX_DEFAULT
REALGAM_TOWER_HUB_EventScript_BattleBingoIntroEnd::
	release
	end

REALGAM_TOWER_HUB_EventScript_BattleBingoCheckCipherReward::
	specialvar VAR_RESULT, BattleBingoShouldAwardCipherCard
	goto_if_eq VAR_RESULT, TRUE, REALGAM_TOWER_HUB_EventScript_BattleBingoAwardCipherCard
	goto REALGAM_TOWER_HUB_EventScript_BattleBingoPrompt

REALGAM_TOWER_HUB_EventScript_BattleBingoPrompt::
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoWelcome, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, REALGAM_TOWER_HUB_EventScript_StartBattleBingo
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoDecline, MSGBOX_DEFAULT
	release
	end

REALGAM_TOWER_HUB_EventScript_StartBattleBingo::
	specialvar VAR_RESULT, BattleBingoPrepareCardSelection
	goto_if_eq VAR_RESULT, 0, REALGAM_TOWER_HUB_EventScript_BattleBingoNoCards
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoChooseCard, MSGBOX_DEFAULT
	dynmultistack 17, 1, FALSE, 6, FALSE, 0, DYN_MULTICHOICE_CB_NONE
	goto_if_eq VAR_RESULT, MULTI_B_PRESSED, REALGAM_TOWER_HUB_EventScript_BattleBingoPrompt
	special StartSelectedBattleBingoBoard
	specialvar VAR_RESULT, BattleBingoGetLastResult
	goto_if_eq VAR_RESULT, BATTLE_BINGO_RESULT_PRIZE, REALGAM_TOWER_HUB_EventScript_BattleBingoPrize
	goto_if_eq VAR_RESULT, BATTLE_BINGO_RESULT_COMPLETE, REALGAM_TOWER_HUB_EventScript_BattleBingoPrize
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoLossTryAgain, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, REALGAM_TOWER_HUB_EventScript_StartBattleBingo
	release
	end

REALGAM_TOWER_HUB_EventScript_BattleBingoNoCards::
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoNoCards, MSGBOX_DEFAULT
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
	specialvar VAR_RESULT, BattleBingoDidFirstClearCipherBoard
	goto_if_eq VAR_RESULT, TRUE, REALGAM_TOWER_HUB_EventScript_BattleBingoCipherCleared
	specialvar VAR_RESULT, BattleBingoShouldAwardCipherCard
	goto_if_eq VAR_RESULT, TRUE, REALGAM_TOWER_HUB_EventScript_BattleBingoAwardCipherCard
REALGAM_TOWER_HUB_EventScript_BattleBingoPlayAgainPrompt::
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoPlayAgain, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, REALGAM_TOWER_HUB_EventScript_StartBattleBingo
	release
	end

REALGAM_TOWER_HUB_EventScript_BattleBingoAwardCipherCard::
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoCipherUnlocked, MSGBOX_DEFAULT
	giveitem ITEM_CIPHER_BINGO_CARD
	goto_if_eq VAR_RESULT, FALSE, REALGAM_TOWER_HUB_EventScript_BattleBingoPlayAgainPrompt
	setflag FLAG_RECEIVED_CIPHER_BINGO_CARD_ATTENDANT
	msgbox REALGAM_TOWER_HUB_Text_BattleBingoCipherBestShot, MSGBOX_DEFAULT
	goto REALGAM_TOWER_HUB_EventScript_BattleBingoPlayAgainPrompt

REALGAM_TOWER_HUB_EventScript_BattleBingoCipherCleared::
	setflag FLAG_REALGAM_HUB_CIPHER_CREDITS_PENDING
	special CB2_StartCreditsSequence
	release
	end

REALGAM_TOWER_HUB_EventScript_OldMan::
	faceplayer
	lock
	msgbox REALGAM_TOWER_HUB_Text_OldMan
	release
	end

REALGAM_TOWER_HUB_EventScript_OldWoman::
	faceplayer
	lock
	msgbox REALGAM_TOWER_HUB_Text_OldWoman
	release
	end

REALGAM_TOWER_HUB_EventScript_Sidney::
	faceplayer
	lock
	goto_if_set FLAG_REALGAM_HUB_SIDNEY_PASSWORD, REALGAM_TOWER_HUB_EventScript_SidneyPassword
	msgbox REALGAM_TOWER_HUB_Text_SidneyIntro1, MSGBOX_DEFAULT
	msgbox REALGAM_TOWER_HUB_Text_SidneyIntro2, MSGBOX_DEFAULT
	showmoneybox 0, 0
	msgbox REALGAM_TOWER_HUB_Text_SidneyOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, REALGAM_TOWER_HUB_EventScript_SidneyDecline
	checkmoney 250000
	goto_if_eq VAR_RESULT, FALSE, REALGAM_TOWER_HUB_EventScript_SidneyDecline
	removemoney 250000
	updatemoneybox
	hidemoneybox
	setflag FLAG_REALGAM_HUB_SIDNEY_PASSWORD
	goto REALGAM_TOWER_HUB_EventScript_SidneyPassword

REALGAM_TOWER_HUB_EventScript_SidneyDecline::
	hidemoneybox
	msgbox REALGAM_TOWER_HUB_Text_SidneyDecline, MSGBOX_DEFAULT
	release
	end

REALGAM_TOWER_HUB_EventScript_SidneyPassword::
	msgbox REALGAM_TOWER_HUB_Text_SidneyPassword, MSGBOX_DEFAULT
	release
	end

REALGAM_TOWER_HUB_Text_BattleBingoIntro:
	.string "Hey There! Welcome to Realgam Tower!\p"
	.string "Our colosseum is currently undergoing\n"
	.string "maintenance due to a helicopter crash,\l"
	.string "but there's good news!\p"
	.string "We recently spent some time overhauling\n"
	.string "our battle bingo cards, and our simulator\l"
	.string "is still up and running.\p"
	.string "You should give it a try! You can win a lot\n"
	.string "of money if you strategize correctly.\p"
	.string "In fact, here, take this, it's on the\n"
	.string "house!$"

REALGAM_TOWER_HUB_Text_BattleBingoComeBack:
	.string "If you want to give it a try, just come\n"
	.string "talk to me and we'll get you set up!$"

REALGAM_TOWER_HUB_Text_BattleBingoWelcome:
	.string "Welcome to Battle Bingo!\p"
	.string "Would you like to play a card?$"

REALGAM_TOWER_HUB_Text_BattleBingoDecline:
	.string "No worries. Come back whenever you\n"
	.string "want to play.$"

REALGAM_TOWER_HUB_Text_BattleBingoChooseCard:
	.string "Which Battle Bingo card would you like\n"
	.string "to play?$"

REALGAM_TOWER_HUB_Text_BattleBingoNoCards:
	.string "You don't have any Battle Bingo cards\n"
	.string "right now, come back later.$"

REALGAM_TOWER_HUB_Text_BattleBingoLossTryAgain:
	.string "Nice try! You'll get 'em next time.\p"
	.string "Would you like to try again?$"

REALGAM_TOWER_HUB_Text_BattleBingoPrize:
	.string "Congratulations! Here's your prize money\n"
	.string "based on your efforts.$"

REALGAM_TOWER_HUB_Text_BattleBingoReceivedPrizeMoney:
	.string "{PLAYER} received ¥{STR_VAR_1}!$"

REALGAM_TOWER_HUB_Text_BattleBingoPlayAgain:
	.string "Would you like to play another round of\n"
	.string "Battle Bingo?$"

REALGAM_TOWER_HUB_Text_BattleBingoCipherUnlocked:
	.string "Hold on... You've cleared every other\n"
	.string "bingo card we've got!\p"
	.string "That means you're ready for this.\n"
	.string "No one has ever beaten it before!$"

REALGAM_TOWER_HUB_Text_BattleBingoCipherBestShot:
	.string "Give it your best shot!$"

REALGAM_TOWER_HUB_Text_BattleBingoCipherCongrats:
	.string "Congratulations! You beat the hardest\n"
	.string "Battle Bingo card ever made.\p"
	.string "I knew you were good, but that was\n"
	.string "something else entirely.$"

REALGAM_TOWER_HUB_Text_OldMan:
	.string "Back in my day, bingo was just numbers\n"
	.string "and lines.\p"
	.string "Why do we always have to overcomplicate\n"
	.string "things nowadays?$"

REALGAM_TOWER_HUB_Text_OldWoman:
	.string "It's important to be strategic with\n"
	.string "the first square you pick.\p"
	.string "Afterwards, you can only move up, down,\n"
	.string "left, or right.$"

REALGAM_TOWER_HUB_Text_SidneyIntro1:
	.string "Hey kid! I came all the way from Hoenn\n"
	.string "to give this a try. But damn, this game\l"
	.string "is hard!$"

REALGAM_TOWER_HUB_Text_SidneyIntro2:
	.string "Luckily, there's a guy I know who can...\n"
	.string "help, if you get my meaning.$"

REALGAM_TOWER_HUB_Text_SidneyOffer:
	.string "Tell ya' what, if you give me a quarter\n"
	.string "of a mil, I'll teach you the password to\l"
	.string "reach him.$"

REALGAM_TOWER_HUB_Text_SidneyDecline:
	.string "Hmph! Your loss. I'll be here if you\n"
	.string "change your mind kid.$"

REALGAM_TOWER_HUB_Text_SidneyPassword:
	.string "Pleasure doing business with you! Go\n"
	.string "back out into the desert, find the third\l"
	.string "of the cactus siblings, and whisper to\l"
	.string "it saying... Ooo, you touch my tralala.\p"
	.string "Once you do that, you'll find him nice\n"
	.string "and easily.$"

REALGAM_TOWER_HUB_EventScript_BingoTable1::
	lock
	msgbox REALGAM_TOWER_HUB_Text_BingoTable1
	release
	end

REALGAM_TOWER_HUB_Text_BingoTable1:
	.string "It's a self-service bingo table.\p"
	.string "It looks like someone was playing\n"
	.string "and left after getting tilted.$"

REALGAM_TOWER_HUB_EventScript_BingoTable2::
	lock
	msgbox REALGAM_TOWER_HUB_Text_BingoTable2
	release
	end

REALGAM_TOWER_HUB_Text_BingoTable2:
	.string "It's a self-service bingo table.\p"
	.string "Someone was playing the FWG card\n"
	.string "and wiped to the bingo boss.$"

REALGAM_TOWER_HUB_EventScript_BingoTable3::
	lock
	msgbox REALGAM_TOWER_HUB_Text_BingoTable3
	release
	end

REALGAM_TOWER_HUB_Text_BingoTable3:
	.string "It's a self-service bingo table.\p"
	.string "It looks like it's out of order after\n"
	.string "someone smashed the screen.$"

REALGAM_TOWER_HUB_EventScript_BingoTable4::
	lock
	goto_if_set FLAG_REALGAM_HUB_STRANGE_BC_TABLE, REALGAM_TOWER_HUB_EventScript_BingoTable4Taken
	msgbox REALGAM_TOWER_HUB_Text_BingoTable4, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, REALGAM_TOWER_HUB_EventScript_BingoTable4End
	giveitem ITEM_STRANGE_BINGO_CARD
	goto_if_eq VAR_RESULT, FALSE, REALGAM_TOWER_HUB_EventScript_BingoTable4End
	setflag FLAG_REALGAM_HUB_STRANGE_BC_TABLE
REALGAM_TOWER_HUB_EventScript_BingoTable4End::
	release
	end

REALGAM_TOWER_HUB_EventScript_BingoTable4Taken::
	msgbox REALGAM_TOWER_HUB_Text_BingoTable4Taken, MSGBOX_DEFAULT
	release
	end

REALGAM_TOWER_HUB_Text_BingoTable4:
	.string "It's a self-service bingo table.\p"
	.string "Wait... someone left a bingo card behind\n"
	.string "with a note saying 'Fuck this'\p"
	.string "Do you want to take it?$"

REALGAM_TOWER_HUB_Text_BingoTable4Taken:
	.string "It's a self-service bingo table.$"
