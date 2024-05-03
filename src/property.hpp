#ifndef PROPERTY_HPP_INCLUDED
#define PROPERTY_HPP_INCLUDED

#include <array>

#include "limits.hpp"
#include "item.hpp"

struct T_Store;
struct T_Home;

struct T_Store {
  char Owner[64];
  char Name[128];
  float Pos[3];
  int Price;
  int World;
  int Pickup;
  int Label;
  int Balance;
  std::array<T_ItemSlot, MAX_STORE_INVENTORY> Items;
};

struct T_Home {
  char Owner[64];
  float Pos[3];
  int Price;
  int World;
  int Pickup;
  int Label;
  int Locked;
  std::array<T_ItemSlot, MAX_HOUSE_INVENTORY> Items;
};

extern std::array<T_Home, MAX_HOUSE> Houses;
extern std::array<T_Store, MAX_BUSINESS> Stores;

bool SaveHouse(int id);
bool LoadHouse(int id);
int GetHouseID(int playerid);
void EmptyHouseSlot(int id, int slot);
void UpdateHouseItem(int id);
void ResetHouseItem(int id);
bool AddHouseItem(int id, T_Item item, int quant);
int GetHouseItemCount(int id);
void HouseSlotInfo(int playerid, int target, int slot);

bool SaveBusiness(const char *type, int id);
bool LoadBusiness(const char *type, int id);
void EmptyBizSlot(const char *type, int id, int slot);
void UpdateBizItem(const char *type, int id);
void ResetBizItem(const char *type, int id);
bool AddBizItem(const char *type, int id, T_Item item, int quant);
int GetBizItemCount(const char *type, int id);
void RestockStoreBiz(int id);

#endif
