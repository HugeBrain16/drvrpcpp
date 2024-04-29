#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED

#include <array>

#include "lib/sampgdk.h"

#include "globals.hpp"
#include "limits.hpp"
#include "item.hpp"
#include "vehicle.hpp"

struct T_PlayerStatus;
struct T_DataState;
struct T_PlayerFlag;
struct T_PlayerJob;
struct T_PlayerMission;
struct T_PlayerCheckpoint;
struct T_PlayerVehicleIndicator;
struct T_Player;

enum E_PlayerJob { Gunmaker, Trucker, Mechanic, Taxi, E_PlayerJob_COUNT };
enum E_PlayerMission { MSweeper, MMower, MMaterial, E_PlayerMission_COUNT };
enum E_PlayerPoint { PPRent, E_PlayerPoint_COUNT };

struct T_PlayerStatus {
  float hunger;
  float thirst;
  float energy;
};

struct T_DataState {
  int housetake = -1;
  int housestore = -1;
  float speedlimit = -1;
};

struct T_PlayerFlag {
  bool MapTP;
  bool Admin;
  bool Helper;
  bool NewAccount;
  bool FirstSpawn;
  bool Dead;
};

struct T_PlayerJob {
  bool joined;
  bool onDuty;
};

struct T_PlayerMission {
  bool active;
};

struct T_PlayerCheckpoint {
  int progress;
};

struct T_PlayerVehicleIndicator {
  int MainBox;
  int Speed;
  int Health;
  int Fuel;
  int Heat;
};

struct T_Player {
  T_PlayerStatus Status;
  T_DataState DataState;
  T_PlayerFlag Flag;
  T_PlayerVehicleIndicator VehicleIndicator;
  std::array<T_ItemSlot, 8> Inventory;
  std::array<T_Vehicle, MAX_PLAYER_VEHICLE> Vehicle;
  std::array<T_PlayerJob, E_PlayerJob_COUNT> Job;
  std::array<T_PlayerMission, E_PlayerMission_COUNT> Mission;
  std::array<T_PlayerCheckpoint, E_PlayerMission_COUNT> Checkpoint;
};

extern std::array<T_Player, MAX_PLAYERS> Player;

char *RetPname(int playerid, bool underscore = false);
bool PlayerHasJob(int playerid, E_PlayerJob job);
bool SaveInventory(int playerid);
bool LoadInventory(int playerid);
void ItemSlotInfo(int playerid, int target, int slot);
void EmptyItemSlot(int playerid, int slot);
void UpdateItem(int playerid);
void ResetItem(int playerid);
bool AddItem(int playerid, T_Item item, int quant);
int GetItemCount(int playerid);
T_Item GetEquipped(int playerid);
char *GetInvText(int playerid);
int GetHouseCount(int playerid);
bool HasHouse(int playerid);
int GetBizCount(const char *type, int playerid);
void ShowVehicleIndicator(int playerid);
void HideVehicleIndicator(int playerid);
void LoadPlayer(int playerid);
void SavePlayer(int playerid);

#endif
