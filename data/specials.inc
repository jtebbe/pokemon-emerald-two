.macro def_special, ptr:req, requests_effects=0, waitstate=0
.if ALLOCATE_SPECIAL_TABLE
	.global SPECIAL_\ptr
	.set SPECIAL_\ptr, __special__
	.if \requests_effects == 0
		.4byte \ptr
	.else
		.4byte \ptr + ROM_SIZE
	.endif
.endif
.set SPECIAL_WAITSTATE_\ptr, \waitstate
.set __special__, __special__ + 1
.endm

.set __special__, 0
.if ALLOCATE_SPECIAL_TABLE
		.align 2
gSpecials::
.endif
	def_special HealPlayerParty
	def_special ShowTrainerIntroSpeech
	def_special ShowTrainerCantBattleSpeech
	def_special GetTrainerFlag
	def_special DoTrainerApproach, waitstate=1
	def_special PlayTrainerEncounterMusic
	def_special HasEnoughMonsForDoubleBattle
	def_special SaveGame, waitstate=1
	def_special GetPlayerTrainerIdOnesDigit
	def_special RockSmashWildEncounter
	def_special GetBattleOutcome
	def_special EggHatch, waitstate=1
	def_special TryFieldPoisonWhiteOut, waitstate=1
	def_special GetPlayerFacingDirection, requests_effects=TRUE
	def_special Special_AreLeadMonEVsMaxedOut
	def_special IsGrassTypeInParty
	def_special ScriptCheckFreePokemonStorageSpace
	def_special SetTrainerFacingDirection
	def_special ScriptGetPartyMonSpecies
	def_special ShouldTryGetTrainerScript
	def_special BufferTMHMMoveName
	def_special GetPCBoxToSendMon
	def_special ShouldShowBoxWasFullMessage
	def_special TryPrepareSecondApproachingTrainer
	def_special PrepareSecondApproachingTrainer
	def_special GetNumberSprayStrength
	def_special GetSprayId
	def_special GetLastUsedSprayType
	def_special ObjectEventInteractionGetApricornTreeData
	def_special ObjectEventInteractionPickApricornTree
	def_special SetSeenMon
	def_special DoesPlayerPartyContainSpecies
	def_special PlayerPartyContainsSpeciesWithPlayerID
	def_special SetHiddenItemFlag, requests_effects=TRUE
	def_special CheckAddCoins, requests_effects=TRUE
	def_special ObjectEventInteractionGetBerryTreeData
	def_special ObjectEventInteractionGetBerryName
	def_special ObjectEventInteractionGetBerryCountString
	def_special ObjectEventInteractionPlantBerryTree
	def_special ObjectEventInteractionPickBerryTree
	def_special ObjectEventInteractionRemoveBerryTree
	def_special ObjectEventInteractionWaterBerryTree
	def_special PlayerHasBerries
	def_special PlayerHasMulch
	def_special Bag_ChooseMulch
	def_special ObjectEventInteractionApplyMulch
	def_special ObjectEventInteractionPullBerryWeed
	def_special ObjectEventInteractionClearBerryPests
	def_special ObjectEventInteractionBerryHasWeed
	def_special ObjectEventInteractionBerryHasPests
	def_special CanWaterBerryPlot
	def_special Bag_ChooseBerry, waitstate=1
	def_special ShowPokemonStorageSystemPC, waitstate=1
	def_special DoWateringBerryTreeAnim, waitstate=1
	def_special GiveEggFromDaycare
