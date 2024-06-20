#include "cmd.hpp"

#include "player.hpp"
#include "utils.hpp"

bool cmd_me(int playerid, Cmd cmd) {
  char txt[256], msg[512];
  const char *name = RetPname(playerid);

  if (sscanf(cmd.args, "%s", txt) == 1) {
    sprintf(msg, "{D6A4D9}* %s %s", name, txt);
    ProxMsg(30.0, playerid, msg, -1);
    return true;
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /me [text]");
}
