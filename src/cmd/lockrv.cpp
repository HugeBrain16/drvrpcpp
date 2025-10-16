#include "cmd.hpp"

#include "player.hpp"
#include "utils.hpp"

CMD(lockrv) {
  unused(cmd);

  float pos[3];
  const char *name = RetPname(playerid);

  for (int i = 0; i < MAX_RENTVEH; i++) {
    GetVehiclePos(VehicleRent[i].ID, &pos[0], &pos[1], &pos[2]);

    if (!strcmp(VehicleRent[i].Owner, name) &&
        IsPlayerInRangeOfPoint(playerid, 5.0, pos[0], pos[1], pos[2])) {
      VehicleRent[i].Locked = !VehicleRent[i].Locked;
      if (VehicleRent[i].Locked)
        GameTextForPlayer(playerid, "~r~Locked", 1000, 5);
      else
        GameTextForPlayer(playerid, "~g~Unlocked", 1000, 5);
      ProxSFX(10.0, playerid, 24600);
      return true;
    }
  }
  SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not near by your rented vehicle!");
  return true;
}
