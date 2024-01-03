#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

struct teacher
{
	int id;
	char name[64];
};


int get_mem(struct teacher** tpp)
{
	struct teacher *tp = NULL;
	tp = (struct teacher*) malloc(sizeof(struct teacher));
	if (tp == NULL) {
		return -1;
	}

	tp->id = 100;
	strcpy(tp->name, "li4");

	*tpp = tp;

	return 0;
}

void free_teacher(struct teacher **tpp)
{
	if (tpp == NULL) {
		return;
	}

	struct teacher *tp = *tpp;

	if (tp != NULL) {
		free(tp);
		*tpp = NULL;
	}
}


int get_mem2(struct teacher* &tp)
{
	tp = (struct teacher*)malloc(sizeof(struct teacher));
	if (tp == NULL) {
		return -1;
	}
	tp->id = 300;
	strcpy(tp->name, "wang5");

	return 0;
}

void free_mem2(struct teacher * &tp)
{
	if (tp != NULL) {
		free(tp);
		tp = NULL;
	}
}


int main(void)
{
	struct teacher *tp = NULL;

	get_mem(&tp);
	cout << "id =" << tp->id << ", name = " << tp->name << endl;
	free_teacher(&tp);

	cout << "00000000000" << endl;

	get_mem2(tp);
	cout << "id =" << tp->id << ", name = " << tp->name << endl;
	free_mem2(tp);

	return 0;
}