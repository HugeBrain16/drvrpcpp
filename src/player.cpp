#include <cstring>

#include "lib/ini.h"

#include "player.hpp"
#include "property.hpp"
#include "utils.hpp"
#include "files.hpp"

std::array<T_Player, MAX_PLAYERS> Player;

const char *RetPname(int playerid, bool underscore) {
  static char name[MAX_PLAYER_NAME];
  name[0] = '\0';

  if (!IsPlayerConnected(playerid))
    return name;

  GetPlayerName(playerid, name, MAX_PLAYER_NAME);

  if (underscore) {
    for (size_t i = 0; i < strlen(name); i++) {
      if (name[i] == '_') name[i] = ' ';
    }
  }

  return name;
}

bool PlayerHasJob(int playerid, E_PlayerJob job) {
  bool res = false;

  for (int i = 0; i < 4; i++) {
    if (Player[playerid].Job[i].joined && i != job) {
      res = true;
      break;
    }
  }

  return res;
}

bool SaveInventory(int playerid) {
  char buff[128];
  const char *name = RetPname(playerid);

  sprintf(buff, PLAYER_INVENTORY, name);
  mINI::INIFile file(buff);
  mINI::INIStructure ini;

  if (fexist(buff))
    file.read(ini);

  for (int i = 0; i < 8; i++) {
    sprintf(buff, "slot%d", i);
    ini[buff]["name"] = Player[playerid].Inventory[i].Item.Name;
    ini[buff]["type"] = std::to_string(static_cast<int>(Player[playerid].Inventory[i].Item.Type));
    ini[buff]["quant"] = std::to_string(Player[playerid].Inventory[i].Quant);
    ini[buff]["equipped"] = std::to_string((int)Player[playerid].Inventory[i].Item.Equipped);
    ini[buff]["durability"] = std::to_string(Player[playerid].Inventory[i].Item.Durability);
  }

  file.write(ini);
  return true;
}

bool LoadInventory(int playerid) {
  char buff[128];
  const char *name = RetPname(playerid);

  sprintf(buff, PLAYER_INVENTORY, name);

  if (!fexist(buff))
    return false;

  mINI::INIFile file(buff);
  mINI::INIStructure ini;

  file.read(ini);

  for (int i = 0; i < 8; i++) {
    sprintf(buff, "slot%d", i);
    strcpy(Player[playerid].Inventory[i].Item.Name, ini[buff]["name"].c_str());
    Player[playerid].Inventory[i].Item.Type = static_cast<ItemType>(std::stoi(ini[buff]["type"]));
    Player[playerid].Inventory[i].Quant = std::stoi(ini[buff]["quant"]);
    // Player[playerid].Inventory[i].Item.Equipped = (bool)std::stoi(ini[buff]["equipped"]);
    Player[playerid].Inventory[i].Item.Durability = std::stoi(ini[buff]["durability"]);
  }
  return true;
}

void ItemSlotInfo(int playerid, int target, int slot) {
  char buff[128];

  if (Player[target].Inventory[slot].Quant != 0) {
    sprintf(buff, "[Slot: %d]", slot);
    SendClientMessage(playerid, 0xFFFF00, buff);
    sprintf(buff, "%s: %dx", Player[target].Inventory[slot].Item.Name, Player[target].Inventory[slot].Quant);
    SendClientMessage(playerid, 0xFFFF00, buff);
    sprintf(buff, "Type: %d", (int)(Player[target].Inventory[slot].Item.Type));
    SendClientMessage(playerid, 0xFFFF00, buff);
  } else
    SendClientMessage(playerid, COLOR_INFO, "Slot is empty!");
}

void EmptyItemSlot(int playerid, int slot) {
  Player[playerid].Inventory[slot] = T_ItemSlot{};
}

void UpdateItem(int playerid) {
  int eslot = -1;

  for (int i = 0; i < 8; i++) {
    if (Player[playerid].Inventory[i].Quant <= 0 || Player[playerid].Inventory[i].Item.Type == ItemType::EMPTY) {
      EmptyItemSlot(playerid, i);
      eslot = i;
    } else if (eslot > -1) {
      memcpy(&Player[playerid].Inventory[eslot], &Player[playerid].Inventory[i], sizeof(Player[playerid].Inventory));
      EmptyItemSlot(playerid, i);
      eslot = i;
    }
  }
}

void ResetItem(int playerid) {
  for (int i = 0; i < 8; i++) {
    EmptyItemSlot(playerid, i);
  }
}

bool AddItem(int playerid, T_Item item, int quant) {
  for (int i = 0; i < 8; i++) {
    struct T_ItemSlot *slot = &Player[playerid].Inventory[i];
    
    if (slot->Quant == 0) {
      strcpy(slot->Item.Name, item.Name);
      slot->Item.Type = item.Type;
      slot->Quant = quant;
      slot->Item.Durability = item.Durability;
      break;
    } else if ((IsStackable(item) || (!IsStackable(item) && quant < 0)) && !strcmp(slot->Item.Name, item.Name) && slot->Item.Type == item.Type) {
      slot->Quant += quant;
      break;
    }
  }

  UpdateItem(playerid);
  return true;
}

int GetItemCount(int playerid) {
  int count = 0;

  for (int i = 0; i < MAX_PLAYER_INVENTORY; i++) {
    if (Player[playerid].Inventory[i].Quant > 0)
      count++;
  }

  return count;
}

T_Item GetEquipped(int playerid) {
  struct T_Item item{};

  for (int i = 0; i < GetItemCount(playerid); i++) {
    if (Player[playerid].Inventory[i].Item.Equipped)
      return Player[playerid].Inventory[i].Item;
  }

  return item;
}

char *GetInvText(int playerid) {
  char *invText = (char *)malloc(256 * sizeof(char));
  char buff[128];
  struct T_ItemSlot *slot;
  invText[0] = '\0';

  strcat(invText, "Name\tQuantity\n");
  for (int i = 0; i < GetItemCount(playerid); i++) {
    slot = &Player[playerid].Inventory[i];

    if (slot->Quant > 0) {
      if (slot->Item.Equipped)
          sprintf(buff, "[E] %s (%d%%)\t%dx\n", slot->Item.Name, slot->Item.Durability, slot->Quant);
      else
          sprintf(buff, "%s\t%dx\n", slot->Item.Name, slot->Quant);
    }
    strcat(invText, buff);
  }

  return invText;
}

int GetHouseCount(int playerid) {
  int count = 0;
  const char *name = RetPname(playerid);

  for (int i = 0; i < MAX_HOUSES; i++) {
    if (!strcmp(Houses[i].Owner, name))
      count++;
  }

  return count;
}

bool HasHouse(int playerid) {
  return GetHouseCount(playerid) > 0;
}

int GetBizCount(const char *type, int playerid) {
  const char *name = RetPname(playerid);
  int result = 0;

  if (!strcmp(type, "store")) {
    for (int i = 0; i < MAX_BUSINESSES; i++) {
      if (!strcmp(Stores[i].Owner, name))
        result++;
    }
  }

  return result;
}

void ShowVehicleIndicator(int playerid) {
  if (IsPlayerInAnyVehicle(playerid)) {
    PlayerTextDrawShow(playerid, Player[playerid].VehicleIndicator.MainBox);
    PlayerTextDrawShow(playerid, Player[playerid].VehicleIndicator.Speed);
    PlayerTextDrawShow(playerid, Player[playerid].VehicleIndicator.Health);
    PlayerTextDrawShow(playerid, Player[playerid].VehicleIndicator.Fuel);
    PlayerTextDrawShow(playerid, Player[playerid].VehicleIndicator.Heat);
  }
}

void HideVehicleIndicator(int playerid) {
  PlayerTextDrawHide(playerid, Player[playerid].VehicleIndicator.MainBox);
  PlayerTextDrawHide(playerid, Player[playerid].VehicleIndicator.Speed);
  PlayerTextDrawHide(playerid, Player[playerid].VehicleIndicator.Health);
  PlayerTextDrawHide(playerid, Player[playerid].VehicleIndicator.Fuel);
  PlayerTextDrawHide(playerid, Player[playerid].VehicleIndicator.Heat);
}

void SavePlayer(int playerid) {
  float pos[4];
  float mainStatus[2];
  char fpath[256];
  const char *name = RetPname(playerid);

  sprintf(fpath, PLAYER_ACCOUNT, name);
  GetPlayerPos(playerid, &pos[0], &pos[1], &pos[2]);
  GetPlayerFacingAngle(playerid, &pos[3]);
  GetPlayerHealth(playerid, &mainStatus[0]);
  GetPlayerArmour(playerid, &mainStatus[1]);

  mINI::INIFile file(fpath);
  mINI::INIStructure ini;
  file.read(ini);
  ini["position"]["x"] = std::to_string(pos[0]);
  ini["position"]["y"] = std::to_string(pos[1]);
  ini["position"]["z"] = std::to_string(pos[2]);
  ini["position"]["r"] = std::to_string(pos[3]);
  ini["position"]["int"] = std::to_string(GetPlayerInterior(playerid));
  ini["position"]["vw"] = std::to_string(GetPlayerVirtualWorld(playerid));
  ini["position"]["skin"] = std::to_string(GetPlayerSkin(playerid));
  ini["status"]["health"] = std::to_string(mainStatus[0]);
  ini["status"]["armour"] = std::to_string(mainStatus[1]);
  ini["status"]["hunger"] = std::to_string(Player[playerid].Status.hunger);
  ini["status"]["thrist"] = std::to_string(Player[playerid].Status.thirst);
  ini["status"]["energy"] = std::to_string(Player[playerid].Status.energy);
  ini["stats"]["money"] = std::to_string(GetPlayerMoney(playerid));
  ini["stats"]["score"] = std::to_string(GetPlayerScore(playerid));
  ini["role"]["admin"] = std::to_string(Player[playerid].Flag.Admin);
  ini["role"]["helper"] = std::to_string(Player[playerid].Flag.Helper);
  ini["job"]["mechanic"] = std::to_string(Player[playerid].Job[Mechanic].joined);
  ini["job"]["gunmaker"] = std::to_string(Player[playerid].Job[Gunmaker].joined);
  ini["job"]["trucker"] = std::to_string(Player[playerid].Job[Trucker].joined);
  ini["job"]["taxi"] = std::to_string(Player[playerid].Job[Taxi].joined);
  file.write(ini);
}

void LoadPlayer(int playerid) {
  char fpath[256];
  const char *name = RetPname(playerid);
  sprintf(fpath, PLAYER_ACCOUNT, name);

  mINI::INIFile file(fpath);
  mINI::INIStructure ini;
  file.read(ini);
  GivePlayerMoney(playerid, std::stoi(ini["stats"]["money"]));
  SetPlayerScore(playerid, std::stoi(ini["stats"]["score"]));
  SetPlayerHealth(playerid, std::stof(ini["status"]["health"]));
  SetPlayerArmour(playerid, std::stof(ini["status"]["armour"]));
  try {
    Player[playerid].Status.hunger = std::stof(ini["status"]["hunger"]);
    Player[playerid].Status.thirst = std::stof(ini["status"]["thirst"]);
    Player[playerid].Status.energy = std::stof(ini["status"]["energy"]);
  } catch (const std::exception& e) {}
  Player[playerid].Flag.Admin = to_bool(ini["role"]["admin"].c_str());
  Player[playerid].Flag.Helper = to_bool(ini["role"]["helper"].c_str());
  Player[playerid].Job[Mechanic].joined = to_bool(ini["job"]["mechanic"].c_str());
  Player[playerid].Job[Trucker].joined = to_bool(ini["job"]["trucker"].c_str());
  Player[playerid].Job[Gunmaker].joined = to_bool(ini["job"]["gunmaker"].c_str());
  Player[playerid].Job[Taxi].joined = to_bool(ini["job"]["taxi"].c_str());
  FreezePlayer(playerid, 1000);
  SetPlayerInterior(playerid, std::stoi(ini["position"]["int"]));
  SetPlayerVirtualWorld(playerid, std::stoi(ini["position"]["vw"]));
}

bool SpeedLimitOn(int playerid) {
  return Player[playerid].DataState.speedlimit > 0;
}

bool IsPlayerAdmin2(int playerid) {
  return IsPlayerAdmin(playerid) || Player[playerid].Flag.Admin;
}

bool IsPlayerHelper(int playerid) {
  return Player[playerid].Flag.Helper;
}

int GetEquippedSlot(int playerid) {
  for (int i = 0; i < 8; i++) {
    if (Player[playerid].Inventory[i].Item.Equipped)
      return i;
  }

  return -1;
}

int GetItemSlot(int playerid, ItemType type) {
  for (int i = 0; i < 8; i++) {
    if (Player[playerid].Inventory[i].Item.Type == type)
      return i;
  }

  return -1;
}

