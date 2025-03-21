#include "cmd.hpp"

#include "property.hpp"
#include "player.hpp"

bool cmd_storage(int playerid, Cmd cmd) {
  char opt[8];
  int houseid = GetHouseID(playerid);
  const char *name = RetPname(playerid);

  if (houseid < 0)
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not inside a house");
  
  if (strcmp(Houses[houseid].Owner, name))
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not the owner of this house!");

  if (sscanf(cmd.args, "%s", opt) == 1) {
    if (!strcmp(opt, "view")) {
      char invText[2048];
      char buff[64];
      char title[16];
      invText[0] = '\0';

      sprintf(title, "Storage (%d/%d)", GetHouseItemCount(houseid), MAX_HOUSE_INVENTORY);
      strcat(invText, "Name\tQuantity\n");
      for (int i = 0; i < GetHouseItemCount(houseid); i++) {
        if (Houses[houseid].Items[i].Quant > 0) {
          sprintf(buff, "%s\t%dx\n", Houses[houseid].Items[i].Item.Name, Houses[houseid].Items[i].Quant);
          strcat(invText, buff);
        }
      }

      return ShowPlayerDialog(playerid, DIALOG_HOUSE_STORAGE_VIEW, DIALOG_STYLE_TABLIST_HEADERS, title, invText, "Close", "");
    } else if (!strcmp(opt, "store")) {
      char *invText = GetInvText(playerid);
      char title[25];

      sprintf(title, "Storage - Store - (%d/%d)", GetHouseItemCount(houseid), MAX_HOUSE_INVENTORY);
      ShowPlayerDialog(playerid, DIALOG_HOUSE_STORAGE_STORE, DIALOG_STYLE_TABLIST_HEADERS, title, invText, "Store", "Cancel");
      free(invText);
      return true;
    } else if (!strcmp(opt, "take")) {
      char invText[2048];
      char buff[64];
      char title[24];
      invText[0] = '\0';

      strcat(invText, "Name\tQuantity\n");
      for (int i = 0; i < GetHouseItemCount(houseid); i++) {
        if (Houses[houseid].Items[i].Quant > 0) {
          sprintf(buff, "%s\t%dx\n", Houses[houseid].Items[i].Item.Name, Houses[houseid].Items[i].Quant);
          strcat(invText, buff);
        }
      }

      sprintf(title, "Storage - Take - (%d/%d)", GetHouseItemCount(houseid), MAX_HOUSE_INVENTORY);
      return ShowPlayerDialog(playerid, DIALOG_HOUSE_STORAGE_TAKE, DIALOG_STYLE_TABLIST_HEADERS, title, invText, "Take", "Close");
    } else return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Unknown action!");
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /storage [view|store|take]");
}
