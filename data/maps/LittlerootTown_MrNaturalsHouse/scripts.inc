LittlerootTown_MrNaturalsHouse_MapScripts::
	.byte 0

LittlerootTown_MrNaturalsHouse_EventScript_MrNatural::
	lock
	faceplayer
	message LittlerootTown_MrNaturalsHouse_Text_MrNaturalGreeting
	waitmessage
	pokemart LittlerootTown_MrNaturalsHouse_Mart_MrNatural
	message LittlerootTown_MrNaturalsHouse_Text_MrNaturalGoodbye
	release
	end

LittlerootTown_MrNaturalsHouse_Mart_MrNatural:
	.2byte ITEM_LONELY_MINT
	.2byte ITEM_ADAMANT_MINT
	.2byte ITEM_NAUGHTY_MINT
	.2byte ITEM_BRAVE_MINT
	.2byte ITEM_BOLD_MINT
	.2byte ITEM_IMPISH_MINT
	.2byte ITEM_LAX_MINT
	.2byte ITEM_RELAXED_MINT
	.2byte ITEM_MODEST_MINT
	.2byte ITEM_MILD_MINT
	.2byte ITEM_RASH_MINT
	.2byte ITEM_QUIET_MINT
	.2byte ITEM_CALM_MINT
	.2byte ITEM_GENTLE_MINT
	.2byte ITEM_CAREFUL_MINT
	.2byte ITEM_SASSY_MINT
	.2byte ITEM_TIMID_MINT
	.2byte ITEM_HASTY_MINT
	.2byte ITEM_JOLLY_MINT
	.2byte ITEM_NAIVE_MINT
	.2byte ITEM_SERIOUS_MINT
	pokemartlistend

LittlerootTown_MrNaturalsHouse_Text_MrNaturalGreeting:
	.string "Ah! You and your partners are in\n"
	.string "need of my services?$"

LittlerootTown_MrNaturalsHouse_Text_MrNaturalGoodbye:
	.string "Au Revoir!$"
