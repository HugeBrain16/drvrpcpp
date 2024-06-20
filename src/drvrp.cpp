#include <unistd.h>

#include <iostream>
#include <cstring>

#include "lib/cmd.h"
#include "lib/ini.h"
#include "lib/sampgdk.h"
#include "lib/streamer.hpp"
#include "lib/stuff.hpp"
#include "lib/sha256.h"

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
  unused(timerid, data);

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
  unused(timerid, data);

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

  unused(timerid, data);

  char txt[128];
  int vid, pid;
  bool ispv;
  int vehicleid;
  float vSpeed, vHealth;
  T_Vehicle *veh;

  for (int playerid = 0; playerid < MAX_PLAYERS; playerid++) {
    if (IsPlayerConnected(playerid)) {
      if (IsPlayerInAnyVehicle(playerid)) {
        vehicleid = GetPlayerVehicleID(playerid);
        vSpeed = GetVehicleSpeed(vehicleid);
        vHealth = RetVehicleHealth(vehicleid);
        ispv = false;

        for (int x = 0; x < MAX_PLAYERS && !ispv; x++) {
          for (int y = 0; y < MAX_PLAYER_VEHICLES; y++) {
            if (vehicleid == Player[x].Vehicle[y].ID) {
              pid = x;
              vid = y;
              ispv = true;
              break;
            }
          }
        }

        veh = ispv ? &Player[pid].Vehicle[vid] : &StaticVehicle[vehicleid];

        if (EngineOn(vehicleid)) {
          float coeff = 0.05f * (vSpeed / 1000);

          veh->Fuel = std::max(0.0f, veh->Fuel - coeff);

          if (veh->Heat > 80) {
            if (vSpeed > veh->lastSpeed)
              veh->Heat += coeff;
            else
              veh->Heat -= coeff;
          } else {
            if (vSpeed > veh->lastSpeed)
              veh->Heat += 0.05f + coeff;
            else
              veh->Heat += 0.05f;
          }
        } else {
          veh->Heat = std::max(26.0f, veh->Heat - 0.005f);
        }

        veh->lastSpeed = vSpeed;

        sprintf(txt, "Speed: ~r~%.0fKm/h", vSpeed);
        if (vSpeed >= (Player[playerid].DataState.speedlimit - 1) && SpeedLimitOn(playerid))
          sprintf(txt, "Speed: ~y~%.0fKm/h", vSpeed);
        if (!EngineOn(vehicleid))
          sprintf(txt, "Speed: ~b~%.0fKm/h", vSpeed);
        PlayerTextDrawSetString(playerid, Player[playerid].VehicleIndicator.Speed, txt);

        sprintf(txt, "Health: ~r~%.2f", vHealth);
        if (!EngineOn(vehicleid))
          sprintf(txt, "Health: ~b~%.2f", vHealth);
        PlayerTextDrawSetString(playerid, Player[playerid].VehicleIndicator.Health, txt);

        sprintf(txt, "Fuel: ~r~%.1f", veh->Fuel);
        if (!EngineOn(vehicleid))
          sprintf(txt, "Fuel: ~b~%.1f", veh->Fuel);
        PlayerTextDrawSetString(playerid, Player[playerid].VehicleIndicator.Fuel, txt);

        sprintf(txt, "Temp: ~r~%.2f", veh->Heat);
        if (!EngineOn(vehicleid))
          sprintf(txt, "Temp: ~b~%.1f", veh->Heat);
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

  for (size_t i = 0; i < WorldPickup.size(); i++) {
    const PickupData *pickup = &WorldPickup[i];

    if (pickup->model > 0) {
      std::cout << "Loading world pickups... (" << i << ")\n";
      Pickup::Create(
        pickup->model,
        pickup->type,
        pickup->x,
        pickup->y,
        pickup->z
      );
    }
  }

  for (size_t i = 0; i < GlobalTextLabel.size(); i++) {
    const TextLabelData *label = &GlobalTextLabel[i];

    if (label->text.length() > 0) {
      std::cout << "Loading world text labels... (" << i << ")\n";

      TextLabel::Create(
        label->text.c_str(),
        label->color,
        label->x,
        label->y,
        label->z,
        label->range
      );
    }
  }

  std::cout << "Loading world objects...\n";

  /* blockage */
  Object::Create(971, 720.06940, -462.57724, 15.39299, 0.00000, 0.00000, 0.00000);
  Object::Create(971, 1042.84998, -1026.01123, 31.09643, 0.00000, 0.00000, 0.00000);
  Object::Create(971, 1025.36536, -1029.33276, 31.63884, 0.00000, 0.00000, 0.00000);
  Object::Create(971, -1420.50977, 2591.11279, 56.94583, 0.00000, 0.00000, 180.00000);
  Object::Create(971, 2386.68213, 1043.31189, 9.92575, 0.00000, 0.00000, 0.00000);
  Object::Create(8167, 2645.81055, -2039.32849, 12.56419, 0.00000, 0.00000, 90.00000);
  Object::Create(8167, 2645.81055, -2039.32849, 15.00648, 0.00000, 0.00000, 90.00000);
  Object::Create(971, -1935.82751, 238.56221, 33.64063, 0.00000, 0.00000, 0.00000);
  Object::Create(971, -1904.62756, 277.66324, 42.39743, 0.00000, 0.00000, 0.00000);
  Object::Create(971, -2716.14404, 216.72392, 3.81582, 0.00000, 0.00000, 90.00000);
  Object::Create(971, 1843.31055, -1855.03943, 12.37510, 0.00000, 0.00000, 90.00000);
  Object::Create(971, 2005.17334, 2303.33716, 9.81711, 0.00000, 0.00000, 0.00000);
  Object::Create(971, 1968.37793, 2162.65747, 12.66515, 0.00000, 0.00000, -90.00000);
  Object::Create(971, 2393.19873, 1483.32202, 12.39729, 0.00000, 0.00000, 0.00000);
  Object::Create(971, -99.80347, 1111.46582, 20.85815, 0.00000, 0.00000, 0.00000);
  Object::Create(971, -2425.07886, 1027.89941, 51.84350, 0.00000, 0.00000, 180.00000);
  Object::Create(971, 2071.52344, -1831.55835, 13.00516, 0.00000, 0.00000, 90.00000);
  Object::Create(971, 488.63022, -1735.32129, 10.59052, 0.00000, 0.00000, -8.46000);

  /* electronic store */
  Object::Create(18869, -2236.96631, 127.79252, 1035.46399, 0.00000, 0.00000, -180.00000);
  Object::Create(18869, -2236.70630, 127.79250, 1035.46399, 0.00000, 0.00000, -180.00000);
  Object::Create(18869, -2236.42627, 127.79250, 1035.46399, 0.00000, 0.00000, -180.00000);
  Object::Create(18869, -2236.10620, 127.79250, 1035.46399, 0.00000, 0.00000, -180.00000);
  Object::Create(18867, -2235.58618, 127.79250, 1035.46399, 0.00000, 0.00000, -180.00000);
  Object::Create(18867, -2235.30640, 127.79250, 1035.46399, 0.00000, 0.00000, -180.00000);
  Object::Create(18867, -2235.00635, 127.79250, 1035.46399, 0.00000, 0.00000, -180.00000);
  Object::Create(18867, -2234.64624, 127.79250, 1035.46399, 0.00000, 0.00000, -180.00000);
  Object::Create(2028, -2231.81104, 127.77340, 1035.59448, 0.00000, 0.00000, 0.00000);
  Object::Create(2226, -2235.28638, 129.41631, 1035.47986, 0.00000, 0.00000, -120.00000);
  Object::Create(2226, -2236.08618, 129.41631, 1035.47986, 0.00000, 0.00000, -120.00000);
  Object::Create(2226, -2234.42627, 129.41631, 1035.47986, 0.00000, 0.00000, -120.00000);
  Object::Create(1781, -2228.45703, 130.95360, 1036.00439, 0.00000, 0.00000, -33.00000);
  Object::Create(1781, -2227.23706, 130.95360, 1036.00439, 0.00000, 0.00000, -33.00000);
  Object::Create(19807, -2237.30664, 137.79880, 1035.53870, 0.00000, 0.00000, 0.00000);
  Object::Create(19807, -2237.02661, 137.79880, 1035.53870, 0.00000, 0.00000, 0.00000);
  Object::Create(19807, -2236.76660, 137.79880, 1035.53870, 0.00000, 0.00000, 0.00000);
  Object::Create(19807, -2236.52661, 137.79880, 1035.53870, 0.00000, 0.00000, 0.00000);
  Object::Create(19807, -2236.28662, 137.79880, 1035.53870, 0.00000, 0.00000, 0.00000);
  Object::Create(2099, -2230.93115, 138.02121, 1034.37671, 0.00000, 0.00000, -33.00000);
  Object::Create(18871, -2236.97729, 127.61670, 1035.79126, 0.00000, 0.00000, -180.00000);
  Object::Create(18871, -2236.79736, 127.61670, 1035.79126, 0.00000, 0.00000, -180.00000);
  Object::Create(18871, -2236.59741, 127.61670, 1035.79126, 0.00000, 0.00000, -180.00000);
  Object::Create(18871, -2236.41724, 127.61670, 1035.79126, 0.00000, 0.00000, -180.00000);
  Object::Create(18871, -2236.21729, 127.61670, 1035.79126, 0.00000, 0.00000, -180.00000);
  Object::Create(18871, -2235.99731, 127.61670, 1035.79126, 0.00000, 0.00000, -180.00000);
  Object::Create(2103, -2228.60181, 133.66570, 1036.00464, 0.00000, 0.00000, -26.00000);
  Object::Create(2103, -2227.51099, 133.75337, 1036.00464, 0.00000, 0.00000, 33.00000);
  Object::Create(2149, -2237.95483, 134.54311, 1036.16431, 0.00000, 0.00000, 0.00000);
  Object::Create(2149, -2237.95483, 133.86310, 1036.16431, 0.00000, 0.00000, 0.00000);
  Object::Create(2149, -2237.95483, 133.24310, 1036.16431, 0.00000, 0.00000, 0.00000);
  Object::Create(2149, -2237.95483, 132.66310, 1036.16431, 0.00000, 0.00000, 0.00000);
  Object::Create(2232, -2232.84497, 127.95030, 1035.02405, 0.00000, 0.00000, 180.00000);
  Object::Create(2232, -2233.74512, 127.95030, 1035.02405, 0.00000, 0.00000, 180.00000);
  Object::Create(1785, -2229.57910, 136.79010, 1036.11255, 0.00000, 0.00000, 0.00000);
  Object::Create(2190, -2241.02222, 130.55769, 1035.47998, 0.00000, 0.00000, 90.00000);
  Object::Create(2190, -2241.02222, 131.33771, 1035.47998, 0.00000, 0.00000, 90.00000);
  Object::Create(2190, -2241.02222, 132.13770, 1035.47998, 0.00000, 0.00000, 90.00000);
  Object::Create(2695, -2231.91724, 127.35710, 1036.89270, 0.00000, -5.00000, 0.00000);
  Object::Create(2641, -2222.01172, 144.32721, 1036.89685, 0.00000, 0.00000, 0.00000);
  Object::Create(2641, -2235.29932, 138.14149, 1036.89685, 0.00000, 0.00000, 0.00000);
  Object::Create(2641, -2232.09937, 138.14149, 1036.89685, 0.00000, 0.00000, 0.00000);
  Object::Create(19893, -2240.77539, 133.84290, 1035.48022, 0.00000, 0.00000, 90.00000);
  Object::Create(19893, -2240.77539, 134.46291, 1035.48022, 0.00000, 0.00000, 90.00000);
  Object::Create(19893, -2240.77539, 135.06290, 1035.48022, 0.00000, 0.00000, 90.00000);
  Object::Create(19893, -2240.77539, 135.60291, 1035.48022, 0.00000, 0.00000, 90.00000);
  Object::Create(18875, -2233.47168, 137.81740, 1036.40649, 0.00000, 0.00000, 0.00000);
  Object::Create(18875, -2233.23169, 137.81740, 1036.40649, 0.00000, 0.00000, 0.00000);
  Object::Create(18875, -2233.73169, 137.81740, 1036.40649, 0.00000, 0.00000, 0.00000);
  Object::Create(18875, -2233.99170, 137.81740, 1036.40649, 0.00000, 0.00000, 0.00000);
  Object::Create(18875, -2234.25171, 137.81740, 1036.40649, 0.00000, 0.00000, 0.00000);

  /* tool store */
  Object::Create(337, 132.14426, 1706.19189, 1003.14368, 177.00000, 90.00000, -97.00000, -1, 1);
  Object::Create(3761, 134.15401, 1696.84436, 1001.57410, 0.00000, 0.00000, 0.00000, -1, 1);
  Object::Create(18644, 135.82140, 1710.93713, 1001.64600, 0.00000, 90.00000, 0.00000, -1, 1);
  Object::Create(18644, 136.36140, 1710.93713, 1001.64600, 0.00000, 90.00000, 0.00000, -1, 1);
  Object::Create(18644, 137.08141, 1710.93713, 1001.64600, 0.00000, 90.00000, 0.00000, -1, 1);
  Object::Create(18644, 137.56140, 1710.93713, 1001.64600, 0.00000, 90.00000, 0.00000, -1, 1);
  Object::Create(18644, 137.56140, 1710.55713, 1001.64600, 0.00000, 90.00000, 0.00000, -1, 1);
  Object::Create(18644, 137.02139, 1710.55713, 1001.64600, 0.00000, 90.00000, 0.00000, -1, 1);
  Object::Create(18644, 136.36140, 1710.55713, 1001.64600, 0.00000, 90.00000, 0.00000, -1, 1);
  Object::Create(18644, 135.84140, 1710.55713, 1001.64600, 0.00000, 90.00000, 0.00000, -1, 1);
  Object::Create(18638, 134.71471, 1710.85034, 1001.70599, 0.00000, -90.00000, -180.00000, -1, 1);
  Object::Create(18638, 134.09470, 1710.85034, 1001.70599, 0.00000, -90.00000, -180.00000, -1, 1);
  Object::Create(18638, 133.53470, 1710.85034, 1001.70599, 0.00000, -90.00000, -180.00000, -1, 1);
  Object::Create(18638, 132.93469, 1710.85034, 1001.70599, 0.00000, -90.00000, -180.00000, -1, 1);
  Object::Create(18638, 132.93469, 1710.49036, 1001.70599, 0.00000, -90.00000, -180.00000, -1, 1);
  Object::Create(18638, 133.51469, 1710.49036, 1001.70599, 0.00000, -90.00000, -180.00000, -1, 1);
  Object::Create(18638, 134.09470, 1710.49036, 1001.70599, 0.00000, -90.00000, -180.00000, -1, 1);
  Object::Create(18638, 134.71471, 1710.49036, 1001.70599, 0.00000, -90.00000, -180.00000, -1, 1);
  Object::Create(18632, 137.59850, 1700.21667, 1001.39551, 190.00000, 0.00000, 0.00000, -1, 1);
  Object::Create(18632, 137.73849, 1700.21667, 1001.39551, 190.00000, 0.00000, 0.00000, -1, 1);
  Object::Create(18632, 137.89850, 1700.21667, 1001.39551, 190.00000, 0.00000, 0.00000, -1, 1);
  Object::Create(18632, 138.05850, 1700.21667, 1001.39551, 190.00000, 0.00000, 0.00000, -1, 1);
  Object::Create(18632, 138.19850, 1700.21667, 1001.39551, 190.00000, 0.00000, 0.00000, -1, 1);
  Object::Create(18632, 138.31850, 1700.21667, 1001.39551, 190.00000, 0.00000, 0.00000, -1, 1);
  Object::Create(18632, 138.43851, 1700.21667, 1001.39551, 190.00000, 0.00000, 0.00000, -1, 1);
  Object::Create(18632, 138.55850, 1700.21667, 1001.39551, 190.00000, 0.00000, 0.00000, -1, 1);
  Object::Create(18632, 138.67850, 1700.21667, 1001.39551, 190.00000, 0.00000, 0.00000, -1, 1);
  Object::Create(18632, 138.81850, 1700.21667, 1001.39551, 190.00000, 0.00000, 0.00000, -1, 1);
  Object::Create(18632, 138.93851, 1700.21667, 1001.39551, 190.00000, 0.00000, 0.00000, -1, 1);
  Object::Create(19324, 145.08090, 1701.52148, 1001.77692, 0.00000, 0.00000, 0.00000, -1, 1);
  Object::Create(2314, 133.37389, 1700.25806, 1001.12329, 0.00000, 0.00000, 0.00000, -1, 1);
  Object::Create(18641, 134.93510, 1700.11414, 1001.64520, 0.00000, 90.00000, 0.00000, -1, 1);
  Object::Create(18641, 134.41510, 1700.11414, 1001.64520, 0.00000, 90.00000, 0.00000, -1, 1);
  Object::Create(18641, 133.91510, 1700.11414, 1001.64520, 0.00000, 90.00000, 0.00000, -1, 1);
  Object::Create(18641, 133.37511, 1700.11414, 1001.64520, 0.00000, 90.00000, 0.00000, -1, 1);
  Object::Create(18641, 133.37511, 1700.49414, 1001.64520, 0.00000, 90.00000, 0.00000, -1, 1);

  for (size_t i = 0; i < JobVehicleSweeper.size(); i++) {
    const VehicleData *vehicle = &JobVehicleSweeper[i];

    if (vehicle->model > 0) {
      std::cout << "Loading job vehicles sweeper...(" << i << ")\n";
      Sweeper[i] = CreateVehicle(
        vehicle->model,
        vehicle->x,
        vehicle->y,
        vehicle->z,
        vehicle->rotation,
        vehicle->color1,
        vehicle->color2,
        vehicle->delay,
        vehicle->siren
      );
    }
  }

  for (size_t i = 0; i < JobVehicleBus.size(); i++) {
    const VehicleData *vehicle = &JobVehicleBus[i];

    if (vehicle->model > 0) {
      std::cout << "Loading job vehicles bus...(" << i << ")\n";
      Bus[i].ID = CreateVehicle(
        vehicle->model,
        vehicle->x,
        vehicle->y,
        vehicle->z,
        vehicle->rotation,
        vehicle->color1,
        vehicle->color2,
        vehicle->delay,
        vehicle->siren
      );
    }
  }

  for (size_t i = 0; i < JobVehicleMower.size(); i++) {
    const VehicleData *vehicle = &JobVehicleMower[i];

    if (vehicle->model > 0) {
      std::cout << "Loading job vehicles mower...(" << i << ")\n";
      Mower[i] = CreateVehicle(
        vehicle->model,
        vehicle->x,
        vehicle->y,
        vehicle->z,
        vehicle->rotation,
        vehicle->color1,
        vehicle->color2,
        vehicle->delay,
        vehicle->siren
      );
    }
  }

  char plateBuffer[10];

  for (size_t i = 0; i < Bus.size(); i++) {
    sprintf(plateBuffer, "BUS-%d", i + 1);
    SetVehicleNumberPlate(Bus[i].ID, plateBuffer);
  }

  for (size_t i = 0; i < Sweeper.size(); i++) {
    sprintf(plateBuffer, "SWEEP-%d", i + 1);
    SetVehicleNumberPlate(Sweeper[i], plateBuffer);
  }

  for (size_t i = 0; i < Bus.size(); i++)
    strcpy(Bus[i].Owner, "None");

  for (size_t i = 0; i < MAX_BUSINESSES; i++)
    LoadBusiness("store", i);

  for (size_t i = 0; i < MAX_HOUSES; i++)
    LoadHouse(i);

  std::cout << "Starting timers...\n";
  SetTimer(60000, true, TC_UpdateRentTime, nullptr);
  SetTimer(100, true, TC_AltPlayerUpdate, nullptr);
  SetTimer(1000, true, TC_NoExplodingVeh, nullptr);

  std::cout << "Server initialized!\n";
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerConnect(int playerid) {
  Player[playerid].Flag.FirstSpawn = true;

  SetPlayerSkillLevel(playerid, WEAPONSKILL_PISTOL, 0);
  SetPlayerColor(playerid, 0x000000FF);
  TogglePlayerSpectating(playerid, 1);

  char file_account[256];
  const char *name = RetPname(playerid);
  sprintf(file_account, PLAYER_ACCOUNT, name);

  if (!fexist(file_account)) {
    ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, "Register",
      "This Username Is Not Registered\nType Your Desired Password to Create Account",
      "Register", "Leave"
    );
  } else {
    ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Login",
      "This Username Is Registered\nType Your Password In Order To Login",
      "Login", "Leave"
    );
  }
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerDisconnect(int playerid, int reason) {
  char msgBuff[64];
  const char *name = RetPname(playerid);
  sprintf(msgBuff, "%s Has disconnected from the server (%s)", name, DCReason[reason]);
  SendClientMessageToAll(0xFFFF00AA, msgBuff);

  if (!Player[playerid].Flag.FirstSpawn) {
    SavePlayer(playerid);
    SaveInventory(playerid);

    for (int i = 0; i < MAX_HOUSES; i++) {
      if (!strcmp(name, Houses[i].Owner)) {
        SaveHouse(i);
      }
    }
  }
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerDeath(int playerid, int killerid, int reason) {
  unused(killerid, reason);

  Player[playerid].Flag.Dead = true;
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerSpawn(int playerid) {
  if (Player[playerid].Flag.FirstSpawn) {
    char msg[128];
    const char *name = RetPname(playerid);
    sprintf(msg, "%s Has connected to the server", name);
    SendClientMessageToAll(0xFFFF00AA, msg);
    Player[playerid].Flag.FirstSpawn = false;

    if (!Player[playerid].Flag.NewAccount) {
      LoadPlayer(playerid);
      LoadInventory(playerid);
    } 

    /* == Vehicle Status UI == */
    /* Background Box */
    Player[playerid].VehicleIndicator.MainBox = CreatePlayerTextDraw(playerid, 634.503662f, 365.500000f, "MainBox");
    PlayerTextDrawLetterSize(playerid, Player[playerid].VehicleIndicator.MainBox, 0.000000f, 7.914812f);
    PlayerTextDrawTextSize(playerid, Player[playerid].VehicleIndicator.MainBox, 531.645690f, 0.000000f);
    PlayerTextDrawAlignment(playerid, Player[playerid].VehicleIndicator.MainBox, 1);
    PlayerTextDrawColor(playerid, Player[playerid].VehicleIndicator.MainBox, 0);
    PlayerTextDrawUseBox(playerid, Player[playerid].VehicleIndicator.MainBox, true);
    PlayerTextDrawBoxColor(playerid, Player[playerid].VehicleIndicator.MainBox, 102);
    PlayerTextDrawSetShadow(playerid, Player[playerid].VehicleIndicator.MainBox, 0);
    PlayerTextDrawSetOutline(playerid, Player[playerid].VehicleIndicator.MainBox, 0);
    PlayerTextDrawFont(playerid, Player[playerid].VehicleIndicator.MainBox, 0);

    /* Speed */
    Player[playerid].VehicleIndicator.Speed = CreatePlayerTextDraw(playerid, 538.331176f, 364.583190f, "Speed:");
    PlayerTextDrawLetterSize(playerid, Player[playerid].VehicleIndicator.Speed, 0.287891f, 1.570831f);
    PlayerTextDrawAlignment(playerid, Player[playerid].VehicleIndicator.Speed, 1);
    PlayerTextDrawColor(playerid, Player[playerid].VehicleIndicator.Speed, -1);
    PlayerTextDrawSetShadow(playerid, Player[playerid].VehicleIndicator.Speed, 0);
    PlayerTextDrawSetOutline(playerid, Player[playerid].VehicleIndicator.Speed, 1);
    PlayerTextDrawBackgroundColor(playerid, Player[playerid].VehicleIndicator.Speed, 51);
    PlayerTextDrawFont(playerid, Player[playerid].VehicleIndicator.Speed, 1);
    PlayerTextDrawSetProportional(playerid, Player[playerid].VehicleIndicator.Speed, 1);

    /* Health */
    Player[playerid].VehicleIndicator.Health = CreatePlayerTextDraw(playerid, 537.862792f, 382.083465f, "Health:");
    PlayerTextDrawLetterSize(playerid, Player[playerid].VehicleIndicator.Health, 0.279457f, 1.594162f);
    PlayerTextDrawAlignment(playerid, Player[playerid].VehicleIndicator.Health, 1);
    PlayerTextDrawColor(playerid, Player[playerid].VehicleIndicator.Health, -1);
    PlayerTextDrawSetShadow(playerid, Player[playerid].VehicleIndicator.Health, 0);
    PlayerTextDrawSetOutline(playerid, Player[playerid].VehicleIndicator.Health, 1);
    PlayerTextDrawBackgroundColor(playerid, Player[playerid].VehicleIndicator.Health, 51);
    PlayerTextDrawFont(playerid, Player[playerid].VehicleIndicator.Health, 1);
    PlayerTextDrawSetProportional(playerid, Player[playerid].VehicleIndicator.Health, 1);

    /* Fuel */
    Player[playerid].VehicleIndicator.Fuel = CreatePlayerTextDraw(playerid, 536.925964f, 400.166625f, "Fuel:");
    PlayerTextDrawLetterSize(playerid, Player[playerid].VehicleIndicator.Fuel, 0.447188f, 1.477498f);
    PlayerTextDrawAlignment(playerid, Player[playerid].VehicleIndicator.Fuel, 1);
    PlayerTextDrawColor(playerid, Player[playerid].VehicleIndicator.Fuel, -1);
    PlayerTextDrawSetShadow(playerid, Player[playerid].VehicleIndicator.Fuel, 0);
    PlayerTextDrawSetOutline(playerid, Player[playerid].VehicleIndicator.Fuel, 1);
    PlayerTextDrawBackgroundColor(playerid, Player[playerid].VehicleIndicator.Fuel, 51);
    PlayerTextDrawFont(playerid, Player[playerid].VehicleIndicator.Fuel, 1);
    PlayerTextDrawSetProportional(playerid, Player[playerid].VehicleIndicator.Fuel, 1);

    /* Temperature */
    Player[playerid].VehicleIndicator.Heat = CreatePlayerTextDraw(playerid, 536.457214f, 418.833374f, "Temp:");
    PlayerTextDrawLetterSize(playerid, Player[playerid].VehicleIndicator.Heat, 0.414860f, 1.459999f);
    PlayerTextDrawAlignment(playerid, Player[playerid].VehicleIndicator.Heat, 1);
    PlayerTextDrawColor(playerid, Player[playerid].VehicleIndicator.Heat, -1);
    PlayerTextDrawSetShadow(playerid, Player[playerid].VehicleIndicator.Heat, 0);
    PlayerTextDrawSetOutline(playerid, Player[playerid].VehicleIndicator.Heat, 1);
    PlayerTextDrawBackgroundColor(playerid, Player[playerid].VehicleIndicator.Heat, 51);
    PlayerTextDrawFont(playerid, Player[playerid].VehicleIndicator.Heat, 1);
    PlayerTextDrawSetProportional(playerid, Player[playerid].VehicleIndicator.Heat, 1);
  }

  if (Player[playerid].Flag.NewAccount) {
    GivePlayerMoney(playerid, 500);
    Player[playerid].Status.hunger = 100.0f;
    Player[playerid].Status.thirst = 100.0f;
    Player[playerid].Status.energy = 100.0f;
    Player[playerid].Flag.NewAccount = false;
  }

  if (Player[playerid].Flag.Dead) {
    Player[playerid].Flag.Dead = false;
    SetPlayerPos(playerid, 2035.5458f, -1413.7125f, 16.9922f);
    SetPlayerFacingAngle(playerid, 132.4680f);
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
  unused(classid);

  if (Player[playerid].Flag.NewAccount) {
    SetSpawnInfo(playerid, 0, STARTER_SKINS[Random.i(16)], 1643.9750f, -2332.2830f, 13.5469f, 0.0455f, 0, 0, 0, 0, 0, 0);
    SpawnPlayer(playerid);
  } else {
    float pos[4];
    char fpath[256];
    const char *name = RetPname(playerid);
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
  }
  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnDialogResponse(int playerid, int dialogid, int response, int listitem, const char *inputtext) {
  const char *name = RetPname(playerid);

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
    ShowPlayerDialog(playerid, DIALOG_HOUSE_STORAGE_STORE_AMOUNT, DIALOG_STYLE_INPUT, "Storage - Store - Amount", "Enter the amount of item(s) to store:", "Confirm", "Cancel");
    break;
  }
  case DIALOG_HOUSE_STORAGE_TAKE_AMOUNT: {
    int slotid = Player[playerid].DataState.housetake;
    int houseid = GetHouseID(playerid);
    struct T_Item item = Houses[houseid].Items[slotid].Item;
    Player[playerid].DataState.housetake = -1;

    if (canint(inputtext)) {
      int amount = std::stoi(inputtext);

      if (Houses[houseid].Items[slotid].Quant < amount) {
        return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Insufficient amount!");
        break;
      }

      AddItem(playerid, item, amount);
      AddHouseItem(houseid, item, -amount);
      break;
    } else {
      return SendClientMessage(playerid, COLOR_ERROR, "ERROR: The amount has to be numeric!");
      break;
    }
  }
  case DIALOG_HOUSE_STORAGE_STORE_AMOUNT: {
    int slotid = Player[playerid].DataState.housestore;
    int houseid = GetHouseID(playerid);
    struct T_Item item = Player[playerid].Inventory[slotid].Item;
    Player[playerid].DataState.housestore = -1;

    if (canint(inputtext)) {
      int amount = std::stoi(inputtext);

      if (Player[playerid].Inventory[slotid].Quant < amount) {
        return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Insufficient amount!");
        break;
      }

      AddHouseItem(houseid, item, amount);
      AddItem(playerid, item, -amount);
      break;
    } else {
      return SendClientMessage(playerid, COLOR_ERROR, "ERROR: The amount has to be numeric!");
      break;
    }
  }
  case DIALOG_STORE: {
    for (int i = 0; i < MAX_BUSINESSES; i++) {
      if (GetPlayerVirtualWorld(playerid) == Stores[i].World) {
        if (GetPlayerMoney(playerid) < Stores[i].Items[listitem].Item.Price)
          return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Not enough money!");

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
    SHA256 sha256;

    sprintf(file_account, PLAYER_ACCOUNT, name);

    if (response) {
      mINI::INIFile file(file_account);
      mINI::INIStructure ini;
      file.read(ini);

      if (!strcmp(ini["account"]["password"].c_str(), sha256(inputtext).c_str())) {
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
    SHA256 sha256;

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
        ini["account"]["password"] = sha256(inputtext).c_str();
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

  return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerCommandPerformed(int playerid, const char *cmdtext, bool success) {
  unused(cmdtext);

  if (!success)
    SendClientMessage(playerid, COLOR_ERROR, "ERROR: Unknown command, type /help or ask in /qna.");

  return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerCommandText(int playerid, const char *cmdtext) {
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

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerEnterVehicle(int playerid, int vehicleid, bool ispassenger) {
  unused(ispassenger);

  for (int i = 0; i < MAX_RENTVEH; i++) {
    if (VehicleRent[i].ID == vehicleid && VehicleRent[i].Locked) {
      GameTextForPlayer(playerid, "~r~Locked", 1000, 4);
      CancelPlayer(playerid);
    }
  }
  return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerExitVehicle(int playerid, int vehicleid) {
  for (int i = 0; i < MAX_PLAYER_VEHICLES; i++) {
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

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerTakeDamage(int playerid, int issuerid, float amount, int weaponid, int bodypart) {
  unused(issuerid, amount);

  float HP;
  GetPlayerHealth(playerid, &HP);

  if (weaponid == WEAPON_COLT45) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 10.0f);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 25.0f);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 8.0f);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 8.0f);
    }
  }

  if (weaponid == WEAPON_SILENCED) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 13.0f);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 27.0f);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 11.0f);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 11.0f);
    }
  }

  if (weaponid == WEAPON_DEAGLE) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 20.0f);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 32.0f);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 13.0f);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 13.0f);
    }
  }

  if (weaponid == WEAPON_TEC9) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 8.0f);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 15.0f);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 4.0f);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 4.0f);
    }
  }

  if (weaponid == WEAPON_UZI) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 9.0f);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 18.0f);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 5.0f);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 5.0f);
    }
  }

  if (weaponid == WEAPON_MP5) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 10.0f);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 25.0f);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 8.0f);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 8.0f);
    }
  }

  if (weaponid == WEAPON_SHOTGUN) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 30.0f);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 48.0f);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 15.0f);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 15.0f);
    }
  }

  if (weaponid == WEAPON_SAWEDOFF) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 38.0f);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 53.0f);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 20.0f);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 20.0f);
    }
  }

  if (weaponid == WEAPON_SHOTGSPA) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 40.0f);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 56.0f);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 20.0f);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 20.0f);
    }
  }

  if (weaponid == WEAPON_AK47) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 40.0f);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 56.0f);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 24.0f);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 24.0f);
    }
  }

  if (weaponid == WEAPON_M4) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 43.0f);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 60.0f);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 25.0f);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 25.0f);
    }
  }

  if (weaponid == WEAPON_RIFLE) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 60.0f);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 80.0f);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 28.0f);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 28.0f);
    }
  }

  if (weaponid == WEAPON_SNIPER) {
    if (bodypart == BODY_PART_TORSO || bodypart == BODY_PART_GROIN) {
      SetPlayerHealth(playerid, HP - 75.0f);
    }
    if (bodypart == BODY_PART_HEAD) {
      SetPlayerHealth(playerid, HP - 90.0f);
    }
    if (bodypart == BODY_PART_LEFT_ARM || bodypart == BODY_PART_RIGHT_ARM) {
      SetPlayerHealth(playerid, HP - 30.0f);
    }
    if (bodypart == BODY_PART_LEFT_LEG || bodypart == BODY_PART_RIGHT_LEG) {
      SetPlayerHealth(playerid, HP - 30.0f);
    }
  }
  return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerWeaponShot(int playerid, int weaponid, int hittype, int hitid, float fX, float fY, float fZ) {
  unused(playerid, fX, fY, fZ);

  if (hittype == BULLET_HIT_TYPE_VEHICLE) {
    float HP;
    GetVehicleHealth(hitid, &HP);

    if (weaponid == WEAPON_COLT45) {
      SetVehicleHealth(hitid, HP - 35.0f);
    }
    if (weaponid == WEAPON_SILENCED) {
      SetVehicleHealth(hitid, HP - 38.0f);
    }
    if (weaponid == WEAPON_DEAGLE) {
      SetVehicleHealth(hitid, HP - 45.0f);
    }
    if (weaponid == WEAPON_SHOTGUN) {
      SetVehicleHealth(hitid, HP - 100.0f);
    }
    if (weaponid == WEAPON_SAWEDOFF) {
      SetVehicleHealth(hitid, HP - 130.0f);
    }
    if (weaponid == WEAPON_SHOTGSPA) {
      SetVehicleHealth(hitid, HP - 180.0f);
    }
    if (weaponid == WEAPON_TEC9) {
      SetVehicleHealth(hitid, HP - 40.0f);
    }
    if (weaponid == WEAPON_UZI) {
      SetVehicleHealth(hitid, HP - 48.0f);
    }
    if (weaponid == WEAPON_MP5) {
      SetVehicleHealth(hitid, HP - 53.0f);
    }
    if (weaponid == WEAPON_AK47) {
      SetVehicleHealth(hitid, HP - 68.0f);
    }
    if (weaponid == WEAPON_M4) {
      SetVehicleHealth(hitid, HP - 70.0f);
    }
    if (weaponid == WEAPON_RIFLE) {
      SetVehicleHealth(hitid, HP - 80.0f);
    }
    if (weaponid == WEAPON_SNIPER) {
      SetVehicleHealth(hitid, HP - 95.0f);
    }
    if (weaponid == WEAPON_BRASSKNUCKLE) {
      SetVehicleHealth(hitid, HP - 8.0f);
    }
    if (weaponid == WEAPON_ROCKETLAUNCHER) {
      SetVehicleHealth(hitid, HP - 1000.0f);
    }
    if (weaponid == WEAPON_MINIGUN) {
      SetVehicleHealth(hitid, HP - 10.0f);
    }
    if (weaponid == WEAPON_GOLFCLUB) {
      SetVehicleHealth(hitid, HP - 12.0f);
    }
    if (weaponid == WEAPON_SHOVEL) {
      SetVehicleHealth(hitid, HP - 12.0f);
    }
    if (weaponid == WEAPON_NITESTICK) {
      SetVehicleHealth(hitid, HP - 10.0f);
    }
    if (weaponid == WEAPON_BAT) {
      SetVehicleHealth(hitid, HP - 12.0f);
    }
    if (weaponid == WEAPON_HEATSEEKER) {
      SetVehicleHealth(hitid, HP - 1000.0f);
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
