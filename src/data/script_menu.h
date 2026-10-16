// multichoice lists
static const struct MenuAction MultichoiceList_BrineyOnDewford[] =
{
    {COMPOUND_STRING("PETALBURG")},
    {COMPOUND_STRING("SLATEPORT")},
    {gText_Exit},
};

const u8 gText_Info2[] = _("INFO");

static const struct MenuAction MultichoiceList_EnterInfo[] =
{
    {COMPOUND_STRING("ENTER")},
    {gText_Info2},
    {gText_Exit},
};



static const struct MenuAction MultichoiceList_Bike[] =
{
    {COMPOUND_STRING("MACH")},
    {COMPOUND_STRING("ACRO")},
};

static const struct MenuAction MultichoiceList_StatusInfo[] =
{
    {COMPOUND_STRING("PSN")},
    {COMPOUND_STRING("PAR")},
    {COMPOUND_STRING("SLP")},
    {COMPOUND_STRING("BRN")},
    {COMPOUND_STRING("FRZ")},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_BrineyOffDewford[] =
{
    {COMPOUND_STRING("DEWFORD")},
    {gText_Exit},
};


static const struct MenuAction MultichoiceList_YesNoInfo2[] =
{
    {gText_Yes},
    {gText_No},
    {gText_Info2},
};

static const struct MenuAction MultichoiceList_ChallengeInfo[] =
{
    {COMPOUND_STRING("CHALLENGE")},
    {COMPOUND_STRING("INFO")},
    {gText_Exit},
};


static const struct MenuAction MultichoiceList_Mechadoll1_Q1[] =
{
    {COMPOUND_STRING("ODDISH")},
    {COMPOUND_STRING("POOCHYENA")},
    {COMPOUND_STRING("TAILLOW")},
};

static const struct MenuAction MultichoiceList_Mechadoll1_Q2[] =
{
    {COMPOUND_STRING("AZURILL")},
    {COMPOUND_STRING("LOTAD")},
    {COMPOUND_STRING("WINGULL")},
};

static const struct MenuAction MultichoiceList_Mechadoll1_Q3[] =
{
    {COMPOUND_STRING("DUSTOX")},
    {COMPOUND_STRING("ZUBAT")},
    {COMPOUND_STRING("NINCADA")},
};

static const struct MenuAction MultichoiceList_Mechadoll2_Q1[] =
{
    {COMPOUND_STRING("RALTS")},
    {COMPOUND_STRING("ZIGZAGOON")},
    {COMPOUND_STRING("SLAKOTH")},
};

static const struct MenuAction MultichoiceList_Mechadoll2_Q2[] =
{
    {COMPOUND_STRING("POOCHYENA")},
    {COMPOUND_STRING("SHROOMISH")},
    {COMPOUND_STRING("ZIGZAGOON")},
};

static const struct MenuAction MultichoiceList_Mechadoll2_Q3[] =
{
    {COMPOUND_STRING("POOCHYENA")},
    {COMPOUND_STRING("ZUBAT")},
    {COMPOUND_STRING("CARVANHA")},
};

static const struct MenuAction MultichoiceList_Mechadoll3_Q1[] =
{
    {COMPOUND_STRING("BURN HEAL")},
    {COMPOUND_STRING("HARBOR MAIL")},
    {COMPOUND_STRING("Same price")},
};

static const struct MenuAction MultichoiceList_Mechadoll3_Q2[] =
{
    {COMPOUND_STRING("¥60")},
    {COMPOUND_STRING("¥55")},
    {COMPOUND_STRING("Nothing")},
};

static const struct MenuAction MultichoiceList_Mechadoll3_Q3[] =
{
    {COMPOUND_STRING("They will cost more.")},
    {COMPOUND_STRING("They will cost less.")},
    {COMPOUND_STRING("Same price")},
};

static const struct MenuAction MultichoiceList_Mechadoll4_Q1[] =
{
    {COMPOUND_STRING("Male")},
    {COMPOUND_STRING("Female")},
    {COMPOUND_STRING("Neither")},
};

static const struct MenuAction MultichoiceList_Mechadoll4_Q2[] =
{
    {COMPOUND_STRING("Elderly men")},
    {COMPOUND_STRING("Elderly ladies")},
    {COMPOUND_STRING("Same number")},
};

static const struct MenuAction MultichoiceList_Mechadoll4_Q3[] =
{
    {COMPOUND_STRING("None")},
    {COMPOUND_STRING("1")},
    {COMPOUND_STRING("2")},
};

static const struct MenuAction MultichoiceList_Mechadoll5_Q1[] =
{
    {COMPOUND_STRING("2")},
    {COMPOUND_STRING("3")},
    {COMPOUND_STRING("4")},
};

static const struct MenuAction MultichoiceList_Mechadoll5_Q2[] =
{
    {COMPOUND_STRING("6")},
    {COMPOUND_STRING("7")},
    {COMPOUND_STRING("8")},
};

static const struct MenuAction MultichoiceList_Mechadoll5_Q3[] =
{
    {COMPOUND_STRING("6")},
    {COMPOUND_STRING("7")},
    {COMPOUND_STRING("8")},
};

static const struct MenuAction MultichoiceList_VendingMachine[] =
{
    {COMPOUND_STRING("FRESH WATER{CLEAR_TO 72}¥200")},
    {COMPOUND_STRING("SODA POP{CLEAR_TO 72}¥300")},
    {COMPOUND_STRING("LEMONADE{CLEAR_TO 72}¥350")},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_MachBikeInfo[] =
{
    {COMPOUND_STRING("HOW TO RIDE")},
    {COMPOUND_STRING("HOW TO TURN")},
    {COMPOUND_STRING("SANDY SLOPES")},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_AcroBikeInfo[] =
{
    {COMPOUND_STRING("WHEELIES")},
    {COMPOUND_STRING("BUNNY-HOPS")},
    {COMPOUND_STRING("JUMP")},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_Satisfaction[] =
{
    {COMPOUND_STRING("Satisfied")},
    {COMPOUND_STRING("Dissatisfied")},
};

static const struct MenuAction MultichoiceList_SternDeepSea[] =
{
    {COMPOUND_STRING("DEEPSEATOOTH")},
    {COMPOUND_STRING("DEEPSEASCALE")},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_UnusedAshVendor[] =
{
    {COMPOUND_STRING("BLUE FLUTE")},
    {COMPOUND_STRING("YELLOW FLUTE")},
    {COMPOUND_STRING("RED FLUTE")},
    {COMPOUND_STRING("WHITE FLUTE")},
    {COMPOUND_STRING("BLACK FLUTE")},
    {COMPOUND_STRING("GLASS CHAIR")},
    {COMPOUND_STRING("GLASS DESK")},
    {gText_Cancel2},
};

static const struct MenuAction MultichoiceList_GameCornerDolls[] =
{
    {COMPOUND_STRING("TREECKO DOLL 1,000 COINS")},
    {COMPOUND_STRING("TORCHIC DOLL 1,000 COINS")},
    {COMPOUND_STRING("MUDKIP DOLL   1,000 COINS")},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_GameCornerTMs[] =
{
    {COMPOUND_STRING("TM32{CLEAR_TO 72}1,500 COINS")},
    {COMPOUND_STRING("TM29{CLEAR_TO 72}3,500 COINS")},
    {COMPOUND_STRING("TM35{CLEAR_TO 72}4,000 COINS")},
    {COMPOUND_STRING("TM24{CLEAR_TO 72}4,000 COINS")},
    {COMPOUND_STRING("TM13{CLEAR_TO 72}4,000 COINS")},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_GameCornerCoins[] =
{
    {COMPOUND_STRING("  50 COINS    ¥1,000")},
    {COMPOUND_STRING("500 COINS  ¥10,000")},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_HowsFishing[] =
{
    {COMPOUND_STRING("Excellent")},
    {COMPOUND_STRING("Not so good")},
};

const u8 gText_LilycoveCity[] = _("LILYCOVE CITY");

static const struct MenuAction MultichoiceList_RightLeft[] =
{
    {COMPOUND_STRING("Right")},
    {COMPOUND_STRING("Left")},
};

static const struct MenuAction MultichoiceList_Floors[] =
{
    {gText_5F},
    {gText_4F},
    {gText_3F},
    {gText_2F},
    {gText_1F},
    {gText_Exit},
};

const u8 gText_RedShard[] = _("RED SHARD");
const u8 gText_YellowShard[] = _("YELLOW SHARD");
const u8 gText_BlueShard[] = _("BLUE SHARD");
const u8 gText_GreenShard[] = _("GREEN SHARD");

static const struct MenuAction MultichoiceList_ShardsR[] =
{
    {gText_RedShard},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_ShardsY[] =
{
    {gText_YellowShard},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_ShardsRY[] =
{
    {gText_RedShard},
    {gText_YellowShard},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_ShardsB[] =
{
    {gText_BlueShard},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_ShardsRB[] =
{
    {gText_RedShard},
    {gText_BlueShard},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_ShardsYB[] =
{
    {gText_YellowShard},
    {gText_BlueShard},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_ShardsRYB[] =
{
    {gText_RedShard},
    {gText_YellowShard},
    {gText_BlueShard},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_ShardsG[] =
{
    {gText_GreenShard},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_ShardsRG[] =
{
    {gText_RedShard},
    {gText_GreenShard},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_ShardsYG[] =
{
    {gText_YellowShard},
    {gText_GreenShard},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_ShardsRYG[] =
{
    {gText_RedShard},
    {gText_YellowShard},
    {gText_GreenShard},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_ShardsBG[] =
{
    {gText_BlueShard},
    {gText_GreenShard},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_ShardsRBG[] =
{
    {gText_RedShard},
    {gText_BlueShard},
    {gText_GreenShard},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_ShardsYBG[] =
{
    {gText_YellowShard},
    {gText_BlueShard},
    {gText_GreenShard},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_ShardsRYBG[] =
{
    {gText_RedShard},
    {gText_YellowShard},
    {gText_BlueShard},
    {gText_GreenShard},
    {gText_Exit},
};

const u8 gText_Opponent[] = _("OPPONENT");

const u8 gText_TradeCenter[] = _("TRADE CENTER");
const u8 gText_Colosseum[] = _("COLOSSEUM");

static const struct MenuAction MultichoiceList_YesNoInfo[] =
{
    {gText_Yes},
    {gText_No},
    {gText_Info2},
};

static const struct MenuAction MultichoiceList_BattleMode[] =
{
    {COMPOUND_STRING("SINGLE BATTLE")},
    {COMPOUND_STRING("DOUBLE BATTLE")},
    {COMPOUND_STRING("MULTI BATTLE")},
    {gText_Info2},
    {gText_Exit},
};


static const struct MenuAction MultichoiceList_LinkServices[] =
{
    {gText_TradeCenter},
    {gText_Colosseum},
    {gText_Exit},
};


static const struct MenuAction MultichoiceList_LinkLeader[] =
{
    {COMPOUND_STRING("JOIN GROUP")},
    {COMPOUND_STRING("BECOME LEADER")},
    {gText_Exit},
};




static const struct MenuAction MultichoiceList_Fossil[] =
{
    {COMPOUND_STRING("CLAW FOSSIL")},
    {COMPOUND_STRING("ROOT FOSSIL")},
    {gText_Exit},
};

static const struct MenuAction MultichoiceList_YesNo[] =
{
    {gText_Yes},
    {COMPOUND_STRING("NO")},
};





static const struct MenuAction MultichoiceList_WheresRayquaza[] =
{
    {COMPOUND_STRING("CAVE OF ORIGIN")},
    {COMPOUND_STRING("MT. PYRE")},
    {COMPOUND_STRING("SKY PILLAR")},
    {COMPOUND_STRING("Don't remember")},
};




static const struct MenuAction MultichoiceList_BerryPlot[] =
{
    {COMPOUND_STRING("FERTILIZE")},
    {COMPOUND_STRING("PLANT BERRY")},
    {gText_Exit},
};

static const struct MenuAction sMultichoiceList_BikeShop[] = {
    {COMPOUND_STRING("BICYCLE{CLEAR_TO 73}{FONT_SMALL}¥1,000,000")},
    {COMPOUND_STRING("NO THANKS")}
};

static const struct MenuAction sMultichoiceList_Eeveelutions[] = {
    {COMPOUND_STRING("EEVEE")},
    {COMPOUND_STRING("FLAREON")},
    {COMPOUND_STRING("JOLTEON")},
    {COMPOUND_STRING("VAPOREON")},
    {COMPOUND_STRING("Quit looking.")}
};

static const u8 gText_SeviiIslands[] = _("SEVII ISLANDS");
static const u8 gText_OneIsland[] = _("ONE ISLAND");
static const u8 gText_TwoIsland[] = _("TWO ISLAND");
static const u8 gText_ThreeIsland[] = _("THREE ISLAND");
static const u8 gText_Vermilion[] = _("VERMILION");

static const struct MenuAction sMultichoiceList_Island23[] = {
    {gText_TwoIsland},
    {gText_ThreeIsland},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_Island13[] = {
    {gText_OneIsland},
    {gText_ThreeIsland},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_Island12[] = {
    {gText_OneIsland},
    {gText_TwoIsland},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_SeviiNavel[] = {
    {gText_SeviiIslands},
    {gText_NavelRock},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_SeviiBirth[] = {
    {gText_SeviiIslands},
    {gText_BirthIsland},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_SeviiNavelBirth[] = {
    {gText_SeviiIslands},
    {gText_NavelRock},
    {gText_BirthIsland},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_Seagallop123[] = {
    {gText_OneIsland},
    {gText_TwoIsland},
    {gText_ThreeIsland},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_SeagallopV23[] = {
    {gText_Vermilion},
    {gText_TwoIsland},
    {gText_ThreeIsland},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_SeagallopV13[] = {
    {gText_Vermilion},
    {gText_OneIsland},
    {gText_ThreeIsland},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_SeagallopV12[] = {
    {gText_Vermilion},
    {gText_OneIsland},
    {gText_TwoIsland},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_SeagallopVermilion[] = {
    {gText_Vermilion},
    {gText_Exit}
};

const u8 sText_NoThanks[] = _("NO THANKS");

static const struct MenuAction sMultichoiceList_GameCornerPokemonPrizes[] = {
#if defined(FIRERED)
    {COMPOUND_STRING("ABRA{CLEAR_TO 85}{FONT_SMALL} 180 COINS")},
    {COMPOUND_STRING("CLEFAIRY{CLEAR_TO 85}{FONT_SMALL} 500 COINS")},
    {COMPOUND_STRING("DRATINI{CLEAR_TO 75}{FONT_SMALL} 2,800 COINS")},
    {COMPOUND_STRING("SCYTHER{CLEAR_TO 75}{FONT_SMALL} 5,500 COINS")},
    {COMPOUND_STRING("PORYGON{CLEAR_TO 75}{FONT_SMALL} 9,999 COINS")},
#else
    {COMPOUND_STRING("ABRA{CLEAR_TO 85}{FONT_SMALL} 120 COINS")},
    {COMPOUND_STRING("CLEFAIRY{CLEAR_TO 85}{FONT_SMALL} 750 COINS")},
    {COMPOUND_STRING("PINSIR{CLEAR_TO 75}{FONT_SMALL} 2,500 COINS")},
    {COMPOUND_STRING("DRATINI{CLEAR_TO 75}{FONT_SMALL} 4,600 COINS")},
    {COMPOUND_STRING("PORYGON{CLEAR_TO 75}{FONT_SMALL} 6,500 COINS")},
#endif
    {sText_NoThanks}
};

static const struct MenuAction sMultichoiceList_GameCornerTMPrizes[] = {
    {COMPOUND_STRING("TM13{CLEAR_TO 72}{FONT_SMALL}4,000 COINS")},
    {COMPOUND_STRING("TM23{CLEAR_TO 72}{FONT_SMALL}3,500 COINS")},
    {COMPOUND_STRING("TM24{CLEAR_TO 72}{FONT_SMALL}4,000 COINS")},
    {COMPOUND_STRING("TM30{CLEAR_TO 72}{FONT_SMALL}4,500 COINS")},
    {COMPOUND_STRING("TM35{CLEAR_TO 72}{FONT_SMALL}4,000 COINS")},
    {sText_NoThanks}
};

static const struct MenuAction sMultichoiceList_GameCornerBattleItemPrizes[] = {
    {COMPOUND_STRING("SMOKE BALL{CLEAR_TO 90}{FONT_SMALL}800 COINS")},
    {COMPOUND_STRING("MIRACLE SEED{CLEAR_TO 80}{FONT_SMALL}1,000 COINS")},
    {COMPOUND_STRING("CHARCOAL{CLEAR_TO 80}{FONT_SMALL}1,000 COINS")},
    {COMPOUND_STRING("MYSTIC WATER{CLEAR_TO 80}{FONT_SMALL}1,000 COINS")},
    {COMPOUND_STRING("YELLOW FLUTE{CLEAR_TO 80}{FONT_SMALL}1,600 COINS")},
    {sText_NoThanks}
};

static const struct MenuAction sMultichoiceList_DeptStoreElevator[] = {
    {COMPOUND_STRING("5F")},
    {COMPOUND_STRING("4F")},
    {COMPOUND_STRING("3F")},
    {COMPOUND_STRING("2F")},
    {COMPOUND_STRING("1F")},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_GameCornerCoinPurchaseCounter[] = {
    {COMPOUND_STRING("{FONT_SMALL} 50 COINS{CLEAR_TO 69}¥1,000")},
    {COMPOUND_STRING("{FONT_SMALL}500 COINS{CLEAR_TO 64}¥10,000")},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_LinkedDirectUnion[] = {
    {COMPOUND_STRING("LINKED GAME PLAY")},
    {COMPOUND_STRING("DIRECT CORNER")},
    {COMPOUND_STRING("UNION ROOM")},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_CeladonVendingMachine[] = {
    {COMPOUND_STRING("FRESH WATER{CLEAR_TO 87}{FONT_SMALL}¥200")},
    {COMPOUND_STRING("SODA POP{CLEAR_TO 87}{FONT_SMALL}¥300")},
    {COMPOUND_STRING("LEMONADE{CLEAR_TO 87}{FONT_SMALL}¥350")},
    {gText_Exit}
};

const u8 sText_FreshWater[] = _("FRESH WATER");
const u8 sText_SodaPop[] = _("SODA POP");
const u8 sText_Lemonade[] = _("LEMONADE");

static const struct MenuAction sMultichoiceList_ThirstyGirlFreshWater[] = {
    {sText_FreshWater},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_ThirstyGirlSodaPop[] = {
    {sText_SodaPop},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_ThirstyGirlFreshWaterSodaPop[] = {
    {sText_FreshWater},
    {sText_SodaPop},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_ThirstyGirlLemonade[] = {
    {sText_Lemonade},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_ThirstyGirlFreshWaterLemonade[] = {
    {sText_FreshWater},
    {sText_Lemonade},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_ThirstyGirlSodaPopLemonade[] = {
    {sText_SodaPop},
    {sText_Lemonade},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_ThirstyGirlFreshWaterSodaPopLemonade[] = {
    {sText_FreshWater},
    {sText_SodaPop},
    {sText_Lemonade},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_RocketHideoutElevator[] = {
    {gText_B1F},
    {gText_B2F},
    {gText_B4F},
    {gText_Exit}
};

static const u8 sText_HelixFossil[] = _("HELIX FOSSIL");
static const u8 sText_DomeFossil[] = _("DOME FOSSIL");
static const u8 sText_OldAmber[] = _("OLD AMBER");

static const struct MenuAction sMultichoiceList_Helix[] = {
    {sText_HelixFossil},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_Dome[] = {
    {sText_DomeFossil},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_Amber[] = {
    {sText_OldAmber},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_HelixAmber[] = {
    {sText_HelixFossil},
    {sText_OldAmber},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_DomeAmber[] = {
    {sText_DomeFossil},
    {sText_OldAmber},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_Mushrooms[] = {
    {COMPOUND_STRING("2 TINYMUSHROOMS")},
    {COMPOUND_STRING("1 BIG MUSHROOM")}
};

static const struct MenuAction sMultichoiceList_RooftopB1F[] = {
    {gText_Rooftop},
    {gText_B1F},
    {gText_Exit}
};

static const struct MenuAction sMultichoiceList_TrainerCardIconTint[] = {
    {gText_Normal},
    {COMPOUND_STRING("BLACK")},
    {COMPOUND_STRING("PINK")},
    {COMPOUND_STRING("SEPIA")}
};


static const struct MenuAction sMultichoiceList_HOF_Quit[] = {
    {gText_HallOfFame},
    {gText_ShopQuit}
};







static const struct MenuAction MultichoiceList_Exit[] =
{
    {gText_Exit},
};

struct MultichoiceListStruct
{
    const struct MenuAction *list;
    u8 count;
};

static const struct MultichoiceListStruct sMultichoiceLists[] =
{
    [MULTI_BRINEY_ON_DEWFORD]          = MULTICHOICE(MultichoiceList_BrineyOnDewford),
    [MULTI_PC]                         = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_ENTERINFO]                  = MULTICHOICE(MultichoiceList_EnterInfo),
    [MULTI_UNUSED_3]                   = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_UNUSED_4]                   = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_UNUSED_5]                   = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_UNUSED_9]                   = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_UNUSED_10]                  = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_BIKE]                       = MULTICHOICE(MultichoiceList_Bike),
    [MULTI_STATUS_INFO]                = MULTICHOICE(MultichoiceList_StatusInfo),
    [MULTI_BRINEY_OFF_DEWFORD]         = MULTICHOICE(MultichoiceList_BrineyOffDewford),
    [MULTI_UNUSED_15]                  = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_UNUSED_12] = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_YESNOINFO]                  = MULTICHOICE(MultichoiceList_YesNoInfo),
    [MULTI_BATTLE_MODE]                = MULTICHOICE(MultichoiceList_BattleMode),
    [MULTI_UNUSED_19]                  = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_YESNOINFO_2]                = MULTICHOICE(MultichoiceList_YesNoInfo2),
    [MULTI_UNUSED_21]                  = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_UNUSED_22]                  = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_CHALLENGEINFO]              = MULTICHOICE(MultichoiceList_ChallengeInfo),
    [MULTI_UNUSED_20] = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_MECHADOLL1_Q1]              = MULTICHOICE(MultichoiceList_Mechadoll1_Q1),
    [MULTI_MECHADOLL1_Q2]              = MULTICHOICE(MultichoiceList_Mechadoll1_Q2),
    [MULTI_MECHADOLL1_Q3]              = MULTICHOICE(MultichoiceList_Mechadoll1_Q3),
    [MULTI_MECHADOLL2_Q1]              = MULTICHOICE(MultichoiceList_Mechadoll2_Q1),
    [MULTI_MECHADOLL2_Q2]              = MULTICHOICE(MultichoiceList_Mechadoll2_Q2),
    [MULTI_MECHADOLL2_Q3]              = MULTICHOICE(MultichoiceList_Mechadoll2_Q3),
    [MULTI_MECHADOLL3_Q1]              = MULTICHOICE(MultichoiceList_Mechadoll3_Q1),
    [MULTI_MECHADOLL3_Q2]              = MULTICHOICE(MultichoiceList_Mechadoll3_Q2),
    [MULTI_MECHADOLL3_Q3]              = MULTICHOICE(MultichoiceList_Mechadoll3_Q3),
    [MULTI_MECHADOLL4_Q1]              = MULTICHOICE(MultichoiceList_Mechadoll4_Q1),
    [MULTI_MECHADOLL4_Q2]              = MULTICHOICE(MultichoiceList_Mechadoll4_Q2),
    [MULTI_MECHADOLL4_Q3]              = MULTICHOICE(MultichoiceList_Mechadoll4_Q3),
    [MULTI_MECHADOLL5_Q1]              = MULTICHOICE(MultichoiceList_Mechadoll5_Q1),
    [MULTI_MECHADOLL5_Q2]              = MULTICHOICE(MultichoiceList_Mechadoll5_Q2),
    [MULTI_MECHADOLL5_Q3]              = MULTICHOICE(MultichoiceList_Mechadoll5_Q3),
    [MULTI_UNUSED_40]                  = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_UNUSED_41]                  = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_VENDING_MACHINE]            = MULTICHOICE(MultichoiceList_VendingMachine),
    [MULTI_MACH_BIKE_INFO]             = MULTICHOICE(MultichoiceList_MachBikeInfo),
    [MULTI_ACRO_BIKE_INFO]             = MULTICHOICE(MultichoiceList_AcroBikeInfo),
    [MULTI_SATISFACTION]               = MULTICHOICE(MultichoiceList_Satisfaction),
    [MULTI_STERN_DEEPSEA]              = MULTICHOICE(MultichoiceList_SternDeepSea),
    [MULTI_UNUSED_ASH_VENDOR]          = MULTICHOICE(MultichoiceList_UnusedAshVendor),
    [MULTI_GAME_CORNER_DOLLS]          = MULTICHOICE(MultichoiceList_GameCornerDolls),
    [MULTI_GAME_CORNER_COINS]          = MULTICHOICE(MultichoiceList_GameCornerCoins),
    [MULTI_HOWS_FISHING]               = MULTICHOICE(MultichoiceList_HowsFishing),
    [MULTI_UNUSED_51]                  = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_RIGHTLEFT]                  = MULTICHOICE(MultichoiceList_RightLeft),
    [MULTI_GAME_CORNER_TMS]            = MULTICHOICE(MultichoiceList_GameCornerTMs),
    [MULTI_FLOORS]                     = MULTICHOICE(MultichoiceList_Floors),
    [MULTI_SHARDS_R]                   = MULTICHOICE(MultichoiceList_ShardsR),
    [MULTI_SHARDS_Y]                   = MULTICHOICE(MultichoiceList_ShardsY),
    [MULTI_SHARDS_RY]                  = MULTICHOICE(MultichoiceList_ShardsRY),
    [MULTI_SHARDS_B]                   = MULTICHOICE(MultichoiceList_ShardsB),
    [MULTI_SHARDS_RB]                  = MULTICHOICE(MultichoiceList_ShardsRB),
    [MULTI_SHARDS_YB]                  = MULTICHOICE(MultichoiceList_ShardsYB),
    [MULTI_SHARDS_RYB]                 = MULTICHOICE(MultichoiceList_ShardsRYB),
    [MULTI_SHARDS_G]                   = MULTICHOICE(MultichoiceList_ShardsG),
    [MULTI_SHARDS_RG]                  = MULTICHOICE(MultichoiceList_ShardsRG),
    [MULTI_SHARDS_YG]                  = MULTICHOICE(MultichoiceList_ShardsYG),
    [MULTI_SHARDS_RYG]                 = MULTICHOICE(MultichoiceList_ShardsRYG),
    [MULTI_SHARDS_BG]                  = MULTICHOICE(MultichoiceList_ShardsBG),
    [MULTI_SHARDS_RBG]                 = MULTICHOICE(MultichoiceList_ShardsRBG),
    [MULTI_SHARDS_YBG]                 = MULTICHOICE(MultichoiceList_ShardsYBG),
    [MULTI_SHARDS_RYBG]                = MULTICHOICE(MultichoiceList_ShardsRYBG),
    [MULTI_LINK_SERVICES]   = MULTICHOICE(MultichoiceList_LinkServices),
    [MULTI_LINK_LEADER]                = MULTICHOICE(MultichoiceList_LinkLeader),
    [MULTI_FOSSIL]                     = MULTICHOICE(MultichoiceList_Fossil),
    [MULTI_YESNO]                      = MULTICHOICE(MultichoiceList_YesNo),
    [MULTI_UNUSED_70] = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_WHERES_RAYQUAZA]            = MULTICHOICE(MultichoiceList_WheresRayquaza),
    [MULTI_UNUSED_72] = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_BERRY_PLOT]                 = MULTICHOICE(MultichoiceList_BerryPlot),
    [MULTI_BIKE_SHOP]                  = MULTICHOICE(sMultichoiceList_BikeShop),
    [MULTI_EEVEELUTIONS]               = MULTICHOICE(sMultichoiceList_Eeveelutions),
    [MULTI_ISLAND_23]                  = MULTICHOICE(sMultichoiceList_Island23),
    [MULTI_ISLAND_13]                  = MULTICHOICE(sMultichoiceList_Island13),
    [MULTI_ISLAND_12]                  = MULTICHOICE(sMultichoiceList_Island12),
    [MULTI_SEVII_NAVEL]                = MULTICHOICE(sMultichoiceList_SeviiNavel),
    [MULTI_SEVII_BIRTH]                = MULTICHOICE(sMultichoiceList_SeviiBirth),
    [MULTI_SEVII_NAVEL_BIRTH]          = MULTICHOICE(sMultichoiceList_SeviiNavelBirth),
    [MULTI_SEAGALLOP_123]              = MULTICHOICE(sMultichoiceList_Seagallop123),
    [MULTI_SEAGALLOP_V23]              = MULTICHOICE(sMultichoiceList_SeagallopV23),
    [MULTI_SEAGALLOP_V13]              = MULTICHOICE(sMultichoiceList_SeagallopV13),
    [MULTI_SEAGALLOP_V12]              = MULTICHOICE(sMultichoiceList_SeagallopV12),
    [MULTI_SEAGALLOP_VERMILION]        = MULTICHOICE(sMultichoiceList_SeagallopVermilion),
    [MULTI_GAME_CORNER_POKEMON_PRIZES] = MULTICHOICE(sMultichoiceList_GameCornerPokemonPrizes),
    [MULTI_GAME_CORNER_TMPRIZES]           = MULTICHOICE(sMultichoiceList_GameCornerTMPrizes),
    [MULTI_GAME_CORNER_BATTLE_ITEM_PRIZES] = MULTICHOICE(sMultichoiceList_GameCornerBattleItemPrizes),
    [MULTI_DEPT_STORE_ELEVATOR]            = MULTICHOICE(sMultichoiceList_DeptStoreElevator),
    [MULTI_GAME_CORNER_COIN_PURCHASE_COUNTER] = MULTICHOICE(sMultichoiceList_GameCornerCoinPurchaseCounter),
    [MULTI_LINKED_DIRECT_UNION]         = MULTICHOICE(sMultichoiceList_LinkedDirectUnion),
    [MULTI_CELADON_VENDING_MACHINE]           = MULTICHOICE(sMultichoiceList_CeladonVendingMachine),
    [MULTI_THIRSTY_GIRL_FRESH_WATER]                   = MULTICHOICE(sMultichoiceList_ThirstyGirlFreshWater),
    [MULTI_THIRSTY_GIRL_SODA_POP]                      = MULTICHOICE(sMultichoiceList_ThirstyGirlSodaPop),
    [MULTI_THIRSTY_GIRL_FRESH_WATER_SODA_POP]          = MULTICHOICE(sMultichoiceList_ThirstyGirlFreshWaterSodaPop),
    [MULTI_THIRSTY_GIRL_LEMONADE]                      = MULTICHOICE(sMultichoiceList_ThirstyGirlLemonade),
    [MULTI_THIRSTY_GIRL_FRESH_WATER_LEMONADE]          = MULTICHOICE(sMultichoiceList_ThirstyGirlFreshWaterLemonade),
    [MULTI_THIRSTY_GIRL_SODA_POP_LEMONADE]             = MULTICHOICE(sMultichoiceList_ThirstyGirlSodaPopLemonade),
    [MULTI_THIRSTY_GIRL_FRESH_WATER_SODA_POP_LEMONADE] = MULTICHOICE(sMultichoiceList_ThirstyGirlFreshWaterSodaPopLemonade),
    [MULTI_ROCKET_HIDEOUT_ELEVATOR]                    = MULTICHOICE(sMultichoiceList_RocketHideoutElevator),
    [MULTI_HELIX]                                      = MULTICHOICE(sMultichoiceList_Helix),
    [MULTI_DOME]                                       = MULTICHOICE(sMultichoiceList_Dome),
    [MULTI_AMBER]                                      = MULTICHOICE(sMultichoiceList_Amber),
    [MULTI_HELIX_AMBER]                                = MULTICHOICE(sMultichoiceList_HelixAmber),
    [MULTI_DOME_AMBER]                                 = MULTICHOICE(sMultichoiceList_DomeAmber),
    [MULTI_MUSHROOMS]                                  = MULTICHOICE(sMultichoiceList_Mushrooms),
    [MULTI_ROOFTOP_B1F]                                = MULTICHOICE(sMultichoiceList_RooftopB1F),
    [MULTI_TRAINER_CARD_ICON_TINT]                     = MULTICHOICE(sMultichoiceList_TrainerCardIconTint),
    [MULTI_HOF_QUIT]                                   = MULTICHOICE(sMultichoiceList_HOF_Quit),
    [MULTI_UNUSED_111] = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_UNUSED_112] = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_UNUSED_113] = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_UNUSED_114] = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_UNUSED_115] = MULTICHOICE(MultichoiceList_Exit),
    [MULTI_UNUSED_116] = MULTICHOICE(MultichoiceList_Exit),
};

static const u8 *const sStdStrings[] =
{
    [STDSTRING_ITEMS] = COMPOUND_STRING("ITEMS"),
    [STDSTRING_KEYITEMS] = COMPOUND_STRING("KEY ITEMS"),
    [STDSTRING_POKEBALLS] = COMPOUND_STRING("POKé BALLS"),
    [STDSTRING_TMHMS] = COMPOUND_STRING("TMs & HMs"),
    [STDSTRING_BERRIES] = COMPOUND_STRING("BERRIES"),
    [STDSTRING_SINGLE] = COMPOUND_STRING("SINGLE"),
    [STDSTRING_DOUBLE] = COMPOUND_STRING("DOUBLE"),
    [STDSTRING_MULTI] = COMPOUND_STRING("MULTI"),
    [STDSTRING_MULTI_LINK] = COMPOUND_STRING("MULTI-LINK"),
    [STDSTRING_BOULDER_BADGE] = gText_Boulderbadge,
    [STDSTRING_CASCADE_BADGE] = gText_Cascadebadge,
    [STDSTRING_THUNDER_BADGE] = gText_Thunderbadge,
    [STDSTRING_RAINBOW_BADGE] = gText_Rainbowbadge,
    [STDSTRING_SOUL_BADGE]    = gText_Soulbadge,
    [STDSTRING_MARSH_BADGE]   = gText_Marshbadge,
    [STDSTRING_VOLCANO_BADGE] = gText_Volcanobadge,
    [STDSTRING_EARTH_BADGE]   = gText_Earthbadge,
    [STDSTRING_COINS]         = COMPOUND_STRING("COINS"),
};

static const u8 *const sPCNameStrings[] =
{
    gText_SomeonesPC,
    gText_LanettesPC,
    gText_PlayersPC,
    gText_LogOff,
};


static const u8 *const sSeagallopDestStrings[] = {
    [SEAGALLOP_VERMILION_CITY] = gText_Vermilion,
    [SEAGALLOP_ONE_ISLAND]     = gText_OneIsland,
    [SEAGALLOP_TWO_ISLAND]     = gText_TwoIsland,
    [SEAGALLOP_THREE_ISLAND]   = gText_ThreeIsland,
    [SEAGALLOP_FOUR_ISLAND]    = COMPOUND_STRING("FOUR ISLAND"),
    [SEAGALLOP_FIVE_ISLAND]    = COMPOUND_STRING("FIVE ISLAND"),
    [SEAGALLOP_SIX_ISLAND]     = COMPOUND_STRING("SIX ISLAND"),
    [SEAGALLOP_SEVEN_ISLAND]   = COMPOUND_STRING("SEVEN ISLAND"),
};
