#include <cstring>

#include "lib/ini.h"
#include "lib/streamer.hpp"

#include "property.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "files.hpp"

using namespace Plugins::Streamer;

std::array<T_Home, MAX_HOUSES> Houses;
std::array<T_Store, MAX_BUSINESSES> Stores;

bool SaveHouse(int id) {
  char buff[128];

  sprintf(buff, BD_HOUSE, id);
  mINI::INIFile file(buff);
  mINI::INIStructure ini;

  if (fexist(buff))
    file.read(ini);
  ini["main"]["owner"] = Houses[id].Owner;
  ini["main"]["x"] = std::to_string(Houses[id].Pos[0]);
  ini["main"]["y"] = std::to_string(Houses[id].Pos[1]);
  ini["main"]["z"] = std::to_string(Houses[id].Pos[2]);
  ini["main"]["price"] = std::to_string(Houses[id].Price);
  ini["main"]["world"] = std::to_string(Houses[id].World);
  ini["main"]["locked"] = std::to_string((int)Houses[id].Locked);

  for (int i = 0; i < 64; i++) {
    sprintf(buff, "slot%d", i);
    ini[buff]["name"] = Houses[id].Items[i].Item.Name;
    ini[buff]["type"] = std::to_string(static_cast<int>(Houses[id].Items[i].Item.Type));
    ini[buff]["quant"] = std::to_string(Houses[id].Items[i].Quant);
    ini[buff]["durability"] =
        std::to_string((int)Houses[id].Items[i].Item.Durability);
  }

  file.write(ini);
  return true;
}

bool LoadHouse(int id) {
  char buff[128];

  sprintf(buff, BD_HOUSE, id);
  if (!fexist(buff))
    return false;

  printf("Loading house...(%d)\n", id);
  mINI::INIFile file(buff);
  mINI::INIStructure ini;

  file.read(ini);
  strcpy(Houses[id].Owner, ini["main"]["owner"].c_str());
  Houses[id].Pos[0] = std::stof(ini["main"]["x"]);
  Houses[id].Pos[1] = std::stof(ini["main"]["y"]);
  Houses[id].Pos[2] = std::stof(ini["main"]["z"]);
  Houses[id].Price = std::stoi(ini["main"]["price"]);
  Houses[id].World = std::stoi(ini["main"]["world"]);
  Houses[id].Locked = (bool)std::stoi(ini["main"]["locked"]);

  for (int i = 0; i < 64; i++) {
    sprintf(buff, "slot%d", i);
    strcpy(Houses[id].Items[i].Item.Name, ini[buff]["name"].c_str());
    Houses[id].Items[i].Item.Type = static_cast<ItemType>(std::stoi(ini[buff]["type"]));
    Houses[id].Items[i].Quant = std::stoi(ini[buff]["quant"]);
    Houses[id].Items[i].Item.Durability = std::stoi(ini[buff]["durability"]);
  }

  if (strcmp(Houses[id].Owner, "None")) {
    sprintf(buff, "{AAAAAA}[ID:%d]\n{FFFFFF}Owner: {FF0000}%s", id,
            Houses[id].Owner);
  } else {
    sprintf(buff, "{AAAAAA}[ID:%d]\n{008000}For Sale $%d", id,
            Houses[id].Price);
  }

  Houses[id].Label =
      TextLabel::Create(buff, 0xFFFFFFAA, Houses[id].Pos[0], Houses[id].Pos[1],
                        Houses[id].Pos[2], 10.0);
  Houses[id].Pickup = Pickup::Create(1273, 0, Houses[id].Pos[0],
                                     Houses[id].Pos[1], Houses[id].Pos[2]);

  return true;
}

int GetHouseID(int playerid) {
  int houseid = -1;

  for (int i = 0; i < MAX_HOUSES; i++) {
    if (GetPlayerVirtualWorld(playerid) == Houses[i].World) {
      houseid = i;
      break;
    }
  }

  return houseid;
}

bool SaveBusiness(const char *type, int id) {
  char buff[128];

  if (!strcmp(type, "store")) {
    sprintf(buff, BIZ_STORE, id);
    mINI::INIFile file(buff);
    mINI::INIStructure ini;

    if (fexist(buff))
      file.read(ini);
    ini["main"]["owner"] = Stores[id].Owner;
    ini["main"]["name"] = Stores[id].Name;
    ini["main"]["x"] = std::to_string(Stores[id].Pos[0]);
    ini["main"]["y"] = std::to_string(Stores[id].Pos[1]);
    ini["main"]["z"] = std::to_string(Stores[id].Pos[2]);
    ini["main"]["price"] = std::to_string(Stores[id].Price);
    ini["main"]["world"] = std::to_string(Stores[id].World);
    ini["main"]["balance"] = std::to_string(Stores[id].Balance);

    for (int i = 0; i < 8; i++) {
      sprintf(buff, "shelf%d", i);
      ini[buff]["name"] = Stores[id].Items[i].Item.Name;
      ini[buff]["type"] = std::to_string(static_cast<int>(Stores[id].Items[i].Item.Type));
      ini[buff]["price"] = std::to_string(Stores[id].Items[i].Item.Price);
      ini[buff]["stock"] = std::to_string(Stores[id].Items[i].Quant);
    }

    file.write(ini);
    return true;
  }
  return false;
}

bool LoadBusiness(const char *type, int id) {
  char buff[128];

  if (!strcmp(type, "store")) {
    sprintf(buff, BIZ_STORE, id);

    if (!fexist(buff))
      return false;
    printf("Loading store business...(%d)\n", id);
    mINI::INIFile file(buff);
    mINI::INIStructure ini;

    file.read(ini);
    strcpy(Stores[id].Owner, ini["main"]["owner"].c_str());
    strcpy(Stores[id].Name, ini["main"]["name"].c_str());
    Stores[id].Pos[0] = std::stof(ini["main"]["x"]);
    Stores[id].Pos[1] = std::stof(ini["main"]["y"]);
    Stores[id].Pos[2] = std::stof(ini["main"]["z"]);
    Stores[id].Price = std::stoi(ini["main"]["price"]);
    Stores[id].World = std::stoi(ini["main"]["world"]);
    Stores[id].Balance = std::stoi(ini["main"]["balance"]);

    for (int i = 0; i < 8; i++) {
      sprintf(buff, "shelf%d", i);
      strcpy(Stores[id].Items[i].Item.Name, ini[buff]["name"].c_str());
      Stores[id].Items[i].Item.Type = static_cast<ItemType>(std::stoi(ini[buff]["type"]));
      Stores[id].Items[i].Item.Price = std::stoi(ini[buff]["price"]);
      Stores[id].Items[i].Quant = std::stoi(ini[buff]["stock"]);
    }

    if (strcmp(Stores[id].Owner, "None")) {
      sprintf(buff,
              "{AAAAAA}[ID:%d]\n{FF0000}%s\n{FF0000}Convenience "
              "Store\n{FFFFFF}Owner: {FF0000}%s",
              id, Stores[id].Name, Stores[id].Owner);
    } else {
      sprintf(buff,
              "{AAAAAA}[ID:%d]\n{FF0000}Convenience Store\n{008000}For "
              "Sale $%d",
              id, Stores[id].Price);
    }

    Stores[id].Label =
        TextLabel::Create(buff, 0xFFFFFFAA, Stores[id].Pos[0],
                          Stores[id].Pos[1], Stores[id].Pos[2], 10.0);
    Stores[id].Pickup = Pickup::Create(
        1274, 0, Stores[id].Pos[0], Stores[id].Pos[1], Stores[id].Pos[2]);
  }
  return false;
}

void EmptyBizSlot(const char *type, int id, int slot) {
  if (!strcmp(type, "store")) {
    Stores[id].Items[slot] = T_ItemSlot{};
  }
}

void EmptyHouseSlot(int id, int slot) {
  Houses[id].Items[slot] = T_ItemSlot{};
}

void UpdateHouseItem(int id) {
  int eslot = -1;

  for (int i = 0; i < 8; i++) {
    if (Houses[id].Items[i].Quant <= 0 || Houses[id].Items[i].Item.Type == ItemType::EMPTY) {
      EmptyHouseSlot(id, i);
      eslot = i;
    } else if (eslot > -1) {
      memcpy(&Houses[id].Items[eslot], &Houses[id].Items[i], sizeof(Houses[id].Items));
      EmptyHouseSlot(id, i);
      eslot = i;
    }
  }
}

void UpdateBizItem(const char *type, int id) {
  int eslot = -1;

  if (!strcmp(type, "store")) {
    for (int i = 0; i < 8; i++) {
      if (Stores[id].Items[i].Quant <= 0 || Stores[id].Items[i].Item.Type == ItemType::EMPTY) {
        EmptyBizSlot(type, id, i);
        eslot = i;
      } else if (eslot > -1) {
        memcpy(&Stores[id].Items[eslot], &Stores[id].Items[i], sizeof(Stores[id].Items));
        EmptyBizSlot(type, id, i);
        eslot = i;
      }
    }
  }
}

void ResetBizItem(const char *type, int id) {
  for (int i = 0; i < 8; i++) {
    EmptyBizSlot(type, id, i);
  }
}

void ResetHouseItem(int id) {
  for (int i = 0; i < 64; i++) {
    EmptyHouseSlot(id, i);
  }
}

bool AddHouseItem(int id, T_Item item, int quant) {
  for (int i = 0; i < 8; i++) {
    if (Houses[id].Items[i].Quant == 0 || !IsStackable(item)) {
      strcpy(Houses[id].Items[i].Item.Name, item.Name);
      Houses[id].Items[i].Item.Type = item.Type;
      Houses[id].Items[i].Quant = quant;
      Houses[id].Items[i].Item.Durability = item.Durability;
      break;
    } else if (!strcmp(Houses[id].Items[i].Item.Name, item.Name) && Houses[id].Items[i].Item.Type == item.Type) {
      Houses[id].Items[i].Quant += quant;
      break;
    }
  }

  UpdateHouseItem(id);
  return true;
}

bool AddBizItem(const char *type, int id, T_Item item, int quant) {
  if (!strcmp(type, "store")) {
    for (int i = 0; i < 8; i++) {
      if (Stores[id].Items[i].Quant == 0) {
        strcpy(Stores[id].Items[i].Item.Name, item.Name);
        Stores[id].Items[i].Item.Type = item.Type;
        Stores[id].Items[i].Item.Price = item.Price;
        Stores[id].Items[i].Quant = quant;
        break;
      } else if (!strcmp(Stores[id].Items[i].Item.Name, item.Name) && Stores[id].Items[i].Item.Type == item.Type) {
        Stores[id].Items[i].Quant += quant;
        break;
      }
    }

    UpdateBizItem("store", id);
    return true;
  }

  return false;
}

int GetBizItemCount(const char *type, int id) {
  int count = 0;

  if (!strcmp(type, "store")) {
    for (int i = 0; i < MAX_STORE_INVENTORY; i++) {
      if (Stores[id].Items[i].Quant > 0)
        count++;
    }
  }

  return count;
}

int GetHouseItemCount(int id) {
  int count = 0;

  for (int y = 0; y < MAX_HOUSE_INVENTORY; y++) {
    if (Houses[id].Items[y].Quant > 0)
      count++;
  }

  return count;
}

void RestockStoreBiz(int id) {
  struct T_Item item;

  strcpy(item.Name, "Bread");
  item.Type = ItemType::BREAD;
  item.Price = 2;
  AddBizItem("store", id, item, 30);

  strcpy(item.Name, "Sprunk");
  item.Type = ItemType::SPRUNK;
  item.Price = 3;
  AddBizItem("store", id, item, 30);

  strcpy(item.Name, "Water");
  item.Type = ItemType::WATER;
  item.Price = 1;
  AddBizItem("store", id, item, 30);
}

void HouseSlotInfo(int playerid, int target, int slot) {
  char buff[64];

  if (Houses[target].Items[slot].Quant != 0) {
    sprintf(buff, "[Slot: %d]", slot);
    SendClientMessage(playerid, 0xFFFF00, buff);
    sprintf(buff, "%s: %dx", Houses[target].Items[slot].Item.Name, Houses[target].Items[slot].Quant);
    SendClientMessage(playerid, 0xFFFF00, buff);
    sprintf(buff, "Type: %d", static_cast<int>(Houses[target].Items[slot].Item.Type));
    SendClientMessage(playerid, 0xFFFF00, buff);
  } else
    SendClientMessage(playerid, COLOR_INFO, "Slot is empty!");
}
