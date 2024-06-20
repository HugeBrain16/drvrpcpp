#include "cmd.hpp"

#include "player.hpp"

bool cmd_getmaterials(int playerid, Cmd cmd) {
  unused(cmd);

  if (!IsPlayerInRangeOfPoint(playerid, 1.5, 613.0717f, 1549.8906f, 5.0001f))
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not nearby the material point");
  if (Player[playerid].Mission[MMaterial].active)
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You haven't collected the previous materials yet!");
  if (GetPlayerMoney(playerid) < 250)
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Not enough money!");
  
  GivePlayerMoney(playerid, -250);
  Player[playerid].Mission[MMaterial].active = true;
  SetPlayerCheckpoint(playerid, 2508.7227f, -2118.3533f, 13.5469f, 8.0);
  return SendClientMessage(playerid, COLOR_INFO, "Go to the checkpoint to collect the materials");
}
