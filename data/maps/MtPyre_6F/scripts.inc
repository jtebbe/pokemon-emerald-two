.set LOCALID_NIKKI, 4

MtPyre_6F_MapScripts::
	.byte 0

MtPyre_6F_EventScript_Valerie::
	trainerbattle_single TRAINER_VALERIE_1, MtPyre_6F_Text_ValerieIntro, MtPyre_6F_Text_ValerieDefeat, MtPyre_6F_EventScript_RegisterValerie
	specialvar VAR_RESULT, ShouldTryRematchBattle
	goto_if_eq VAR_RESULT, TRUE, MtPyre_6F_EventScript_RematchValerie
	msgbox MtPyre_6F_Text_ValeriePostBattle, MSGBOX_DEFAULT
	release
	end

MtPyre_6F_EventScript_RegisterValerie::
	special PlayerFaceTrainerAfterBattle
	waitmovement 0
	msgbox MtPyre_6F_Text_ValerieRegister, MSGBOX_DEFAULT
	register_matchcall TRAINER_VALERIE_1
	release
	end

MtPyre_6F_EventScript_RematchValerie::
	trainerbattle_rematch TRAINER_VALERIE_1, MtPyre_6F_Text_ValerieRematchIntro, MtPyre_6F_Text_ValerieRematchDefeat
	msgbox MtPyre_6F_Text_ValeriePostRematch, MSGBOX_AUTOCLOSE
	end

MtPyre_6F_EventScript_Cedric::
	trainerbattle_single TRAINER_CEDRIC, MtPyre_6F_Text_CedricIntro, MtPyre_6F_Text_CedricDefeat
	msgbox MtPyre_6F_Text_CedricPostBattle, MSGBOX_AUTOCLOSE
	end

MtPyre_6F_EventScript_Nikki::
	lockall
	faceplayer
	msgbox MtPyre_6F_Text_NikkiIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_IKKIN, MtPyre_6F_Text_NikkiDefeat
	msgbox MtPyre_6F_Text_NikkiExit, MSGBOX_DEFAULT
	giveitem ITEM_CHOICE_SPECS
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_NIKKI
	fadescreen FADE_FROM_BLACK
	setflag FLAG_AGARTHAN_1_DEFEATED
	clearflag FLAG_SHOW_HOUSE_AGARTHAN_1
	releaseall
	end

MtPyre_6F_Text_NikkiIntro:
	.string "Hmm?\p"
	.string "You can actually see me?\n"
	.string "How exciting!\p"
	.string "It's been a while since I had a\n"
	.string "real challenge!\p"
	.string "I am the Occultist, witnesser\n"
	.string "of truths, of the ancient sect.\p"
	.string "Bare your soul to me, so that I\n"
	.string "may see your apex!$"

MtPyre_6F_Text_NikkiDefeat:
	.string "Haha! How exciting!$"

MtPyre_6F_Text_NikkiExit:
	.string "Your soul shone bright during\n"
	.string "our battle!\p"
	.string "But you're going to have to reach a.\n"
	.string "higher level to beat the rest of us.\p"
	.string "Keep fighting and training, and\n"
	.string "you might survive to reach the peak.\p"
	.string "You've earned this, and you're\n"
	.string "going to need it.$"

MtPyre_6F_Text_ValerieIntro:
	.string "When I'm here…\n"
	.string "A curious power flows into me…$"

MtPyre_6F_Text_ValerieDefeat:
	.string "The power is ebbing away…$"

MtPyre_6F_Text_ValeriePostBattle:
	.string "Perhaps the power is from the spirits\n"
	.string "of POKéMON in fitful sleep here…$"

MtPyre_6F_Text_ValerieRegister:
	.string "Fufufu… I lost the match, but…\n"
	.string "I have this little ability…\p"
	.string "Without ever laying my hands on\n"
	.string "your POKéNAV… Hiyah!$"

MtPyre_6F_Text_ValerieRematchIntro:
	.string "Behind you…\n"
	.string "What is it…$"

MtPyre_6F_Text_ValerieRematchDefeat:
	.string "Something faded away…$"

MtPyre_6F_Text_ValeriePostRematch:
	.string "The POKéMON at rest here…\n"
	.string "Sometimes, they play…$"

MtPyre_6F_Text_CedricIntro:
	.string "Have you lost your bearings?\n"
	.string "Have no fear for I am here!$"

MtPyre_6F_Text_CedricDefeat:
	.string "Weren't you lost?$"

MtPyre_6F_Text_CedricPostBattle:
	.string "I had this feeling that a lost TRAINER\n"
	.string "would be panicked and easy to beat.\p"
	.string "It's dirty and I won't try it again…$"

