#include "cmd.hpp"

#include "player.hpp"

bool cmd_noadmin(int playerid, Cmd cmd) {
  int idx;

  if (!IsPlayerAdmin(playerid))
    return false;
  if (sscanf(cmd.args, "%d", &idx) == 1) {
    if (!IsPlayerConnected(playerid))
      return SendClientMessage(playerid, COLOR_ERROR, "Error: Player is not connected");
    if (!Player[idx].Flag.Admin)
      return SendClientMessage(playerid, COLOR_ERROR, "Error: Player is not an admin");
    
    Player[idx].Flag.Admin = false;
    SendClientMessage(idx, -1, "{00FF00}[ADMIN PROMOTE]:{FFFFFF} Your admin role has been removed by RCON Administrator");
    return true;
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /noadmin <playerid>");
}
