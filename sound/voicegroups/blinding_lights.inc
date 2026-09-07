voice_group blinding_lights
	voice_directsound 60, 0, DirectSoundWaveData_synth_pluck, 255, 0, 255, 127 @ 0 - synth pluck
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_square_wave, 255, 0, 255, 127 @ 1 - triangle lead
	voice_keysplit voicegroup_french_horn_keysplit, keysplit_french_horn @ 2 - french horn
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_fingered_bass, 255, 253, 0, 149 @ 3 - bass guitar
	voice_keysplit_all voicegroup_blinding_lights_drumset @ 4 - drums
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_nylon_str_guitar, 255, 249, 25, 127 @ 5 - jazz guitar
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_square_wave, 255, 0, 255, 127 @ 6 - sine lead
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_synth_bass, 255, 252, 0, 127 @ 7 - smooth synth
