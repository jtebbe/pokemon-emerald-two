PetalburgCity_Mart_MapScripts::
	.byte 0

PetalburgCity_Mart_EventScript_EvoItemSeller::
	lock
	faceplayer
	goto_if_set FLAG_BADGE03_GET, PetalburgCity_Mart_EventScript_Nice
	msgbox PetalburgCity_Mart_Text_ComeBackLater, MSGBOX_DEFAULT
	release
	end

PetalburgCity_Mart_EventScript_Nice::
	message gText_HowMayIServeYou
	waitmessage
	pokemart PetalburgCity_Mart_Pokemart_EvolutionItems
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

PetalburgCity_Mart_EventScript_Clerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	goto_if_set FLAG_IS_CHAMPION, PetalburgCity_Mart_Champion
	goto_if_set FLAG_BADGE08_GET, PetalburgCity_Mart_Badge8
	goto_if_set FLAG_BADGE07_GET, PetalburgCity_Mart_Badge7
	goto_if_set FLAG_BADGE06_GET, PetalburgCity_Mart_Badge6
	goto_if_set FLAG_BADGE05_GET, PetalburgCity_Mart_Badge5
	goto_if_set FLAG_BADGE04_GET, PetalburgCity_Mart_Badge4
	goto_if_set FLAG_BADGE03_GET, PetalburgCity_Mart_Badge3
	goto_if_set FLAG_BADGE02_GET, PetalburgCity_Mart_Badge2
	goto_if_set FLAG_BADGE01_GET, PetalburgCity_Mart_Badge1
	goto_if_set FLAG_ADVENTURE_STARTED, PetalburgCity_Mart_Badge0
	pokemart PetalburgCity_Mart_Pokemart_PreAdventure
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

PetalburgCity_Mart_Badge0::
	pokemart PetalburgCity_Mart_Pokemart_Badge0
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

PetalburgCity_Mart_Badge1::
	pokemart PetalburgCity_Mart_Pokemart_Badge1
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

PetalburgCity_Mart_Badge2::
	pokemart PetalburgCity_Mart_Pokemart_Badge2
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

PetalburgCity_Mart_Badge3::
	pokemart PetalburgCity_Mart_Pokemart_Badge3
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

PetalburgCity_Mart_Badge4::
	pokemart PetalburgCity_Mart_Pokemart_Badge4
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

PetalburgCity_Mart_Badge5::
	pokemart PetalburgCity_Mart_Pokemart_Badge5
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

PetalburgCity_Mart_Badge6::
	pokemart PetalburgCity_Mart_Pokemart_Badge6
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

PetalburgCity_Mart_Badge7::
	pokemart PetalburgCity_Mart_Pokemart_Badge7
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

PetalburgCity_Mart_Badge8::
	pokemart PetalburgCity_Mart_Pokemart_Badge8
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

PetalburgCity_Mart_Champion::
	pokemart PetalburgCity_Mart_Pokemart_BadgeChampion
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
PetalburgCity_Mart_Pokemart_EvolutionItems:
	.2byte ITEM_FIRE_STONE
	.2byte ITEM_WATER_STONE
	.2byte ITEM_THUNDER_STONE
	.2byte ITEM_LEAF_STONE
	.2byte ITEM_MOON_STONE
	.2byte ITEM_SUN_STONE
	.2byte ITEM_SHINY_STONE
	.2byte ITEM_DUSK_STONE
	.2byte ITEM_DAWN_STONE
	.2byte ITEM_ICE_STONE
	.2byte ITEM_AUSPICIOUS_ARMOR
	.2byte ITEM_MALICIOUS_ARMOR
	.2byte ITEM_BLACK_AUGURITE
	.2byte ITEM_CHIPPED_POT
	.2byte ITEM_CRACKED_POT
	.2byte ITEM_GALARICA_CUFF
	.2byte ITEM_GALARICA_WREATH
	.2byte ITEM_MASTERPIECE_TEACUP
	.2byte ITEM_UNREMARKABLE_TEACUP
	.2byte ITEM_METAL_ALLOY
	.2byte ITEM_PEAT_BLOCK
	.2byte ITEM_SWEET_APPLE
	.2byte ITEM_SYRUPY_APPLE
	.2byte ITEM_TART_APPLE
	.2byte ITEM_DEEP_SEA_SCALE
	.2byte ITEM_DEEP_SEA_TOOTH
	.2byte ITEM_DRAGON_SCALE
	.2byte ITEM_UPGRADE
	.2byte ITEM_DUBIOUS_DISC
	.2byte ITEM_ELECTIRIZER
	.2byte ITEM_MAGMARIZER
	.2byte ITEM_METAL_COAT
	.2byte ITEM_PROTECTOR
	.2byte ITEM_KINGS_ROCK
	.2byte ITEM_OVAL_STONE
	.2byte ITEM_PRISM_SCALE
	.2byte ITEM_RAZOR_FANG
	.2byte ITEM_RAZOR_CLAW
	.2byte ITEM_REAPER_CLOTH
	.2byte ITEM_SACHET
	.2byte ITEM_WHIPPED_DREAM
	.2byte ITEM_STRAWBERRY_SWEET
	.2byte ITEM_LOVE_SWEET
	.2byte ITEM_STRAWBERRY_SWEET
	.2byte ITEM_CLOVER_SWEET
	.2byte ITEM_FLOWER_SWEET
	.2byte ITEM_STAR_SWEET
	.2byte ITEM_RIBBON_SWEET
	pokemartlistend

	.align 2
PetalburgCity_Mart_Pokemart_PreAdventure:
	.2byte ITEM_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_EXP_CANDY_XL
	pokemartlistend
	
	.align 2
PetalburgCity_Mart_Pokemart_Badge0:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_EXP_CANDY_XL
	.2byte ITEM_ABILITY_CAPSULE
	pokemartlistend

	.align 2
PetalburgCity_Mart_Pokemart_Badge1:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_REPEL
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_EXP_CANDY_XL
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_ABILITY_PATCH
	pokemartlistend

	.align 2
PetalburgCity_Mart_Pokemart_Badge2:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_REPEL
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_EXP_CANDY_XL
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_ABILITY_PATCH
	pokemartlistend

	.align 2
PetalburgCity_Mart_Pokemart_Badge3:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_LUXURY_BALL
	.2byte ITEM_NEST_BALL
	.2byte ITEM_NET_BALL
	.2byte ITEM_REPEAT_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_X_SP_ATK
	.2byte ITEM_X_SPEED
	.2byte ITEM_X_ATTACK
	.2byte ITEM_X_DEFENSE
	.2byte ITEM_DIRE_HIT
	.2byte ITEM_GUARD_SPEC
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_EXP_CANDY_XL
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_ABILITY_PATCH
	pokemartlistend

	.align 2
PetalburgCity_Mart_Pokemart_Badge4:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_LUXURY_BALL
	.2byte ITEM_NEST_BALL
	.2byte ITEM_NET_BALL
	.2byte ITEM_REPEAT_BALL
	.2byte ITEM_TIMER_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_X_SP_ATK
	.2byte ITEM_X_SPEED
	.2byte ITEM_X_ATTACK
	.2byte ITEM_X_DEFENSE
	.2byte ITEM_DIRE_HIT
	.2byte ITEM_GUARD_SPEC
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_EXP_CANDY_XL
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_ABILITY_PATCH
	pokemartlistend

	.align 2
PetalburgCity_Mart_Pokemart_Badge5:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_LUXURY_BALL
	.2byte ITEM_NEST_BALL
	.2byte ITEM_NET_BALL
	.2byte ITEM_REPEAT_BALL
	.2byte ITEM_TIMER_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_REVIVE
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_X_SP_ATK
	.2byte ITEM_X_SPEED
	.2byte ITEM_X_ATTACK
	.2byte ITEM_X_DEFENSE
	.2byte ITEM_DIRE_HIT
	.2byte ITEM_GUARD_SPEC
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_EXP_CANDY_XL
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_ABILITY_PATCH
	pokemartlistend

	.align 2
PetalburgCity_Mart_Pokemart_Badge6:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_LUXURY_BALL
	.2byte ITEM_NEST_BALL
	.2byte ITEM_NET_BALL
	.2byte ITEM_REPEAT_BALL
	.2byte ITEM_TIMER_BALL
	.2byte ITEM_DIVE_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_REVIVE
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_X_SP_ATK
	.2byte ITEM_X_SPEED
	.2byte ITEM_X_ATTACK
	.2byte ITEM_X_DEFENSE
	.2byte ITEM_DIRE_HIT
	.2byte ITEM_GUARD_SPEC
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_EXP_CANDY_XL
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_ABILITY_PATCH
	pokemartlistend

	.align 2
PetalburgCity_Mart_Pokemart_Badge7:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_LUXURY_BALL
	.2byte ITEM_NEST_BALL
	.2byte ITEM_NET_BALL
	.2byte ITEM_REPEAT_BALL
	.2byte ITEM_TIMER_BALL
	.2byte ITEM_DIVE_BALL
	.2byte ITEM_DREAM_BALL
	.2byte ITEM_DUSK_BALL
	.2byte ITEM_HEAL_BALL
	.2byte ITEM_QUICK_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_FULL_RESTORE
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_REVIVE
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_X_SP_ATK
	.2byte ITEM_X_SPEED
	.2byte ITEM_X_ATTACK
	.2byte ITEM_X_DEFENSE
	.2byte ITEM_DIRE_HIT
	.2byte ITEM_GUARD_SPEC
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_EXP_CANDY_XL
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_ABILITY_PATCH
	pokemartlistend
	
	.align 2
PetalburgCity_Mart_Pokemart_Badge8:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_LUXURY_BALL
	.2byte ITEM_NEST_BALL
	.2byte ITEM_NET_BALL
	.2byte ITEM_REPEAT_BALL
	.2byte ITEM_TIMER_BALL
	.2byte ITEM_DIVE_BALL
	.2byte ITEM_DREAM_BALL
	.2byte ITEM_DUSK_BALL
	.2byte ITEM_HEAL_BALL
	.2byte ITEM_QUICK_BALL
	.2byte ITEM_HEAVY_BALL
	.2byte ITEM_LURE_BALL
	.2byte ITEM_FRIEND_BALL
	.2byte ITEM_LOVE_BALL
	.2byte ITEM_LEVEL_BALL
	.2byte ITEM_FAST_BALL
	.2byte ITEM_MOON_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_FULL_RESTORE
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_REVIVE
	.2byte ITEM_MAX_REVIVE
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_X_SP_ATK
	.2byte ITEM_X_SPEED
	.2byte ITEM_X_ATTACK
	.2byte ITEM_X_DEFENSE
	.2byte ITEM_DIRE_HIT
	.2byte ITEM_GUARD_SPEC
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_EXP_CANDY_XL
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_ABILITY_PATCH
	pokemartlistend

	.align 2
PetalburgCity_Mart_Pokemart_BadgeChampion:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_MASTER_BALL
	.2byte ITEM_LUXURY_BALL
	.2byte ITEM_NEST_BALL
	.2byte ITEM_NET_BALL
	.2byte ITEM_REPEAT_BALL
	.2byte ITEM_TIMER_BALL
	.2byte ITEM_DIVE_BALL
	.2byte ITEM_DREAM_BALL
	.2byte ITEM_DUSK_BALL
	.2byte ITEM_HEAL_BALL
	.2byte ITEM_QUICK_BALL
	.2byte ITEM_HEAVY_BALL
	.2byte ITEM_LURE_BALL
	.2byte ITEM_FRIEND_BALL
	.2byte ITEM_LOVE_BALL
	.2byte ITEM_LEVEL_BALL
	.2byte ITEM_FAST_BALL
	.2byte ITEM_MOON_BALL
	.2byte ITEM_BEAST_BALL
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_HYPER_POTION
	.2byte ITEM_FULL_RESTORE
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_REVIVE
	.2byte ITEM_MAX_REVIVE
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_X_SP_ATK
	.2byte ITEM_X_SPEED
	.2byte ITEM_X_ATTACK
	.2byte ITEM_X_DEFENSE
	.2byte ITEM_DIRE_HIT
	.2byte ITEM_GUARD_SPEC
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_EXP_CANDY_XL
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_ABILITY_PATCH
	pokemartlistend

PetalburgCity_Mart_EventScript_Woman::
	msgbox PetalburgCity_Mart_Text_WeakWillGrowStronger, MSGBOX_NPC
	end

PetalburgCity_Mart_EventScript_Boy::
	msgbox PetalburgCity_Mart_Text_RepelIsUseful, MSGBOX_NPC
	end

PetalburgCity_Mart_EventScript_Man::
	msgbox PetalburgCity_Mart_Text_TakeSomeAntidotesWithYou, MSGBOX_NPC
	end

PetalburgCity_Mart_Text_WeakWillGrowStronger:
	.string "Even if a POKéMON is weak now,\n"
	.string "it will grow stronger.\p"
	.string "The most important thing is love!\n"
	.string "Love for your POKéMON!$"

PetalburgCity_Mart_Text_RepelIsUseful:
	.string "Do you use REPEL?\n"
	.string "It keeps POKéMON away, so it's\l"
	.string "useful when you're in a hurry.$"

PetalburgCity_Mart_Text_TakeSomeAntidotesWithYou:
	.string "Do you have any ANTIDOTES with\n"
	.string "you?\p"
	.string "If you walk around with a poisoned\n"
	.string "POKéMON, it will lose HP until it faints.\l"
	.string "Take some ANTIDOTES with you.$"

PetalburgCity_Mart_Text_ComeBackLater:
	.string "Hmm... Come back and see me after you\n"
	.string "get three badges. I'll sell you some\l"
	.string "items to help your partners evolve!$"

