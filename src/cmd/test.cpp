#include "cmd.hpp"

#include "utils.hpp"
#include "player.hpp"
#include "item.hpp"

CMD(test) {
    char opt[16];
    struct T_Item item;

    if (!IsPlayerAdmin2(playerid))
        return false;

    if (sscanf(cmd.args, "%s", opt) == 1) {
        if (!strcmp(opt, "givepistol")) {
            strcpy(item.Name, "Colt45");
            item.Type = ItemType::PISTOL;
            item.Durability = 100;
            AddItem(playerid, item, 1); 
        } else if (!strcmp(opt, "pistolammo")) {
            strcpy(item.Name, "Colt45 Ammo");
            item.Type = ItemType::PISTOLAMMO;
            AddItem(playerid, item, 12);
        } else if (!strcmp(opt, "giverifle")) {
            strcpy(item.Name, "Rifle");
            item.Type = ItemType::RIFLE;
            item.Durability = 100;
            AddItem(playerid, item, 1); 
        } else if (!strcmp(opt, "rifleammo")) {
            strcpy(item.Name, "Rifle Ammo");
            item.Type = ItemType::RIFLEAMMO;
            AddItem(playerid, item, 5);
        } else if (!strcmp(opt, "giveshotgun")) {
            strcpy(item.Name, "Shotgun");
            item.Type = ItemType::SHOTGUN;
            item.Durability = 100;
            AddItem(playerid, item, 1); 
        } else if (!strcmp(opt, "shotgunammo")) {
            strcpy(item.Name, "Shotgun Ammo");
            item.Type = ItemType::SHOTGUNAMMO;
            AddItem(playerid, item, 5);
        }
    }

    return true;
}
