#include "cmd.hpp"

#include "player.hpp"

bool cmd_tmt(int playerid, Cmd cmd) {
  unused(cmd);

  if (!IsPlayerAdmin(playerid))
    return false;
  
  Player[playerid].Flag.MapTP = !Player[playerid].Flag.MapTP;
  Player[playerid].Flag.MapTP ? SendClientMessage(playerid, -1, "Toggle On") : SendClientMessage(playerid, -1, "Toggle Off");
  return true;
}
