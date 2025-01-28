LavaridgeTown_Mart_MapScripts::
	.byte 0

LavaridgeTown_Mart_EventScript_Clerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	goto_if_set FLAG_IS_CHAMPION, LavaridgeTown_Mart_Pokemart_BadgeChampion
	goto_if_set FLAG_BADGE08_GET, LavaridgeTown_Mart_Badge8
	goto_if_set FLAG_BADGE07_GET, LavaridgeTown_Mart_Badge7
	goto_if_set FLAG_BADGE06_GET, LavaridgeTown_Mart_Badge6
	goto_if_set FLAG_BADGE05_GET, LavaridgeTown_Mart_Badge5
	goto_if_set FLAG_BADGE04_GET, LavaridgeTown_Mart_Badge4
	goto_if_set FLAG_BADGE03_GET, LavaridgeTown_Mart_Badge3
	goto_if_set FLAG_BADGE02_GET, LavaridgeTown_Mart_Badge2
	goto_if_set FLAG_BADGE01_GET, LavaridgeTown_Mart_Badge1
	goto_if_set FLAG_ADVENTURE_STARTED, LavaridgeTown_Mart_Badge0
	pokemart LavaridgeTown_Mart_Pokemart_PreAdventure
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_Mart_Badge0::
	pokemart LavaridgeTown_Mart_Pokemart_Badge0
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_Mart_Badge1::
	pokemart LavaridgeTown_Mart_Pokemart_Badge1
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_Mart_Badge2::
	pokemart LavaridgeTown_Mart_Pokemart_Badge2
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_Mart_Badge3::
	pokemart LavaridgeTown_Mart_Pokemart_Badge3
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_Mart_Badge4::
	pokemart LavaridgeTown_Mart_Pokemart_Badge4
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_Mart_Badge5::
	pokemart LavaridgeTown_Mart_Pokemart_Badge5
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_Mart_Badge6::
	pokemart LavaridgeTown_Mart_Pokemart_Badge6
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_Mart_Badge7::
	pokemart LavaridgeTown_Mart_Pokemart_Badge7
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_Mart_Badge8::
	pokemart LavaridgeTown_Mart_Pokemart_Badge8
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

LavaridgeTown_Mart_Champion::
	pokemart LavaridgeTown_Mart_Pokemart_BadgeChampion
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LavaridgeTown_Mart_Pokemart_PreAdventure:
	.2byte ITEM_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_RARE_CANDY
	.2byte ITEM_EXP_CANDY_XL
	pokemartlistend
	
	.align 2
LavaridgeTown_Mart_Pokemart_Badge0:
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
LavaridgeTown_Mart_Pokemart_Badge1:
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
LavaridgeTown_Mart_Pokemart_Badge2:
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
LavaridgeTown_Mart_Pokemart_Badge3:
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
LavaridgeTown_Mart_Pokemart_Badge4:
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
LavaridgeTown_Mart_Pokemart_Badge5:
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
LavaridgeTown_Mart_Pokemart_Badge6:
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
LavaridgeTown_Mart_Pokemart_Badge7:
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
LavaridgeTown_Mart_Pokemart_Badge8:
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
LavaridgeTown_Mart_Pokemart_BadgeChampion:
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

LavaridgeTown_Mart_EventScript_ExpertM::
	msgbox LavaridgeTown_Mart_Text_XSpeedFirstStrike, MSGBOX_NPC
	end

LavaridgeTown_Mart_EventScript_OldWoman::
	msgbox LavaridgeTown_Mart_Text_LocalSpecialtyOnMtChimney, MSGBOX_NPC
	end

LavaridgeTown_Mart_Text_XSpeedFirstStrike:
	.string "Use X SPEED to add to a POKéMON's\n"
	.string "SPEED in battle.\p"
	.string "That will help it get in the first\n"
	.string "strike--a decided advantage!$"

LavaridgeTown_Mart_Text_LocalSpecialtyOnMtChimney:
	.string "On MT. CHIMNEY's peak, there's a local\n"
	.string "specialty that you can buy only there.\p"
	.string "Give it to a POKéMON--it will be elated.$"

