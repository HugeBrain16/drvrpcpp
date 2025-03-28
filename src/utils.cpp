#include <string>
#include <sys/stat.h>
#include <fstream>
#include <cstring>
#include <cmath>

#include "lib/sampgdk.h"

#include "utils.hpp"
#include "globals.hpp"
#include "statics.hpp"

GRand Random;

bool fexist(const char *fname) {
  struct stat buffer;
  return (stat(fname, &buffer) == 0);
}

bool canint(const char *str) {
  bool result = false;
  std::string string(str);

  try {
    std::stoi(str);
    result = true;
  } catch (const std::exception &e) {}

  return result;
}

int ftouch(const char *fname) {
  std::fstream fs;
  fs.open(fname, std::ios::out);
  fs.close();
  return (int)!fexist(fname);
}

bool to_bool(const char *string) {
  return strcmp(string, "false") ? true : false;
}

void ProxMsg(float radius, int playerid, const char *string, int color) {
  float pos[3];

  GetPlayerPos(playerid, &pos[0], &pos[1], &pos[2]);
  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (IsPlayerInRangeOfPoint(i, radius, pos[0], pos[1], pos[2]))
      SendClientMessage(i, color, string);
  }
}

void ProxSFX(float radius, int playerid, int soundid) {
  float pos[3];

  GetPlayerPos(playerid, &pos[0], &pos[1], &pos[2]);
  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (IsPlayerInRangeOfPoint(i, radius, pos[0], pos[1], pos[2]))
      PlayerPlaySound(i, soundid, pos[0], pos[1], pos[2]);
  }
}

int GetCheckpointCount(E_PlayerMission mission) {
  int count = 0;

  for (int i = 0; i < MAX_MISSION_CHECKPOINTS; i++) {
    if (MissionCheckpoint[mission][i][0] != 0 ||
        MissionCheckpoint[mission][i][1] != 0 ||
        MissionCheckpoint[mission][i][2] != 0) {
      count++;
    }
  }

  return count;
}

int GetPlayerPointCount(E_PlayerPoint point) {
  int count = 0;

  for (int i = 0; i < MAX_PLAYER_POINTS; i++) {
    if (PlayerPoint[point][i][0] != 0 || PlayerPoint[point][i][1] != 0 ||
        PlayerPoint[point][i][2] != 0) {
      count++;
    }
  }

  return count;
}

float GetVehicleSpeed(int vehicleid) {
  float pos[3];

  if (GetVehicleVelocity(vehicleid, &pos[0], &pos[1], &pos[2])) {
    return static_cast<float>(sqrt((pos[0] * pos[0]) + (pos[1] * pos[1]) + (pos[2] * pos[2])) * 181.5);
  }

  return 0.0f;
}

void SetVehicleSpeed(int vehicleid, float speed) {
  float velocity = speed / 181.5f;

  float pos[3];
  if (GetVehicleVelocity(vehicleid, &pos[0], &pos[1], &pos[2])) {
    float magnitude = static_cast<float>(sqrt((pos[0] * pos[0]) + (pos[1] * pos[1]) + (pos[2] * pos[2])));
    float newVelocity = velocity / magnitude;

    SetVehicleVelocity(vehicleid, pos[0] * newVelocity, pos[1] * newVelocity, pos[2] * newVelocity);
  }
}

void SetPlayerNextCheckpoint(int playerid, float x, float y, float z, float r) {
  DisablePlayerCheckpoint(playerid);
  SetPlayerCheckpoint(playerid, x, y, z, r);
}

void SAMPGDK_CALL TC_FreezePlayer(int timerid, void *data) {
  unused(timerid);

  int playerid = (int)data;
  TogglePlayerControllable(playerid, true);
}

void FreezePlayer(int playerid, int time) {
  TogglePlayerControllable(playerid, false);
  SetTimer(time, false, TC_FreezePlayer, (void *)playerid);
}

float RetVehicleHealth(int vehicleid) {
  float health;
  GetVehicleHealth(vehicleid, &health);
  return health;
}

void CancelPlayer(int playerid) {
  float pos[3];

  GetPlayerPos(playerid, &pos[0], &pos[1], &pos[2]);
  SetPlayerPos(playerid, pos[0], pos[1], pos[2]);
}

bool EngineOn(int vehicleid) {
  int x[7];

  GetVehicleParamsEx(vehicleid, &x[0], &x[1], &x[2], &x[3], &x[4], &x[5], &x[6]);
  return x[0] && x[0] != VEHICLE_PARAMS_UNSET;
}

bool SetEngine(int vehicleid, int engine) {
  int x[7];
  GetVehicleParamsEx(vehicleid, &x[0], &x[1], &x[2], &x[3], &x[4], &x[5], &x[6]);
  return SetVehicleParamsEx(vehicleid, engine, x[1], x[2], x[3], x[4], x[5], x[6]);
}

const char *GenerateNumberPlate() {
  static char plate[10];
  const char *letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  for (int i = 0; i < 4; i++) {
    plate[i] = letters[Random.i(strlen(letters))];
  }
  plate[4] = ' ';

  for (int i = 5; i < 9; i++) {
    plate[i] = static_cast<char>('0' + Random.i(10));
  }
  plate[9] = '\0';

  return plate;
}

