REALGAM_LOUNGE_MapScripts::
	.byte 0

REALGAM_LOUNGE_EventScript_Cameraman::
	lock
	faceplayer
	msgbox REALGAM_LOUNGE_Text_CameramanColosseum, MSGBOX_DEFAULT
	release
	end

REALGAM_LOUNGE_Text_CameramanColosseum:
	.string "Battle bingo is exciting, but I sure do\n"
	.string "hope the colosseum opens again soon.\p"
	.string "It's hard to make a living as a cameraman\n"
	.string "when most people just stream their\l"
	.string "battle bingo after all.$"

REALGAM_LOUNGE_EventScript_Man2::
	lock
	faceplayer
	msgbox REALGAM_LOUNGE_Text_Man2, MSGBOX_DEFAULT
	release
	end

REALGAM_LOUNGE_Text_Man2:
	.string "Honestly I'm surprised this place is\n"
	.string "still in business after they worked\l"
	.string "with Cipher like that.\p"
	.string "I hear there's even an unreleased card\n"
	.string "celebrating their collaboration hidden\l"
	.string "somewhere.$"

REALGAM_LOUNGE_EventScript_Picknicker::
	lock
	faceplayer
	msgbox REALGAM_LOUNGE_Text_Picknicker, MSGBOX_DEFAULT
	release
	end

REALGAM_LOUNGE_Text_Picknicker:
	.string "A lot of people come out here from all\n"
	.string "over Orre. Even with bingo centers in\l"
	.string "most major cities now!\p"
	.string "There must be something about trying\n"
	.string "it out where it originated.$"

REALGAM_LOUNGE_EventScript_Gentleman::
	lock
	faceplayer
	msgbox REALGAM_LOUNGE_Text_Gentleman, MSGBOX_DEFAULT
	release
	end

REALGAM_LOUNGE_Text_Gentleman:
	.string "Back in the day, battle bingo wasn't\n"
	.string "very popular. A lot of cards ended up\l"
	.string "on people's bookshelves, under beds,\l"
	.string "and even just on the ground.\p"
	.string "Now they're popular enough that they\n"
	.string "literally cause fights at major retailers!\l"
	.string "How silly is that?$"

REALGAM_LOUNGE_EventScript_Bookcase1::
	lock
	msgbox REALGAM_LOUNGE_Text_Bookcase1, MSGBOX_DEFAULT
	release
	end

REALGAM_LOUNGE_Text_Bookcase1:
	.string "Contemporary books on battle strats\n"
	.string "line the bookcase. You already read\l"
	.string "them during breaks at Team Snagem.$"

REALGAM_LOUNGE_EventScript_Bookcase2::
	lock
	msgbox REALGAM_LOUNGE_Text_Bookcase2, MSGBOX_DEFAULT
	release
	end

REALGAM_LOUNGE_Text_Bookcase2:
	.string "Among various books and magazines,\n"
	.string "you find a GQ article featuring a\l"
	.string "shirtless Archie, who took on a lucrative\l"
	.string "modeling career after departing Team\l"
	.string "Aqua. Nice.$"

REALGAM_LOUNGE_EventScript_LegendBingoBookcase::
	lock
	goto_if_set FLAG_REALGAM_LOUNGE_LEGEND_BC_GIFT, REALGAM_LOUNGE_EventScript_LegendBingoBookcaseEmpty
	msgbox REALGAM_LOUNGE_Text_LegendBingoBookcasePrompt, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, REALGAM_LOUNGE_EventScript_LegendBingoBookcaseEnd
	msgbox REALGAM_LOUNGE_Text_LegendBingoBookcaseFound, MSGBOX_DEFAULT
	giveitem ITEM_LEGEND_BINGO_CARD
	goto_if_eq VAR_RESULT, FALSE, REALGAM_LOUNGE_EventScript_LegendBingoBookcaseEnd
	setflag FLAG_REALGAM_LOUNGE_LEGEND_BC_GIFT
	release
	end

REALGAM_LOUNGE_EventScript_LegendBingoBookcaseEmpty::
	msgbox REALGAM_LOUNGE_Text_LegendBingoBookcaseEmpty, MSGBOX_DEFAULT

REALGAM_LOUNGE_EventScript_LegendBingoBookcaseEnd::
	release
	end

REALGAM_LOUNGE_Text_LegendBingoBookcasePrompt:
	.string "It looks like just a bunch of useless\n"
	.string "old manuals. Do you want to dig around\l"
	.string "in it?$"

REALGAM_LOUNGE_Text_LegendBingoBookcaseFound:
	.string "After rummaging around a bit, you\n"
	.string "find a loose bingo card!$"

REALGAM_LOUNGE_Text_LegendBingoBookcaseEmpty:
	.string "There's nothing more in here but\n"
	.string "dusty old books.$"

REALGAM_LOUNGE_EventScript_RichBoy::
	lock
	faceplayer
	msgbox REALGAM_LOUNGE_Text_RichBoy, MSGBOX_DEFAULT
	release
	end

REALGAM_LOUNGE_Text_RichBoy:
	.string "I can't believe it! I was able to get\n"
	.string "every bingo card ever sold on the\l"
	.string "market, and I'm still missing some?!\p"
	.string "No one told me money can't buy\n"
	.string "everything! This is bullshit!$"

REALGAM_LOUNGE_EventScript_Woman4::
	lock
	faceplayer
	msgbox REALGAM_LOUNGE_Text_Woman4, MSGBOX_DEFAULT
	release
	end

REALGAM_LOUNGE_Text_Woman4:
	.string "Hey there! Have you caught the bingo\n"
	.string "craze too?\p"
	.string "Did you also hear the rumor that there's\n"
	.string "a hidden bingo card hidden in this room\l"
	.string "somewhere?\p"
	.string "Wait, you didn't? Then back off!$"
