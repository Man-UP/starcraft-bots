void Group::planAttack(Unit *target, Enemy enemyClass)
{
    //resolution - 10px x 10px
    //grouprange in px
    
    Position centrePosition = target->getPosition();
    Position topLeft = centrePosition + Position(-groupRange / 20, -groupRange / 20);
    
    int matrixSize = (groupRange / 10) + 1;
    int **threatMatrix = new int*[matrixSize];
    bool **occupiedMatrix = new bool*[matrixSize];

    for (int i = 0 ;i< matrixSize ; i++) 
    {
        threatMatrix[i] = new int[matrixSize];
        occupiedMatrix[i] = new int[matrixSize];
        for(int j=0; j< matrixSize; j++)   
        {
            threatMatrix[i][j] = 0;
            occupiedMatrix[i][j] = false;
        } 
            
    }    
    int matrixCentre = matrixSize/2 +1;
    
    threatMatrix[matrixCentre][matrixCentre] = 1000;
    occupiedMatrix[matrixCentre][matrixCentre] = true;
    
    //setup done

    set<EnemyUnit *> enemySet = enemyClass.getSet();
    
    for (set<EnemyUnit *>::iterator enemyItr = enemySet.begin(); enemyItr != enemySet.end(); enemyItr++)
    {
        EnemyUnit * enemyDude = *enemyItr;
        if (!enemyDude->dead)
        {
            Position currentEnemyPos = enemyDude->getPosition();
            int currentEnemyDamage = enemyDude->getType().groundWeapon().damageAmount();
            int currentEnemyRange = enemyDude->getType().groundWeapon().maxRange();
            for (int i = 0; i < matrixSize; i++)
            {
                for (int j = 0; j < matrixSize; j++)
                {
                    Position currentMatrixPos = Position(topLeft) + Position(i*10,j*10);
                    if (currentMatrixPos.getDistance(currentEnemyPos) <= currentEnemyRange())
                    {
                        threatMatrix[i][j] += currentEnemyDamage;
                    }//in range
                }//for j
            }//for i
        }//if not dead
    }//iter over enemies, fill threat matrix

    for (set<Unit *>::iterator dudeItr = groupDudes.begin(); dudeItr != groupDudes.end(); dudeItr++)
    {
        double bestScore = 1000000.0;
        int bestI;
        int bestJ;
        Unit * dude = *dudeItr;
        
        for (int i = 0; i < matrixSize; i++)
        {
            for (int j = 0; j < matrixSize; j++)
            {
                if (!occupiedMatrix[i][j]) 
                {
                    Position currentMatrixPos = Position(topLeft) + Position(i*10,j*10);
                    double currentScore = positionScore(threatMatrix[i][j],dude->getPosition().getDistance(currentMatrixPos),dude);
                    if (currentScore < bestScore)
                    {
                        bestScore = currentScore;
                        bestI = i;
                        bestJ = j;
                    }
                }           
            }//for j
        }//for i
        occupiedMatrix[bestI][bestJ] = true;
        dudeInfoMap[dude].moveTarget = Position(topLeft) + Position(bestI*10,bestJ*10);
        dudeInfoMap[dude].state = DudeMoving;
    }
}

double Group::positionScore(int squareThreat, double distance, Unit *dude)
{
    double speed = dude->getType().topSpeed();
    int health = dude->getHitPoints();
    int shield = dude->getShields();
    
    double time = distance / speed;
    double hits = (health + shield) / (double) squareThreat;
    
    return time * hits;
}//positionScore
