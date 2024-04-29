#include "item.hpp"

bool IsStackable(T_Item item) {
  switch (item.Type) {
  case ITEM_9MM:
    return false;
  case ITEM_RIFLE:
    return false;
  case ITEM_SHOTGUN:
    return false;
  default:
    return true;
  }
}
