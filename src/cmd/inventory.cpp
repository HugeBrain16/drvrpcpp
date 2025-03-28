#include "cmd.hpp"

#include "player.hpp"

bool cmd_inventory(int playerid, Cmd cmd) {
  unused(cmd);

  char title[16];
  char *inv = GetInvText(playerid);
  sprintf(title, "Inventory (%d/%d)", GetItemCount(playerid), MAX_PLAYER_INVENTORY);
  ShowPlayerDialog(playerid, DIALOG_INVENTORY, DIALOG_STYLE_TABLIST_HEADERS, title, inv, "Select", "Close");
  free(inv);
  return true;
}
