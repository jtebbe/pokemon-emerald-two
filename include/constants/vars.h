#ifndef GUARD_CONSTANTS_VARS_H
#define GUARD_CONSTANTS_VARS_H

#include "constants/vars_frlg.h"

#define VARS_START 0x4000

// temporary vars
// The first 0x10 vars are temporary--they are cleared every time a map is loaded.
#define TEMP_VARS_START            0x4000
#define VAR_TEMP_0                 (TEMP_VARS_START + 0x0)
#define VAR_TEMP_1                 (TEMP_VARS_START + 0x1)
#define VAR_TEMP_2                 (TEMP_VARS_START + 0x2)
#define VAR_TEMP_3                 (TEMP_VARS_START + 0x3) // Note: Used when the player checks a TV
#define VAR_TEMP_4                 (TEMP_VARS_START + 0x4)
#define VAR_TEMP_5                 (TEMP_VARS_START + 0x5)
#define VAR_TEMP_6                 (TEMP_VARS_START + 0x6)
#define VAR_TEMP_7                 (TEMP_VARS_START + 0x7)
#define VAR_TEMP_8                 (TEMP_VARS_START + 0x8)
#define VAR_TEMP_9                 (TEMP_VARS_START + 0x9)
#define VAR_TEMP_A                 (TEMP_VARS_START + 0xA)
#define VAR_TEMP_B                 (TEMP_VARS_START + 0xB)
#define VAR_TEMP_C                 (TEMP_VARS_START + 0xC)
#define VAR_TEMP_D                 (TEMP_VARS_START + 0xD)
#define VAR_TEMP_E                 (TEMP_VARS_START + 0xE)
#define VAR_TEMP_F                 (TEMP_VARS_START + 0xF)
#define TEMP_VARS_END              VAR_TEMP_F
#define NUM_TEMP_VARS              (TEMP_VARS_END - TEMP_VARS_START + 1)

// object gfx id vars
// These 0x10 vars are used to dynamically control a map object's sprite.
// For example, the rival's sprite id is dynamically set based on the player's gender.
// See VarGetObjectEventGraphicsId().
#define VAR_OBJ_GFX_ID_0           0x4010
#define VAR_OBJ_GFX_ID_1           0x4011
#define VAR_OBJ_GFX_ID_2           0x4012
#define VAR_OBJ_GFX_ID_3           0x4013
#define VAR_OBJ_GFX_ID_4           0x4014
#define VAR_OBJ_GFX_ID_5           0x4015
#define VAR_OBJ_GFX_ID_6           0x4016
#define VAR_OBJ_GFX_ID_7           0x4017
#define VAR_OBJ_GFX_ID_8           0x4018
#define VAR_OBJ_GFX_ID_9           0x4019
#define VAR_OBJ_GFX_ID_A           0x401A
#define VAR_OBJ_GFX_ID_B           0x401B
#define VAR_OBJ_GFX_ID_C           0x401C
#define VAR_OBJ_GFX_ID_D           0x401D
#define VAR_OBJ_GFX_ID_E           0x401E
#define VAR_OBJ_GFX_ID_F           0x401F

// general purpose vars
#define VAR_REPEL_STEP_COUNT                             0x4021
#define VAR_STARTER_MON                                  0x4023 // 0=Treecko, 1=Torchic, 2=Mudkip
#define VAR_MIRAGE_RND_H                                 0x4024 // Unused if OW_USE_DAILY_SEED_FOR_VANILLA_VARIABLES is TRUE
#define VAR_MIRAGE_RND_L                                 0x4025 // Unused if OW_USE_DAILY_SEED_FOR_VANILLA_VARIABLES is TRUE
#define VAR_POISON_STEP_COUNTER                          0x402B
#define VAR_RESET_RTC_ENABLE                             0x402C


#define VAR_PC_BOX_TO_SEND_MON                           0x4036
#define VAR_ABNORMAL_WEATHER_LOCATION                    0x4037
#define VAR_SHOULD_END_ABNORMAL_WEATHER                  0x4039
#define VAR_ALTERING_CAVE_WILD_SET                       0x403E
#define VAR_DAYS                                         0x4040
#define VAR_POKELOT_PRIZE_ITEM                           0x4045
#define VAR_NATIONAL_DEX                                 0x4046
#define VAR_BIRCH_STATE                                  0x4049
#define VAR_POKELOT_RND1                                 0x404B // Unused if OW_USE_DAILY_SEED_FOR_VANILLA_VARIABLES is TRUE
#define VAR_POKELOT_RND2                                 0x404C // Unused if OW_USE_DAILY_SEED_FOR_VANILLA_VARIABLES is TRUE
#define VAR_UNUSED_0x404E                                0x404E // Unused Var
#define VAR_CABLE_CLUB_STATE                             0x4087

#define VARS_END                                         0x40FF
#define VARS_COUNT                                       (VARS_END - VARS_START + 1)

#define SPECIAL_VARS_START            0x8000
// special vars
// They are commonly used as parameters to commands, or return values from commands.
#define VAR_0x8000                    0x8000
#define VAR_0x8001                    0x8001
#define VAR_0x8002                    0x8002
#define VAR_0x8003                    0x8003
#define VAR_0x8004                    0x8004
#define VAR_0x8005                    0x8005
#define VAR_0x8006                    0x8006
#define VAR_0x8007                    0x8007
#define VAR_0x8008                    0x8008
#define VAR_0x8009                    0x8009
#define VAR_0x800A                    0x800A
#define VAR_0x800B                    0x800B
#define VAR_FACING                    0x800C
#define VAR_RESULT                    0x800D
#define VAR_ITEM_ID                   0x800E
#define VAR_LAST_TALKED               0x800F

#define SPECIAL_VARS_END              0x8015

// If an overworld trigger uses this pseudo-variable as the trigger check,
// then the script will be run using RunScriptImmediately instead of in the
// global script context. This means it will run faster, but cannot do any
// cutscenes nor call a wait command. Used for weather effects in vanilla.
#define TRIGGER_RUN_IMMEDIATELY   0

// Temp var aliases


#if !IS_FRLG
// Unused numeric slots remain available for new scripts. Do not renumber live IDs.
#define VAR_UNUSED_0x8010 0x8010
#define VAR_UNUSED_0x8011 0x8011
#define VAR_UNUSED_0x8012 0x8012
#define VAR_UNUSED_0x8013 0x8013
#define VAR_UNUSED_0x8014 0x8014
#define VAR_UNUSED_0x8015 0x8015
#define VAR_UNUSED_0x4020 0x4020
#define VAR_UNUSED_0x4022 0x4022
#define VAR_UNUSED_0x4026 0x4026
#define VAR_UNUSED_0x4027 0x4027
#define VAR_UNUSED_0x4028 0x4028
#define VAR_UNUSED_0x4029 0x4029
#define VAR_UNUSED_0x402A 0x402A
#define VAR_UNUSED_0x402D 0x402D
#define VAR_UNUSED_0x402E 0x402E
#define VAR_UNUSED_0x402F 0x402F
#define VAR_UNUSED_0x4030 0x4030
#define VAR_UNUSED_0x4031 0x4031
#define VAR_UNUSED_0x4032 0x4032
#define VAR_UNUSED_0x4033 0x4033
#define VAR_UNUSED_0x4034 0x4034
#define VAR_UNUSED_0x4035 0x4035
#define VAR_UNUSED_0x4038 0x4038
#define VAR_UNUSED_0x403A 0x403A
#define VAR_UNUSED_0x403B 0x403B
#define VAR_UNUSED_0x403C 0x403C
#define VAR_UNUSED_0x403D 0x403D
#define VAR_UNUSED_0x403F 0x403F
#define VAR_UNUSED_0x4041 0x4041
#define VAR_UNUSED_0x4042 0x4042
#define VAR_UNUSED_0x4043 0x4043
#define VAR_UNUSED_0x4044 0x4044
#define VAR_UNUSED_0x4047 0x4047
#define VAR_UNUSED_0x4048 0x4048
#define VAR_UNUSED_0x404A 0x404A
#define VAR_UNUSED_0x404D 0x404D
#define VAR_UNUSED_0x404F 0x404F
#define VAR_UNUSED_0x4050 0x4050
#define VAR_UNUSED_0x4051 0x4051
#define VAR_UNUSED_0x4052 0x4052
#define VAR_UNUSED_0x4053 0x4053
#define VAR_UNUSED_0x4054 0x4054
#define VAR_UNUSED_0x4055 0x4055
#define VAR_UNUSED_0x4056 0x4056
#define VAR_UNUSED_0x4057 0x4057
#define VAR_UNUSED_0x4058 0x4058
#define VAR_UNUSED_0x4059 0x4059
#define VAR_UNUSED_0x405A 0x405A
#define VAR_UNUSED_0x405B 0x405B
#define VAR_UNUSED_0x405C 0x405C
#define VAR_UNUSED_0x405D 0x405D
#define VAR_UNUSED_0x405E 0x405E
#define VAR_UNUSED_0x405F 0x405F
#define VAR_UNUSED_0x4060 0x4060
#define VAR_UNUSED_0x4061 0x4061
#define VAR_UNUSED_0x4062 0x4062
#define VAR_UNUSED_0x4063 0x4063
#define VAR_UNUSED_0x4064 0x4064
#define VAR_UNUSED_0x4065 0x4065
#define VAR_UNUSED_0x4066 0x4066
#define VAR_UNUSED_0x4067 0x4067
#define VAR_UNUSED_0x4068 0x4068
#define VAR_UNUSED_0x4069 0x4069
#define VAR_UNUSED_0x406A 0x406A
#define VAR_UNUSED_0x406B 0x406B
#define VAR_UNUSED_0x406C 0x406C
#define VAR_UNUSED_0x406D 0x406D
#define VAR_UNUSED_0x406E 0x406E
#define VAR_UNUSED_0x406F 0x406F
#define VAR_UNUSED_0x4070 0x4070
#define VAR_UNUSED_0x4071 0x4071
#define VAR_UNUSED_0x4072 0x4072
#define VAR_UNUSED_0x4073 0x4073
#define VAR_UNUSED_0x4074 0x4074
#define VAR_UNUSED_0x4075 0x4075
#define VAR_UNUSED_0x4076 0x4076
#define VAR_UNUSED_0x4077 0x4077
#define VAR_UNUSED_0x4078 0x4078
#define VAR_UNUSED_0x4079 0x4079
#define VAR_UNUSED_0x407A 0x407A
#define VAR_UNUSED_0x407B 0x407B
#define VAR_UNUSED_0x407C 0x407C
#define VAR_UNUSED_0x407D 0x407D
#define VAR_UNUSED_0x407E 0x407E
#define VAR_UNUSED_0x407F 0x407F
#define VAR_UNUSED_0x4080 0x4080
#define VAR_UNUSED_0x4081 0x4081
#define VAR_UNUSED_0x4082 0x4082
#define VAR_UNUSED_0x4083 0x4083
#define VAR_UNUSED_0x4084 0x4084
#define VAR_UNUSED_0x4085 0x4085
#define VAR_UNUSED_0x4086 0x4086
#define VAR_UNUSED_0x4088 0x4088
#define VAR_UNUSED_0x4089 0x4089
#define VAR_UNUSED_0x408A 0x408A
#define VAR_UNUSED_0x408B 0x408B
#define VAR_UNUSED_0x408C 0x408C
#define VAR_UNUSED_0x408D 0x408D
#define VAR_UNUSED_0x408E 0x408E
#define VAR_UNUSED_0x408F 0x408F
#define VAR_UNUSED_0x4090 0x4090
#define VAR_UNUSED_0x4091 0x4091
#define VAR_UNUSED_0x4092 0x4092
#define VAR_UNUSED_0x4093 0x4093
#define VAR_UNUSED_0x4094 0x4094
#define VAR_UNUSED_0x4095 0x4095
#define VAR_UNUSED_0x4096 0x4096
#define VAR_UNUSED_0x4097 0x4097
#define VAR_UNUSED_0x4098 0x4098
#define VAR_UNUSED_0x4099 0x4099
#define VAR_UNUSED_0x409A 0x409A
#define VAR_UNUSED_0x409B 0x409B
#define VAR_UNUSED_0x409C 0x409C
#define VAR_UNUSED_0x409D 0x409D
#define VAR_UNUSED_0x409E 0x409E
#define VAR_UNUSED_0x409F 0x409F
#define VAR_UNUSED_0x40A0 0x40A0
#define VAR_UNUSED_0x40A1 0x40A1
#define VAR_UNUSED_0x40A2 0x40A2
#define VAR_UNUSED_0x40A3 0x40A3
#define VAR_UNUSED_0x40A4 0x40A4
#define VAR_UNUSED_0x40A5 0x40A5
#define VAR_UNUSED_0x40A6 0x40A6
#define VAR_UNUSED_0x40A7 0x40A7
#define VAR_UNUSED_0x40A8 0x40A8
#define VAR_UNUSED_0x40A9 0x40A9
#define VAR_UNUSED_0x40AA 0x40AA
#define VAR_UNUSED_0x40AB 0x40AB
#define VAR_UNUSED_0x40AC 0x40AC
#define VAR_UNUSED_0x40AD 0x40AD
#define VAR_UNUSED_0x40AE 0x40AE
#define VAR_UNUSED_0x40AF 0x40AF
#define VAR_UNUSED_0x40B0 0x40B0
#define VAR_UNUSED_0x40B1 0x40B1
#define VAR_UNUSED_0x40B2 0x40B2
#define VAR_UNUSED_0x40B3 0x40B3
#define VAR_UNUSED_0x40B4 0x40B4
#define VAR_UNUSED_0x40B5 0x40B5
#define VAR_UNUSED_0x40B6 0x40B6
#define VAR_UNUSED_0x40B7 0x40B7
#define VAR_UNUSED_0x40B8 0x40B8
#define VAR_UNUSED_0x40B9 0x40B9
#define VAR_UNUSED_0x40BA 0x40BA
#define VAR_UNUSED_0x40BB 0x40BB
#define VAR_UNUSED_0x40BC 0x40BC
#define VAR_UNUSED_0x40BD 0x40BD
#define VAR_UNUSED_0x40BE 0x40BE
#define VAR_UNUSED_0x40BF 0x40BF
#define VAR_UNUSED_0x40C0 0x40C0
#define VAR_UNUSED_0x40C1 0x40C1
#define VAR_UNUSED_0x40C2 0x40C2
#define VAR_UNUSED_0x40C3 0x40C3
#define VAR_UNUSED_0x40C4 0x40C4
#define VAR_UNUSED_0x40C5 0x40C5
#define VAR_UNUSED_0x40C6 0x40C6
#define VAR_UNUSED_0x40C7 0x40C7
#define VAR_UNUSED_0x40C8 0x40C8
#define VAR_UNUSED_0x40C9 0x40C9
#define VAR_UNUSED_0x40CA 0x40CA
#define VAR_UNUSED_0x40CB 0x40CB
#define VAR_UNUSED_0x40CC 0x40CC
#define VAR_UNUSED_0x40CD 0x40CD
#define VAR_UNUSED_0x40CE 0x40CE
#define VAR_UNUSED_0x40CF 0x40CF
#define VAR_UNUSED_0x40D0 0x40D0
#define VAR_UNUSED_0x40D1 0x40D1
#define VAR_UNUSED_0x40D2 0x40D2
#define VAR_UNUSED_0x40D3 0x40D3
#define VAR_UNUSED_0x40D4 0x40D4
#define VAR_UNUSED_0x40D5 0x40D5
#define VAR_UNUSED_0x40D6 0x40D6
#define VAR_UNUSED_0x40D7 0x40D7
#define VAR_UNUSED_0x40D8 0x40D8
#define VAR_UNUSED_0x40D9 0x40D9
#define VAR_UNUSED_0x40DA 0x40DA
#define VAR_UNUSED_0x40DB 0x40DB
#define VAR_UNUSED_0x40DC 0x40DC
#define VAR_UNUSED_0x40DD 0x40DD
#define VAR_UNUSED_0x40DE 0x40DE
#define VAR_UNUSED_0x40DF 0x40DF
#define VAR_UNUSED_0x40E0 0x40E0
#define VAR_UNUSED_0x40E1 0x40E1
#define VAR_UNUSED_0x40E2 0x40E2
#define VAR_UNUSED_0x40E3 0x40E3
#define VAR_UNUSED_0x40E4 0x40E4
#define VAR_UNUSED_0x40E5 0x40E5
#define VAR_UNUSED_0x40E6 0x40E6
#define VAR_UNUSED_0x40E7 0x40E7
#define VAR_UNUSED_0x40E8 0x40E8
#define VAR_UNUSED_0x40E9 0x40E9
#define VAR_UNUSED_0x40EA 0x40EA
#define VAR_UNUSED_0x40EB 0x40EB
#define VAR_UNUSED_0x40EC 0x40EC
#define VAR_UNUSED_0x40ED 0x40ED
#define VAR_UNUSED_0x40EE 0x40EE
#define VAR_UNUSED_0x40EF 0x40EF
#define VAR_UNUSED_0x40F0 0x40F0
#define VAR_UNUSED_0x40F1 0x40F1
#define VAR_UNUSED_0x40F2 0x40F2
#define VAR_UNUSED_0x40F3 0x40F3
#define VAR_UNUSED_0x40F4 0x40F4
#define VAR_UNUSED_0x40F5 0x40F5
#define VAR_UNUSED_0x40F6 0x40F6
#define VAR_UNUSED_0x40F7 0x40F7
#define VAR_UNUSED_0x40F8 0x40F8
#define VAR_UNUSED_0x40F9 0x40F9
#define VAR_UNUSED_0x40FA 0x40FA
#define VAR_UNUSED_0x40FB 0x40FB
#define VAR_UNUSED_0x40FC 0x40FC
#define VAR_UNUSED_0x40FD 0x40FD
#define VAR_UNUSED_0x40FE 0x40FE
#define VAR_UNUSED_0x40FF 0x40FF
#endif

#if TESTING
#define TESTING_VARS_START                  0x9000
#define TESTING_VAR_DIFFICULTY              (TESTING_VARS_START + 0x0)
#define TESTING_VAR_TRAINER_SLIDES          (TESTING_VARS_START + 0x1)
#define TESTING_VAR_UNUSED_2                (TESTING_VARS_START + 0x2)
#define TESTING_VAR_UNUSED_3                (TESTING_VARS_START + 0x3)
#define TESTING_VAR_UNUSED_4                (TESTING_VARS_START + 0x4)
#define TESTING_VAR_UNUSED_5                (TESTING_VARS_START + 0x5)
#define TESTING_VAR_UNUSED_6                (TESTING_VARS_START + 0x6)
#define TESTING_VAR_UNUSED_7                (TESTING_VARS_START + 0x7)
#endif // TESTING

#endif // GUARD_CONSTANTS_VARS_H
