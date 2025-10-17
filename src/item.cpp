#include "item.hpp"

bool IsStackable(T_Item item) {
  switch (item.Type) {
  case ItemType::PISTOL:
    return false;
  case ItemType::RIFLE:
    return false;
  case ItemType::SHOTGUN:
    return false;
  default:
    return true;
  }
}

bool IsItemGun(T_Item item) {
  switch (item.Type) {
  case ItemType::PISTOL:
    return true;
  case ItemType::RIFLE:
    return true;
  case ItemType::SHOTGUN:
    return true;
  default:
    return false;
  }
}
