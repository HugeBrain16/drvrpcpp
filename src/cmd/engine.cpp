#include "cmd.hpp"

#include "player.hpp"

bool cmd_engine(int playerid, Cmd cmd) {
  int vehicleid = GetPlayerVehicleID(playerid);
  int vid, engine, lights, alarm, doors, bonnet, boot, objective;
  float health;

  if (!IsPlayerInAnyVehicle(playerid))
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not in a vehicle!");
  if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: You're not the driver of this vehicle!");
  if (HasNoEngine(GetPlayerVehicleID(playerid)))
    return SendClientMessage(playerid, COLOR_ERROR, "ERROR: This vehicle has no engine!");
  
  for (int i = 0; i < MAX_PLAYER_VEHICLE; i++) {
    if (Player[playerid].Vehicle[i].ID == vehicleid) {
      vid = i;
      break;
    }
  }

  GetVehicleHealth(vehicleid, &health);
  GetVehicleParamsEx(vehicleid, &engine, &lights, &alarm, &doors, &bonnet, &boot, &objective);
  if (!engine) {
    if (IsPlayerInVehicle(playerid, Player[playerid].Vehicle[vid].ID) && Player[playerid].Vehicle[vid].Oil <= 0)
      return SendClientMessage(playerid, COLOR_ERROR, "ERROR: This vehicle has no oil!");
    if (IsPlayerInVehicle(playerid, Player[playerid].Vehicle[vid].ID) && Player[playerid].Vehicle[vid].Fuel <= 0)
      return SendClientMessage(playerid, COLOR_ERROR, "ERROR: This vehicle has no fuel!");
    if (IsPlayerInVehicle(playerid, Player[playerid].Vehicle[vid].ID) && Player[playerid].Vehicle[vid].Battery <= 0)
      return SendClientMessage(playerid, COLOR_ERROR, "ERROR: This vehicle's battery is dead!");
    
    SetVehicleParamsEx(vehicleid, 1, lights, alarm, doors, bonnet, boot, objective);
  } else {
    SetVehicleParamsEx(vehicleid, 0, lights, alarm, doors, bonnet, boot, objective);
  }
  if (engine == VEHICLE_PARAMS_UNSET) {
    if (IsPlayerInVehicle(playerid, Player[playerid].Vehicle[vid].ID) && Player[playerid].Vehicle[vid].Oil <= 0)
      return SendClientMessage(playerid, COLOR_ERROR, "ERROR: This vehicle has no oil!");
    if (IsPlayerInVehicle(playerid, Player[playerid].Vehicle[vid].ID) && Player[playerid].Vehicle[vid].Fuel <= 0)
      return SendClientMessage(playerid, COLOR_ERROR, "ERROR: This vehicle has no fuel!");
    if (IsPlayerInVehicle(playerid, Player[playerid].Vehicle[vid].ID) && Player[playerid].Vehicle[vid].Battery <= 0)
      return SendClientMessage(playerid, COLOR_ERROR, "ERROR: This vehicle's battery is dead!");
    
    SetVehicleParamsEx(vehicleid, 1, lights, alarm, doors, bonnet, boot, objective);
  }
  return true;
}
