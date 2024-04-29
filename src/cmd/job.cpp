#include "cmd.hpp"

#include "player.hpp"

bool cmd_job(int playerid, Cmd cmd) {
  char opt[2][16];

  if (sscanf(cmd.args, "%s %s", opt[0], opt[1]) == 2) {
    if (!strcmp(opt[0], "join")) {
      if (!strcmp(opt[1], "gunmaker")) {
        if (!IsPlayerInRangeOfPoint(playerid, 1.5, -757.2897, -133.7420, 65.8281))
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You're not nearby the job point.");
        if (GetPlayerScore(playerid) < 3)
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You need to be level 3 to join this job.");
        if (Player[playerid].Job[Gunmaker].joined)
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You already joined job.");
        if (PlayerHasJob(playerid, Gunmaker))
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You can't join this job as you already joined another job.");

        Player[playerid].Job[Gunmaker].joined = true;
        SendClientMessage(playerid, COLOR_INFO, "You're now a Gunmaker!");
        return true;
      } else if (!strcmp(opt[1], "mechanic")) {
        if (!IsPlayerInRangeOfPoint(playerid, 1.5, 2139.5847, -1733.7576, 17.2891))
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You're not nearby the job point.");
        if (GetPlayerScore(playerid) < 2)
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You need to be level 2 to join this job.");
        if (Player[playerid].Job[Mechanic].joined)
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You already joined job.");
        if (PlayerHasJob(playerid, Mechanic))
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You can't join this job as you already joined another job.");

        Player[playerid].Job[Mechanic].joined = true;
        SendClientMessage(playerid, COLOR_INFO, "You're now a Mechanic!");
        return true;
      } else if (!strcmp(opt[1], "trucker")) {
        if (!IsPlayerInRangeOfPoint(playerid, 1.5, -49.8569, -269.3626, 6.6332))
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You're not nearby the job point.");
        if (GetPlayerScore(playerid) < 2)
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You need to be level 2 to join this job.");
        if (Player[playerid].Job[Trucker].joined)
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You already joined job.");
        if (PlayerHasJob(playerid, Trucker))
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You can't join this job as you already joined another job.");

        Player[playerid].Job[Trucker].joined = true;
        SendClientMessage(playerid, COLOR_INFO, "You're now a Trucker!");
        return true;
      } else return SendClientMessage(playerid, COLOR_ERROR, "Error: Invalid option.");
    } else if (!strcmp(opt[0], "quit")) {
      if (!strcmp(opt[1], "gunmaker")) {
        if (!IsPlayerInRangeOfPoint(playerid, 1.5, -757.2897, -133.7420, 65.8281))
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You're too far away from this job point");
        if (!Player[playerid].Job[Gunmaker].joined)
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You're not in this job");
        
        Player[playerid].Job[Gunmaker].joined = false;
        SendClientMessage(playerid, COLOR_INFO, "You quit from this job");
        return true;
      }
      if (!strcmp(opt[1], "mechanic")) {
        if (!IsPlayerInRangeOfPoint(playerid, 1.5, 2139.5847, -1733.7576, 17.2891))
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You're not near by Mechanic Job Point");
        if (!Player[playerid].Job[Mechanic].joined)
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You're not in this job");

        Player[playerid].Job[Mechanic].joined = false;
        SendClientMessage(playerid, COLOR_INFO, "You quit from this job");
        return true;
      }
      if (!strcmp(opt[1], "trucker")) {
        if (!IsPlayerInRangeOfPoint(playerid, 1.5, -49.8569, -269.3626, 6.6332))
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You're not near by the Job Point");
        if (!Player[playerid].Job[Trucker].joined)
          return SendClientMessage(playerid, COLOR_ERROR, "Error: You're not in this job");

        Player[playerid].Job[Trucker].joined = false;
        SendClientMessage(playerid, COLOR_ERROR, "You quit from this job");
        return true;
      } else return SendClientMessage(playerid, COLOR_ERROR, "Error: Invalid option.");
    } else return SendClientMessage(playerid, COLOR_ERROR, "Error: Invalid option.");
  } else return SendClientMessage(playerid, COLOR_USAGE, "Usage: /job [join | quit] [job name]");
}
