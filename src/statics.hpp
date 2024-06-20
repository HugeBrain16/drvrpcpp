#ifndef STATICS_HPP_INCLUDED
#define STATICS_HPP_INCLUDED

#include "player.hpp"
#include "misc.hpp"

extern const int STARTER_SKINS[16];
extern const float MissionCheckpoint[E_PlayerMission_COUNT][MAX_MISSION_CHECKPOINTS][3];
extern const float PlayerPoint[E_PlayerPoint_COUNT][MAX_PLAYER_POINTS][3];
extern const char DCReason[3][16];
extern const std::array<PickupData, MAX_PICKUPS> WorldPickup;
extern const std::array<BuildingData, MAX_BUILDING_OBJECTS> RemoveBuildingData;
extern const std::array<VehicleData, MAX_VEHICLES> JobVehicleSweeper;
extern const std::array<VehicleData, MAX_VEHICLES> JobVehicleBus;
extern const std::array<VehicleData, MAX_VEHICLES> JobVehicleMower;
extern const std::array<TextLabelData, MAX_TEXT_LABELS> GlobalTextLabel;

#endif
