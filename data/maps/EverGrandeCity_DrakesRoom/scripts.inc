EverGrandeCity_DrakesRoom_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, EverGrandeCity_DrakesRoom_OnFrame
	map_script MAP_SCRIPT_ON_LOAD, EverGrandeCity_DrakesRoom_OnLoad
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, EverGrandeCity_DrakesRoom_OnWarp
	.byte 0

@ Unused, Drake uses Drakes identical OnWarp for some reason
EverGrandeCity_DrakesRoom_OnWarp:
	map_script_2 VAR_TEMP_1, 0, EverGrandeCity_DrakesRoom_EventScript_PlayerTurnNorth
	.2byte 0

EverGrandeCity_DrakesRoom_EventScript_PlayerTurnNorth::
	turnobject LOCALID_PLAYER, DIR_NORTH
	end

EverGrandeCity_DrakesRoom_OnFrame:
	map_script_2 VAR_ELITE_4_STATE, 3, EverGrandeCity_DrakesRoom_EventScript_WalkInCloseDoor
	.2byte 0

EverGrandeCity_DrakesRoom_EventScript_WalkInCloseDoor::
	lockall
	call PokemonLeague_EliteFour_EventScript_WalkInCloseDoor
	setvar VAR_ELITE_4_STATE, 4
	releaseall
	end

EverGrandeCity_DrakesRoom_OnLoad:
	call_if_set FLAG_DEFEATED_ELITE_4_DRAKE, EverGrandeCity_DrakesRoom_EventScript_ResetAdvanceToNextRoom
	call_if_eq VAR_ELITE_4_STATE, 4, EverGrandeCity_DrakesRoom_EventScript_CloseDoor
	end

EverGrandeCity_DrakesRoom_EventScript_ResetAdvanceToNextRoom::
	call PokemonLeague_EliteFour_EventScript_ResetAdvanceToNextRoom
	return

EverGrandeCity_DrakesRoom_EventScript_CloseDoor::
	call PokemonLeague_EliteFour_EventScript_CloseDoor
	return

EverGrandeCity_DrakesRoom_EventScript_Drake::
	lock
	faceplayer
	goto_if_set FLAG_IS_CHAMPION, EverGrandeCity_DrakesRoom_EventScript_Drake_Rematch
	goto_if_set FLAG_DEFEATED_ELITE_4_DRAKE, EverGrandeCity_DrakesRoom_EventScript_PostBattleSpeech
	playbgm MUS_ENCOUNTER_ELITE_FOUR, FALSE
	msgbox EverGrandeCity_DrakesRoom_Text_IntroSpeech, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_DRAKE, EverGrandeCity_DrakesRoom_Text_Defeat
	goto EverGrandeCity_DrakesRoom_EventScript_Defeated
	end

EverGrandeCity_DrakesRoom_EventScript_Drake_Rematch::
	goto_if_set FLAG_DEFEATED_ELITE_4_DRAKE, EverGrandeCity_DrakesRoom_EventScript_DefeatedRematch
	playbgm MUS_ENCOUNTER_ELITE_FOUR, FALSE
	msgbox EverGrandeCity_DrakesRoom_Text_IntroSpeechRematch, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_DRAKE_REMATCH, EverGrandeCity_DrakesRoom_Text_DefeatRematch
	setflag FLAG_DEFEATED_ELITE_4_DRAKE
	setflag FLAG_DRAKE_REMATCH_COMPLETE
	call PokemonLeague_EliteFour_SetAdvanceToNextRoomMetatiles
	goto EverGrandeCity_DrakesRoom_EventScript_DefeatedRematch
	end

EverGrandeCity_DrakesRoom_EventScript_PostBattleSpeech::
	msgbox EverGrandeCity_DrakesRoom_Text_PostBattleSpeech, MSGBOX_DEFAULT
	release
	end

EverGrandeCity_DrakesRoom_EventScript_Defeated::
	setvar VAR_0x8004, FANCOUNTER_DEFEATED_DRAKE
	special Script_TryGainNewFanFromCounter
	setflag FLAG_DEFEATED_ELITE_4_DRAKE
	call PokemonLeague_EliteFour_SetAdvanceToNextRoomMetatiles
	msgbox EverGrandeCity_DrakesRoom_Text_PostBattleSpeech, MSGBOX_DEFAULT
	release
	end

EverGrandeCity_DrakesRoom_EventScript_DefeatedRematch::
	msgbox EverGrandeCity_DrakesRoom_Text_PostBattleSpeechRematch, MSGBOX_DEFAULT
	release
	end

EverGrandeCity_DrakesRoom_Text_IntroSpeech:
	.string "I am the last of the POKéMON LEAGUE\n"
	.string "ELITE FOUR, DRAKE the DRAGON master!\p"
	.string "In their natural state, POKéMON are\n"
	.string "wild living things. They are free.\p"
	.string "At times, they hinder us.\n"
	.string "At times, they help us.\p"
	.string "For us to battle with POKéMON as\n"
	.string "partners, do you know what it takes?\p"
	.string "Do you know what is needed?\p"
	.string "If you don't, then you will never\n"
	.string "prevail over me!$"

EverGrandeCity_DrakesRoom_Text_Defeat:
	.string "Superb, it should be said.$"

EverGrandeCity_DrakesRoom_Text_PostBattleSpeech:
	.string "You deserve every credit for coming\n"
	.string "this far as a TRAINER of POKéMON.\p"
	.string "You do seem to know what is needed.\p"
	.string "Yes, what a TRAINER needs is a\n"
	.string "virtuous heart.\p"
	.string "POKéMON touch the good hearts of\n"
	.string "TRAINERS and learn good from wrong.\p"
	.string "They touch the good hearts of\n"
	.string "TRAINERS and grow strong.\p"
	.string "Go! Go onwards!\n"
	.string "The CHAMPION is waiting!$"

	EverGrandeCity_DrakesRoom_Text_IntroSpeechRematch:
	.string "So, you've returned. Welcome back,\n"
	.string "young one.\p"
	.string "I hope you've grown stronger, I am\n"
	.string "a level above the previous three,\l"
	.string "now more than ever.\p"
	.string "Let us not delay. Beat me with all\n"
	.string "your might!$"

EverGrandeCity_DrakesRoom_Text_DefeatRematch:
	.string "You remain worthy of your title,\n"
	.string "after all...$"

EverGrandeCity_DrakesRoom_Text_PostBattleSpeechRematch:
	.string "One more fight standing between you\n"
	.string "and your crown.\p"
	.string "Seize it, as you did this fight, and\n"
	.string "be victorious!$"
