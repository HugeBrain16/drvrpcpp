#include "cmd.hpp"

#include "lib/streamer.hpp"

#include "property.hpp"
#include "utils.hpp"

using namespace Plugins::Streamer;

bool cmd_createbiz(int playerid, Cmd cmd) {
  char opt[16];
  int price;
  float pos[3];
  char buff[256];

  GetPlayerPos(playerid, &pos[0], &pos[1], &pos[2]);
  if (sscanf(cmd.args, "%s %i", opt, &price) == 2) {
    if (!strcmp(opt, "store")) {
      for (int i = 0; i < MAX_BUSINESS; i++) {
        sprintf(buff, BIZ_STORE, i);
        if (!fexist(buff)) {
          strcpy(Stores[i].Owner, "None");
          strcpy(Stores[i].Name, "None");
          Stores[i].Pos[0] = pos[0];
          Stores[i].Pos[1] = pos[1];
          Stores[i].Pos[2] = pos[2];
          Stores[i].Price = price;
          Stores[i].World = WORLD_BUSINESS + i;

          sprintf(buff, "{AAAAAA}[ID:%d]\n{FF0000}Convenience Store\n{008000}For Sale $%d", i, price);
          Stores[i].Pickup = Pickup::Create(1274, 0, pos[0], pos[1], pos[2]);
          Stores[i].Label = TextLabel::Create(buff, 0xFFFFFFAA, pos[0], pos[1], pos[2], 10.0);
          RestockStoreBiz(i);
          SaveBusiness("store", i);
          return SendClientMessage(playerid, COLOR_INFO, "Business created!");
        }
      }

      return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Can't create anymore business!");
    } else return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Unknown business type!");
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /createbiz [type] [price]");
}
