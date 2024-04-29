#include "cmd.hpp"

#include "player.hpp"

bool cmd_inventory(int playerid, Cmd cmd) {
  char *inv = GetInvText(playerid);
  ShowPlayerDialog(playerid, DIALOG_INVENTORY, DIALOG_STYLE_TABLIST_HEADERS, "Inventory", inv, "Select", "Close");
  free(inv);
  return true;
}
