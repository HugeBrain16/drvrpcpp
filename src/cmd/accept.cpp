#include "cmd.hpp"

#include "utils.hpp"


CMD(accept) {
    char opt[16];

    if (sscanf(cmd.args, "%s", opt) == 1) {
        if (!strcmp(opt, "give")) {
            int giverid = Player[playerid].DataState.giverid;
            int amount = Player[playerid].DataState.giveamount;
            struct T_Item *item = Player[playerid].DataState.giveitem;

            if (!Player[playerid].Flag.AcceptGive)
                return SendClientMessage(playerid, COLOR_ERROR, "Error: no active give offer");
            if (!IsPlayerConnected(giverid))
                return SendClientMessage(playerid, COLOR_ERROR, "Error: the giver is no longer connected");
            if (GetPlayerDistanceFromPlayer(playerid, giverid) > 2.0f || !PlayersInSameUniverse(playerid, giverid))
                return SendClientMessage(playerid, COLOR_ERROR, "Error: Player is too far away");

            AddItem(playerid, *item, amount);
            AddItem(giverid, *item, -amount);
            Player[giverid].DataState.givetarget = -1;

            char msg[128];
            const char *name = RetPname(playerid);
            sprintf(msg, "%s accepted the item", name);
            SendClientMessage(giverid, COLOR_INFO, msg);
            sprintf(msg, "Received [%s %dx]", item->Name, amount);
            SendClientMessage(playerid, COLOR_INFO, msg);

            GiveReset(playerid);
        }
    }

    return true;
}

