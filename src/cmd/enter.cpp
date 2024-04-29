#include "cmd.hpp"

#include "property.hpp"

bool cmd_enter(int playerid, Cmd cmd) {
  if (IsPlayerInAnyVehicle(playerid))
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Can't enter while in vehicle!");
  if (GetPlayerInterior(playerid) != 0 && GetPlayerVirtualWorld(playerid) != 0)
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're already inside a building!");

  for (int i = 0; i < MAX_BUSINESS; i++) {
    if (IsPlayerInRangeOfPoint(playerid, 1.5, Stores[i].Pos[0], Stores[i].Pos[1], Stores[i].Pos[2])) {
      SetPlayerInterior(playerid, 6);
      SetPlayerVirtualWorld(playerid, Stores[i].World);
      SetPlayerPos(playerid, -26.83, -55.58, 1003.54);
      return true;
    }
  }
  for (int i = 0; i < MAX_HOUSE; i++) {
    if (IsPlayerInRangeOfPoint(playerid, 1.5, Houses[i].Pos[0], Houses[i].Pos[1], Houses[i].Pos[2])) {
      if (Houses[i].Locked)
        return SendClientMessage(playerid, COLOR_ERROR, "ERROR: The door is locked!");
      
      SetPlayerInterior(playerid, 1);
      SetPlayerVirtualWorld(playerid, Houses[i].World);
      SetPlayerPos(playerid, 224.28, 1289.19, 1082.14);
      return true;
    }
  }
  return true;
}
