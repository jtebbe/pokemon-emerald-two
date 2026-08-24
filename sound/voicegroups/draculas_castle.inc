voice_group draculas_castle
	voice_keysplit_all voicegroup_draculas_castle_drumset @ 0
	.rept 9
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_glockenspiel, 255, 165, 51, 242 @ 10
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_xylophone, 255, 235, 0, 204 @ 11
	.rept 7
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_church_organ3_low, 255, 76, 154, 188 @ 19
	.rept 10
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sd90_classical_distortion_guitar_low, 255, 0, 255, 127 @ 30
	voice_directsound 60, 0, DirectSoundWaveData_sd90_classical_distortion_guitar_low, 255, 0, 255, 127 @ 31
	.rept 2
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_fingered_bass, 255, 253, 0, 149 @ 34
	.rept 13
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_keysplit voicegroup_strings_keysplit, keysplit_strings @ 48
	.rept 3
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_classical_choir_voice_ahhs, 255, 0, 255, 127 @ 52
	.rept 7
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_keysplit voicegroup_french_horn_keysplit, keysplit_french_horn @ 60
	voice_keysplit voicegroup_trumpet_keysplit, keysplit_trumpet @ 61
	.rept 6
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sd90_classical_oboe, 255, 165, 154, 127 @ 68
	.rept 4
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_flute, 255, 0, 255, 127 @ 73
