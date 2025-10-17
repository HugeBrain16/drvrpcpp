#ifndef ITEM_HPP_INCLUDED
#define ITEM_HPP_INCLUDED

enum class ItemType {
  EMPTY,
  SPRUNK,
  BREAD,
  WATER,
  MATERIAL,

  PISTOL,
  RIFLE,
  SHOTGUN,
  PISTOLAMMO,
  RIFLEAMMO,
  SHOTGUNAMMO
};

struct T_Item {
  char Name[64];
  ItemType Type;
  int Price;
  bool Equipped;
  int Durability;
};

struct T_ItemSlot {
  T_Item Item;
  int Quant;
};

bool IsStackable(T_Item item);
bool IsItemGun(T_Item item);

#endif
