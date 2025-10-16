#include "cmd.hpp"

#include "player.hpp"
#include "utils.hpp"
#include "statics.hpp"

CMD(rentveh) {
  unused(cmd);

  char buff[32];
  const char *name = RetPname(playerid);

  for (int i = 0; i < GetPlayerPointCount(PPRent); i++) {
    if (IsPlayerInRangeOfPoint(playerid, 1.5, PlayerPoint[PPRent][i][0], PlayerPoint[PPRent][i][1], PlayerPoint[PPRent][i][2])) {
      for (int v = 0; v < MAX_RENTVEH; v++) {
        if (!VehicleRent[i].Rented) {
          if (GetPlayerMoney(playerid) < 60)
            return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Not enough money!");

          GivePlayerMoney(playerid, -60);
          VehicleRent[i].ID = CreateVehicle(462, PlayerPoint[PPRent][i][0], PlayerPoint[PPRent][i][1], PlayerPoint[PPRent][i][2], 0, 1, 0, 0, false);
          sprintf(buff, "RENT-%d", i);
          SetVehicleNumberPlate(VehicleRent[i].ID, buff);
          PutPlayerInVehicle(playerid, VehicleRent[i].ID, 0);
          VehicleRent[i].RentTime = 30;
          strcpy(VehicleRent[i].Owner, name);
          VehicleRent[i].Rented = true;
          return SendClientMessage(playerid, COLOR_INFO, "Use /lockrv to lock the vehicle");
        }
      }
    }
  }
  return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not near any rent point!");
}
