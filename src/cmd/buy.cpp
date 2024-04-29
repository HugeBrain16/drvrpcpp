#include "cmd.hpp"

#include "property.hpp"

bool cmd_buy(int playerid, Cmd cmd) {
  char invText[256];
  char buff[64];
  invText[0] = '\0';

  strcat(invText, "Name\tPrice\tStock\n");

  for (int i = 0; i < MAX_BUSINESS; i++) {
    if (GetPlayerVirtualWorld(playerid) == Stores[i].World) {
      for (int t = 0; t < GetBizItemCount("store", i); t++) {
        if (Stores[i].Items[t].Quant > 0)
          sprintf(buff, "%s\t$%d\t%dx\n", Stores[i].Items[t].Item.Name, Stores[i].Items[t].Item.Price, Stores[i].Items[t].Quant);
        strcat(invText, buff);
      }

      ShowPlayerDialog(playerid, DIALOG_STORE, DIALOG_STYLE_TABLIST_HEADERS, "Store", invText, "Buy", "Close");
      return true;
    }
  }

  return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not inside any business!");
}
