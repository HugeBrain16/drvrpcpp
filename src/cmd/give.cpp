#include "cmd.hpp"

#include "player.hpp"
#include "utils.hpp"


CMD(give) {
    int target;

    if (sscanf(cmd.args, "%d", &target) == 1) {
        if (playerid == target)
            return SendClientMessage(playerid, COLOR_ERROR, "Error: Cannot give to yourself");
        if (Player[playerid].DataState.givetarget > -1)
            return SendClientMessage(playerid, COLOR_ERROR, "Error: You're already in a process of giving items");
        if (!IsPlayerConnected(target))
            return SendClientMessage(playerid, COLOR_ERROR, "Error: Invalid target");
        if (GetPlayerDistanceFromPlayer(playerid, target) > 2.0f || !PlayersInSameUniverse(playerid, target))
            return SendClientMessage(playerid, COLOR_ERROR, "Error: Player is too far away");

        Player[playerid].DataState.givetarget = target;

        char title[128];
        char *inv = GetInvText(playerid);
        const char *name = RetPname(target);
        sprintf(title, "Inventory (%d/%d) - Giving to %s", GetItemCount(playerid), MAX_PLAYER_INVENTORY, name);
        ShowPlayerDialog(playerid, DIALOG_GIVE, DIALOG_STYLE_TABLIST_HEADERS, title, inv, "Select", "Close");
        free(inv);
        return true;
    } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /give <targetid>");    
}

