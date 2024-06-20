#include "cmd.hpp"

#include "player.hpp"

bool cmd_saveplayer(int playerid, Cmd cmd) {
  int target;

  if (!IsPlayerAdmin(playerid))
    return false;

  if (sscanf(cmd.args, "%d", &target) == 1) {
    if (!IsPlayerConnected(target))
      return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Player is not connected!");

    SavePlayer(target);
    SaveInventory(target);
    return SendClientMessage(playerid, COLOR_INFO, "Player saved!");
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /saveplayer [playerid]");
}
