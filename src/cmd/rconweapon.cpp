#include "cmd.hpp"

bool cmd_rconweapon(int playerid, Cmd cmd) {
  unused(cmd);

  if (!IsPlayerAdmin(playerid))
    return false;
  GivePlayerWeapon(playerid, WEAPON_MP5, 64);
  GivePlayerWeapon(playerid, WEAPON_M4, 64);
  return true;
}
