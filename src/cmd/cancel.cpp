#include "cmd.hpp"

#include "utils.hpp"


CMD(cancel) {
    char opt[16];
    char msg[128];
    const char *name = RetPname(playerid);

    if (sscanf(cmd.args, "%s", opt) == 1) {
        if (!strcmp(opt, "give")) {
            int giverid = Player[playerid].DataState.giverid;
            int targetid = Player[playerid].DataState.givetarget;

            if (targetid > -1) {
                Player[playerid].DataState.givetarget = -1;
                GiveReset(targetid);

                sprintf(msg, "%s cancelled the give offer", name);
                SendClientMessage(targetid, COLOR_INFO, msg);
                SendClientMessage(playerid, COLOR_INFO, "Give offer cancelled");
            } else if (giverid > -1 && Player[playerid].Flag.AcceptGive) {
                Player[giverid].DataState.givetarget = -1;
                GiveReset(playerid);

                sprintf(msg, "%s refused the give offer", name);
                SendClientMessage(giverid, COLOR_INFO, msg);
                SendClientMessage(playerid, COLOR_INFO, "Give offer refused");
            }
        }
    }

    return true;
}

