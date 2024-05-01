#include <unistd.h>

#include <cstdio>
#include <cstring>

#include "lib/cmd.h"
#include "lib/ini.h"
#include "lib/sampgdk.h"
#include "lib/streamer.hpp"
#include "lib/stuff.hpp"

#include "limits.hpp"
#include "utils.hpp"
#include "player.hpp"
#include "item.hpp"
#include "vehicle.hpp"
#include "property.hpp"
#include "statics.hpp"
#include "dialog.hpp"
#include "files.hpp"
#include "cmd/cmd.hpp"

using namespace Plugins::Streamer;

void SAMPGDK_CALL TC_UpdateRentTime(int timerid, void *data) {
  for (int i = 0; i < MAX_RENTVEH; i++) {
    if (VehicleRent[i].Rented) {
      if (VehicleRent[i].RentTime > 0) {
        VehicleRent[i].RentTime--;
      } else {
        DestroyVehicle(VehicleRent[i].ID);
        strcpy(VehicleRent[i].Owner, "");
        VehicleRent[i].Rented = false;
      }
    }
  }
}

void SAMPGDK_CALL TC_NoExplodingVeh(int timerid, void *data) {
  for (int i = 0; i < MAX_VEHICLES; i++) {
    if (IsVehicleConnected(i)) {
      if (RetVehicleHealth(i) < 250) {
        SetVehicleHealth(i, 260);
        SetEngine(i, 0);
      }
    }
  }
}

void SAMPGDK_CALL TC_AltPlayerUpdate(int timerid, void *data) {
  /* less aggressive (low update rate) player update */
  char txt[128];
  int vid, pid;
  bool ispv;
  int vehicleid;

  for (int playerid = 0; playerid < MAX_PLAYERS; playerid++) {
    if (IsPlayerConnected(playerid)) {
      if (IsPlayerInAnyVehicle(playerid)) {
        vehicleid = GetPlayerVehicleID(playerid);
        ispv = false;

        for (int x = 0; x < MAX_PLAYERS; x++) {
          if (ispv)
            break;

          for (int y = 0; y < MAX_PLAYER_VEHICLE; y++) {
            if (vehicleid == Player[x].Vehicle[y].ID) {
              pid = x;
              vid = y;
              ispv = true;
              break;
            }
          }
        }

        sprintf(txt, "Speed: ~r~%.0fKm/h", GetVehicleSpeed(vehicleid));
        if (!EngineOn(vehicleid))
          sprintf(txt, "Speed: ~b~%.0fKm/h", GetVehicleSpeed(vehicleid));
        if (GetVehicleSpeed(vehicleid) >= (Player[playerid].DataState.speedlimit - 1) && SpeedLimitOn(playerid))
          sprintf(txt, "Speed: ~y~%.0fKm/h", GetVehicleSpeed(vehicleid));
        PlayerTextDrawSetString(playerid, Player[playerid].VehicleIndicator.Speed, txt);

        sprintf(txt, "Health: ~r~%.2f", RetVehicleHealth(vehicleid));
        if (!EngineOn(vehicleid))
          sprintf(txt, "Health: ~b~%.2f", RetVehicleHealth(vehicleid));
        PlayerTextDrawSetString(playerid, Player[playerid].VehicleIndicator.Health, txt);

        strcpy(txt, "Fuel: ~r~-");
        if (ispv)
          sprintf(txt, "Fuel: ~r~%.1f", Player[pid].Vehicle[vid].Fuel);
        if (!EngineOn(vehicleid)) {
          strcpy(txt, "Fuel: ~b~-");
          if (ispv)
            sprintf(txt, "Fuel: ~b~%.1f", Player[pid].Vehicle[vid].Fuel);
        }
        PlayerTextDrawSetString(playerid, Player[playerid].VehicleIndicator.Fuel, txt);

        strcpy(txt, "Temp: ~r~-");
        if (ispv)
          sprintf(txt, "Temp: ~r~%.2f", Player[pid].Vehicle[vid].Heat);
        if (!EngineOn(vehicleid)) {
          strcpy(txt, "Temp: ~b~-");
          if (ispv)
            sprintf(txt, "Temp: ~b~%.2f", Player[pid].Vehicle[vid].Heat);
        }
        PlayerTextDrawSetString(playerid, Player[playerid].VehicleIndicator.Heat, txt);
      }
    }
  }
}

PLUGIN_EXPORT bool PLUGIN_CALL OnGameModeInit() {
  char gmText[32];
  sprintf(gmText, "DRV-RP %s", VERSION);

  char cwd[256];
  getcwd(cwd, sizeof(cwd));
  printf("Plugin CWD: %s\n", cwd);

  SetGameModeText(gmText);
  EnableStuntBonusForAll(0);
  SetNameTagDrawDistance(10.0);
  ShowPlayerMarkers(0);
  DisableInteriorEnterExits();
  ManualVehicleEngineAndLights();
  EnableVehicleFriendlyFire();
  SetDeathDropAmount(0);
  ShowNameTags(1);

  for (int i = 0; i < MAX_PICKUPS; i++) {
    if (WorldPickup[i][0] != 0 ||
        WorldPickup[i][2] != 0 ||
        WorldPickup[i][3] != 0 ||
        WorldPickup[i][4] != 0) {
      Pickup::Create(WorldPickup[i][0],
        WorldPickup[i][1],
        WorldPickup[i][2],
        WorldPickup[i][3],
        WorldPickup[i][4]
      );
    }
  } 

  TextLabel::Create("[Gun Parts Crafting Point]", 0xFFFFFFAA, -12.9450, 2350.7974, 24.1406, 30.0);
  TextLabel::Create("[Materials Point]\n/getmaterials to buy 3 materials for {008000}$250", 0xFFF157AA, 613.0717, 1549.8906, 5.0001, 30.0);
  TextLabel::Create("[Gun Maker Point]", 0xFF0000AA, -752.7269, -131.6847, 65.8281, 10.0);
  TextLabel::Create("[Gun Maker Job Point]", 0xFFFFFFAA, -757.2897, -133.7420, 65.8281, 10.0);
  TextLabel::Create("[Sweeper Sidejob]", 0xFFF157AA, 1313.1063, -875.3223, 39.5781, 10.0);
  TextLabel::Create("[Product Point]\n/getproduct to buy 1 product for {008000}$150", 0xFFF157AA, 2197.5491, -2661.5784, 13.5469, 30.0);
  TextLabel::Create("[Rent Vehicle Point]\n/rentveh to rent Faggio\nfor {008000}$60{FFF157} in {00AAAA}30 minutes", 0xFFF157FF, 1562.2598, -2300.6880, 13.5650, 30.0);
  TextLabel::Create("[Rent Vehicle Point]\n/rentveh to rent Faggio\nfor {008000}$60{FFF157} in {00AAAA}30 minutes", 0xFFF157FF, 1926.1271, -1788.2462, 13.3906, 30.0);
  TextLabel::Create("[Mechanic Job Point]", 0xFF0000AA, 2139.5847, -1733.7576, 17.2891, 10.0);
  TextLabel::Create("[Mechanic Duty Point]", 0xFFF157AA, 2914.6526, -802.2943, 11.0469, 10.0);
  TextLabel::Create("[Component Point]\n/getcomponents to buy 10 components for {008000}$200", 0xFFF157AA, 2286.4944, -2013.8217, 13.5442, 10.0);
  TextLabel::Create("[Electronic Buy Point]", 0x008000AA, -2237.0012, 130.1817, 1035.4141, 10.0);
  TextLabel::Create("[Tool Buy Point]", 0x008000AA, 148.2934, 1698.5463, 1002.1363, 10.0);
  TextLabel::Create("[Electronic Exit Point]", 0xFFFFFFAA, -2240.7827, 137.1640, 1035.4141, 10.0);
  TextLabel::Create("[Tool Exit Point]", 0xFFFFFFAA, 140.8128, 1710.8275, 1002.1363, 10.0);
  TextLabel::Create("[Clothes Exit Point]", 0xFFFFFFAA, 161.3896, -96.8334, 1001.8047, 10.0);
  TextLabel::Create("[Clothes Buy Point]", 0x008000AA, 161.6251, -83.2522, 1001.8047, 10.0);
  TextLabel::Create("[Bus Driver Sidejob]", 0xFFF157AA, 1271.9991, -2038.5074, 59.0828, 10.0);
  TextLabel::Create("[Mower Sidejob]", 0xFFF157AA, 764.2607, -1304.5879, 13.5613, 10.0);
  TextLabel::Create("[Dealership Point]", 0xAAAAAAAA, 542.3506, -1292.6149, 17.2422, 10.0);
  TextLabel::Create("[Vehicle Delete Point]", 0xFF0000AA, -1880.4781, -1681.4792, 21.7500, 10.0);
  TextLabel::Create("[Drivers License Center]\n{AAAAAA}Type /enter to enter", 0xFFFFFFAA, 1111.5823, -1796.9653, 16.5938, 10.0);
  TextLabel::Create("[Drivers License Point]\nType {FFFF00}/getlicense{FFFFFF} to get driving license for {008000}$100", 0xAAAAAAAA, 1490.2838, 1305.7026, 1093.2964, 10.0);
  TextLabel::Create("[Loading Bay Point]\n{AAAAAA}Type /loadtruck to load truck", 0xFFFFFFAA, -14.6017, -270.7789, 5.4297, 10.0);
  TextLabel::Create("[Trucker Job Point]", 0xFF0000AA, -49.8569, -269.3626, 6.6332, 10.0);
  TextLabel::Create("[Fishing Area]\nRadius 50.0", 0xFFFFFFAA, 383.3073, -2080.4578, 7.8359, 10.0);
  TextLabel::Create("[Bait Shop]", 0xFFFF00AA, 359.3359, -2032.1019, 7.8359, 10.0);
  TextLabel::Create("[Sell Fish Point]", 0xFFFFFFAA, -50.6201, -233.6625, 6.7646, 10.0);
  TextLabel::Create("[Restaurant Buy Point]", 0x008000AA, 450.4843, -83.6519, 999.5547, 10.0);
  TextLabel::Create("[Restaurant Exit Point]", 0xFFFFFFAA, 460.5504, -88.6155, 999.5547, 10.0);
  TextLabel::Create("[Drivers License Center Exit Point]", 0xFFFFFFAA, 1494.4346, 1303.5786, 1093.2891, 10.0);

  Object::Create(971, 720.06940, -462.57724, 15.39299, 0.00000, 0.00000,
                 0.00000);
  Object::Create(971, 1042.84998, -1026.01123, 31.09643, 0.00000, 0.00000,
                 0.00000);
  Object::Create(971, 1025.36536, -1029.33276, 31.63884, 0.00000, 0.00000,
                 0.00000);
  Object::Create(971, -1420.50977, 2591.11279, 56.94583, 0.00000, 0.00000,
                 180.00000);
  Object::Create(971, 2386.68213, 1043.31189, 9.92575, 0.00000, 0.00000,
                 0.00000);
  Object::Create(8167, 2645.81055, -2039.32849, 12.56419, 0.00000, 0.00000,
                 90.00000);
  Object::Create(8167, 2645.81055, -2039.32849, 15.00648, 0.00000, 0.00000,
                 90.00000);
  Object::Create(971, -1935.82751, 238.56221, 33.64063, 0.00000, 0.00000,
                 0.00000);
  Object::Create(971, -1904.62756, 277.66324, 42.39743, 0.00000, 0.00000,
                 0.00000);
  Object::Create(971, -2716.14404, 216.72392, 3.81582, 0.00000, 0.00000,
                 90.00000);
  Object::Create(971, 1843.31055, -1855.03943, 12.37510, 0.00000, 0.00000,
                 90.00000);
  Object::Create(971, 2005.17334, 2303.33716, 9.81711, 0.00000, 0.00000,
                 0.00000);
  Object::Create(971, 1968.37793, 2162.65747, 12.66515, 0.00000, 0.00000,
                 -90.00000);
  Object::Create(971, 2393.19873, 1483.32202, 12.39729, 0.00000, 0.00000,
                 0.00000);
  Object::Create(971, -99.80347, 1111.46582, 20.85815, 0.00000, 0.00000,
                 0.00000);
  Object::Create(971, -2425.07886, 1027.89941, 51.84350, 0.00000, 0.00000,
                 180.00000);
  Object::Create(971, 2071.52344, -1831.55835, 13.00516, 0.00000, 0.00000,
                 90.00000);
  Object::Create(971, 488.63022, -1735.32129, 10.59052, 0.00000, 0.00000,
                 -8.46000);

  Object::Create(18869, -2236.96631, 127.79252, 1035.46399, 0.00000, 0.00000,
                 -180.00000);
  Object::Create(18869, -2236.70630, 127.79250, 1035.46399, 0.00000, 0.00000,
                 -180.00000);
  Object::Create(18869, -2236.42627, 127.79250, 1035.46399, 0.00000, 0.00000,
                 -180.00000);
  Object::Create(18869, -2236.10620, 127.79250, 1035.46399, 0.00000, 0.00000,
                 -180.00000);
  Object::Create(18867, -2235.58618, 127.79250, 1035.46399, 0.00000, 0.00000,
                 -180.00000);
  Object::Create(18867, -2235.30640, 127.79250, 1035.46399, 0.00000, 0.00000,
                 -180.00000);
  Object::Create(18867, -2235.00635, 127.79250, 1035.46399, 0.00000, 0.00000,
                 -180.00000);
  Object::Create(18867, -2234.64624, 127.79250, 1035.46399, 0.00000, 0.00000,
                 -180.00000);
  Object::Create(2028, -2231.81104, 127.77340, 1035.59448, 0.00000, 0.00000,
                 0.00000);
  Object::Create(2226, -2235.28638, 129.41631, 1035.47986, 0.00000, 0.00000,
                 -120.00000);
  Object::Create(2226, -2236.08618, 129.41631, 1035.47986, 0.00000, 0.00000,
                 -120.00000);
  Object::Create(2226, -2234.42627, 129.41631, 1035.47986, 0.00000, 0.00000,
                 -120.00000);
  Object::Create(1781, -2228.45703, 130.95360, 1036.00439, 0.00000, 0.00000,
                 -33.00000);
  Object::Create(1781, -2227.23706, 130.95360, 1036.00439, 0.00000, 0.00000,
                 -33.00000);
  Object::Create(19807, -2237.30664, 137.79880, 1035.53870, 0.00000, 0.00000,
                 0.00000);
  Object::Create(19807, -2237.02661, 137.79880, 1035.53870, 0.00000, 0.00000,
                 0.00000);
  Object::Create(19807, -2236.76660, 137.79880, 1035.53870, 0.00000, 0.00000,
                 0.00000);
  Object::Create(19807, -2236.52661, 137.79880, 1035.53870, 0.00000, 0.00000,
                 0.00000);
  Object::Create(19807, -2236.28662, 137.79880, 1035.53870, 0.00000, 0.00000,
                 0.00000);
  Object::Create(2099, -2230.93115, 138.02121, 1034.37671, 0.00000, 0.00000,
                 -33.00000);
  Object::Create(18871, -2236.97729, 127.61670, 1035.79126, 0.00000, 0.00000,
                 -180.00000);
  Object::Create(18871, -2236.79736, 127.61670, 1035.79126, 0.00000, 0.00000,
                 -180.00000);
  Object::Create(18871, -2236.59741, 127.61670, 1035.79126, 0.00000, 0.00000,
                 -180.00000);
  Object::Create(18871, -2236.41724, 127.61670, 1035.79126, 0.00000, 0.00000,
                 -180.00000);
  Object::Create(18871, -2236.21729, 127.61670, 1035.79126, 0.00000, 0.00000,
                 -180.00000);
  Object::Create(18871, -2235.99731, 127.61670, 1035.79126, 0.00000, 0.00000,
                 -180.00000);
  Object::Create(2103, -2228.60181, 133.66570, 1036.00464, 0.00000, 0.00000,
                 -26.00000);
  Object::Create(2103, -2227.51099, 133.75337, 1036.00464, 0.00000, 0.00000,
                 33.00000);
  Object::Create(2149, -2237.95483, 134.54311, 1036.16431, 0.00000, 0.00000,
                 0.00000);
  Object::Create(2149, -2237.95483, 133.86310, 1036.16431, 0.00000, 0.00000,
                 0.00000);
  Object::Create(2149, -2237.95483, 133.24310, 1036.16431, 0.00000, 0.00000,
                 0.00000);
  Object::Create(2149, -2237.95483, 132.66310, 1036.16431, 0.00000, 0.00000,
                 0.00000);
  Object::Create(2232, -2232.84497, 127.95030, 1035.02405, 0.00000, 0.00000,
                 180.00000);
  Object::Create(2232, -2233.74512, 127.95030, 1035.02405, 0.00000, 0.00000,
                 180.00000);
  Object::Create(1785, -2229.57910, 136.79010, 1036.11255, 0.00000, 0.00000,
                 0.00000);
  Object::Create(2190, -2241.02222, 130.55769, 1035.47998, 0.00000, 0.00000,
                 90.00000);
  Object::Create(2190, -2241.02222, 131.33771, 1035.47998, 0.00000, 0.00000,
                 90.00000);
  Object::Create(2190, -2241.02222, 132.13770, 1035.47998, 0.00000, 0.00000,
                 90.00000);
  Object::Create(2695, -2231.91724, 127.35710, 1036.89270, 0.00000, -5.00000,
                 0.00000);
  Object::Create(2641, -2222.01172, 144.32721, 1036.89685, 0.00000, 0.00000,
                 0.00000);
  Object::Create(2641, -2235.29932, 138.14149, 1036.89685, 0.00000, 0.00000,
                 0.00000);
  Object::Create(2641, -2232.09937, 138.14149, 1036.89685, 0.00000, 0.00000,
                 0.00000);
  Object::Create(19893, -2240.77539, 133.84290, 1035.48022, 0.00000, 0.00000,
                 90.00000);
  Object::Create(19893, -2240.77539, 134.46291, 1035.48022, 0.00000, 0.00000,
                 90.00000);
  Object::Create(19893, -2240.77539, 135.06290, 1035.48022, 0.00000, 0.00000,
                 90.00000);
  Object::Create(19893, -2240.77539, 135.60291, 1035.48022, 0.00000, 0.00000,
                 90.00000);
  Object::Create(18875, -2233.47168, 137.81740, 1036.40649, 0.00000, 0.00000,
                 0.00000);
  Object::Create(18875, -2233.23169, 137.81740, 1036.40649, 0.00000, 0.00000,
                 0.00000);
  Object::Create(18875, -2233.73169, 137.81740, 1036.40649, 0.00000, 0.00000,
                 0.00000);
  Object::Create(18875, -2233.99170, 137.81740, 1036.40649, 0.00000, 0.00000,
                 0.00000);
  Object::Create(18875, -2234.25171, 137.81740, 1036.40649, 0.00000, 0.00000,
                 0.00000);

  Object::Create(337, 132.14426, 1706.19189, 1003.14368, 177.00000, 90.00000,
                 -97.00000, -1, 1);
  Object::Create(3761, 134.15401, 1696.84436, 1001.57410, 0.00000, 0.00000,
                 0.00000, -1, 1);
  Object::Create(18644, 135.82140, 1710.93713, 1001.64600, 0.00000, 90.00000,
                 0.00000, -1, 1);
  Object::Create(18644, 136.36140, 1710.93713, 1001.64600, 0.00000, 90.00000,
                 0.00000, -1, 1);
  Object::Create(18644, 137.08141, 1710.93713, 1001.64600, 0.00000, 90.00000,
                 0.00000, -1, 1);
  Object::Create(18644, 137.56140, 1710.93713, 1001.64600, 0.00000, 90.00000,
                 0.00000, -1, 1);
  Object::Create(18644, 137.56140, 1710.55713, 1001.64600, 0.00000, 90.00000,
                 0.00000, -1, 1);
  Object::Create(18644, 137.02139, 1710.55713, 1001.64600, 0.00000, 90.00000,
                 0.00000, -1, 1);
  Object::Create(18644, 136.36140, 1710.55713, 1001.64600, 0.00000, 90.00000,
                 0.00000, -1, 1);
  Object::Create(18644, 135.84140, 1710.55713, 1001.64600, 0.00000, 90.00000,
                 0.00000, -1, 1);
  Object::Create(18638, 134.71471, 1710.85034, 1001.70599, 0.00000, -90.00000,
                 -180.00000, -1, 1);
  Object::Create(18638, 134.09470, 1710.85034, 1001.70599, 0.00000, -90.00000,
                 -180.00000, -1, 1);
  Object::Create(18638, 133.53470, 1710.85034, 1001.70599, 0.00000, -90.00000,
                 -180.00000, -1, 1);
  Object::Create(18638, 132.93469, 1710.85034, 1001.70599, 0.00000, -90.00000,
                 -180.00000, -1, 1);
  Object::Create(18638, 132.93469, 1710.49036, 1001.70599, 0.00000, -90.00000,
                 -180.00000, -1, 1);
  Object::Create(18638, 133.51469, 1710.49036, 1001.70599, 0.00000, -90.00000,
                 -180.00000, -1, 1);
  Object::Create(18638, 134.09470, 1710.49036, 1001.70599, 0.00000, -90.00000,
                 -180.00000, -1, 1);
  Object::Create(18638, 134.71471, 1710.49036, 1001.70599, 0.00000, -90.00000,
                 -180.00000, -1, 1);
  Object::Create(18632, 137.59850, 1700.21667, 1001.39551, 190.00000, 0.00000,
                 0.00000, -1, 1);
  Object::Create(18632, 137.73849, 1700.21667, 1001.39551, 190.00000, 0.00000,
                 0.00000, -1, 1);
  Object::Create(18632, 137.89850, 1700.21667, 1001.39551, 190.00000, 0.00000,
                 0.00000, -1, 1);
  Object::Create(18632, 138.05850, 1700.21667, 1001.39551, 190.00000, 0.00000,
                 0.00000, -1, 1);
  Object::Create(18632, 138.19850, 1700.21667, 1001.39551, 190.00000, 0.00000,
                 0.00000, -1, 1);
  Object::Create(18632, 138.31850, 1700.21667, 1001.39551, 190.00000, 0.00000,
                 0.00000, -1, 1);
  Object::Create(18632, 138.43851, 1700.21667, 1001.39551, 190.00000, 0.00000,
                 0.00000, -1, 1);
  Object::Create(18632, 138.55850, 1700.21667, 1001.39551, 190.00000, 0.00000,
                 0.00000, -1, 1);
  Object::Create(18632, 138.67850, 1700.21667, 1001.39551, 190.00000, 0.00000,
                 0.00000, -1, 1);
  Object::Create(18632, 138.81850, 1700.21667, 1001.39551, 190.00000, 0.00000,
                 0.00000, -1, 1);
  Object::Create(18632, 138.93851, 1700.21667, 1001.39551, 190.00000, 0.00000,
                 0.00000, -1, 1);
  Object::Create(19324, 145.08090, 1701.52148, 1001.77692, 0.00000, 0.00000,
                 0.00000, -1, 1);
  Object::Create(2314, 133.37389, 1700.25806, 1001.12329, 0.00000, 0.00000,
                 0.00000, -1, 1);
  Object::Create(18641, 134.93510, 1700.11414, 1001.64520, 0.00000, 90.00000,
                 0.00000, -1, 1);
  Object::Create(18641, 134.41510, 1700.11414, 1001.64520, 0.00000, 90.00000,
                 0.00000, -1, 1);
  Object::Create(18641, 133.91510, 1700.11414, 1001.64520, 0.00000, 90.00000,
                 0.00000, -1, 1);
  Object::Create(18641, 133.37511, 1700.11414, 1001.64520, 0.00000, 90.00000,
                 0.00000, -1, 1);
  Object::Create(18641, 133.37511, 1700.49414, 1001.64520, 0.00000, 90.00000,
                 0.00000, -1, 1);

  // job vehicles
  Sweeper[0] = CreateVehicle(574, 1306.1726, -875.7529, 39.3935, -90.0000, 1, 0,
                             100, false);
  Sweeper[1] = CreateVehicle(574, 1306.1902, -873.5123, 39.3935, -90.0000, 1, 0,
                             100, false);
  Sweeper[2] = CreateVehicle(574, 1306.1666, -871.2911, 39.3935, -90.0000, 1, 0,
                             100, false);
  Sweeper[3] = CreateVehicle(574, 1306.1667, -869.1107, 39.3935, -90.0000, 1, 0,
                             100, false);
  Sweeper[4] = CreateVehicle(574, 1306.1678, -866.8701, 39.3935, -90.0000, 1, 0,
                             100, false);

  Bus[0].ID = CreateVehicle(437, 1244.9365, -2013.4041, 59.8729, 180.0000, 6, 7,
                            100, false);
  Bus[1].ID = CreateVehicle(437, 1250.0365, -2013.4041, 59.8729, 180.0000, 6, 7,
                            100, false);
  Bus[2].ID = CreateVehicle(437, 1255.0365, -2013.4041, 59.8729, 180.0000, 6, 7,
                            100, false);
  Bus[3].ID = CreateVehicle(437, 1260.0365, -2013.4041, 59.8729, 180.0000, 6, 7,
                            100, false);
  Bus[4].ID = CreateVehicle(437, 1265.0365, -2013.4041, 59.8729, 180.0000, 6, 7,
                            100, false);
  Bus[5].ID = CreateVehicle(437, 1270.0365, -2013.4041, 59.8729, 180.0000, 6, 7,
                            100, false);
  Bus[6].ID = CreateVehicle(437, 1275.0365, -2013.4041, 59.8729, 180.0000, 6, 7,
                            100, false);

  Mower[0] = CreateVehicle(572, 767.8790, -1307.7762, 13.1944, 0.0000, 3, 0,
                           100, false);
  Mower[1] = CreateVehicle(572, 771.1190, -1307.7563, 13.1944, 0.0000, 3, 0,
                           100, false);
  Mower[2] = CreateVehicle(572, 774.1190, -1307.7563, 13.1944, 0.0000, 3, 0,
                           100, false);
  Mower[3] = CreateVehicle(572, 777.3190, -1307.7563, 13.1944, 0.0000, 3, 0,
                           100, false);

  char plateBuffer[16];

  for (int i = 0; i < Bus.size(); i++) {
    sprintf(plateBuffer, "BUS-%d", i + 1);
    SetVehicleNumberPlate(Bus[i].ID, plateBuffer);
  }

  for (int i = 0; i < Sweeper.size(); i++) {
    sprintf(plateBuffer, "SWEEP-%d", i + 1);
    SetVehicleNumberPlate(Sweeper[i], plateBuffer);
  }

  for (int i = 0; i < Bus.size(); i++)
    strcpy(Bus[i].Owner, "None");

  for (int i = 0; i < MAX_BUSINESS; i++) {
    LoadBusiness("store", i);
  }

  for (int i = 0; i < MAX_HOUSE; i++) {
    LoadHouse(i);
  }

  SetTimer(60000, true, TC_UpdateRentTime, nullptr);
  SetTimer(100, true, TC_AltPlayerUpdate, nullptr);
  SetTimer(1000, true, TC_NoExplodingVeh, nullptr);
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerConnect(int playerid) {
  Player[playerid].Flag.FirstSpawn = true;

  SetPlayerSkillLevel(playerid, WEAPONSKILL_PISTOL, 0);
  SetPlayerColor(playerid, 0x000000FF);
  TogglePlayerSpectating(playerid, 1);

  char file_account[256];
  char *name = RetPname(playerid);
  sprintf(file_account, PLAYER_ACCOUNT, name);

  if (!fexist(file_account)) {
    ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD,
                     "Register",
                     "This Username Is Not Registered\nType Your Desired "
                     "Password to Create Account",
                     "Register", "Leave");
  } else {
    ShowPlayerDialog(
        playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Login",
        "This Username Is Registered\nType Your Password In Order To Login",
        "Login", "Leave");
  }

  free(name);
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerDisconnect(int playerid, int reason) {
  char msgBuff[64];
  char *name = RetPname(playerid);
  sprintf(msgBuff, "%s Has disconnected from the server (%s)", name,
          DCReason[reason]);
  SendClientMessageToAll(0xFFFF00AA, msgBuff);

  if (!Player[playerid].Flag.FirstSpawn) {
    SavePlayer(playerid);
    SaveInventory(playerid);

    for (int i = 0; i < MAX_HOUSE; i++) {
      if (!strcmp(name, Houses[i].Owner)) {
        SaveHouse(i);
      }
    }
  }

  free(name);
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerDeath(int playerid, int killerid, int reason) {
  Player[playerid].Flag.Dead = true;
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerSpawn(int playerid) {
  if (Player[playerid].Flag.FirstSpawn) {
    char msg[128];
    char *name = RetPname(playerid);
    sprintf(msg, "%s Has connected to the server", name);
    SendClientMessageToAll(0xFFFF00AA, msg);
    Player[playerid].Flag.FirstSpawn = false;

    if (!Player[playerid].Flag.NewAccount) {
      LoadPlayer(playerid);
      LoadInventory(playerid);
    }

    RemoveBuildingForPlayer(playerid, 1302, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 1209, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 955, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 1775, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 1776, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 956, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 2489, -2237.6328, 127.5547, 1035.6875,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2481, -2237.6328, 127.5781, 1036.7969,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2490, -2237.6406, 127.5547, 1036.3984,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2495, -2237.6406, 127.5547, 1036.0391,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2488, -2237.6328, 127.5547, 1035.3281,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2483, -2236.5078, 127.5625, 1036.6094,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2504, -2235.0859, 127.6406, 1035.8516,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2503, -2235.5703, 127.6406, 1035.8516,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2501, -2234.6328, 127.6406, 1035.8516,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2513, -2236.5313, 127.6641, 1035.5703,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2490, -2229.7188, 127.5547, 1036.5391,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2495, -2229.0938, 127.5547, 1036.5313,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2477, -2223.5703, 128.2422, 1036.4922,
                            0.25);
    RemoveBuildingForPlayer(playerid, 928, -2225.1406, 128.2969, 1034.6719,
                            0.25);
    RemoveBuildingForPlayer(playerid, 926, -2224.2500, 128.4141, 1034.6563,
                            0.25);
    RemoveBuildingForPlayer(playerid, 14558, -2223.3438, 128.4219, 1035.2031,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2484, -2240.8125, 131.0781, 1036.3047,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2362, -2238.3281, 129.2656, 1035.4453,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2497, -2237.2266, 131.1328, 1037.6875,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2493, -2233.3516, 129.2734, 1035.4063,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2510, -2233.6406, 129.2344, 1037.8906,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2494, -2233.6094, 129.5234, 1035.4063,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2492, -2233.1016, 129.5234, 1035.4063,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2491, -2233.1016, 129.7734, 1034.4063,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2496, -2233.3516, 129.7734, 1035.4063,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2486, -2234.4531, 131.7500, 1035.4063,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2459, -2234.7031, 131.9922, 1034.3984,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2484, -2233.9922, 132.1016, 1036.8281,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2474, -2234.1250, 132.1172, 1035.1484,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2487, -2226.1641, 129.7500, 1037.5469,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2499, -2231.4766, 130.3203, 1037.6953,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2471, -2228.0547, 130.3281, 1035.8125,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2470, -2227.4141, 130.6875, 1036.0391,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2469, -2228.4688, 130.7188, 1036.0391,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2503, -2223.5391, 131.0703, 1035.8438,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2501, -2223.5391, 131.6406, 1035.8438,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2469, -2237.9063, 133.1953, 1036.0391,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2470, -2237.8750, 134.2500, 1036.0391,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2487, -2238.1719, 135.7969, 1037.5469,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2504, -2236.8516, 137.8906, 1035.8516,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2501, -2237.3047, 137.8906, 1035.8516,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2486, -2233.4766, 132.4453, 1035.8047,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2498, -2231.9453, 132.8125, 1037.5703,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2512, -2235.9922, 133.4375, 1037.8438,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2459, -2234.7031, 135.0625, 1034.3984,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2464, -2233.9844, 135.1641, 1036.1953,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2474, -2234.1250, 135.8672, 1035.1484,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2498, -2232.3281, 136.2422, 1037.5703,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2471, -2235.3672, 137.8516, 1035.9063,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2503, -2236.3672, 137.8906, 1035.8516,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2466, -2235.2969, 137.9609, 1036.5547,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2513, -2235.1875, 137.9297, 1036.2500,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2467, -2235.5391, 137.9609, 1034.4141,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2474, -2235.4453, 138.2109, 1035.3984,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2511, -2229.2734, 132.2813, 1037.8594,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2503, -2223.5391, 132.4375, 1035.8438,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2501, -2223.5391, 133.0078, 1035.8438,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2486, -2228.5469, 133.3984, 1035.6641,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2474, -2227.8984, 133.7656, 1035.5156,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2484, -2228.0938, 133.7734, 1036.8281,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2486, -2227.5703, 134.0938, 1035.2813,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2512, -2226.4922, 134.0156, 1037.8438,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2499, -2230.1563, 135.2578, 1037.6953,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2478, -2226.3750, 136.9922, 1034.8281,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2474, -2229.6797, 137.1406, 1036.0391,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2465, -2227.9219, 137.0234, 1036.8594,
                            0.25);
    RemoveBuildingForPlayer(playerid, 2480, -2226.3672, 137.0781, 1036.4922,
                            0.25);
    RemoveBuildingForPlayer(playerid, 5043, 1843.3672, -1856.3203, 13.8750,
                            0.25);
    RemoveBuildingForPlayer(playerid, 5340, 2644.8594, -2039.2344, 14.0391,
                            0.25);
    RemoveBuildingForPlayer(playerid, 5422, 2071.4766, -1831.4219, 14.5625,
                            0.25);
    RemoveBuildingForPlayer(playerid, 5856, 1024.9844, -1029.3516, 33.1953,
                            0.25);
    RemoveBuildingForPlayer(playerid, 5779, 1041.3516, -1025.9297, 32.6719,
                            0.25);
    RemoveBuildingForPlayer(playerid, 6400, 488.2813, -1734.6953, 12.3906,
                            0.25);
    RemoveBuildingForPlayer(playerid, 10575, -2716.3516, 217.4766, 5.3828,
                            0.25);
    RemoveBuildingForPlayer(playerid, 11313, -1935.8594, 239.5313, 35.3516,
                            0.25);
    RemoveBuildingForPlayer(playerid, 11319, -1904.5313, 277.8984, 42.9531,
                            0.25);
    RemoveBuildingForPlayer(playerid, 9625, -2425.7266, 1027.9922, 52.2813,
                            0.25);
    RemoveBuildingForPlayer(playerid, 9093, 2386.6563, 1043.6016, 11.5938,
                            0.25);
    RemoveBuildingForPlayer(playerid, 8957, 2393.7656, 1483.6875, 12.7109,
                            0.25);
    RemoveBuildingForPlayer(playerid, 7709, 2006.0000, 2303.7266, 11.3125,
                            0.25);
    RemoveBuildingForPlayer(playerid, 7891, 1968.7422, 2162.4922, 12.0938,
                            0.25);
    RemoveBuildingForPlayer(playerid, 3294, -1420.5469, 2591.1563, 57.7422,
                            0.25);
    RemoveBuildingForPlayer(playerid, 3294, -100.0000, 1111.4141, 21.6406,
                            0.25);
    RemoveBuildingForPlayer(playerid, 13028, 720.0156, -462.5234, 16.8594,
                            0.25);

    Player[playerid].VehicleIndicator.MainBox =
        CreatePlayerTextDraw(playerid, 634.503662, 365.500000, "MainBox");
    PlayerTextDrawLetterSize(playerid, Player[playerid].VehicleIndicator.MainBox,
                             0.000000, 7.914812);
    PlayerTextDrawTextSize(playerid, Player[playerid].VehicleIndicator.MainBox,
                           531.645690, 0.000000);
    PlayerTextDrawAlignment(playerid, Player[playerid].VehicleIndicator.MainBox, 1);
    PlayerTextDrawColor(playerid, Player[playerid].VehicleIndicator.MainBox, 0);
    PlayerTextDrawUseBox(playerid, Player[playerid].VehicleIndicator.MainBox, true);
    PlayerTextDrawBoxColor(playerid, Player[playerid].VehicleIndicator.MainBox, 102);
    PlayerTextDrawSetShadow(playerid, Player[playerid].VehicleIndicator.MainBox, 0);
    PlayerTextDrawSetOutline(playerid, Player[playerid].VehicleIndicator.MainBox, 0);
    PlayerTextDrawFont(playerid, Player[playerid].VehicleIndicator.MainBox, 0);

    Player[playerid].VehicleIndicator.Speed =
        CreatePlayerTextDraw(playerid, 538.331176, 364.583190, "Speed:");
    PlayerTextDrawLetterSize(playerid, Player[playerid].VehicleIndicator.Speed,
                             0.287891, 1.570831);
    PlayerTextDrawAlignment(playerid, Player[playerid].VehicleIndicator.Speed, 1);
    PlayerTextDrawColor(playerid, Player[playerid].VehicleIndicator.Speed, -1);
    PlayerTextDrawSetShadow(playerid, Player[playerid].VehicleIndicator.Speed, 0);
    PlayerTextDrawSetOutline(playerid, Player[playerid].VehicleIndicator.Speed, 1);
    PlayerTextDrawBackgroundColor(playerid, Player[playerid].VehicleIndicator.Speed,
                                  51);
    PlayerTextDrawFont(playerid, Player[playerid].VehicleIndicator.Speed, 1);
    PlayerTextDrawSetProportional(playerid, Player[playerid].VehicleIndicator.Speed,
                                  1);

    Player[playerid].VehicleIndicator.Health =
        CreatePlayerTextDraw(playerid, 537.862792, 382.083465, "Health:");
    PlayerTextDrawLetterSize(playerid, Player[playerid].VehicleIndicator.Health,
                             0.279457, 1.594162);
    PlayerTextDrawAlignment(playerid, Player[playerid].VehicleIndicator.Health, 1);
    PlayerTextDrawColor(playerid, Player[playerid].VehicleIndicator.Health, -1);
    PlayerTextDrawSetShadow(playerid, Player[playerid].VehicleIndicator.Health, 0);
    PlayerTextDrawSetOutline(playerid, Player[playerid].VehicleIndicator.Health, 1);
    PlayerTextDrawBackgroundColor(playerid, Player[playerid].VehicleIndicator.Health,
                                  51);
    PlayerTextDrawFont(playerid, Player[playerid].VehicleIndicator.Health, 1);
    PlayerTextDrawSetProportional(playerid, Player[playerid].VehicleIndicator.Health,
                                  1);

    Player[playerid].VehicleIndicator.Fuel =
        CreatePlayerTextDraw(playerid, 536.925964, 400.166625, "Fuel:");
    PlayerTextDrawLetterSize(playerid, Player[playerid].VehicleIndicator.Fuel,
                             0.447188, 1.477498);
    PlayerTextDrawAlignment(playerid, Player[playerid].VehicleIndicator.Fuel, 1);
    PlayerTextDrawColor(playerid, Player[playerid].VehicleIndicator.Fuel, -1);
    PlayerTextDrawSetShadow(playerid, Player[playerid].VehicleIndicator.Fuel, 0);
    PlayerTextDrawSetOutline(playerid, Player[playerid].VehicleIndicator.Fuel, 1);
    PlayerTextDrawBackgroundColor(playerid, Player[playerid].VehicleIndicator.Fuel,
                                  51);
    PlayerTextDrawFont(playerid, Player[playerid].VehicleIndicator.Fuel, 1);
    PlayerTextDrawSetProportional(playerid, Player[playerid].VehicleIndicator.Fuel, 1);

    Player[playerid].VehicleIndicator.Heat =
        CreatePlayerTextDraw(playerid, 536.457214, 418.833374, "Temp:");
    PlayerTextDrawLetterSize(playerid, Player[playerid].VehicleIndicator.Heat,
                             0.414860, 1.459999);
    PlayerTextDrawAlignment(playerid, Player[playerid].VehicleIndicator.Heat, 1);
    PlayerTextDrawColor(playerid, Player[playerid].VehicleIndicator.Heat, -1);
    PlayerTextDrawSetShadow(playerid, Player[playerid].VehicleIndicator.Heat, 0);
    PlayerTextDrawSetOutline(playerid, Player[playerid].VehicleIndicator.Heat, 1);
    PlayerTextDrawBackgroundColor(playerid, Player[playerid].VehicleIndicator.Heat,
                                  51);
    PlayerTextDrawFont(playerid, Player[playerid].VehicleIndicator.Heat, 1);
    PlayerTextDrawSetProportional(playerid, Player[playerid].VehicleIndicator.Heat, 1);

    free(name);
  }

  if (Player[playerid].Flag.NewAccount) {
    GivePlayerMoney(playerid, 500);
    Player[playerid].Status.hunger = 100.0;
    Player[playerid].Status.thirst = 100.0;
    Player[playerid].Status.energy = 100.0;
    Player[playerid].Flag.NewAccount = false;
  }

  if (Player[playerid].Flag.Dead) {
    Player[playerid].Flag.Dead = false;
    SetPlayerPos(playerid, 2035.5458,-1413.7125,16.9922);
    SetPlayerFacingAngle(playerid, 132.4680);
    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, 0);
    FreezePlayer(playerid, 3000);
    GivePlayerMoney(playerid, -100);
    HideVehicleIndicator(playerid);
    SendClientMessage(playerid, COLOR_ERROR, "-$100 Hospital bill");
  }
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerClickMap(int playerid, float x, float y,
                                                float z) {
  if (Player[playerid].Flag.MapTP) {
    int vehicleid = GetPlayerVehicleID(playerid);

    if (IsPlayerAdmin(playerid) && GetPlayerInterior(playerid) == 0) {
      if (!IsPlayerInAnyVehicle(playerid))
        SetPlayerPos(playerid, x, y, z);
      SetVehiclePos(vehicleid, x, y, z);
    }
  }
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnVehicleCreated(int vehicleid) {
  if (!HasNoEngine(vehicleid))
    SetVehicleParamsEx(vehicleid, 0, 0, 0, 0, 0, 0, 0);
  SetVehicleParamsEx(vehicleid, 1, 0, 0, 0, 0, 0, 0);
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerRequestClass(int playerid, int classid) {
  if (Player[playerid].Flag.NewAccount) {
    SetSpawnInfo(playerid, 0, STARTER_SKINS[Random.i(16)], 1643.9750, -2332.2830,
                 13.5469, 0.0455, 0, 0, 0, 0, 0, 0);
    SpawnPlayer(playerid);
  } else {
    float pos[4];
    char fpath[256];
    char *name = RetPname(playerid);
    sprintf(fpath, PLAYER_ACCOUNT, name);

    mINI::INIFile file(fpath);
    mINI::INIStructure ini;
    file.read(ini);
    pos[0] = std::stof(ini["position"]["x"]);
    pos[1] = std::stof(ini["position"]["y"]);
    pos[2] = std::stof(ini["position"]["z"]);
    pos[3] = std::stof(ini["position"]["r"]);
    SetSpawnInfo(playerid, 0, std::stoi(ini["position"]["skin"]), pos[0],
                 pos[1], pos[2], pos[3], 0, 0, 0, 0, 0, 0);
    SpawnPlayer(playerid);

    free(name);
  }
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnDialogResponse(int playerid, int dialogid,
                                                int response, int listitem,
                                                const char *inputtext) {
  char *name = RetPname(playerid);

  switch (dialogid) {
  case DIALOG_HOUSE_STORAGE_TAKE: {
    Player[playerid].DataState.housetake = listitem;
    ShowPlayerDialog(playerid, DIALOG_HOUSE_STORAGE_TAKE_AMOUNT,
                     DIALOG_STYLE_INPUT, "Storage - Take - Amount",
                     "Enter the amount of item(s) to take:", "Confirm",
                     "Cancel");
    break;
  }
  case DIALOG_HOUSE_STORAGE_STORE: {
    Player[playerid].DataState.housestore = listitem;
    ShowPlayerDialog(playerid, DIALOG_HOUSE_STORAGE_STORE_AMOUNT,
                     DIALOG_STYLE_INPUT, "Storage - Store - Amount",
                     "Enter the amount of item(s) to store:", "Confirm",
                     "Cancel");
    break;
  }
  case DIALOG_HOUSE_STORAGE_TAKE_AMOUNT: {
    int slotid = Player[playerid].DataState.housetake;
    int houseid = GetHouseID(playerid);
    struct T_Item item = Houses[houseid].Items[slotid].Item;
    Player[playerid].DataState.housetake = -1;

    if (canint(inputtext)) {
      int amount = std::stoi(inputtext);

      if (Houses[houseid].Items[slotid].Quant < amount)
        return SendClientMessage(playerid, COLOR_ERROR,
                                 "ERROR: Insufficient amount!");

      AddItem(playerid, item, amount);
      AddHouseItem(houseid, item, -amount);
    } else
      return SendClientMessage(playerid, COLOR_ERROR,
                               "ERROR: The amount has to be numeric!");
  }
  case DIALOG_HOUSE_STORAGE_STORE_AMOUNT: {
    int slotid = Player[playerid].DataState.housestore;
    int houseid = GetHouseID(playerid);
    struct T_Item item = Player[playerid].Inventory[slotid].Item;
    Player[playerid].DataState.housestore = -1;

    if (canint(inputtext)) {
      int amount = std::stoi(inputtext);

      if (Player[playerid].Inventory[slotid].Quant < amount)
        return SendClientMessage(playerid, COLOR_ERROR,
                                 "ERROR: Insufficient amount!");

      AddHouseItem(houseid, item, amount);
      AddItem(playerid, item, -amount);
    } else
      return SendClientMessage(playerid, COLOR_ERROR,
                               "ERROR: The amount has to be numeric!");
  }
  case DIALOG_STORE: {
    for (int i = 0; i < MAX_BUSINESS; i++) {
      if (GetPlayerVirtualWorld(playerid) == Stores[i].World) {
        if (GetPlayerMoney(playerid) < Stores[i].Items[listitem].Item.Price)
          return SendClientMessage(playerid, COLOR_ERROR,
                                   "ERROR: Not enough money!");
        AddItem(playerid, Stores[i].Items[listitem].Item, 1);
        AddBizItem("store", playerid, Stores[i].Items[listitem].Item, -1);
        GivePlayerMoney(playerid, -Stores[i].Items[listitem].Item.Price);
      }
    }
    break;
  }
  case DIALOG_INVENTORY: {
    break;
  }
  case DIALOG_LOGIN: {
    char file_account[256];
    sprintf(file_account, PLAYER_ACCOUNT, name);

    if (response) {
      mINI::INIFile file(file_account);
      mINI::INIStructure ini;
      file.read(ini);

      if (!strcmp(ini["account"]["password"].c_str(), inputtext)) {
        TogglePlayerSpectating(playerid, 0);
        SetPlayerColor(playerid, 0xFFFFFFFF);
        Player[playerid].Flag.FirstSpawn = true;
      } else
        ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Login",
                         "{AA0000}Password Incorrect!{FFFFFF}\nThis Username "
                         "Is Registered\nType Your Password In Order To Login",
                         "Login", "Leave");
    }
    break;
  }
  case DIALOG_REGISTER: {
    char file_account[256];
    sprintf(file_account, PLAYER_ACCOUNT, name);

    if (response) {
      if (strlen(inputtext) < 8) {
        ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD,
                         "Register",
                         "{AA0000}Password Is Too Short(min 8 char, max 32 "
                         "char){FFFFFF}\nThis Username Is Not Registered\nType "
                         "Your Desired Password to Create Account",
                         "Register", "Leave");
      } else if (strlen(inputtext) > 32) {
        ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD,
                         "Register",
                         "{AA0000}Password Is Too Long(min 8 char, max 32 "
                         "char){FFFFFF}\nThis Username Is Not Registered\nType "
                         "Your Desired Password to Create Account",
                         "Register", "Leave");
      } else {
        mINI::INIFile file(file_account);
        mINI::INIStructure ini;
        ini["account"]["password"] = inputtext;
        if (file.write(ini)) {
          SendClientMessage(playerid, -1,
                            "{FFFF00}[DATABASE]{FFFFFF} Your account has been "
                            "created and saved into database");
          TogglePlayerSpectating(playerid, 0);
          SetPlayerColor(playerid, 0xFFFFFFFF);
          Player[playerid].Flag.NewAccount = true;
        } else
          SendClientMessage(playerid, COLOR_ERROR,
                            "Error: Account creation failed!");
      }
    }
    break;
  }
  }

  free(name);
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerCommandPerformed(int playerid,
                                                        const char *cmdtext,
                                                        bool success) {
  if (!success)
    SendClientMessage(playerid, COLOR_ERROR,
                      "ERROR: Unknown command, type /help or ask in /qna.");
  return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerCommandText(int playerid,
                                                   const char *cmdtext) {
  Cmd cmd = cmdparse(cmdtext);
  
  if (!strcmp(cmd.name, "ssveh")) {
    return cmd_ssveh(playerid, cmd);
  } else if (!strcmp(cmd.name, "getmaterials")) {
    return cmd_getmaterials(playerid, cmd);
  } else if (!strcmp(cmd.name, "storage")) {
    return cmd_storage(playerid, cmd);
  } else if (!strcmp(cmd.name, "rconweapon")) {
    return cmd_rconweapon(playerid, cmd);
  } else if (!strcmp(cmd.name, "setskin")) {
    return cmd_setskin(playerid, cmd);
  } else if (!strcmp(cmd.name, "money")) {
    return cmd_money(playerid, cmd);
  } else if (!strcmp(cmd.name, "tmt")) {
    return cmd_tmt(playerid, cmd);
  } else if (!strcmp(cmd.name, "beadmin")) {
    return cmd_beadmin(playerid, cmd);
  } else if (!strcmp(cmd.name, "noadmin")) {
    return cmd_noadmin(playerid, cmd);
  } else if (!strcmp(cmd.name, "job")) {
    return cmd_job(playerid, cmd);
  } else if (!strcmp(cmd.name, "me")) {
    return cmd_me(playerid, cmd);
  } else if (!strcmp(cmd.name, "do")) {
    return cmd_do(playerid, cmd);
  } else if (!strcmp(cmd.name, "sweeper")) {
    return cmd_sweeper(playerid, cmd);
  } else if (!strcmp(cmd.name, "engine") || !strcmp(cmd.name, "e")) {
    return cmd_engine(playerid, cmd);
  } else if (!strcmp(cmd.name, "rentveh")) {
    return cmd_rentveh(playerid, cmd);
  } else if (!strcmp(cmd.name, "lockrv")) {
    return cmd_lockrv(playerid, cmd);
  } else if (!strcmp(cmd.name, "mower")) {
    return cmd_mower(playerid, cmd);
  } else if (!strcmp(cmd.name, "createhouse")) {
    return cmd_createhouse(playerid, cmd);
  } else if (!strcmp(cmd.name, "createbiz")) {
    return cmd_createbiz(playerid, cmd);
  } else if (!strcmp(cmd.name, "enter")) {
    return cmd_enter(playerid, cmd);
  } else if (!strcmp(cmd.name, "exit")) {
    return cmd_exit(playerid, cmd);
  } else if (!strcmp(cmd.name, "buy")) {
    return cmd_buy(playerid, cmd);
  } else if (!strcmp(cmd.name, "inventory") || !strcmp(cmd.name, "inv")) {
    return cmd_inventory(playerid, cmd);
  } else if (!strcmp(cmd.name, "buyhouse")) {
    return cmd_buyhouse(playerid, cmd);
  } else if (!strcmp(cmd.name, "lockhouse")) {
    return cmd_lockhouse(playerid, cmd);
  } else if (!strcmp(cmd.name, "saveplayer")) {
    return cmd_saveplayer(playerid, cmd);
  } else if (!strcmp(cmd.name, "speedlimit")) {
    return cmd_speedlimit(playerid, cmd);
  }
  return false;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerEnterVehicle(int playerid, int vehicleid,
                                                    bool ispassenger) {
  for (int i = 0; i < MAX_RENTVEH; i++) {
    if (VehicleRent[i].ID == vehicleid && VehicleRent[i].Locked) {
      GameTextForPlayer(playerid, "~r~Locked", 1000, 4);
      CancelPlayer(playerid);
    }
  }
  return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerExitVehicle(int playerid,
                                                   int vehicleid) {
  for (int i = 0; i < MAX_PLAYER_VEHICLE; i++) {
    if (vehicleid == Player[playerid].Vehicle[i].ID) {
      if (Player[playerid].Vehicle[i].Lock &&
          GetVehicleType(Player[playerid].Vehicle[i].ID) != VTYPE_BIKE) {
        GameTextForPlayer(playerid, "~r~Locked", 1000, 5);
        PutPlayerInVehicle(playerid, vehicleid, GetPlayerVehicleSeat(playerid));
      }
    }
  }
  return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerStateChange(int playerid, int newstate,
                                                   int oldstate) {
  char txt[128];

  if (oldstate == PLAYER_STATE_DRIVER && newstate == PLAYER_STATE_ONFOOT) {
    HideVehicleIndicator(playerid);

    if (Player[playerid].Mission[MSweeper].active) {
      DisablePlayerCheckpoint(playerid);
      RemovePlayerFromVehicle(playerid);
      SetSweeperToRespawn2(2000);
      SendClientMessage(playerid, COLOR_ERROR, "Job canceled!");
      Player[playerid].Mission[MSweeper].active = false;
    } else if (Player[playerid].Mission[MMower].active) {
      DisablePlayerCheckpoint(playerid);
      RemovePlayerFromVehicle(playerid);
      SendClientMessage(playerid, COLOR_ERROR, "Job canceled!");
      Player[playerid].Mission[MMower].active = false;
    }
  } else if (oldstate == PLAYER_STATE_ONFOOT &&
             newstate == PLAYER_STATE_DRIVER) {
    if (IsPlayerInCar(playerid) || IsPlayerInTruck(playerid) ||
        IsPlayerOnBike(playerid) || IsPlayerOnQuad(playerid))
      ShowVehicleIndicator(playerid);

    for (int i = 0; i < MAX_RENTVEH; i++) {
      if (GetPlayerVehicleID(playerid) == VehicleRent[i].ID) {
        sprintf(txt, "This vehicle is rented by: %s, Time left: %d minute(s)",
                VehicleRent[i].Owner, VehicleRent[i].RentTime);
        SendClientMessage(playerid, COLOR_INFO, txt);
      }
    }
  }
  return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerInteriorChange(int playerid, int newint,
                                                      int oldint) {
  if (oldint == 0 && newint > 0)
    FreezePlayer(playerid, 1000);
  return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerEnterCheckpoint(int playerid) {
  if (Player[playerid].Mission[MSweeper].active) {
    int progress = Player[playerid].Checkpoint[MSweeper].progress;

    if (progress < GetCheckpointCount(MSweeper)) {
      SetPlayerNextCheckpoint(playerid,
                              MissionCheckpoint[MSweeper][progress][0],
                              MissionCheckpoint[MSweeper][progress][1],
                              MissionCheckpoint[MSweeper][progress][2], 8.0);
      Player[playerid].Checkpoint[MSweeper].progress++;
      return 1;
    }
    DisablePlayerCheckpoint(playerid);
    Player[playerid].Mission[MSweeper].active = false;
    RemovePlayerFromVehicle(playerid);
    GivePlayerMoney(playerid, 15);
    SendClientMessage(playerid, COLOR_INFO, "Job completed! +$15");
    Player[playerid].Checkpoint[MSweeper].progress = 0;
    SetSweeperToRespawn2(2000);
  } else if (Player[playerid].Mission[MMower].active) {
    int progress = Player[playerid].Checkpoint[MMower].progress;

    if (progress < GetCheckpointCount(MMower)) {
      SetPlayerNextCheckpoint(playerid, MissionCheckpoint[MMower][progress][0],
                              MissionCheckpoint[MMower][progress][1],
                              MissionCheckpoint[MMower][progress][2], 2.0);
      Player[playerid].Checkpoint[MMower].progress++;
      return 1;
    }
    DisablePlayerCheckpoint(playerid);
    Player[playerid].Mission[MMower].active = false;
    RemovePlayerFromVehicle(playerid);
    GivePlayerMoney(playerid, 5);
    SendClientMessage(playerid, COLOR_INFO, "Job completed! +$5");
  } else if (Player[playerid].Mission[MMaterial].active) {
    struct T_Item item;
    strcpy(item.Name, "Material");
    item.Type = ITEM_MATERIAL;

    DisablePlayerCheckpoint(playerid);
    Player[playerid].Mission[MMaterial].active = false;
    AddItem(playerid, item, 3);
    SendClientMessage(playerid, COLOR_INFO, "You've received 3 materials!");
  }
  return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerUpdate(int playerid) {
  if (IsPlayerInAnyVehicle(playerid)) {
    int vehicleid = GetPlayerVehicleID(playerid);

    if (GetVehicleSpeed(vehicleid) > Player[playerid].DataState.speedlimit && SpeedLimitOn(playerid)) {
      SetVehicleSpeed(vehicleid, Player[playerid].DataState.speedlimit);
    }
  }
  return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerTakeDamage(int playerid, int issuerid,
                                                  float amount, int weaponid,
                                                  int bodypart) {
  float HP;
  GetPlayerHealth(playerid, &HP);

  if (weaponid == WEAPON_COLT45) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 10);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 25);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 8);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 8);
    }
  }

  if (weaponid == WEAPON_SILENCED) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, (HP - 13));
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, (HP - 27));
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, (HP - 11));
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, (HP - 11));
    }
  }

  if (weaponid == WEAPON_DEAGLE) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 20);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 32);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 13);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 13);
    }
  }

  if (weaponid == WEAPON_TEC9) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 8);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 15);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 4);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 4);
    }
  }

  if (weaponid == WEAPON_UZI) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 9);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 18);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 5);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 5);
    }
  }

  if (weaponid == WEAPON_MP5) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 10);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 25);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 8);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 8);
    }
  }

  if (weaponid == WEAPON_SHOTGUN) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 30);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 48);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 15);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 15);
    }
  }

  if (weaponid == WEAPON_SAWEDOFF) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 38);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 53);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 20);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 20);
    }
  }

  if (weaponid == WEAPON_SHOTGSPA) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 40);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 56);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 20);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 20);
    }
  }

  if (weaponid == WEAPON_AK47) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 40);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 56);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 24);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 24);
    }
  }

  if (weaponid == WEAPON_M4) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 43);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 60);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 25);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 25);
    }
  }

  if (weaponid == WEAPON_RIFLE) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 60);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 80);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 28);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 28);
    }
  }

  if (weaponid == WEAPON_SNIPER) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 75);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 90);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 30);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 30);
    }
  }
  return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerWeaponShot(int playerid, int weaponid,
                                                  int hittype, int hitid,
                                                  float fX, float fY,
                                                  float fZ) {
  if (hittype == BULLET_HIT_TYPE_VEHICLE) {
    float HP;
    GetVehicleHealth(hitid, &HP);

    if (weaponid == WEAPON_COLT45) {
      SetVehicleHealth(hitid, (HP - 35.0));
    }
    if (weaponid == WEAPON_SILENCED) {
      SetVehicleHealth(hitid, (HP - 38.0));
    }
    if (weaponid == WEAPON_DEAGLE) {
      SetVehicleHealth(hitid, (HP - 45.0));
    }
    if (weaponid == WEAPON_SHOTGUN) {
      SetVehicleHealth(hitid, (HP - 100.0));
    }
    if (weaponid == WEAPON_SAWEDOFF) {
      SetVehicleHealth(hitid, (HP - 130.0));
    }
    if (weaponid == WEAPON_SHOTGSPA) {
      SetVehicleHealth(hitid, (HP - 180.0));
    }
    if (weaponid == WEAPON_TEC9) {
      SetVehicleHealth(hitid, (HP - 40.0));
    }
    if (weaponid == WEAPON_UZI) {
      SetVehicleHealth(hitid, (HP - 48.0));
    }
    if (weaponid == WEAPON_MP5) {
      SetVehicleHealth(hitid, (HP - 53.0));
    }
    if (weaponid == WEAPON_AK47) {
      SetVehicleHealth(hitid, (HP - 68.0));
    }
    if (weaponid == WEAPON_M4) {
      SetVehicleHealth(hitid, (HP - 70.0));
    }
    if (weaponid == WEAPON_RIFLE) {
      SetVehicleHealth(hitid, (HP - 80.0));
    }
    if (weaponid == WEAPON_SNIPER) {
      SetVehicleHealth(hitid, (HP - 95.0));
    }
    if (weaponid == WEAPON_BRASSKNUCKLE) {
      SetVehicleHealth(hitid, (HP - 8.0));
    }
    if (weaponid == WEAPON_ROCKETLAUNCHER) {
      SetVehicleHealth(hitid, (HP - 1000.0));
    }
    if (weaponid == WEAPON_MINIGUN) {
      SetVehicleHealth(hitid, (HP - 10.0));
    }
    if (weaponid == WEAPON_GOLFCLUB) {
      SetVehicleHealth(hitid, (HP - 12.0));
    }
    if (weaponid == WEAPON_SHOVEL) {
      SetVehicleHealth(hitid, (HP - 12.0));
    }
    if (weaponid == WEAPON_NITESTICK) {
      SetVehicleHealth(hitid, (HP - 10.0));
    }
    if (weaponid == WEAPON_BAT) {
      SetVehicleHealth(hitid, (HP - 12.0));
    }
    if (weaponid == WEAPON_HEATSEEKER) {
      SetVehicleHealth(hitid, (HP - 1000.0));
    }
  }
  return 1;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
  return sampgdk::Supports() | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
  return sampgdk::Load(ppData);
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() { sampgdk::Unload(); }

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() { sampgdk::ProcessTick(); }
