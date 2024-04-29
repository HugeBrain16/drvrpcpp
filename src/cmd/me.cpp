#include "cmd.hpp"

#include "player.hpp"
#include "utils.hpp"

bool cmd_me(int playerid, Cmd cmd) {
  char txt[128];
  char *name;

  if (sscanf(cmd.args, "%s", txt) == 1) {
    name = RetPname(playerid);
    sprintf(txt, "{D6A4D9}* %s %s", name, txt);
    ProxMsg(30.0, playerid, txt, -1);
    free(name);
    return true;
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /me [text]");
}
