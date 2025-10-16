#include "cmd.hpp"

#include "player.hpp"

CMD(tmt) {
  unused(cmd);

  if (!IsPlayerAdmin(playerid))
    return false;
  
  Player[playerid].Flag.MapTP = !Player[playerid].Flag.MapTP;
  Player[playerid].Flag.MapTP ? SendClientMessage(playerid, -1, "Toggle On") : SendClientMessage(playerid, -1, "Toggle Off");
  return true;
}
