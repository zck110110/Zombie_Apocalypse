
#include <iostream>
#include <stdlib.h>
#include "Creature.h"

using namespace std;

CCreature::CCreature(void)
{
}


CCreature::~CCreature(void)
{
}

CCreature::CCreature(TYPE type)
{
	this->type = type;

	// Randomly assign an age for human, ranging from 0 to 70 years old and be precise to days; If type is zambie, assign 0.
	if (ZOMBIE == type)
		this->age = 0;
	else
		this->age = (int)(70*365*drand48()); // days

	// Randomly assign a gender based on the actual gender probability, assuming this figure is same for both human and zambie
	if (drand48() < P_MALE)	
		this->gender = MALE;
	else
		this->gender = FEMALE;
}

CCreature::CCreature(TYPE type, int age)
{
    this->type = type;
    this->age = age;
    
    // Randomly assign a gender based on the actual gender probability, assuming this figure is same for both human and zambie
    if (drand48() < P_MALE)
        this->gender = MALE;
    else
        this->gender = FEMALE;
}

CCreature::CCreature(TYPE type, int age, GENDER gender)
{
	this->type = type;
	this->age = age;
	this->gender = gender;
}
 
TYPE CCreature::GetType()
{
	return this->type;
}

int CCreature::GetAge()
{
    if (HUMAN == this->type)
        return this->age / 365; // return by year for human
    
 	return age;
}

GENDER CCreature::GetGender()
{
 	return this->gender;
}

void CCreature::SetAge(int age)
{
    this->age = age;
}

void CCreature::SetGender(GENDER gender)
{
    this->gender = gender;
}

void CCreature::SetType(TYPE type)
{
    this->type = type;
}

void CCreature::Infect()
{
 	if (drand48() < INFECT_PROBABILITY)
 	{
        this->type = INFECTED;
        this->age = 0;
 	}
}

void CCreature::ChangeToZombie() //change to zombie
{
    if (INFECTED == this->type && this->age > 1)
    {
        this->type = ZOMBIE;
        this->age = 0;
    }
}

void CCreature::Grow() //grow up one day
{
    this->age++;
}



