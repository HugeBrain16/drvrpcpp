#include "cmd.hpp"

#include "lib/streamer.hpp"

#include "property.hpp"
#include "utils.hpp"

using namespace Plugins::Streamer;

bool cmd_createhouse(int playerid, Cmd cmd) {
  int price;
  float pos[3];
  char buff[256];

  GetPlayerPos(playerid, &pos[0], &pos[1], &pos[2]);
  if (sscanf(cmd.args, "%i", &price) == 1) {
    for (int i = 0; i < MAX_HOUSE; i++) {
      sprintf(buff, BD_HOUSE, i);
      if (!fexist(buff)) {
        strcpy(Houses[i].Owner, "None");
        Houses[i].Pos[0] = pos[0];
        Houses[i].Pos[1] = pos[1];
        Houses[i].Pos[2] = pos[2];
        Houses[i].Price = price;
        Houses[i].World = WORLD_HOUSE + i;

        sprintf(buff, "{AAAAAA}[ID:%d]\n{008000}For Sale $%d", i, price);
        Houses[i].Label = TextLabel::Create(buff, 0xFFFFFFAA, pos[0], pos[1], pos[2], 10.0);
        Houses[i].Pickup = Pickup::Create(1273, 0, pos[0], pos[1], pos[2]);
        SaveHouse(i);
        return SendClientMessage(playerid, COLOR_INFO, "House created!");
      }
    }

    return SendClientMessage(playerid, COLOR_ERROR, "Error: Can't create anymore house!");
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /createhouse [price]");
}
