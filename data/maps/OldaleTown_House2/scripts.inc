OldaleTown_House2_MapScripts::
	.byte 0

OldaleTown_House2_EventScript_Woman::
	msgbox OldaleTown_House2_Text_PokemonLevelUp, MSGBOX_NPC
	end

OldaleTown_House2_EventScript_Man::
	msgbox OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon, MSGBOX_NPC
	end

OldaleTown_House2_EventScriptDebugButton::
	lockall
	goto_if_set FLAG_DEBUG_ENABLED, OldaleTown_House2_EventScriptDebugButtonPressed
	goto OldaleTown_House2_EventScriptPressDebugButton
OldaleTown_House2_EventScriptPressDebugButton::
	msgbox OldaleTown_House2_Text_StrangeButton, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, OldaleTown_House2_EventScriptDebugButtonPressed
	goto OldaleTown_House2_EventScriptDebugButtonNotPressed
OldaleTown_House2_EventScriptDebugButtonPressed::
	setflag FLAG_DEBUG_ENABLED
	msgbox OldaleTown_House2_Text_PressedButton, MSGBOX_DEFAULT
	releaseall
	end
OldaleTown_House2_EventScriptDebugButtonNotPressed::
	msgbox OldaleTown_House2_Text_DidntPressButton, MSGBOX_DEFAULT
	releaseall
	end

OldaleTown_House2_Text_StrangeButton:
	.string "These people have a strange button\n"
	.string "in their sink. Press it?$"
OldaleTown_House2_Text_PressedButton:
	.string "Wow! You found the debug button!\n"
	.string "Hold R and press Start to open\l"
	.string "the debug menu.$"
OldaleTown_House2_Text_DidntPressButton:
	.string "Probably best not to press things\n"
	.string "in people's sinks anyways.$"

OldaleTown_House2_Text_PokemonLevelUp:
	.string "When POKéMON battle, they eventually\n"
	.string "level up and become stronger.$"

OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon:
	.string "If the POKéMON with you become\n"
	.string "stronger, you'll be able to go farther\l"
	.string "away from here.$"

