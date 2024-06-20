#include <cstring>

#include "vehicle.hpp"
#include "utils.hpp"

std::array<int, 5> Sweeper;
std::array<JOBV_BUS, 7> Bus;
std::array<int, 4> Mower;
std::array<T_Rent, MAX_RENTVEH> VehicleRent;
std::array<T_Vehicle, MAX_VEHICLES> StaticVehicle;

bool IsPlayerInSweeper(int playerid) {
  for (size_t i = 0; i < Sweeper.size(); i++) {
    if (GetPlayerVehicleID(playerid) == Sweeper[i])
      return true;
  }

  return false;
}

bool IsVehicleConnected(int vehicleid) {
  float pos[3];
  GetVehiclePos(vehicleid, &pos[0], &pos[1], &pos[2]);

  if (pos[0] == 0 && pos[1] == 0 && pos[2] == 0)
    return false;
  return true;
}

bool IsPlayerDriver(int playerid) {
  if (IsPlayerConnected(playerid) &&
      GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
    return true;
  return false;
}

int GetVehicleDriverID(int vehicleid) {
  if (IsVehicleConnected(vehicleid)) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
      if (IsPlayerConnected(i) && GetPlayerVehicleID(i) == vehicleid &&
          IsPlayerDriver(i))
        return i;
    }
  }
  return -1;
}

bool IsVehicleInUse(int vehicleid) {
  if (IsVehicleConnected(vehicleid) && GetVehicleDriverID(vehicleid) != -1)
    return true;
  return false;
}

int GetVehicleType(int vehicleid) {
  int type = 0;
  if (IsVehicleConnected(vehicleid)) {
    switch (GetVehicleModel(vehicleid)) {
    case 416:
    case 445:
    case 602:
    case 485:
    case 568:
    case 429:
    case 499:
    case 424:
    case 536:
    case 496:
    case 504:
    case 422:
    case 609:
    case 498:
    case 401:
    case 575:
    case 518:
    case 402:
    case 541:
    case 482:
    case 431:
    case 438:
    case 457:
    case 527:
    case 483:
    case 524:
    case 415:
    case 542:
    case 589:
    case 480:
    case 596:
    case 599:
    case 597:
    case 598:
    case 578:
    case 486:
    case 507:
    case 562:
    case 585:
    case 427:
    case 419:
    case 587:
    case 490:
    case 528:
    case 533:
    case 544:
    case 407:
    case 565:
    case 455:
    case 530:
    case 526:
    case 466:
    case 604:
    case 492:
    case 474:
    case 434:
    case 502:
    case 503:
    case 494:
    case 579:
    case 545:
    case 411:
    case 546:
    case 559:
    case 508:
    case 571:
    case 400:
    case 403:
    case 517:
    case 410:
    case 551:
    case 500:
    case 418:
    case 572:
    case 423:
    case 516:
    case 582:
    case 467:
    case 404:
    case 514:
    case 603:
    case 600:
    case 413:
    case 426:
    case 436:
    case 547:
    case 489:
    case 441:
    case 594:
    case 564:
    case 515:
    case 479:
    case 534:
    case 505:
    case 442:
    case 440:
    case 475:
    case 543:
    case 605:
    case 495:
    case 567:
    case 428:
    case 405:
    case 535:
    case 458:
    case 580:
    case 439:
    case 561:
    case 409:
    case 560:
    case 550:
    case 506:
    case 601:
    case 574:
    case 566:
    case 549:
    case 420:
    case 459:
    case 576:
    case 583:
    case 451:
    case 558:
    case 552:
    case 540:
    case 491:
    case 412:
    case 478:
    case 421:
    case 529:
    case 555:
    case 456:
    case 554:
    case 477:
      type = VTYPE_CAR;
      break;

    case 581:
    case 523:
    case 462:
    case 521:
    case 463:
    case 522:
    case 461:
    case 448:
    case 468:
    case 586:
      type = VTYPE_BIKE;
      break;

    case 509:
    case 481:
    case 510:
      type = VTYPE_BMX;
      break;

    case 471:
      type = VTYPE_QUAD;
      break;

    case 472:
    case 473:
    case 493:
    case 595:
    case 484:
    case 430:
    case 453:
    case 452:
    case 446:
    case 454:
      type = VTYPE_SEA;
      break;

    case 548:
    case 425:
    case 417:
    case 487:
    case 497:
    case 563:
    case 501:
    case 465:
    case 447:
    case 469:
    case 488:
      type = VTYPE_HELI;
      break;

    case 592:
    case 577:
    case 511:
    case 512:
    case 593:
    case 520:
    case 553:
    case 464:
    case 476:
    case 519:
    case 460:
    case 513:
    case 539:
      type = VTYPE_PLANE;
      break;

    case 588:
    case 437:
    case 532:
    case 433:
    case 414:
    case 443:
    case 470:
    case 432:
    case 525:
    case 531:
    case 408:
      type = VTYPE_HEAVY;
      break;

    case 406:
    case 573:
    case 444:
    case 556:
    case 557:
      type = VTYPE_MONSTER;
      break;

    case 435:
    case 450:
    case 591:
    case 606:
    case 607:
    case 610:
    case 584:
    case 608:
    case 611:
      type = VTYPE_TRAILER;
      break;

    case 590:
    case 569:
    case 537:
    case 538:
    case 570:
    case 449:
      type = VTYPE_TRAIN;
      break;
    }
  }
  return type;
}

bool HasNoEngine(int vehicleid) {
  switch (GetVehicleModel(vehicleid)) {
  case 481:
  case 509:
  case 510:
    return true;
  }
  return false;
}

bool IsPlayerInCar(int playerid) {
  if (IsPlayerConnected(playerid) && IsPlayerInAnyVehicle(playerid)) {
    if (GetVehicleType(GetPlayerVehicleID(playerid)) == VTYPE_CAR)
      return true;
  }
  return false;
}

bool IsPlayerInTruck(int playerid) {
  if (IsPlayerConnected(playerid) && IsPlayerInAnyVehicle(playerid)) {
    if (GetVehicleType(GetPlayerVehicleID(playerid)) == VTYPE_HEAVY)
      return true;
  }
  return false;
}

bool IsPlayerOnBike(int playerid) {
  if (IsPlayerConnected(playerid) && IsPlayerInAnyVehicle(playerid)) {
    if (GetVehicleType(GetPlayerVehicleID(playerid)) == VTYPE_BIKE)
      return true;
  }
  return false;
}

bool IsPlayerOnQuad(int playerid) {
  if (IsPlayerConnected(playerid) && IsPlayerInAnyVehicle(playerid)) {
    if (GetVehicleType(GetPlayerVehicleID(playerid)) == VTYPE_QUAD)
      return true;
  }
  return false;
}

void SetSweeperToRespawn() {
  for (size_t i = 0; i < Sweeper.size(); i++) {
    if (!IsVehicleInUse(Sweeper[i]))
      SetVehicleToRespawn(Sweeper[i]);
  }
}

void SAMPGDK_CALL TC_SetSweeperToRespawn2(int timerid, void *data) {
  unused(timerid, data);

  SetSweeperToRespawn();
}

void SetSweeperToRespawn2(int delay) {
  SetTimer(delay, false, TC_SetSweeperToRespawn2, nullptr);
}

int SpawnStaticVehicle(int modelid, float x, float y, float z, float r, int color1, int color2, int respawnDelay, bool siren) {
  int vehicleid = CreateVehicle(modelid, x, y, z, r, color1, color2, respawnDelay, siren);

  if (IsVehicleConnected(vehicleid)) {
    StaticVehicle[vehicleid].ID = vehicleid;
    strcpy(StaticVehicle[vehicleid].Owner, "None");
    StaticVehicle[vehicleid].Color[0] = color1;
    StaticVehicle[vehicleid].Color[1] = color2;
    StaticVehicle[vehicleid].Model = modelid;
    StaticVehicle[vehicleid].Fuel = 100.0;
    strcpy(StaticVehicle[vehicleid].Plate, GenerateNumberPlate());

    SetVehicleNumberPlate(vehicleid, StaticVehicle[vehicleid].Plate);
  }

  return vehicleid;
}

