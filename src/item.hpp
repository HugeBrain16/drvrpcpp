#ifndef ITEM_HPP_INCLUDED
#define ITEM_HPP_INCLUDED

struct T_Item;
struct T_ItemSlot;

enum E_ItemType {
  ITEM_EMPTY,
  ITEM_SPRUNK,
  ITEM_BREAD,
  ITEM_WATER,
  ITEM_MATERIAL,
  ITEM_9MM,
  ITEM_RIFLE,
  ITEM_SHOTGUN
};

struct T_Item {
  char Name[64];
  E_ItemType Type;
  int Price;
  bool Equipped;
  int Durability;
};

struct T_ItemSlot {
  T_Item Item;
  int Quant;
};

bool IsStackable(T_Item item);

#endif
