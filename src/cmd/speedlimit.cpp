#include "cmd.hpp"

#include "player.hpp"

CMD(speedlimit) {
  float speed;

  if (sscanf(cmd.args, "%f", &speed) == 1) {
    Player[playerid].DataState.speedlimit = speed;
    return SendClientMessage(playerid, COLOR_INFO, "Speed limit set!");
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /speedlimit [speed(kmh)]");
}
