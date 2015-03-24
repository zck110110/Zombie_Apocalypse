//
//  CMPICommunicator.h
//  Zambie
//
//  Created by RUGANG YAO on 25/10/2014.
//  Copyright (c) 2014 ___YAORUGANG___. All rights reserved.
//

#ifndef __Zambie__CMPICommunicator__
#define __Zambie__CMPICommunicator__

#define TAG     2013
#define PROC1   0
#define PROC2   1

#include <mpi.h>
#include <stdio.h>
#include "Creature.h"

struct Cell
{
    int index;
    
    TYPE type;
    int age;
    GENDER gender;
};

struct Status
{
    int nHuman;
    int nFemale;
    int nMale;
    int nZambie;
    int nInfected;
};

class CMPICommunicator
{
private:
    int rank;
    int size;
    MPI_Datatype cellDatatype;
    MPI_Datatype statusDatatype;
    
public:
    void Init(int argc, char *argv[]);
    void Finalize();
    void Send(int dest, CCreature **row);
    CCreature** Receive(int source);
    void SendStatus(int dest, Status status);
    Status ReceiveStatus(int source);
    
    int GetRank();
    int GetSize();
};

#endif /* defined(__Zambie__CMPICommunicator__) */
