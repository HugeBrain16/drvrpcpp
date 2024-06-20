#include "cmd.hpp"

#include "player.hpp"

bool cmd_mower(int playerid, Cmd cmd) {
  unused(cmd);

  for (size_t i = 0; i < Mower.size(); i++) {
    if (IsPlayerInVehicle(playerid, Mower[i])) {
      if (Player[playerid].Mission[MMower].active)
        return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're already in mower mission!");
      
      Player[playerid].Mission[MMower].active = true;
      SetPlayerCheckpoint(playerid, 778.5635f, -1295.9993f, 13.5641f, 2.0);
      SendClientMessage(playerid, COLOR_INFO, "Follow checkpoints to complete the job!");
      return true;
    }
  }

  return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not in mower!");
}
