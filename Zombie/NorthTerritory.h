#pragma once

#include "MPICommunicator.h"
#include "MersenneTwister.h"

#define SIZE_X  (768 + 2)   //  adding 2 rows as boundries.
#define SIZE_Y  1024
#define MOVE	0.1
#define POPULATION_DENSITY	0.16
#define BIRTH_RATE   (13.6/1000)
#define DEATH_RATE   (6.5/1000)
#define RANDOM_GENERATOR 64

enum DIRECTION{UP, DOWN, LEFT, RIGHT};
    
class CNorthTerritory
{
private:
    CCreature*** m_Mesh;
    MTRand m_MTRand[RANDOM_GENERATOR];

private:
	CCreature*** CreateMesh();
	void DeleteMesh(CCreature*** Mesh);
    bool PairWithMale(int x, int y);
    bool RandomlyPair(int i, int j, DIRECTION &direction); // the parameter direction indicates which neighbour is paired.
    
public:
	CNorthTerritory(void);
	~CNorthTerritory(void);

	void Initialize();
	void Move();
    void Infect();
    void Die();
    void Born();
    void PrintTotal(int year, Status status);
    void Print(int year);
    CCreature** GetRow(int row);
    void MergeRow(int row, CCreature** p);
    void CleanBoundries();
    Status GetStatus();
    
    void PrintTest();

private:
#if defined(_OPENMP)
    bool m_Locks[SIZE_X];
    void Lock(int i);
    void Unlock(int i);
#endif

};

