#include "cmd.hpp"

#include "player.hpp"

bool cmd_beadmin(int playerid, Cmd cmd) {
  int idx;

  if (!IsPlayerAdmin(playerid))
    return false;
  
  if (sscanf(cmd.args, "%d", &idx) == 1) {
    if (!IsPlayerConnected(playerid))
      return SendClientMessage(playerid, COLOR_ERROR, "Error: Player is not connected");
    if (Player[idx].Flag.Admin)
      return SendClientMessage(playerid, COLOR_ERROR, "Error: Player is an admin");
    
    Player[idx].Flag.Admin = true;
    SendClientMessage(idx, -1, "{00FF00}[ADMIN PROMOTE]:{FFFFFF} You have been promoted as an Administrator by RCON Administrator");
    return true;
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /beadmin <playerid>");
}
