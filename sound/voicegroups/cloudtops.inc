voice_group cloudtops
	voice_directsound 60, 0, DirectSoundWaveData_sd90_classical_oboe, 255, 0, 255, 127 @ 0 - lead reed
	voice_keysplit voicegroup_french_horn_keysplit, keysplit_french_horn @ 1 - french horn
	voice_keysplit voicegroup_trumpet_keysplit, keysplit_trumpet @ 2 - brass
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_timpani, 255, 0, 180, 246 @ 3 - timpani
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_square_wave, 255, 0, 255, 127 @ 4 - square lead
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_pizzicato_strings, 255, 226, 0, 38 @ 5 - pizzicato
	voice_keysplit voicegroup_strings_keysplit, keysplit_strings @ 6 - strings
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_harp, 255, 242, 51, 242 @ 7 - harp high
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_harp, 255, 242, 51, 242 @ 8 - harp echo
	voice_keysplit_all voicegroup_cloudtops_drumset @ 9 - drums
