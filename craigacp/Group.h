#pragma once
#include "DudeInfo.h"
#include "Enemy.h"
#include <math.h>
#include <limits>
#include <windows.h>
#include <BWTA.h>
#include <BWAPI.h>
using namespace std;
using namespace BWAPI;

enum Formation {

  FormMess,
  FormLine

};

enum MatchType {
  Dragoons,
  Zealots,
  Mutalisks
};

enum State {

  Waiting,
  Attack,
  Move

};

extern Enemy enemyClass;
extern MatchType game_type;

class Group {

public:

  Group();
  //Methods
  void move(Position destination);
  void attack(Unit* new_target);
  void onFrame();
  void addDude(Unit* dude, DudeInfo info);
  void setFormation(Formation new_formation, double parameter);

  //Variables
  set<Unit*> groupDudes;
  map<Unit*, DudeInfo> dudeInfoMap;
  Formation formation;
  Unit* target;
  Position destination;
  State group_state;
  Position centroid;
  bool isAttacking;
  double formation_parameter;

private:
  void updateCentroid();
  void planAttack(Unit* target);
  double positionScore(int squareThreat, double distance, Unit* something);
  void formation_move();
  set<Position> circles;
  int groupRange;
};