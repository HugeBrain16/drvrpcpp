#include "lib/sampgdk.h"
#include "dialog.hpp"

DIALOG(house_storage_take, "Storage - Take", DIALOG_STYLE): {
    Player[playerid].DataState.housetake = listitem;
    ShowPlayerDialog(playerid, DIALOG_HOUSE_STORAGE_TAKE_AMOUNT,
                     DIALOG_STYLE_INPUT, "Storage - Take - Amount",
                     "Enter the amount of item(s) to take:", "Confirm",
                     "Cancel");
    break;
  }
  case DIALOG_HOUSE_STORAGE_STORE: {
    Player[playerid].DataState.housestore = listitem;
    ShowPlayerDialog(playerid, DIALOG_HOUSE_STORAGE_STORE_AMOUNT, DIALOG_STYLE_INPUT, "Storage - Store - Amount", "Enter the amount of item(s) to store:", "Confirm", "Cancel");
    break;
  }
  case DIALOG_HOUSE_STORAGE_TAKE_AMOUNT: {
    int slotid = Player[playerid].DataState.housetake;
    int houseid = GetHouseID(playerid);
    struct T_ItemSlot *slot = &Houses[houseid].Items[slotid];
    Player[playerid].DataState.housetake = -1;

    if (canint(inputtext)) {
      int amount = std::stoi(inputtext);

      if (slot->Quant < amount) {
        return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Insufficient amount!");
        break;
      }

      AddItem(playerid, slot->Item, amount);
      AddHouseItem(houseid, slot->Item, -amount);
      break;
    } else {
      return SendClientMessage(playerid, COLOR_ERROR, "ERROR: The amount has to be numeric!");
      break;
    }
  }
  case DIALOG_HOUSE_STORAGE_STORE_AMOUNT: {
    int slotid = Player[playerid].DataState.housestore;
    int houseid = GetHouseID(playerid);
    struct T_ItemSlot *slot = &Player[playerid].Inventory[slotid];
    Player[playerid].DataState.housestore = -1;

    if (canint(inputtext)) {
      int amount = std::stoi(inputtext);

      if (slot->Quant < amount) {
        return SendClientMessage(playerid, COLOR_ERROR, "ERROR: Insufficient amount!");
        break;
      }

      if (slot->Item.Equipped && IsItemGun(slot->Item)) {
        HolsterEquipped(playerid);
        UnloadHolstered(playerid);
      }

      AddHouseItem(houseid, slot->Item, amount);
      AddItem(playerid, slot->Item, -amount);
      break;
    } else {
      return SendClientMessage(playerid, COLOR_ERROR, "ERROR: The amount has to be numeric!");
      break;
    }
  }
