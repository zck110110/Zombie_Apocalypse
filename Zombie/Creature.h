#pragma once

#define P_MALE 0.5
#define INFECT_PROBABILITY  0.4
#define ZOMBIE_LIFESPAN     120       // life span for zambie, by days

enum TYPE{ ZOMBIE, HUMAN, INFECTED };
enum GENDER{ MALE, FEMALE };

class CCreature
{
private:
	TYPE type;	//zombie:1,human:-1,infected:0
 	int age;	// baby,young ,adult, old
 	GENDER gender; // 1 for male , 0 for female

public:
	CCreature(void);
	CCreature(TYPE type);
    CCreature(TYPE type, int age);
	CCreature(TYPE type, int age, GENDER gender);
	~CCreature(void);

public:
	TYPE GetType();
	int GetAge();
	GENDER GetGender();
    void SetType(TYPE type);
    void SetAge(int age);
    void SetGender(GENDER gender);
	void Infect();
	void ChangeToZombie();
	void Grow();
	bool ToDie();
};

