#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "Employee.h"
#include "Technician.h"
#include "Manager.h"
#include "SalesMan.h"
#include "SalesManager.h"

using namespace std;

int main(void)
{

#if 0
	Employee *em1 = new Technician;

	em1->getPay();
	em1->upLevel(1);
	em1->displayStatus();

	delete em1;
#endif
#if 0
	Employee *em = new Manager;
	em->upLevel(1);
	em->getPay();
	em->displayStatus();
	
	delete em;
#endif 
#if 0
	Employee *em = new SalesMan;

	em->upLevel(0);
	em->getPay();
	em->displayStatus();

	delete em;
#endif
#if 0
	Employee *em = new SalesManager;
	
	em->init();
	em->upLevel(1);
	em->getPay();
	em->displayStatus();

	delete em;
#endif
	Employee *person_array[] = {new Technician, new Manager, new SalesMan, new SalesManager};

	for (int i = 0; i < sizeof(person_array) / sizeof(person_array[0]); i++) {
		person_array[i]->init();
		person_array[i]->upLevel(0);
		person_array[i]->getPay();
		person_array[i]->displayStatus();
	}

	for (int i = 0; i < sizeof(person_array) / sizeof(person_array[0]); i++) {
		delete person_array[i];
	}



	return 0;
}