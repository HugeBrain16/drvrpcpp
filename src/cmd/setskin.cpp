#include "cmd.hpp"

bool cmd_setskin(int playerid, Cmd cmd) {
  int skinid = 0;
  
  if (!IsPlayerAdmin(playerid))
    return false;
  
  if (sscanf(cmd.args, "%d", &skinid) == 1)
    SetPlayerSkin(playerid, skinid);
  else
    return SendClientMessage(playerid, -1, "Usage: /setskin <skinid>");
  
  return true;
}
