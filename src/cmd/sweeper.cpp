#include "cmd.hpp"

#include "player.hpp"

bool cmd_sweeper(int playerid, Cmd cmd) {
  unused(cmd);

  if (!IsPlayerInSweeper(playerid))
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not in any sweeper");
  if (Player[playerid].Mission[MSweeper].active)
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're already in this mission");
  
  Player[playerid].Mission[MSweeper].active = true;
  SetPlayerCheckpoint(playerid, 1324.2096f, -864.3158f, 39.5781f, 8.0);
  SendClientMessage(playerid, COLOR_INFO, "Follow the checkpoints to complete the job!");
  return true;
}
