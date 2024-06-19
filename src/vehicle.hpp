#ifndef VEHICLE_HPP_INCLUDED
#define VEHICLE_HPP_INCLUDED

#include <array>

#include "lib/sampgdk.h"

#include "limits.hpp"

#define VTYPE_CAR 1
#define VTYPE_HEAVY 2
#define VTYPE_MONSTER 3
#define VTYPE_BIKE 4
#define VTYPE_QUAD 5
#define VTYPE_BMX 6
#define VTYPE_HELI 7
#define VTYPE_PLANE 8
#define VTYPE_SEA 9
#define VTYPE_TRAILER 10
#define VTYPE_TRAIN 11
#define VTYPE_BOAT VTYPE_SEA
#define VTYPE_BICYCLE VTYPE_BMX

struct T_VDamageStatus {
  int Panel;
  int Door;
  int Light;
  int Tire;
};

struct T_Vehicle {
  int ID;
  char Owner[MAX_PLAYER_NAME];
  int Color[2];
  int Model;
  char Plate[10];
  float ParkPos[4];
  float LastPos[4];
  float Fuel;
  bool Lock;
  T_VDamageStatus DamageStatus;
  int UpgradeWheel;
  int UpgradeSpoiler;
  int UpgradeRoof;
  int UpgradeExhaust;
  int UpgradeHood;
  int UpgradeFB;
  int UpgradeRB;
  int UpgradeLamp;
  int UpgradeSkirt;
  int UpgradeNitro;
  int UpgradeH;
  int UpgradeLV;
  int UpgradeRV;
  int Paintjob;
  int Radiator;
  float RadiatorHealth;
  float Heat;
  float Oil;
  float Battery;
  float lastSpeed;
};

struct JOBV_BUS {
  char Owner[MAX_PLAYER_NAME];
  int ID;
};

struct T_Rent {
  int ID;
  char Owner[32];
  int RentTime;
  bool Rented;
  bool Locked;
};

extern std::array<int, 5> Sweeper;
extern std::array<JOBV_BUS, 7> Bus;
extern std::array<int, 4> Mower;
extern std::array<T_Rent, MAX_RENTVEH> VehicleRent;
extern std::array<T_Vehicle, MAX_VEHICLES> StaticVehicle;

bool IsPlayerInSweeper(int playerid);
bool IsVehicleConnected(int vehicleid);
bool IsPlayerDriver(int playerid);
int GetVehicleDriverID(int vehicleid);
bool IsVehicleInUse(int vehicleid);
int GetVehicleType(int vehicleid);
bool HasNoEngine(int vehicleid);
bool IsPlayerInCar(int playerid);
bool IsPlayerInTruck(int playerid);
bool IsPlayerOnBike(int playerid);
bool IsPlayerOnQuad(int playerid);
void SetSweeperToRespawn();
void SetSweeperToRespawn2(int delay);
int SpawnStaticVehicle(int modelid, float x, float y, float z, float r, int color1, int color2, int respawnDelay, bool siren);

#endif
