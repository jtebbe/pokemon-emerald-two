voice_group gaia_gate
	voice_keysplit_all voicegroup_gaia_gate_drumset @ 0
	voice_keysplit voicegroup_piano_keysplit, keysplit_gaia_gate_piano @ 1
	.rept 33
	voice_keysplit voicegroup_piano_keysplit, keysplit_gaia_gate_piano
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_fretless_bass, 255, 253, 0, 188 @ 35
	.rept 5
	voice_keysplit voicegroup_piano_keysplit, keysplit_gaia_gate_piano
	.endr
	voice_keysplit voicegroup_strings_keysplit, keysplit_strings @ 41
	.rept 6
	voice_keysplit voicegroup_piano_keysplit, keysplit_gaia_gate_piano
	.endr
	voice_keysplit voicegroup_strings_keysplit, keysplit_strings @ 48
	voice_keysplit voicegroup_piano_keysplit, keysplit_gaia_gate_piano
	voice_keysplit voicegroup_strings_keysplit, keysplit_strings @ 50
	voice_keysplit voicegroup_piano_keysplit, keysplit_gaia_gate_piano
	voice_directsound 60, 0, DirectSoundWaveData_classical_choir_voice_ahhs, 255, 0, 255, 127 @ 52
	.rept 42
	voice_keysplit voicegroup_piano_keysplit, keysplit_gaia_gate_piano
	.endr
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_string_ensemble_60, 255, 0, 255, 196 @ 95
	.rept 23
	voice_keysplit voicegroup_piano_keysplit, keysplit_gaia_gate_piano
	.endr
	voice_directsound_no_resample 60, 64, DirectSoundWaveData_sc88pro_orchestra_cymbal_crash, 255, 246, 0, 216 @ 119
