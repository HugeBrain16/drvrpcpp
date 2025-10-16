#include "cmd.hpp"

#include "player.hpp"

CMD(behelper) {
  int idx;
  char msg[128];
  const char *name = RetPname(playerid);

  if (!IsPlayerAdmin2(playerid))
    return false;
  
  if (sscanf(cmd.args, "%d", &idx) == 1) {
    if (!IsPlayerConnected(idx))
      return SendClientMessage(playerid, COLOR_ERROR, "Error: Player is not connected");
    if (IsPlayerHelper(idx))
      return SendClientMessage(playerid, COLOR_ERROR, "Error: Player is a helper");
    
    Player[idx].Flag.Helper = true;
    sprintf(msg, "{00FF00}[ADMIN PROMOTE]:{FFFFFF} You have been promoted as a Helper by Admin: %s", name);
    SendClientMessage(idx, -1, msg);
    return true;
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /behelper <playerid>");
}

