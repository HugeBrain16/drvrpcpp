#include "cmd.hpp"

#include "player.hpp"
#include "item.hpp"

CMD_ALIAS(reload, "r", "reload") {
    int gunSlot = GetEquippedSlot(playerid);
    int ammoSlot;
    int weaponid;
    int currentAmmo = 0;
    int ammo = 0;
    int ammoAdd = 0;
    struct T_Item *gun;

    unused(weaponid, cmd);

    if (gunSlot == -1)
        return SendClientMessage(playerid, COLOR_ERROR, "Error: No guns equipped");
    if (!IsItemGun(Player[playerid].Inventory[gunSlot].Item))
        return SendClientMessage(playerid, COLOR_ERROR, "Error: Item equipped is not a gun");

    gun = &Player[playerid].Inventory[gunSlot].Item;

    if (gun->Type == ItemType::PISTOL) {
        GetPlayerWeaponData(playerid, WEAPON_SLOT_PISTOL, &weaponid, &currentAmmo);
        ammoSlot = GetItemSlot(playerid, ItemType::PISTOLAMMO);

        if (ammoSlot > -1) {
            ammo = Player[playerid].Inventory[ammoSlot].Quant;
            ammoAdd = -currentAmmo + 12;

            if (ammoAdd > ammo)
                ammoAdd = ammo;

            AddItem(playerid, Player[playerid].Inventory[ammoSlot].Item, -ammoAdd);
            GiveWeaponWithAnim(playerid, WEAPON_COLT45, ammoAdd, "COLT45", "COLT45_RELOAD", "COLT45_CROUCHRELOAD");
        } else return SendClientMessage(playerid, COLOR_ERROR, "Error: You don't have any ammo for your equipped gun");
    } else if (gun->Type == ItemType::RIFLE) {
        GetPlayerWeaponData(playerid, WEAPON_SLOT_LONG_RIFLE, &weaponid, &currentAmmo);
        ammoSlot = GetItemSlot(playerid, ItemType::RIFLEAMMO);

        if (ammoSlot > -1) {
            ammo = Player[playerid].Inventory[ammoSlot].Quant;
            ammoAdd = -currentAmmo + 5;

            if (ammoAdd > ammo)
                ammoAdd = ammo;

            AddItem(playerid, Player[playerid].Inventory[ammoSlot].Item, -ammoAdd);
            GiveWeaponWithAnim(playerid, WEAPON_RIFLE, ammoAdd, "RIFLE", "RIFLE_LOAD", "RIFLE_CROUCHLOAD");
        } else return SendClientMessage(playerid, COLOR_ERROR, "Error: You don't have any ammo for your equipped gun");
    } else if (gun->Type == ItemType::SHOTGUN) {
        GetPlayerWeaponData(playerid, WEAPON_SLOT_SHOTGUN, &weaponid, &currentAmmo);
        ammoSlot = GetItemSlot(playerid, ItemType::SHOTGUNAMMO);

        if (ammoSlot > -1) {
            ammo = Player[playerid].Inventory[ammoSlot].Quant;
            ammoAdd = -currentAmmo + 5;

            if (ammoAdd > ammo)
                ammoAdd = ammo;

            AddItem(playerid, Player[playerid].Inventory[ammoSlot].Item, -ammoAdd);
            GiveWeaponWithAnim(playerid, WEAPON_SHOTGUN, ammoAdd, "SHOTGUN", "SHOTGUN_FIRE_POOR", "SHOTGUN_FIRE_POOR");
        } else return SendClientMessage(playerid, COLOR_ERROR, "Error: You don't have any ammo for your equipped gun");
    }

    return true;
}
