#include "cmd.hpp"

#include "utils.hpp"

CMD_ALIAS(vehicle, "veh", "vehicle") {
    char opt[16];
    int vehicleid = GetPlayerVehicleID(playerid);
    if (vehicleid < 1)
        vehicleid = GetNearestVehicle(playerid, 3.2f);

    if (vehicleid < 1)
        return SendClientMessage(playerid, COLOR_ERROR, "Error: No vehicle nearby");

    if (sscanf(cmd.args, "%s", opt) == 1) {
        if (!strcmp(opt, "trunk"))
            ToggleVehicleParam(vehicleid, VEH_PARAM_TRUNK);
        else if (!strcmp(opt, "hood"))
            ToggleVehicleParam(vehicleid, VEH_PARAM_HOOD);
        else return SendClientMessage(playerid, COLOR_ERROR, "Error: Invalid action");

        return true;
    } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /vehicle <part>");
}

