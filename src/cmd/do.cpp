#include "cmd.hpp"

#include "player.hpp"
#include "utils.hpp"

CMD(cmd_do) {
  char txt[256], msg[512];
  const char *name = RetPname(playerid);

  if (sscanf(cmd.args, "%s", txt) == 1) {
    sprintf(msg, "{D6A4D9}* %s (( %s ))", txt, name);
    ProxMsg(30.0, playerid, msg, -1);
    return true;
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /do [text]");
}
