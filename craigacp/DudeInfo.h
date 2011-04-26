#pragma once
#define NOMINMAX
#include <math.h>
#include <BWAPI.h>
#include <BWTA.h>

using namespace std;
using namespace BWAPI;

enum DudeState
{
    DudeStartAttack,
    DudeAttacking,
    DudeMoving,
    DudeCooldown,
    DudeWaiting
};

struct DudeInfo
{
    public:
        Unit *dude;
        int health;
        Position position;
        bool dead;
        Unit *target;
        Position moveTarget;
        DudeState state;
};
    
