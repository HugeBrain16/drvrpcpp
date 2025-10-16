#include "cmd.hpp"

#include "player.hpp"

CMD(noadmin) {
  int idx;
  char msg[128];
  const char *name = RetPname(playerid);

  if (!IsPlayerAdmin(playerid))
    return false;
  
  if (sscanf(cmd.args, "%d", &idx) == 1) {
    if (!IsPlayerConnected(playerid))
      return SendClientMessage(playerid, COLOR_ERROR, "Error: Player is not connected");
    if (!Player[idx].Flag.Admin)
      return SendClientMessage(playerid, COLOR_ERROR, "Error: Player is not an admin");
    
    Player[idx].Flag.Admin = false;
    sprintf(msg, "{00FF00}[ADMIN PROMOTE]:{FFFFFF} Your Administrator role has been revoked by Admin: %s", name);
    SendClientMessage(idx, -1, msg);
    return true;
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /noadmin <playerid>");
}

