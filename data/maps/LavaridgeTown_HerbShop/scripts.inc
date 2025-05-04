.set LOCALID_JAKE, 4

LavaridgeTown_HerbShop_MapScripts::
	.byte 0

LavaridgeTown_HerbShop_EventScript_Clerk::
	lock
	faceplayer
	message LavaridgeTown_HerbShop_Text_WelcomeToHerbShop
	waitmessage
	pokemart LavaridgeTown_HerbShop_Pokemart
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LavaridgeTown_HerbShop_Pokemart:
	.2byte ITEM_ENERGY_POWDER
	.2byte ITEM_ENERGY_ROOT
	.2byte ITEM_HEAL_POWDER
	.2byte ITEM_REVIVAL_HERB
	pokemartlistend

LavaridgeTown_HerbShop_EventScript_ExpertM::
	msgbox LavaridgeTown_HerbShop_Text_HerbalMedicineWorksButMonWillDislike, MSGBOX_NPC
	end

LavaridgeTown_HerbShop_EventScript_OldMan::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_CHARCOAL, LavaridgeTown_HerbShop_EventScript_ExplainCharcoal
	msgbox LavaridgeTown_HerbShop_Text_YouveComeToLookAtHerbalMedicine, MSGBOX_DEFAULT
	giveitem ITEM_CHARCOAL
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_CHARCOAL
	release
	end

LavaridgeTown_HerbShop_EventScript_ExplainCharcoal::
	msgbox LavaridgeTown_HerbShop_Text_ExplainCharcoal, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_HerbShop_Text_WelcomeToHerbShop:
	.string "Welcome to the HERB SHOP, home of\n"
	.string "effective and inexpensive medicine!$"

LavaridgeTown_HerbShop_Text_YouveComeToLookAtHerbalMedicine:
	.string "You've come to look at herbal medicine\n"
	.string "in LAVARIDGE?\p"
	.string "That's rather commendable.\p"
	.string "I like you! Take this!$"

LavaridgeTown_HerbShop_Text_ExplainCharcoal:
	.string "That CHARCOAL I gave you, it's used\n"
	.string "for making herbal medicine.\p"
	.string "It also does wonders when held by\n"
	.string "a POKéMON.\p"
	.string "It intensifies the power of FIRE-type\n"
	.string "moves.$"

LavaridgeTown_HerbShop_Text_HerbalMedicineWorksButMonWillDislike:
	.string "Herbal medicine works impressively well.\n"
	.string "But your POKéMON will dislike you for it.\l"
	.string "It must be horribly bitter!$"

LavaridgeTown_HerbShop_EventScript_Jake::
	lockall
	faceplayer
	msgbox LavaridgeTown_HerbShop_Text_JakeIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_JAKE, LavaridgeTown_HerbShop_Text_JakeDefeat
	msgbox LavaridgeTown_HerbShop_Text_JakePostDefeat, MSGBOX_DEFAULT
	giveitem ITEM_LIFE_ORB
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_JAKE
	fadescreen FADE_FROM_BLACK
	setflag FLAG_AGARTHAN_4_DEFEATED
	clearflag FLAG_SHOW_HOUSE_AGARTHAN_4
	releaseall
	end

LavaridgeTown_HerbShop_Text_JakeIntro:
	.string "Oh? So another comes to me before\n"
	.string "their time. How fortunate!\p"
	.string "The path to Agartha isn't an easy\n"
	.string "one, kid.\p"
	.string "Let's see if your heart can keep\n"
	.string "up with the stress, or if you'll\l"
	.string "just be another cadaver!$"

LavaridgeTown_HerbShop_Text_JakeDefeat:
	.string "What an adrenaline rush!$"

LavaridgeTown_HerbShop_Text_JakePostDefeat:
	.string "Well, seems you and your partners\n"
	.string "are in great physical condition,\l"
	.string "but Agartha yet demands more.\p"
	.string "I wonder if your mind will\n"
	.string "still crumble under the pressure.\p"
	.string "Still, you've earned this, and\n"
	.string "I will simply collect it should\l"
	.string "you fail, hehehe...$"

