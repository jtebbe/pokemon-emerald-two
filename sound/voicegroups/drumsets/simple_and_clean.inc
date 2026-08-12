voice_group simple_and_clean_drumset, 35
	voice_directsound_no_resample 60, 64, DirectSoundWaveData_sc88pro_rnd_kick, 255, 0, 255, 242 @ 35
	.rept 2
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound_no_resample 60, 64, DirectSoundWaveData_sc88pro_rnd_snare, 255, 0, 255, 242 @ 38
	.rept 18
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound_no_resample 60, 79, DirectSoundWaveData_unknown_close_hihat, 255, 127, 0, 188 @ 57
	.rept 26
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound_no_resample 64, 64, DirectSoundWaveData_sd90_open_triangle, 255, 0, 255, 0 @ 84
