voice_group nightcall
	voice_keysplit voicegroup_piano_keysplit, keysplit_piano @ 0 - electric piano
	voice_directsound 60, 0, DirectSoundWaveData_synth_pluck, 255, 0, 255, 127 @ 1 - smooth synth low
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_fingered_bass, 255, 253, 0, 149 @ 2 - bass guitar
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_synth_bass, 255, 252, 0, 100 @ 3 - sci-fi bass
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_square_wave, 255, 0, 255, 127 @ 4 - electric guitar
	voice_keysplit_all voicegroup_nightcall_drumset @ 5 - drums
	voice_directsound 60, 0, DirectSoundWaveData_synth_pluck, 255, 0, 255, 100 @ 6 - synth pluck high
