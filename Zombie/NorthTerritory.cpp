
#include "NorthTerritory.h"
#include <iomanip>
#include <omp.h>
#include <stdlib.h>
using namespace std;

CNorthTerritory::CNorthTerritory(void)
{
}


CNorthTerritory::~CNorthTerritory(void)
{
}

void CNorthTerritory::Initialize()
{
    // Initialize random number generators
    for (int i = 0; i < 64; i++)
    {
        srand(i);
        m_MTRand[i].seed(drand48());
    }
    
	// Initialize Mesh, setting all cells to NULL.
	m_Mesh = CreateMesh();

	// Creating two Zombies, putting them in the middle of the Mesh.
	m_Mesh[SIZE_X/2][SIZE_Y/2] = new CCreature(ZOMBIE);
	m_Mesh[SIZE_X/2+1][SIZE_Y/2+1] = new CCreature(ZOMBIE);
    
	// Creating humans based on the population density
	for (int i = 1; i < SIZE_X - 1; i++)
		for (int j = 0; j < SIZE_Y; j++)
			if (m_MTRand[0].randExc() < POPULATION_DENSITY && NULL == m_Mesh[i][j])
            {
				m_Mesh[i][j] = new CCreature(HUMAN);
            }
    
    // Initialize locks of rows
    #if defined(_OPENMP)
    for (int i = 0; i < SIZE_X; i++)
        m_Locks[i] = false;
    #endif
}

#if defined(_OPENMP)
void CNorthTerritory::Lock(int i)
{
    int iUp     = (i - 1 + SIZE_X) % SIZE_X;
    int iDown   = (i + 1 + SIZE_X) % SIZE_X;

    for (bool lock = false; false == lock; )
    {
        #pragma omp critical (LockRegion)
        {
            lock = !m_Locks[iUp] && !m_Locks[i] && !m_Locks[iDown];
            if (lock)
            {
                m_Locks[iUp]    = true;
                m_Locks[i]      = true;
                m_Locks[iDown]  = true;
            }
        }
    }
}

void CNorthTerritory::Unlock(int i) // Index i should be ranging from 1 - SIZE_X - 1, since the first and last row are boundaries.
{
    int iUp     = (i - 1 + SIZE_X) % SIZE_X;
    int iDown   = (i + 1 + SIZE_X) % SIZE_X;

    #pragma omp critical (LockRegion)
    {
        m_Locks[iUp]    = false;
        m_Locks[i]      = false;
        m_Locks[iDown]  = false;
    }
}
#endif

void CNorthTerritory::Born()
{
    // refine the birth rate
    int Population = 0;
    int PotentialPaired = 1;
   
    CCreature*** MeshA = this->m_Mesh;
    MTRand* mtRand = this->m_MTRand;
    
    #if defined(_OPENMP)
    #pragma omp parallel for default(none) shared(MeshA, Population, PotentialPaired)
    #endif
    for (int i = 1; i < SIZE_X - 1; i++)
        for (int j = 0; j < SIZE_Y; j++)
        {
            if (NULL == MeshA[i][j])
                continue;
            
            if (HUMAN == MeshA[i][j]->GetType())
            {
                Population++;
                
                if (FEMALE == MeshA[i][j]->GetGender() && MeshA[i][j]->GetAge() >= 16 && MeshA[i][j]->GetAge() <= 55)
                {
                    DIRECTION d;
                    if (RandomlyPair(i, j, d))
                        PotentialPaired++;
                }
            }
        }
    
    double birthRate = (BIRTH_RATE/365) * (Population/PotentialPaired); // the refined birth rate.
    
    // Give birth based on th refined birth rate
    #if defined(_OPENMP)
    #pragma omp parallel for default(none) shared(MeshA, mtRand, birthRate)
    #endif
    for (int i = 1; i < SIZE_X - 1; i++)
    {
        int iUp, iDown, jLeft, jRight;
        int nIndex = 0; // Index of randow number generator mtRand that will be used in the thread, default is 0.
        
        #if defined(_OPENMP)
        Lock(i);
        nIndex = omp_get_thread_num() % RANDOM_GENERATOR;
        #endif
        
        for (int j = 0; j < SIZE_Y; j++)
        {
            if (NULL == MeshA[i][j])
                continue;
            
            // If it is a female human, and ageing from 16 to 55, probably let her pairs with neighbours in 4 directions,and then give birth.
            if (HUMAN == MeshA[i][j]->GetType() && FEMALE == MeshA[i][j]->GetGender() && MeshA[i][j]->GetAge() >= 16 && MeshA[i][j]->GetAge() <= 55 && mtRand[nIndex].randExc() < birthRate)
            {
                iUp     = (i - 1 + SIZE_X) % SIZE_X;
                iDown   = (i + 1 + SIZE_X) % SIZE_X;
                jLeft   = (j - 1 + SIZE_Y) % SIZE_Y;
                jRight  = (j + 1 + SIZE_Y) % SIZE_Y;
                
                DIRECTION direction;
                if (RandomlyPair(i, j, direction))
                {
                    if (UP == direction)
                    {
                        if (NULL == MeshA[iDown][j])
                            MeshA[iDown][j] = new CCreature(HUMAN, 0);
                        else if (NULL == MeshA[i][jLeft])
                            MeshA[i][jLeft] = new CCreature(HUMAN, 0);
                        else if (NULL == MeshA[i][jRight])
                            MeshA[i][jRight] = new CCreature(HUMAN, 0);
                    }
                    else if (DOWN == direction)
                    {
                        if (NULL == MeshA[iUp][j])
                            MeshA[iUp][j] = new CCreature(HUMAN, 0);
                        else if (NULL == MeshA[i][jLeft])
                            MeshA[i][jLeft] = new CCreature(HUMAN, 0);
                        else if (NULL == MeshA[i][jRight])
                            MeshA[i][jRight] = new CCreature(HUMAN, 0);
                    }
                    else if (LEFT == direction)
                    {
                        if (NULL == MeshA[i][jRight])
                            MeshA[i][jRight] = new CCreature(HUMAN, 0);
                        else if (NULL == MeshA[iUp][j])
                            MeshA[iUp][j] = new CCreature(HUMAN, 0);
                        else if (NULL == MeshA[iDown][j])
                            MeshA[iDown][j] = new CCreature(HUMAN, 0);
                    }
                    else if (RIGHT == direction)
                    {
                        if (NULL == MeshA[i][jLeft])
                            MeshA[i][jLeft] = new CCreature(HUMAN, 0);
                        else if (NULL == MeshA[iUp][j])
                            MeshA[iUp][j] = new CCreature(HUMAN, 0);
                        else if (NULL == MeshA[iDown][j])
                            MeshA[iDown][j] = new CCreature(HUMAN, 0);
                    }
                }
            }
        }
        
        #if defined(_OPENMP)
        Unlock(i);
        #endif
    }
}

void CNorthTerritory::Die()
{
    CCreature*** MeshA = this->m_Mesh;
    MTRand* mtRand = this->m_MTRand;
    
    #if defined(_OPENMP)
    #pragma omp parallel for default(none) shared(MeshA, mtRand)
    #endif
    for (int i = 1; i < SIZE_X - 1; i++)
    {
        int nIndex = 0; // Index of randow number generator mtRand that will be used in the thread, default is 0.
        
        #if defined(_OPENMP)
        nIndex = omp_get_thread_num() % RANDOM_GENERATOR;
        #endif
        
        for (int j = 0; j < SIZE_Y; j++)
        {
            if (NULL == MeshA[i][j])
                continue;
            
            switch (MeshA[i][j]->GetType())
            {
                case ZOMBIE:
                {
                    if (MeshA[i][j]->GetAge() > ZOMBIE_LIFESPAN) // Let zambie dies if a zambie's lifespan exceeds 270 days.
                    {
                        delete MeshA[i][j];
                        MeshA[i][j] = NULL;
                    }
                    break;
                }
                case HUMAN:
                {
                    double deathProbability = 0.0;
                    int age = MeshA[i][j]->GetAge();
                    
                    if (age <= 23)
                        deathProbability = DEATH_RATE/365 * 0.8;
                    else if (age <= 47)
                        deathProbability = DEATH_RATE/365;
                    else
                        deathProbability = DEATH_RATE/365 * 1.2;
                    
                    if (mtRand[nIndex].randExc() < deathProbability)
                    {
                        delete MeshA[i][j];
                        MeshA[i][j] = NULL;
                    }
                    break;
                }
                case INFECTED:
                    break;
            }
        }
    }
}

void CNorthTerritory::Infect()
{
    CCreature*** MeshA = this->m_Mesh;
    MTRand* mtRand = this->m_MTRand;
    
    #if defined(_OPENMP)
    #pragma omp parallel for default(none) shared(MeshA, mtRand)
    #endif
    for (int i = 1; i < SIZE_X - 1; i++)
    {
        int iUp, iDown, jLeft, jRight;
        int nIndex = 0; // Index of randow number generator mtRand that will be used in the thread, default is 0.
        
        #if defined(_OPENMP)
        Lock(i);
        nIndex = omp_get_thread_num() % RANDOM_GENERATOR;
        #endif

        for (int j = 0; j < SIZE_Y; j++)
        {
            if (MeshA[i][j] != NULL && ZOMBIE == MeshA[i][j]->GetType())
            {
                iUp     = (i - 1 + SIZE_X) % SIZE_X;
                iDown   = (i + 1 + SIZE_X) % SIZE_X;
                jLeft   = (j - 1 + SIZE_Y) % SIZE_Y;
                jRight  = (j + 1 + SIZE_Y) % SIZE_Y;

                double direction = mtRand[nIndex].randExc();//probablity for biting towards a certain direction
                
                if (direction < 2.5*MOVE && MeshA[iUp][j] != NULL && HUMAN == MeshA[iUp][j]->GetType()) // towards to up, except when i equals to the first row.
                {
                    MeshA[iUp][j]->Infect();
                }
                else if (direction < 5.0*MOVE && MeshA[iDown][j] != NULL && HUMAN == MeshA[iDown][j]->GetType()) // towards to down, except when i equals to the last row.
                {
                    MeshA[iDown][j]->Infect();
                }
                else if (direction < 7.5*MOVE && MeshA[i][jLeft] != NULL && HUMAN == MeshA[i][jLeft]->GetType()) // towards to left, except when j equals to the first column.
                {
                    MeshA[i][jLeft]->Infect();
                }
                else if (direction < 10*MOVE && MeshA[i][jRight] != NULL && HUMAN == MeshA[i][jRight]->GetType()) // towards to right, except when j equals to the last column.
                {
                    MeshA[i][jRight]->Infect();
                }
            }
        }
        
        #if defined(_OPENMP)
        Unlock(i);
        #endif
    }
}

void CNorthTerritory::Move()
{
    CCreature*** MeshB = CreateMesh();
    CCreature*** MeshA = this->m_Mesh;
    MTRand* mtRand = this->m_MTRand;
    
    #if defined(_OPENMP)
    #pragma omp parallel for default(none) shared(MeshA, MeshB, mtRand)
    #endif
    for (int i = 1; i < SIZE_X - 1; i++)
    {
        int iUp, iDown, jLeft, jRight;
        int nIndex = 0; // Index of randow number generator mtRand that will be used in the thread, default is 0.
        
        #if defined(_OPENMP)
        Lock(i);
        nIndex = omp_get_thread_num() % RANDOM_GENERATOR;
        #endif
        
        for (int j = 0; j < SIZE_Y; j++)
        {
            if (NULL == MeshA[i][j])
                continue;
            
            MeshA[i][j]->Grow(); // grow up 1 day.
            MeshA[i][j]->ChangeToZombie();
            
            iUp     = (i - 1 + SIZE_X) % SIZE_X;
            iDown   = (i + 1 + SIZE_X) % SIZE_X;
            jLeft   = (j - 1 + SIZE_Y) % SIZE_Y;
            jRight  = (j + 1 + SIZE_Y) % SIZE_Y;

            double move = mtRand[nIndex].randExc();
            if (move < 1.0*MOVE && NULL == MeshA[iUp][j] && NULL == MeshB[iUp][j])
            {
                MeshB[iUp][j] = MeshA[i][j];
                MeshA[i][j] = NULL;
            }
            else if (move < 2.0*MOVE && NULL == MeshA[iDown][j] && NULL == MeshB[iDown][j])
            {
                MeshB[iDown][j] = MeshA[i][j];
                MeshA[i][j] = NULL;
            }
            else if (move < 3.0*MOVE && NULL == MeshA[i][jLeft] && NULL == MeshB[i][jLeft])
            {
                MeshB[i][jLeft] = MeshA[i][j];
                MeshA[i][j] = NULL;
            }
            else if (move < 4.0*MOVE && NULL == MeshA[i][jRight] && NULL == MeshB[i][jRight])
            {
                MeshB[i][jRight] = MeshA[i][j];
                MeshA[i][j] = NULL;
            }
            else
            {
                MeshB[i][j] = MeshA[i][j];
                MeshA[i][j] = NULL;
            }
        }
        
        #if defined(_OPENMP)
        Unlock(i);
        #endif
    }
    
    DeleteMesh(MeshA);  // Swap MeshA and MeshB
    this->m_Mesh = MeshB;
    MeshB = NULL;
}

CCreature** CNorthTerritory::GetRow(int row)
{
    return m_Mesh[row];
}

void CNorthTerritory::MergeRow(int row, CCreature **p)
{
    for (int j = 0; j < SIZE_Y; j++)
    {
        if (p[j] != NULL && NULL == m_Mesh[row][j])
            m_Mesh[row][j] = p[j];
    }
}

CCreature*** CNorthTerritory::CreateMesh()
{
	CCreature*** Mesh = new CCreature**[SIZE_X];

	for (int i = 0; i < SIZE_X; i++)
	{
		Mesh[i] = new CCreature*[SIZE_Y];

		for (int j = 0; j < SIZE_Y; j++)
			Mesh[i][j] = NULL;
	}

	return Mesh;
}

void CNorthTerritory::DeleteMesh(CCreature*** Mesh)
{
	for (int i = 0; i < SIZE_X; i++)
		delete [] Mesh[i];

	delete [] Mesh;
}

bool CNorthTerritory::RandomlyPair(int i, int j, DIRECTION &direction)
{
    int iUp     = (i - 1 + SIZE_X) % SIZE_X;
    int iDown   = (i + 1 + SIZE_X) % SIZE_X;
    int jLeft   = (j - 1 + SIZE_Y) % SIZE_Y;
    int jRight  = (j + 1 + SIZE_Y) % SIZE_Y;
    
    double d = drand48();
    if (d < 0.25)
    {
        if (PairWithMale(iUp, j))
        {
            direction = UP;
            return true;
        }
    }
    else if (d < 5.0)
    {
        if (PairWithMale(iDown, j))
        {
            direction = DOWN;
            return  true;
        }
    }
    else if (d < 7.5)
    {
        if (PairWithMale(i, jLeft))
        {
            direction = LEFT;
            return true;
        }
    }
    else if (d < 1.0)
    {
        if (PairWithMale(i, jRight))
        {
            direction = RIGHT;
            return true;
        }
    }
    
    return false;
}

bool CNorthTerritory::PairWithMale(int x, int y)
{
    if (m_Mesh[x][y] != NULL
        && HUMAN == m_Mesh[x][y]->GetType()
        && MALE == m_Mesh[x][y]->GetGender()
        && m_Mesh[x][y]->GetAge() >= 16
        && m_Mesh[x][y]->GetAge() <= 55)
    {
        return true;
    }
    
    return false;
}

Status CNorthTerritory::GetStatus()
{
    Status status;
    status.nHuman = 0;
    status.nFemale = 0;
    status.nMale = 0;
    status.nZambie = 0;
    status.nInfected = 0;
    
    for (int i = 1; i < SIZE_X - 1; i++)
        for (int j = 0; j < SIZE_Y; j++)
        {
            if (NULL == m_Mesh[i][j])
                continue;
            
            switch (m_Mesh[i][j]->GetType())
            {
                case HUMAN:
                    status.nHuman++;
                    if (FEMALE == m_Mesh[i][j]->GetGender())
                        status.nFemale++;
                    else
                        status.nMale++;
                    break;
                case ZOMBIE:
                    status.nZambie++;
                    break;
                case INFECTED:
                    status.nInfected++;
            }
        }
    
    return status;
}

void CNorthTerritory::PrintTotal(int year, Status status)
{
    for (int i = 1; i < SIZE_X - 1; i++)
        for (int j = 0; j < SIZE_Y; j++)
        {
            if (NULL == m_Mesh[i][j])
                continue;
            
            switch (m_Mesh[i][j]->GetType())
            {
                case HUMAN:
                    status.nHuman++;
                    if (FEMALE == m_Mesh[i][j]->GetGender())
                        status.nFemale++;
                    else
                        status.nMale++;
                    break;
                case ZOMBIE:
                    status.nZambie++;
                    break;
                case INFECTED:
                    status.nInfected++;
            }
        }
    
    cout<<left<<setw(4)<<year<<right<<setw(8)<<status.nHuman<<setw(9)<<status.nFemale<<setw(7)<<status.nMale<<setw(9)<<status.nZambie<<setw(11)<<status.nInfected<<endl;
}

void CNorthTerritory::Print(int year)
{
    int nHuman = 0;
    int nFemale = 0;
    int nMale = 0;
    int nZambie = 0;
    int nInfected = 0;
    
    for (int i = 1; i < SIZE_X - 1; i++)
        for (int j = 0; j < SIZE_Y; j++)
        {
            if (NULL == m_Mesh[i][j])
                continue;
            
            switch (m_Mesh[i][j]->GetType())
            {
                case HUMAN:
                    nHuman++;
                    if (FEMALE == m_Mesh[i][j]->GetGender())
                        nFemale++;
                    else
                        nMale++;
                    break;
                case ZOMBIE:
                    nZambie++;
                    break;
                case INFECTED:
                    nInfected++;
            }
        }
    
    cout<<left<<setw(4)<<year<<right<<setw(8)<<nHuman<<setw(9)<<nFemale<<setw(7)<<nMale<<setw(9)<<nZambie<<setw(11)<<nInfected<<endl;
}

void CNorthTerritory::CleanBoundries()
{
    for (int j = 0; j < SIZE_Y; j++)
    {
        if (m_Mesh[0][j] != NULL)
        {
            delete m_Mesh[0][j];
            m_Mesh[0][j] = NULL;
        }
        
        if (m_Mesh[SIZE_X - 1][j] != NULL)
        {
            delete m_Mesh[SIZE_X - 1][j];
            m_Mesh[SIZE_X - 1][j] = NULL;
        }
    }
}

void CNorthTerritory::PrintTest()
{
    int total = 0;
    for (int i = 0; i < SIZE_X; i++)
    {
        for (int j = 0; j < SIZE_Y; j++)
        {
            if (m_Mesh[i][j] != NULL && ZOMBIE == m_Mesh[i][j]->GetType())
                cout<<"0"<<" ";
            else if (m_Mesh[i][j] != NULL && HUMAN == m_Mesh[i][j]->GetType())
                cout<<"1"<<" ";
            else
                cout<<"_"<<" ";
        }
        cout<<endl;
    }
}