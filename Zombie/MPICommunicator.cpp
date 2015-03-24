//
//  CMPICommunicator.cpp
//  Zombie
//
//  Created by RUGANG YAO, CHIKAI ZHANG, CHUAN QIN on 25/10/2014.
//  Copyright (c) 2014 ___YAORUGANG_CHIKAIZHANG_CHUANQIN_. All rights reserved.
//

#include "MPICommunicator.h"
#include "NorthTerritory.h"


void CMPICommunicator::Init(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    MPI_Type_contiguous(sizeof(Cell), MPI_BYTE, &cellDatatype);
    MPI_Type_commit(&cellDatatype);
    
    MPI_Type_contiguous(sizeof(Status), MPI_BYTE, &statusDatatype);
    MPI_Type_commit(&statusDatatype);
}

void CMPICommunicator::Finalize()
{
    MPI_Finalize();
}

void CMPICommunicator::Send(int dest, CCreature **row)
{
    int n = 0;  // calculating how many cells need to be sent.
    for (int j = 0; j < SIZE_Y; j++)
    {
        if (row[j] != NULL)
            n++;
    }

    Cell *pCell = new Cell[n];
    int i = 0;
    for (int j = 0; j < SIZE_Y; j++)
    {
        if (row[j] != NULL)
        {
            pCell[i].index = j;
            pCell[i].age = row[j]->GetAge();
            pCell[i].gender = row[j]->GetGender();
            pCell[i].type = row[j]->GetType();
            
            i++;
        }
    }
    
    MPI_Send(pCell, n, cellDatatype, dest, TAG, MPI_COMM_WORLD);
}

void CMPICommunicator::SendStatus(int dest, Status status)
{
    MPI_Send(&status, 1, statusDatatype, dest, TAG, MPI_COMM_WORLD);
}

CCreature** CMPICommunicator::Receive(int source)
{
    int count;
    MPI_Status status;
    
    // probing how many cells will be receiving
    MPI_Probe(source, TAG, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, cellDatatype, &count);
    
    Cell *pCell = new Cell[count];
    MPI_Recv(pCell, count, cellDatatype, source, TAG, MPI_COMM_WORLD, &status);
    
    CCreature** row = new CCreature*[SIZE_Y];
    for (int j = 0; j < SIZE_Y; j++)
    {
        row[j] = NULL;
    }
    
    for (int i = 0; i < count; i++)
    {
        CCreature *p = new CCreature();
        
        p->SetType(pCell[i].type);
        p->SetAge(pCell[i].age);
        p->SetGender(pCell[i].gender);
        
        row[pCell[i].index] = p;
    }
    
    return row;
}

Status CMPICommunicator::ReceiveStatus(int source)
{
    MPI_Status status;
    Status s;
    
    MPI_Recv(&s, 1, statusDatatype, source, TAG, MPI_COMM_WORLD, &status);
    
    return s;
}

int CMPICommunicator::GetRank()
{
    return this->rank;
}

int CMPICommunicator::GetSize()
{
    return this->size;
}