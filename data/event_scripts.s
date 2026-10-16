#include "config/general.h"
#include "config/battle.h"
#include "config/item.h"
#include "constants/global.h"
#include "constants/apricorn_tree.h"
#include "constants/battle.h"
#include "constants/battle_special.h"
#include "constants/berry.h"
#include "constants/cable_club.h"
#include "constants/coins.h"
#include "constants/comparison_operators.h"
#include "constants/daycare.h"
#include "constants/difficulty.h"
#include "constants/easy_chat.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/fame_checker.h"
#include "constants/field_effects.h"
#include "constants/field_move.h"
#include "constants/field_poison.h"
#include "constants/field_specials.h"
#include "constants/field_tasks.h"
#include "constants/field_weather.h"
#include "constants/flags.h"
#include "constants/follower_npc.h"
#include "constants/item.h"
#include "constants/items.h"
#include "constants/heal_locations.h"
#include "constants/layouts.h"
#include "constants/map_scripts.h"
#include "constants/maps.h"
#include "constants/mass_outbreak.h"
#include "constants/metatile_labels.h"
#include "constants/move_relearner.h"
#include "constants/moves.h"
#include "constants/party_menu.h"
#include "constants/pokeball.h"
#include "constants/pokedex.h"
#include "constants/pokemon.h"
#include "constants/random_mon_generation.h"
#include "constants/rtc.h"
#include "constants/roulette.h"
#include "constants/script_menu.h"
#include "constants/seagallop.h"
#include "constants/siirtc.h"
#include "constants/songs.h"
#include "constants/sound.h"
#include "constants/species.h"
#include "constants/trade.h"
#include "constants/trainers.h"
#include "constants/trainer_card.h"
#include "constants/union_room.h"
#include "constants/vars.h"
#include "constants/weather.h"
#include "constants/speaker_names.h"
#include "constants/bxpy.h" 
	.include "asm/macros.inc"
	.include "asm/macros/event.inc"
	.include "constants/constants.inc"

	.section script_data, "aw", %progbits

	.set ALLOCATE_SCRIPT_CMD_TABLE, 1
	.include "data/script_cmd_table.inc"

.align 2
gSpecialVars::
	.4byte gSpecialVar_0x8000
	.4byte gSpecialVar_0x8001
	.4byte gSpecialVar_0x8002
	.4byte gSpecialVar_0x8003
	.4byte gSpecialVar_0x8004
	.4byte gSpecialVar_0x8005
	.4byte gSpecialVar_0x8006
	.4byte gSpecialVar_0x8007
	.4byte gSpecialVar_0x8008
	.4byte gSpecialVar_0x8009
	.4byte gSpecialVar_0x800A
	.4byte gSpecialVar_0x800B
	.4byte gSpecialVar_Facing
	.4byte gSpecialVar_Result
	.4byte gSpecialVar_ItemId
	.4byte gSpecialVar_LastTalked
	.4byte NULL
	.4byte NULL
	.4byte gSpecialVar_MonBoxId
	.4byte gSpecialVar_MonBoxPos
	.4byte gSpecialVar_Unused_0x8014
	.4byte gTrainerBattleParameter + 2 // gTrainerBattleParameter.params.opponentA

	.purgem def_special
	.set ALLOCATE_SPECIAL_TABLE, 1
	.include "data/specials.inc"

gStdScripts::
	.4byte Std_ObtainItem              @ STD_OBTAIN_ITEM
	.4byte Std_FindItem                @ STD_FIND_ITEM
	.4byte Std_MsgboxNPC               @ MSGBOX_NPC
	.4byte Std_MsgboxSign              @ MSGBOX_SIGN
	.4byte Std_MsgboxDefault           @ MSGBOX_DEFAULT
	.4byte Std_MsgboxYesNo             @ MSGBOX_YESNO
	.4byte Std_MsgboxAutoclose         @ MSGBOX_AUTOCLOSE
	.4byte Std_PutItemAway             @ STD_PUT_ITEM_AWAY
	.4byte Std_ReceivedItem            @ STD_RECEIVED_ITEM
gStdScripts_End::

@ Engine-only helpers. World maps supply no scripts or events.
	.include "data/scripts/std_msgbox.inc"
	.include "data/scripts/engine.inc"
	.include "data/scripts/berry_tree.inc"
	.include "data/scripts/obtain_item.inc"
	.include "data/scripts/item_ball_scripts.inc"
	.include "data/scripts/trainer_battle.inc"
	.include "data/scripts/trainer_script.inc"
	.include "data/scripts/field_move_scripts.inc"
	.include "data/scripts/surf.inc"
	.include "data/scripts/flash.inc"
	.include "data/scripts/repel.inc"
	.include "data/scripts/movement.inc"
	.include "data/scripts/follower.inc"
	.include "data/scripts/dexnav.inc"
	.include "data/scripts/apricorn_tree.inc"
	.include "data/scripts/wild_encounter.inc"
	.include "data/scripts/bxpy.inc"
	.include "data/text/save.inc"
	.include "data/text/birch_speech.inc"
	.include "data/text/new_game_intro_frlg.inc"
	.include "data/text/obtain_item.inc"
	.include "data/text/surf.inc"
	.include "data/text/pc.inc"
	.include "data/text/pc_transfer.inc"
	.include "data/text/move_relearner.inc"

Common_EventScript_NopReturn::
    return

Common_EventScript_End::
Common_EventScript_NopEnd::
    end

EventScript_EggHatch::
    lockall
    msgbox Text_EggHatchHuh, MSGBOX_DEFAULT
    special EggHatch
    releaseall
    end

Text_EggHatchHuh:
    .string "Huh?$"

EventScript_FieldPoison::
    lockall
    special TryFieldPoisonWhiteOut
    releaseall
    end
