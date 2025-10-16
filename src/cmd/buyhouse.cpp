#include "cmd.hpp"

#include "lib/streamer.hpp"

#include "property.hpp"
#include "player.hpp"

using namespace Plugins::Streamer;

CMD(buyhouse) {
  unused(cmd);

  char buff[128];
  const char *name = RetPname(playerid);

  for (int i = 0; i < MAX_HOUSES; i++) {
    if (IsPlayerInRangeOfPoint(playerid, 1.5, Houses[i].Pos[0], Houses[i].Pos[1], Houses[i].Pos[2])) {
      if (GetPlayerMoney(playerid) < Houses[i].Price)
        return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Not enough money!");
      if (strcmp(Houses[i].Owner, "None"))
        return SendClientMessage(playerid, COLOR_ERROR, "ERROR: This house is owned!");

      GivePlayerMoney(playerid, -Houses[i].Price);
      strcpy(Houses[i].Owner, name);
      sprintf(buff, "{AAAAAA}[ID:%d]\n{FFFFFF}Owner: {FF0000}%s", i, Houses[i].Owner);
      TextLabel::UpdateText(Houses[i].Label, 0xFFFFFFAA, buff);
      SaveHouse(i);
      return SendClientMessage(playerid, COLOR_INFO, "House bought!");
    }
  }

  return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not near by any houses!");
}
