#include "cmd.hpp"

#include "utils.hpp"

bool cmd_ssveh(int playerid, Cmd cmd) { 
  int id;
  float pos[3];

  if (!IsPlayerAdmin(playerid))
    return false;

  if (sscanf(cmd.args, "%d", &id) == 1) {
    GetPlayerPos(playerid, &pos[0], &pos[1], &pos[2]);
    SpawnStaticVehicle(id, pos[0], pos[1], pos[2], 0, Random.i(256), Random.i(256), -1, false);
    return SendClientMessage(playerid, COLOR_INFO, "Static vehicle spawned!");
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /ssveh [modelid]");
}
