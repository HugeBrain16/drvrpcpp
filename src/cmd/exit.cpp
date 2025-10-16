#include "cmd.hpp"

#include "property.hpp"

CMD(exit) {
  unused(cmd);

  if (GetPlayerInterior(playerid) == 0 && GetPlayerVirtualWorld(playerid) == 0)
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not inside a building!");

  for (int i = 0; i < MAX_BUSINESSES; i++) {
    if (GetPlayerVirtualWorld(playerid) == Stores[i].World) {
      SetPlayerInterior(playerid, 0);
      SetPlayerVirtualWorld(playerid, 0);
      SetPlayerPos(playerid, Stores[i].Pos[0], Stores[i].Pos[1], Stores[i].Pos[2]);
      return true;
    }
  }

  for (int i = 0; i < MAX_HOUSES; i++) {
    if (GetPlayerVirtualWorld(playerid) == Houses[i].World) {
      if (Houses[i].Locked)
        return SendClientMessage(playerid, COLOR_ERROR, "ERROR: The door is locked!");
      
      SetPlayerInterior(playerid, 0);
      SetPlayerVirtualWorld(playerid, 0);
      SetPlayerPos(playerid, Houses[i].Pos[0], Houses[i].Pos[1], Houses[i].Pos[2]);
      return true;
    }
  }

  return true;
}
