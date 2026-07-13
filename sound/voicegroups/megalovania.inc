voice_group megalovania
	.rept 16
	voice_keysplit voicegroup_piano_keysplit, keysplit_piano
	.endr
	voice_keysplit_all voicegroup_megalovania_drumset @ 16
	voice_keysplit voicegroup_piano_keysplit, keysplit_piano
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_organ2, 255, 0, 255, 165 @ 18
	.rept 11
	voice_keysplit voicegroup_piano_keysplit, keysplit_piano
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sd90_classical_distortion_guitar_low, 255, 0, 255, 127 @ 30
	.rept 3
	voice_keysplit voicegroup_piano_keysplit, keysplit_piano
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_fingered_bass, 255, 253, 0, 149 @ 34
	.rept 13
	voice_keysplit voicegroup_piano_keysplit, keysplit_piano
	.endr
	voice_keysplit voicegroup_strings_keysplit, keysplit_strings @ 48
	.rept 3
	voice_keysplit voicegroup_piano_keysplit, keysplit_piano
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_classical_choir_voice_ahhs, 255, 0, 255, 127 @ 52
	.rept 2
	voice_keysplit voicegroup_piano_keysplit, keysplit_piano
	.endr
	voice_keysplit voicegroup_piano_keysplit, keysplit_piano @ 55
	.rept 5
	voice_keysplit voicegroup_piano_keysplit, keysplit_piano
	.endr
	voice_keysplit voicegroup_trumpet_keysplit, keysplit_trumpet @ 61
	.rept 18
	voice_keysplit voicegroup_piano_keysplit, keysplit_piano
	.endr
	voice_keysplit voicegroup_piano_keysplit, keysplit_piano @ 80
	voice_directsound 60, 0, DirectSoundWaveData_sd90_classical_distortion_guitar_high, 255, 0, 255, 127 @ 81
