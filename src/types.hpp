#ifndef MISC_HPP_INCLUDED
#define MISC_HPP_INCLUDED

#include <string>

struct BuildingData {
  int id;
  float x;
  float y;
  float z;
  float range;
};

struct VehicleData {
  int model;
  float x;
  float y;
  float z;
  float rotation;
  int color1;
  int color2;
  int delay;
  bool siren;
};

struct TextLabelData {
  std::string text;
  unsigned int color;
  float x;
  float y;
  float z;
  float range;
};

struct PickupData {
  int model;
  int type;
  float x;
  float y;
  float z;
};

struct ObjectData {
  int model;
  double x;
  double y;
  double z;
  double rx;
  double ry;
  double rz;
  int world = -1;
  int inter = -1;
};

#endif
