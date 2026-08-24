voice_group forest_maze
	.rept 15
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_tubular_bell, 255, 165, 90, 216 @ 15
	.rept 29
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_pizzicato_strings, 255, 226, 0, 38 @ 45
	.rept 2
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_keysplit voicegroup_strings_keysplit, keysplit_strings @ 48
	.rept 3
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_classical_choir_voice_ahhs, 255, 0, 255, 127 @ 52
	.rept 15
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sd90_classical_oboe, 255, 165, 154, 127 @ 68
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	voice_keysplit voicegroup_french_horn_keysplit, keysplit_french_horn @ 70
	.rept 2
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_flute, 255, 0, 255, 127 @ 73
