// Zambie.cpp : Defines the entry point for the console application.
//

#include <iomanip>
#include "NorthTerritory.h"
#include "MPICommunicator.h"
using namespace std;

int main(int argc, char **argv) 
{
    CMPICommunicator Comm;
    Comm.Init(argc, argv);
    
    CNorthTerritory NorthTerritory;
    NorthTerritory.Initialize();
 
    if (Comm.GetRank() == PROC1)
        cout<<setw(4)<<"Year"<<setw(8)<<"Human"<<setw(9)<<"Female"<<setw(7)<<"Male"<<setw(9)<<"Zombie"<<setw(11)<<"Infected"<<endl;
    
    for (int year = 1; year <= 10; year++)        // year loop
    {
        for (int day = 0; day < 365; day++)
        {
            NorthTerritory.Move();
            NorthTerritory.Born();
            NorthTerritory.Die();
            NorthTerritory.Infect();
        
            if (Comm.GetRank() == PROC1)
            {
                Comm.Send(PROC2, NorthTerritory.GetRow(0));
                Comm.Send(PROC2, NorthTerritory.GetRow(SIZE_X - 1));
            
                CCreature** p = Comm.Receive(PROC2);
                NorthTerritory.MergeRow(SIZE_X - 2, p);
                delete [] p;
            
                p = Comm.Receive(PROC2);
                NorthTerritory.MergeRow(1, p);
                delete [] p;
            }
            else if (Comm.GetRank() == PROC2)
            {
                CCreature** p = Comm.Receive(PROC1);
                NorthTerritory.MergeRow(SIZE_X - 2, p);
                delete [] p;
            
                p = Comm.Receive(PROC1);
                NorthTerritory.MergeRow(1, p);
                delete [] p;
            
                Comm.Send(PROC1, NorthTerritory.GetRow(0));
                Comm.Send(PROC1, NorthTerritory.GetRow(SIZE_X - 1));
            }
        
            NorthTerritory.CleanBoundries();
        }
    
        if (Comm.GetRank() == PROC1)
        {
            Status status = Comm.ReceiveStatus(PROC2);
            NorthTerritory.PrintTotal(year, status);
        }
        else if (Comm.GetRank() == PROC2)
        {
            Status status = NorthTerritory.GetStatus();
            Comm.SendStatus(PROC1, status);
        }
    }
    
    Comm.Finalize();
}

