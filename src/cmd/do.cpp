#include "cmd.hpp"

#include "player.hpp"
#include "utils.hpp"

bool cmd_do(int playerid, Cmd cmd) {
  char txt[128];
  const char *name = RetPname(playerid);

  if (sscanf(cmd.args, "%s", txt) == 1) {
    sprintf(txt, "{D6A4D9}* %s (( %s ))", txt, name);
    ProxMsg(30.0, playerid, txt, -1);
    return true;
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /do [text]");
}
