#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

#include <cstdio>

#include "lib/grand.h"

#include "player.hpp"

extern GRand Random;

template <typename... Args> inline void unused(Args&&...) {}

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
bool EngineOn(int vehicleid);
bool SetEngine(int vehicleid, int engine);
const char *GenerateNumberPlate();

#endif
