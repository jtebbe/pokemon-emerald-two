REALGAM_MART_MapScripts::
	.byte 0

REALGAM_MART_EventScript_Clerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	special SetNextPokemartBuyOnly
	pokemart REALGAM_MART_Pokemart
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

REALGAM_MART_EventScript_LittleCupCardManiac::
	lock
	faceplayer
	checkitem ITEM_LC_BINGO_CARD
	goto_if_eq VAR_RESULT, TRUE, REALGAM_MART_EventScript_LittleCupCardManiacAlreadySold
	msgbox REALGAM_MART_Text_GotLastLittleCupCard, MSGBOX_DEFAULT
	msgbox REALGAM_MART_Text_LittleCupCardRare, MSGBOX_DEFAULT
	showmoneybox 0, 0
	msgbox REALGAM_MART_Text_LittleCupCardPrice, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, REALGAM_MART_EventScript_LittleCupCardManiacDecline
	checkmoney 100000
	goto_if_eq VAR_RESULT, FALSE, REALGAM_MART_EventScript_LittleCupCardManiacDecline
	checkitemspace ITEM_LC_BINGO_CARD
	goto_if_eq VAR_RESULT, FALSE, REALGAM_MART_EventScript_LittleCupCardManiacNoRoom
	msgbox REALGAM_MART_Text_LittleCupCardSale, MSGBOX_DEFAULT
	removemoney 100000
	updatemoneybox
	hidemoneybox
	giveitem ITEM_LC_BINGO_CARD
	msgbox REALGAM_MART_Text_LittleCupCardAfterSale, MSGBOX_DEFAULT
	release
	end

REALGAM_MART_EventScript_LittleCupCardManiacDecline::
	hidemoneybox
	msgbox REALGAM_MART_Text_LittleCupCardDecline, MSGBOX_DEFAULT
	release
	end

REALGAM_MART_EventScript_LittleCupCardManiacNoRoom::
	hidemoneybox
	msgbox gText_TooBadBagIsFull, MSGBOX_DEFAULT
	release
	end

REALGAM_MART_EventScript_LittleCupCardManiacAlreadySold::
	msgbox REALGAM_MART_Text_LittleCupCardAfterSale, MSGBOX_DEFAULT
	release
	end

REALGAM_MART_EventScript_RichBoy::
	lock
	faceplayer
	msgbox REALGAM_MART_Text_RichBoySmuggleRumor, MSGBOX_DEFAULT
	release
	end

	.align 2
REALGAM_MART_Pokemart:
	.2byte ITEM_NORMAL_BINGO_CARD
	pokemartlistend

REALGAM_MART_Text_GotLastLittleCupCard:
	.string "Haha! I got the last LITTLE CUP\n"
	.string "BINGO CARD from the shop just now!\p"
	.string "It's full of the cutest little guys.$"

REALGAM_MART_Text_LittleCupCardRare:
	.string "What's that? You want it?\p"
	.string "You're gonna have to pay a LOT\n"
	.string "because it's that rare, you know?$"

REALGAM_MART_Text_LittleCupCardPrice:
	.string "I suppose I can let it go for...\n"
	.string "say, ¥100,000.\p"
	.string "Do you have that kind of money?$"

REALGAM_MART_Text_LittleCupCardDecline:
	.string "I thought not, it's a prestigious\n"
	.string "collector's item after all!$"

REALGAM_MART_Text_LittleCupCardSale:
	.string "Really? You have that much?\p"
	.string "I suppose I can go find another\n"
	.string "and still make profit.\p"
	.string "Here, take it then.$"

REALGAM_MART_Text_LittleCupCardAfterSale:
	.string "Enjoy those cuties!\p"
	.string "I'm going to go on eBayleef later\n"
	.string "and see if I can get it cheaper.$"

REALGAM_MART_Text_RichBoySmuggleRumor:
	.string "I hear that with enough money,\n"
	.string "there's a guy who can smuggle\l"
	.string "items into the bingo simulations\l"
	.string "for you.\p"
	.string "They say he's a bit eccentric\n"
	.string "though, so he's probably hiding\l"
	.string "somewhere odd.\p"
	.string "I'll have to check with my hedge\n"
	.string "fund-managing dad to see if he'll\l"
	.string "lend me some cash when I find\l"
	.string "the guy!$"
