#include "cmd.hpp"

#include "player.hpp"

CMD(beadmin) {
  int idx;
  char msg[128];
  const char *name = RetPname(playerid);

  if (!IsPlayerAdmin(playerid))
    return false;
  
  if (sscanf(cmd.args, "%d", &idx) == 1) {
    if (!IsPlayerConnected(idx))
      return SendClientMessage(playerid, COLOR_ERROR, "Error: Player is not connected");
    if (Player[idx].Flag.Admin)
      return SendClientMessage(playerid, COLOR_ERROR, "Error: Player is an admin");
    
    Player[idx].Flag.Admin = true;
    sprintf(msg, "{00FF00}[ADMIN PROMOTE]:{FFFFFF} You have been promoted as an Administrator by Admin: %s", name);
    SendClientMessage(idx, -1, msg);
    return true;
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /beadmin <playerid>");
}
