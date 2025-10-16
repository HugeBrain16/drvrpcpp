#include "cmd.hpp"

#include "player.hpp"
#include "property.hpp"

CMD(lockhouse) {
  unused(cmd);

  const char *name = RetPname(playerid);

  for (int i = 0; i < MAX_HOUSES; i++) {
    if (GetPlayerVirtualWorld(playerid) == 0) {
      if (IsPlayerInRangeOfPoint(playerid, 1.5, Houses[i].Pos[0], Houses[i].Pos[1], Houses[i].Pos[2]) && !strcmp(Houses[i].Owner, name)) {
        Houses[i].Locked = !Houses[i].Locked;

        if (Houses[i].Locked)
          SendClientMessage(playerid, COLOR_INFO, "The door has been locked!");
        else
          SendClientMessage(playerid, COLOR_INFO, "The door has been unlocked!");

        return true;
      }
    } else if (GetPlayerVirtualWorld(playerid) == Houses[i].World && !strcmp(Houses[i].Owner, name)) {
      Houses[i].Locked = !Houses[i].Locked;

      if (Houses[i].Locked)
        SendClientMessage(playerid, COLOR_INFO, "The door has been locked!");
      else
        SendClientMessage(playerid, COLOR_INFO, "The door has been unlocked!");

      return true;
    }
  }

  return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not nearby or inside of your house!");
}
