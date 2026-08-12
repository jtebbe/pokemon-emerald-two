voice_group beneath_the_sea
	voice_keysplit_all voicegroup_beneath_the_sea_drumset @ 0
	voice_keysplit voicegroup_piano_keysplit, keysplit_piano @ 1
	voice_keysplit voicegroup_piano_keysplit, keysplit_piano @ 2
	.rept 2
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_keysplit voicegroup_piano_keysplit, keysplit_piano @ 5
	.rept 2
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_glockenspiel, 255, 165, 51, 242 @ 8
	.rept 2
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_xylophone, 255, 235, 0, 204 @ 11
	.rept 12
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_nylon_str_guitar, 255, 249, 25, 127 @ 24
	.rept 13
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_synth_bass, 255, 252, 0, 127 @ 38
	.rept 41
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_square_1 60, 0, 0, 2, 3, 3, 6, 2 @ 80
	.rept 9
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_keysplit voicegroup_strings_keysplit, keysplit_strings @ 90
