#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <array>

#include "cmd.hpp"

#include "sampgdk.h"
#include "streamer.hpp"

#define VERSION "0.1.0-dev"

// job vehicles
std::array<int, 5> Sweeper;
struct JOBV_BUS {
  char Owner[MAX_PLAYER_NAME];
  int ID;
};
std::array<JOBV_BUS, 7> Bus;
std::array<int, 4> Mower;

char DCReason[3][16] = {
  "Timeout/Crash",
  "Quit",
  "Banned/Kicked"
};

using namespace Plugins::Streamer;

int main() {
  printf("[Deathrow Vatos RolePlay]\n");
  return 0;
}

char *RetPname(int playerid, bool underscore = false) {
  char *name = (char*) malloc(MAX_PLAYER_NAME * sizeof(char));
  if(!IsPlayerConnected(playerid)) return name;
  GetPlayerName(playerid, name, sizeof(name));
  if (underscore) for(int i; i < strlen(name); i++) if(name[i] == '_') name[i] = ' ';
  return name;
}

bool HasNoEngine(int vehicleid) {
  switch(GetVehicleModel(vehicleid)) {
    case 481: return true;
    case 509: return true;
    case 510: return true;
    default: return false;
  }
}

PLUGIN_EXPORT bool PLUGIN_CALL OnGameModeInit() {
  char gmText[32];
  sprintf(gmText, "DRV-RP %s", VERSION);

  SetGameModeText(gmText);
  EnableStuntBonusForAll(0);
  SetNameTagDrawDistance(10.0);
  ShowPlayerMarkers(0);
  DisableInteriorEnterExits();
  ManualVehicleEngineAndLights();
  EnableVehicleFriendlyFire();
  SetDeathDropAmount(1000);
	ShowNameTags(1);
	AllowAdminTeleport(1);

  // pickups creations
  Pickup::Create(19832,0,-12.9450,2350.7974,24.1406); //gun crafting point
	Pickup::Create(2037,0,613.0717,1549.8906,5.0001); // material point
	Pickup::Create(2044,0,-752.7269,-131.6847,65.8281); // gun making
	Pickup::Create(1275,0,-757.2897,-133.7420,65.8281); // gun maker job take
	Pickup::Create(1239,0,1313.1063,-875.3223,39.5781); // sweeper
	Pickup::Create(1239,0,1271.9991,-2038.5074,59.0828); // bus
	Pickup::Create(1239,0,764.2607,-1304.5879,13.5613); // mower
	Pickup::Create(2037,0,2197.5491,-2661.5784,13.5469); // product point
	Pickup::Create(1239,0,1562.2598,-2300.6880,13.5650); // rent vehicle Point
	Pickup::Create(1239,0,1926.1271,-1788.2462,13.3906); // rent vehicle Point2
	Pickup::Create(1275,0,2139.5847,-1733.7576,17.2891); // mechanic job pickup
	Pickup::Create(1275,0,-49.8569,-269.3626,6.6332); // trucker job point
	Pickup::Create(1275,0,2914.6526,-802.2943,11.0469); // mechanic duty point
	Pickup::Create(2037,0,2286.4944,-2013.8217,13.5442); // getcomponent point
	Pickup::Create(1239,0,-2237.0012,130.1817,1035.4141); // buypoint electronic
	Pickup::Create(1239,0,148.2934,1698.5463,1002.1363); // buypoint tool
	Pickup::Create(1318,0,-2240.7827,137.1640,1035.4141); // exit point electronic
	Pickup::Create(1318,0,140.8128,1710.8275,1002.1363); // exit point tool
	Pickup::Create(1239,0,161.6251,-83.2522,1001.8047); // buypoint clothes
	Pickup::Create(1239,0,450.4843,-83.6519,999.5547); // restaurant buy point
	Pickup::Create(1318,0,161.3896,-96.8334,1001.8047); // clothes exit point
	Pickup::Create(1239,0,542.3506,-1292.6149,17.2422); // dealership
	Pickup::Create(1239,0,-1880.4781,-1681.4792,21.7500); // vehicle destroy point
	Pickup::Create(19605,0,1111.5823,-1796.9653,16.5938); // driving license
	Pickup::Create(1239,0,01490.2838,1305.7026,1093.2964); // driving license point
	Pickup::Create(1239,0,-14.6017,-270.7789,5.4297); // Load Truck point
	Pickup::Create(1239,0,383.3073,-2080.4578,7.8359); // fishing area
	Pickup::Create(1239,0,359.3359,-2032.1019,7.8359); // bait shop
	Pickup::Create(1239,0,-50.6201,-233.6625,6.7646); // sellfish point
  
  // 3d text
  TextLabel::Create("[Gun Parts Crafting Point]", 0xFFFFFFAA,-12.9450,2350.7974,24.1406,30.0);
	TextLabel::Create("[Materials Point]\n/getmaterials to buy 3 materials for {008000}$250",0xFFF157AA,613.0717,1549.8906,5.0001,30.0);
	TextLabel::Create("[Gun Maker Point]",0xFF0000AA,-752.7269,-131.6847,65.8281,10.0);
	TextLabel::Create("[Gun Maker Job Point]",0xFFFFFFAA,-757.2897,-133.7420,65.8281,10.0);
	TextLabel::Create("[Sweeper Sidejob]",0xFFF157AA,1313.1063,-875.3223,39.5781,10.0);
	TextLabel::Create("[Product Point]\n/getproduct to buy 1 product for {008000}$150",0xFFF157AA,2197.5491,-2661.5784,13.5469,30.0);
	TextLabel::Create("[Rent Vehicle Point]\n/rentveh to rent Faggio\nfor {008000}$60{FFF157} in {00AAAA}30 minutes",0xFFF157FF,1562.2598,-2300.6880,13.5650,30.0);
	TextLabel::Create("[Rent Vehicle Point]\n/rentveh to rent Faggio\nfor {008000}$60{FFF157} in {00AAAA}30 minutes",0xFFF157FF,1926.1271,-1788.2462,13.3906,30.0);
	TextLabel::Create("[Mechanic Job Point]", 0xFF0000AA,2139.5847,-1733.7576,17.2891,10.0);
	TextLabel::Create("[Mechanic Duty Point]",0xFFF157AA,2914.6526,-802.2943,11.0469,10.0);
	TextLabel::Create("[Component Point]\n/getcomponents to buy 10 components for {008000}$200",0xFFF157AA,2286.4944,-2013.8217,13.5442,10.0);
	TextLabel::Create("[Electronic Buy Point]",0x008000AA,-2237.0012,130.1817,1035.4141,10.0);
	TextLabel::Create("[Tool Buy Point]",0x008000AA,148.2934,1698.5463,1002.1363,10.0);
	TextLabel::Create("[Electronic Exit Point]",0xFFFFFFAA,-2240.7827,137.1640,1035.4141,10.0);
	TextLabel::Create("[Tool Exit Point]",0xFFFFFFAA,140.8128,1710.8275,1002.1363,10.0);
	TextLabel::Create("[Clothes Exit Point]",0xFFFFFFAA,161.3896,-96.8334,1001.8047,10.0);
	TextLabel::Create("[Clothes Buy Point]",0x008000AA,161.6251,-83.2522,1001.8047,10.0);
	TextLabel::Create("[Bus Driver Sidejob]",0xFFF157AA,1271.9991,-2038.5074,59.0828,10.0);
	TextLabel::Create("[Mower Sidejob]",0xFFF157AA,764.2607,-1304.5879,13.5613,10.0);
	TextLabel::Create("[Dealership Point]",0xAAAAAAAA,542.3506,-1292.6149,17.2422,10.0);
	TextLabel::Create("[Vehicle Delete Point]",0xFF0000AA,-1880.4781,-1681.4792,21.7500,10.0);
	TextLabel::Create("[Drivers License Center]\n{AAAAAA}Type /enter to enter",0xFFFFFFAA,1111.5823,-1796.9653,16.5938,10.0);
	TextLabel::Create("[Drivers License Point]\nType {FFFF00}/getlicense{FFFFFF} to get driving license for {008000}$100",0xAAAAAAAA,1490.2838,1305.7026,1093.2964,10.0);
	TextLabel::Create("[Loading Bay Point]\n{AAAAAA}Type /loadtruck to load truck",0xFFFFFFAA,-14.6017,-270.7789,5.4297,10.0);
	TextLabel::Create("[Trucker Job Point]",0xFF0000AA,-49.8569,-269.3626,6.6332,10.0);
	TextLabel::Create("[Fishing Area]\nRadius 50.0",0xFFFFFFAA,383.3073,-2080.4578,7.8359,10.0);
	TextLabel::Create("[Bait Shop]",0xFFFF00AA,359.3359,-2032.1019,7.8359,10.0);
	TextLabel::Create("[Sell Fish Point]",0xFFFFFFAA,-50.6201,-233.6625,6.7646,10.0);
	TextLabel::Create("[Restaurant Buy Point]",0x008000AA,450.4843,-83.6519,999.5547,10.0);
	TextLabel::Create("[Restaurant Exit Point]",0xFFFFFFAA,460.5504,-88.6155,999.5547,10.0);
	TextLabel::Create("[Drivers License Center Exit Point]",0xFFFFFFAA,1494.4346,1303.5786,1093.2891,10.0);
  
  // job vehicles
  Sweeper[0] = CreateVehicle(574, 1306.1726, -875.7529, 39.3935, -90.0000, 1, 0, 100, false);
  Sweeper[1] = CreateVehicle(574, 1306.1902, -873.5123, 39.3935, -90.0000, 1, 0, 100, false);
  Sweeper[2] = CreateVehicle(574, 1306.1666, -871.2911, 39.3935, -90.0000, 1, 0, 100, false);
  Sweeper[3] = CreateVehicle(574, 1306.1667, -869.1107, 39.3935, -90.0000, 1, 0, 100, false);
  Sweeper[4] = CreateVehicle(574, 1306.1678, -866.8701, 39.3935, -90.0000, 1, 0, 100, false);
  
  Bus[0].ID = CreateVehicle(437, 1244.9365, -2013.4041, 59.8729, 180.0000, 6, 7, 100, false);
  Bus[1].ID = CreateVehicle(437, 1250.0365, -2013.4041, 59.8729, 180.0000, 6, 7, 100, false);
  Bus[2].ID = CreateVehicle(437, 1255.0365, -2013.4041, 59.8729, 180.0000, 6, 7, 100, false);
  Bus[3].ID = CreateVehicle(437, 1260.0365, -2013.4041, 59.8729, 180.0000, 6, 7, 100, false);
  Bus[4].ID = CreateVehicle(437, 1265.0365, -2013.4041, 59.8729, 180.0000, 6, 7, 100, false);
  Bus[5].ID = CreateVehicle(437, 1270.0365, -2013.4041, 59.8729, 180.0000, 6, 7, 100, false);
  Bus[6].ID = CreateVehicle(437, 1275.0365, -2013.4041, 59.8729, 180.0000, 6, 7, 100, false);

  Mower[0] = CreateVehicle(572, 767.8790, -1307.7762, 13.1944, 0.0000, 3, 0, 100, false);
  Mower[1] = CreateVehicle(572, 771.1190, -1307.7563, 13.1944, 0.0000, 3, 0, 100, false);
  Mower[2] = CreateVehicle(572, 774.1190, -1307.7563, 13.1944, 0.0000, 3, 0, 100, false);
  Mower[3] = CreateVehicle(572, 777.3190, -1307.7563, 13.1944, 0.0000, 3, 0, 100, false);

  char plateBuffer[16];

  for(int i; i < Bus.size(); i++) {
    sprintf(plateBuffer, "BUS-%d", i + 1);
    SetVehicleNumberPlate(Bus[i].ID, plateBuffer);
  }
  
  for(int i; i < Sweeper.size(); i++) {
    sprintf(plateBuffer, "SWEEP-%d", i + 1);
    SetVehicleNumberPlate(Sweeper[i], plateBuffer);
  }

  for(int i; i < Bus.size(); i++)
    strcpy(Bus[i].Owner, "None");
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerConnect(int playerid) {
  SetPlayerSkillLevel(playerid, WEAPONSKILL_PISTOL, 0);
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerDisconnect(int playerid, int reason) {
  char msgBuff[64];
  sprintf(msgBuff, "%s Has disconnected from the server (%s)", RetPname(playerid), DCReason[reason]);
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerClickMap(int playerid, float x, float y, float z) {
  int vehicleid = GetPlayerVehicleID(playerid);

  if (IsPlayerAdmin(playerid) && GetPlayerInterior(playerid) == 0) {
    if (!IsPlayerInAnyVehicle(playerid)) SetPlayerPos(playerid, x, y, z);
    SetVehiclePos(vehicleid, x, y, z);
  }
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnVehicleCreated(int vehicleid) {
  if(!HasNoEngine(vehicleid)) SetVehicleParamsEx(vehicleid, 0, 0, 0, 0, 0, 0, 0);
  SetVehicleParamsEx(vehicleid, 1, 0, 0, 0, 0, 0, 0);
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerRequestClass(int playerid, int classid) {
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerCommandText(int playerid, const char *cmdtext) {
  Cmd cmd(cmdtext);

  if(!strcmp(cmd.name, "rconweapon")) {
    if(!IsPlayerAdmin(playerid)) return false;
    GivePlayerWeapon(playerid, WEAPON_MP5, 64);
    GivePlayerWeapon(playerid, WEAPON_M4, 64);
    return true;
  }
  return false;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
  return sampgdk::Supports() | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
  return sampgdk::Load(ppData);
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
  sampgdk::Unload();
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() {
  sampgdk::ProcessTick();
}
