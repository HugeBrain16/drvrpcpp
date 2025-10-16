#include "cmd.hpp"

#include "player.hpp"

CMD(nohelper) {
  int idx;
  char msg[128];
  const char *name = RetPname(playerid);

  if (!IsPlayerAdmin2(playerid))
    return false;
  
  if (sscanf(cmd.args, "%d", &idx) == 1) {
    if (!IsPlayerConnected(idx))
      return SendClientMessage(playerid, COLOR_ERROR, "Error: Player is not connected");
    if (!IsPlayerHelper(idx))
      return SendClientMessage(playerid, COLOR_ERROR, "Error: Player is not a helper");
    
    Player[idx].Flag.Helper = false;
    sprintf(msg, "{00FF00}[HELPER PROMOTE]:{FFFFFF} Your Helper role has been revoked by Admin: %s", name);
    SendClientMessage(idx, -1, msg);
    return true;
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /nohelper <playerid>");
}

