LilycoveCity_DepartmentStore_3F_MapScripts::
	.byte 0

LilycoveCity_DepartmentStore_3F_EventScript_ClerkLeft::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart LilycoveCity_DepartmentStore_3F_Pokemart_Vitamins
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_3F_Pokemart_Vitamins:
	.2byte ITEM_BOOSTER_ENERGY
	.2byte ITEM_DRAGON_FANG
	.2byte ITEM_ELECTRIC_SEED
	.2byte ITEM_MISTY_SEED
	.2byte ITEM_MAGNET
	.2byte ITEM_MYSTIC_WATER
	.2byte ITEM_NEVER_MELT_ICE
	.2byte ITEM_TWISTED_SPOON
	.2byte ITEM_POISON_BARB
	.2byte ITEM_SHARP_BEAK
	.2byte ITEM_SILVER_POWDER
	.2byte ITEM_SNOWBALL
	.2byte ITEM_SPELL_TAG
	.2byte ITEM_FLOAT_STONE
	.2byte ITEM_ABILITY_SHIELD
	.2byte ITEM_BIG_ROOT
	.2byte ITEM_BINDING_BAND
	.2byte ITEM_DESTINY_KNOT
	.2byte ITEM_EXPERT_BELT
	.2byte ITEM_GRIP_CLAW
	.2byte ITEM_HEAVY_DUTY_BOOTS
	.2byte ITEM_LAGGING_TAIL
	.2byte ITEM_LAX_INCENSE
	.2byte ITEM_LUCK_INCENSE
	.2byte ITEM_ROCKY_HELMET
	.2byte ITEM_SHED_SHELL
	.2byte ITEM_SMOKE_BALL
	.2byte ITEM_STICKY_BARB
	.2byte ITEM_THROAT_SPRAY
	.2byte ITEM_UTILITY_UMBRELLA
	.2byte ITEM_ZOOM_LENS
	.2byte ITEM_ADRENALINE_ORB
	pokemartlistend

LilycoveCity_DepartmentStore_3F_EventScript_ClerkRight::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart LilycoveCity_DepartmentStore_3F_Pokemart_StatBoosters
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_3F_Pokemart_StatBoosters:
	.2byte ITEM_CLEFABLITE
	.2byte ITEM_VICTREEBELITE
	.2byte ITEM_STARMINITE
	.2byte ITEM_DRAGONINITE
	.2byte ITEM_MEGANIUMITE
	.2byte ITEM_FERALIGITE
	.2byte ITEM_SKARMORITE
	.2byte ITEM_FROSLASSITE
	.2byte ITEM_EMBOARITE
	.2byte ITEM_EXCADRITE
	.2byte ITEM_SCOLIPITE
	.2byte ITEM_SCRAFTINITE
	.2byte ITEM_EELEKTROSSITE
	.2byte ITEM_CHANDELURITE
	.2byte ITEM_CHESNAUGHTITE
	.2byte ITEM_DELPHOXITE
	.2byte ITEM_GRENINJITE
	.2byte ITEM_PYROARITE
	.2byte ITEM_FLOETTITE
	.2byte ITEM_MALAMARITE
	.2byte ITEM_BARBARACITE
	.2byte ITEM_DRAGALGITE
	.2byte ITEM_HAWLUCHANITE
	.2byte ITEM_ZYGARDITE
	.2byte ITEM_DRAMPANITE
	.2byte ITEM_FALINKSITE
	.2byte ITEM_HEATRANITE
	.2byte ITEM_DARKRANITE
	.2byte ITEM_ZERAORITE
	.2byte ITEM_RAICHUNITE_X
	.2byte ITEM_RAICHUNITE_Y
	.2byte ITEM_CHIMECHITE
	.2byte ITEM_ABSOLITE_Z
	.2byte ITEM_STARAPTITE
	.2byte ITEM_GARCHOMPITE_Z
	.2byte ITEM_LUCARIONITE_Z
	.2byte ITEM_GOLURKITE
	.2byte ITEM_MEOWSTICITE
	.2byte ITEM_CRABOMINITE
	.2byte ITEM_GOLISOPITE
	.2byte ITEM_MAGEARNITE
	.2byte ITEM_SCOVILLAINITE
	.2byte ITEM_BAXCALIBRITE
	.2byte ITEM_TATSUGIRINITE
	.2byte ITEM_GLIMMORANITE 
	pokemartlistend

LilycoveCity_DepartmentStore_3F_EventScript_TriathleteM::
	msgbox LilycoveCity_DepartmentStore_3F_Text_ItemsBestForTougheningPokemon, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_3F_EventScript_PokefanM::
	msgbox LilycoveCity_DepartmentStore_3F_Text_WantMoreEndurance, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_3F_EventScript_Woman::
	msgbox LilycoveCity_DepartmentStore_3F_Text_GaveCarbosToSpeedUpMon, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_3F_Text_ItemsBestForTougheningPokemon:
	.string "For quickly toughening up POKéMON,\n"
	.string "items are the best.\p"
	.string "PROTEIN boosts ATTACK,\n"
	.string "and CALCIUM raises SP. ATK.$"

LilycoveCity_DepartmentStore_3F_Text_WantMoreEndurance:
	.string "I want my POKéMON to have more\n"
	.string "endurance.\p"
	.string "I'm trying to decide whether to raise\n"
	.string "DEFENSE with IRON, or SP. DEF with ZINC.$"

LilycoveCity_DepartmentStore_3F_Text_GaveCarbosToSpeedUpMon:
	.string "I gave a CARBOS to my POKéMON,\n"
	.string "and its SPEED went up.$"

