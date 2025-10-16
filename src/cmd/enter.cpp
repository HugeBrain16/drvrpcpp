#include "cmd.hpp"

#include "property.hpp"

CMD(enter) {
  unused(cmd);

  if (IsPlayerInAnyVehicle(playerid))
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Can't enter while in vehicle!");
  if (GetPlayerInterior(playerid) != 0 && GetPlayerVirtualWorld(playerid) != 0)
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're already inside a building!");

  for (int i = 0; i < MAX_BUSINESSES; i++) {
    if (IsPlayerInRangeOfPoint(playerid, 1.5, Stores[i].Pos[0], Stores[i].Pos[1], Stores[i].Pos[2])) {
      SetPlayerInterior(playerid, 6);
      SetPlayerVirtualWorld(playerid, Stores[i].World);
      SetPlayerPos(playerid, -26.83f, -55.58f, 1003.54f);
      return true;
    }
  }
  for (int i = 0; i < MAX_HOUSES; i++) {
    if (IsPlayerInRangeOfPoint(playerid, 1.5, Houses[i].Pos[0], Houses[i].Pos[1], Houses[i].Pos[2])) {
      if (Houses[i].Locked)
        return SendClientMessage(playerid, COLOR_ERROR, "ERROR: The door is locked!");
      
      SetPlayerInterior(playerid, 1);
      SetPlayerVirtualWorld(playerid, Houses[i].World);
      SetPlayerPos(playerid, 224.28f, 1289.19f, 1082.14f);
      return true;
    }
  }
  return true;
}
