voice_group bloody_tears
	voice_keysplit_all voicegroup_bloody_tears_drumset @ 0
	.rept 15
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_organ2, 255, 0, 255, 127 @ 16
	.rept 2
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_church_organ3_high, 255, 76, 154, 188 @ 19
	.rept 7
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_nylon_str_guitar, 255, 249, 25, 127 @ 27
	.rept 5
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_fingered_bass, 255, 253, 0, 149 @ 33
	.rept 5
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_synth_bass, 255, 252, 0, 127 @ 39
	.rept 8
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_keysplit voicegroup_bloody_tears_strings_keysplit, keysplit_bloody_tears_strings @ 48
	.rept 7
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_keysplit voicegroup_trumpet_keysplit, keysplit_trumpet @ 56
	.rept 37
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_string_ensemble_72, 255, 0, 255, 196 @ 94
	.rept 24
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_orchestra_cymbal_crash, 255, 246, 0, 216 @ 119
