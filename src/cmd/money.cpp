#include "cmd.hpp"

CMD(money) {
  int amount = 0;

  if (!IsPlayerAdmin(playerid))
    return false;
  if (sscanf(cmd.args, "%d", &amount) == 1)
    GivePlayerMoney(playerid, amount);
  else
    return SendClientMessage(playerid, -1, "Usage: /money <amount>");
  
  return true;
}
