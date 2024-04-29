#include "cmd.hpp"

#include "property.hpp"
#include "player.hpp"

bool cmd_storage(int playerid, Cmd cmd) {
  char opt[8];
  int houseid = GetHouseID(playerid);
  char *name = RetPname(playerid);

  if (houseid < 0)
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not inside a house");
  
  if (strcmp(Houses[houseid].Owner, name)) {
    free(name);
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not the owner of this house!");
  }
  free(name);

  if (sscanf(cmd.args, "%s", opt) == 1) {
    if (!strcmp(opt, "view")) {
      char invText[2048];
      char buff[64];
      invText[0] = '\0';

      strcat(invText, "Name\tQuantity\n");
      for (int i = 0; i < GetHouseItemCount(houseid); i++) {
        if (Houses[houseid].Items[i].Quant > 0) {
          sprintf(buff, "%s\t%dx\n", Houses[houseid].Items[i].Item.Name, Houses[houseid].Items[i].Quant);
          strcat(invText, buff);
        }
      }

      return ShowPlayerDialog(playerid, DIALOG_HOUSE_STORAGE_VIEW, DIALOG_STYLE_TABLIST_HEADERS, "Storage", invText, "Close", "");
    } else if (!strcmp(opt, "store")) {
      char *invText = GetInvText(playerid);
      ShowPlayerDialog(playerid, DIALOG_HOUSE_STORAGE_STORE, DIALOG_STYLE_TABLIST_HEADERS, "Storage - Store", invText, "Store", "Cancel");
      free(invText);
      return true;
    } else if (!strcmp(opt, "take")) {
      char invText[2048];
      char buff[64];
      invText[0] = '\0';

      strcat(invText, "Name\tQuantity\n");
      for (int i = 0; i < GetHouseItemCount(houseid); i++) {
        if (Houses[houseid].Items[i].Quant > 0) {
          sprintf(buff, "%s\t%dx\n", Houses[houseid].Items[i].Item.Name, Houses[houseid].Items[i].Quant);
          strcat(invText, buff);
        }
      }

      return ShowPlayerDialog(playerid, DIALOG_HOUSE_STORAGE_TAKE, DIALOG_STYLE_TABLIST_HEADERS, "Storage - Take", invText, "Take", "Close");
    } else return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Unknown action!");
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /storage [view|store|take]");
}
