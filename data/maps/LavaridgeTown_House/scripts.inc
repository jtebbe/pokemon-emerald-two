LavaridgeTown_House_MapScripts::
	.byte 0

LavaridgeTown_House_EventScript_OldMan::
	lock
	faceplayer
	msgbox LavaridgeTown_House_Text_WantChangeTeraType, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, LavaridgeTown_House_EventScript_ComeAgainAnytime
	goto LavaridgeTown_House_EventScript_Who
	end

LavaridgeTown_House_EventScript_ComeAgainAnytime::
	msgbox LavaridgeTown_House_Text_ComeAgain, MSGBOX_NPC
	release
	end

LavaridgeTown_House_EventScript_AndDone::
	msgbox LavaridgeTown_House_Text_WantChangeMore, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, LavaridgeTown_House_EventScript_ComeAgainAnytime
	goto LavaridgeTown_House_EventScript_Who
	end

LavaridgeTown_House_EventScript_Who::
    msgbox LavaridgeTown_House_Text_WhichMon, MSGBOX_DEFAULT
	special ChoosePartyMon
	waitstate
	goto_if_ne VAR_0x8004, PARTY_NOTHING_CHOSEN, LavaridgeTown_House_EventScript_ChangeTeraType
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, LavaridgeTown_House_EventScript_ComeAgainAnytime
	end

LavaridgeTown_House_EventScript_ChangeTeraType::
	specialvar VAR_RESULT, ScriptGetPartyMonSpecies
	goto_if_eq VAR_RESULT, SPECIES_EGG, LavaridgeTown_House_EventScript_Egg
	msgbox LavaridgeTown_House_Text_WhichType, MSGBOX_DEFAULT
	dynmultichoice 1, 1, TRUE, 4, 0, DYN_MULTICHOICE_CB_NONE, LavaridgeTown_House_ChangeTeraType_Text_0, LavaridgeTown_House_ChangeTeraType_Text_1, LavaridgeTown_House_ChangeTeraType_Text_2, LavaridgeTown_House_ChangeTeraType_Text_3, LavaridgeTown_House_ChangeTeraType_Text_4, LavaridgeTown_House_ChangeTeraType_Text_5, LavaridgeTown_House_ChangeTeraType_Text_6, LavaridgeTown_House_ChangeTeraType_Text_7, LavaridgeTown_House_ChangeTeraType_Text_8, LavaridgeTown_House_ChangeTeraType_Text_9, LavaridgeTown_House_ChangeTeraType_Text_10, LavaridgeTown_House_ChangeTeraType_Text_11, LavaridgeTown_House_ChangeTeraType_Text_12, LavaridgeTown_House_ChangeTeraType_Text_13, LavaridgeTown_House_ChangeTeraType_Text_14, LavaridgeTown_House_ChangeTeraType_Text_15, LavaridgeTown_House_ChangeTeraType_Text_16, LavaridgeTown_House_ChangeTeraType_Text_17, LavaridgeTown_House_ChangeTeraType_Text_18
	switch VAR_RESULT
	case 0, LavaridgeTown_House_EventScript_Normal
	case 1, LavaridgeTown_House_EventScript_Fighting
	case 2, LavaridgeTown_House_EventScript_Flying
	case 3, LavaridgeTown_House_EventScript_Poison
	case 4, LavaridgeTown_House_EventScript_Ground
	case 5, LavaridgeTown_House_EventScript_Rock
	case 6, LavaridgeTown_House_EventScript_Bug
	case 7, LavaridgeTown_House_EventScript_Ghost
	case 8, LavaridgeTown_House_EventScript_Steel
	case 9, LavaridgeTown_House_EventScript_Fire
	case 10, LavaridgeTown_House_EventScript_Water
	case 11, LavaridgeTown_House_EventScript_Grass
	case 12, LavaridgeTown_House_EventScript_Electric
	case 13, LavaridgeTown_House_EventScript_Psychic
	case 14, LavaridgeTown_House_EventScript_Ice
	case 15, LavaridgeTown_House_EventScript_Dragon
	case 16, LavaridgeTown_House_EventScript_Dark
	case 17, LavaridgeTown_House_EventScript_Fairy
	case 18, LavaridgeTown_House_EventScript_Stellar
	end

LavaridgeTown_House_EventScript_Normal::
	setteratype TYPE_NORMAL, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Fighting::
	setteratype TYPE_FIGHTING, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Flying::
	setteratype TYPE_FLYING, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Poison::
	setteratype TYPE_POISON, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Ground::
	setteratype TYPE_GROUND, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Rock::
	setteratype TYPE_ROCK, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Bug::
	setteratype TYPE_BUG, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Ghost::
	setteratype TYPE_GHOST, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Steel::
	setteratype TYPE_STEEL, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Fire::
	setteratype TYPE_FIRE, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Water::
	setteratype TYPE_WATER, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Grass::
	setteratype TYPE_GRASS, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Electric::
	setteratype TYPE_ELECTRIC, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Psychic::
	setteratype TYPE_PSYCHIC, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Ice::
	setteratype TYPE_ICE, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Dragon::
	setteratype TYPE_DRAGON, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Dark::
	setteratype TYPE_DARK, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Fairy::
	setteratype TYPE_FAIRY, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_EventScript_Stellar::
	setteratype TYPE_STELLAR, VAR_0x8004
	goto LavaridgeTown_House_EventScript_AndDone
	end

LavaridgeTown_House_ChangeTeraType_Text_0:
	.string "Normal$"

LavaridgeTown_House_ChangeTeraType_Text_1:
	.string "Fighting$"

LavaridgeTown_House_ChangeTeraType_Text_2:
	.string "Flying$"

LavaridgeTown_House_ChangeTeraType_Text_3:
	.string "Poison$"

LavaridgeTown_House_ChangeTeraType_Text_4:
	.string "Ground$"

LavaridgeTown_House_ChangeTeraType_Text_5:
	.string "Rock$"

LavaridgeTown_House_ChangeTeraType_Text_6:
	.string "Bug$"

LavaridgeTown_House_ChangeTeraType_Text_7:
	.string "Ghost$"

LavaridgeTown_House_ChangeTeraType_Text_8:
	.string "Steel$"

LavaridgeTown_House_ChangeTeraType_Text_9:
	.string "Fire$"

LavaridgeTown_House_ChangeTeraType_Text_10:
	.string "Water$"

LavaridgeTown_House_ChangeTeraType_Text_11:
	.string "Grass$"

LavaridgeTown_House_ChangeTeraType_Text_12:
	.string "Electric$"

LavaridgeTown_House_ChangeTeraType_Text_13:
	.string "Psychic$"

LavaridgeTown_House_ChangeTeraType_Text_14:
	.string "Ice$"

LavaridgeTown_House_ChangeTeraType_Text_15:
	.string "Dragon$"

LavaridgeTown_House_ChangeTeraType_Text_16:
	.string "Dark$"

LavaridgeTown_House_ChangeTeraType_Text_17:
	.string "Fairy$"

LavaridgeTown_House_ChangeTeraType_Text_18:
	.string "Stellar$"

LavaridgeTown_House_EventScript_Egg::
	msgbox LavaridgeTown_House_Text_CantRateEgg MSGBOX_DEFAULT
	goto LavaridgeTown_House_EventScript_ComeAgainAnytime
	end

LavaridgeTown_House_EventScript_Zigzagoon::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_ZIGZAGOON, CRY_MODE_NORMAL
	msgbox LavaridgeTown_House_Text_Zigzagoon, MSGBOX_DEFAULT
	waitmoncry
	release
	end

LavaridgeTown_House_Text_WantChangeTeraType:
	.string "Hey there! Would you like to change\n"
	.string "your partner's tera type?$"

LavaridgeTown_House_Text_CantRateEgg:
	.string "Uh... I can't do this to an egg.$"

LavaridgeTown_House_Text_WhichMon:
	.string "Who needs changing?$"

LavaridgeTown_House_Text_WhichType:
	.string "What tera type should they have?$"

LavaridgeTown_House_Text_ComeAgain:
	.string "Come again anytime!$"

LavaridgeTown_House_Text_WantChangeMore:
	.string "And... done!\n"
	.string "Do you want to make another change?$"

LavaridgeTown_House_Text_Zigzagoon:
	.string "ZIGZAGOON: Pshoo!$"

