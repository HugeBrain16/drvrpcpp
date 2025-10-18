#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

#include <cstdio>

#include "lib/grand.h"

#include "player.hpp"

extern GRand Random;

template <typename... Args> inline void unused(Args&&...) {}

enum VehicleParam {
  VEH_PARAM_ENGINE,
  VEH_PARAM_LIGHTS,
  VEH_PARAM_ALARM,
  VEH_PARAM_DOORS,
  VEH_PARAM_HOOD,
  VEH_PARAM_TRUNK,
  VEH_PARAM_OBJECTIVE
};

enum WeaponSlot {
  WEAPON_SLOT_UNKNOWN = -1,
  WEAPON_SLOT_UNARMED,
  WEAPON_SLOT_MELEE,
  WEAPON_SLOT_PISTOL,
  WEAPON_SLOT_SHOTGUN,
  WEAPON_SLOT_MACHINE_GUN,
  WEAPON_SLOT_ASSAULT_RIFLE,
  WEAPON_SLOT_LONG_RIFLE,
  WEAPON_SLOT_ARTILLERY,
  WEAPON_SLOT_EXPLOSIVES,
  WEAPON_SLOT_EQUIPMENT,
  WEAPON_SLOT_GIFT,
  WEAPON_SLOT_GADGET,
  WEAPON_SLOT_DETONATOR
};

bool fexist(const char *fname);
bool canint(const char *str);
int ftouch(const char *fname);
bool to_bool(const char *string);
void ProxMsg(float radius, int playerid, const char *string, int color);
void ProxSFX(float radius, int playerid, int soundid);
int GetCheckpointCount(E_PlayerMission mission);
int GetPlayerPointCount(E_PlayerPoint point);
float GetVehicleSpeed(int vehicleid);
void SetVehicleSpeed(int vehicleid, float speed);
void SetPlayerNextCheckpoint(int playerid, float x, float y, float z, float r);
void FreezePlayer(int playerid, int time);
float RetVehicleHealth(int vehicleid);
void CancelPlayer(int playerid);
bool GetVehicleParam(int vehicleid, VehicleParam param);
bool SetVehicleParam(int vehicleid, VehicleParam param, int state);
bool ToggleVehicleParam(int vehicleid, VehicleParam param);
bool GetEngineState(int vehicleid);
bool GetLightsState(int vehicleid);
bool GetHoodState(int vehicleid);
bool GetTrunkState(int vehicleid);
bool SetEngineState(int vehicleid, int state);
bool SetLightsState(int vehicleid, int state);
bool SetHoodState(int vehicleid, int state);
bool SetTrunkState(int vehicleid, int state);
int GetNearestVehicle(int playerid, float maxdist = 10.0f);
const char *GenerateNumberPlate();
bool ApplyPlayerAnimation(int playerid, const char *animlib, const char *animname, float fDelta, bool loop, bool lockx, bool locky, bool freeze, int time, bool forcesync = false);
bool GiveWeaponWithAnim(int playerid, int weaponid, int ammo, const char *animlib, const char *animname, const char *animcrouchname);
void HolsterWeapon(int playerid, T_ItemSlot *slot);
void UnloadHolstered(int playerid);
void HolsterEquipped(int playerid);
float GetPlayerDistanceFromPlayer(int playerid, int targetid);
bool PlayersInSameUniverse(int playerid, int targetid);
void GiveReset(int playerid);

#endif
