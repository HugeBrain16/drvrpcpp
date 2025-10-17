#include "cmd.hpp"

#include "player.hpp"
#include "utils.hpp"

CMD_ALIAS(engine, "e", "engine") {
  unused(cmd);

  int vehicleid = GetPlayerVehicleID(playerid);
  int pid{}, vid{};
  bool ispv;

  if (!IsPlayerInAnyVehicle(playerid))
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not in a vehicle!");
  if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not the driver of this vehicle!");
  if (HasNoEngine(GetPlayerVehicleID(playerid)))
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Vehicle has no engine!");
  
  for (int x = 0; x < MAX_PLAYERS; x++) {
    if (ispv)
      break;

    for (int y = 0; y < MAX_PLAYER_VEHICLES; y++) {
      if (Player[x].Vehicle[y].ID == vehicleid) {
        pid = x;
        vid = y;
        ispv = true;
        break;
      }
    }
  }

  if (!GetEngineState(vehicleid)) {
    if (RetVehicleHealth(vehicleid) <= 260)
      return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Engine is too damaged!");

    if (ispv) {
      if (IsPlayerInVehicle(playerid, Player[pid].Vehicle[vid].ID) && Player[pid].Vehicle[vid].Oil <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Vehicle has no oil!");
      if (IsPlayerInVehicle(playerid, Player[pid].Vehicle[vid].ID) && Player[pid].Vehicle[vid].Fuel <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Vehicle has no fuel!");
      if (IsPlayerInVehicle(playerid, Player[pid].Vehicle[vid].ID) && Player[pid].Vehicle[vid].Battery <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Vehicle's battery is dead!");
    }

    SetEngineState(vehicleid, 1);
    SetLightsState(vehicleid, 1);
    return true;
  } else {
    SetEngineState(vehicleid, 0);
    SetLightsState(vehicleid, 0);
    return true;
  }
}
