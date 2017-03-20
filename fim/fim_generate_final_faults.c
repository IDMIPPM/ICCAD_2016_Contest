/*
	Comments
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef FIM_H
#include "fim.h"
#endif
#include "proof/fra/fra.h"
#include "proof/fraig/fraig.h"
#include "proof/cec/cec.h"

extern int Abc_NtkDarSec(Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Fra_Sec_t * p);
extern int Abc_NtkDarCec(Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int nConfLimit, int fPartition, int fVerbose);

//__________________________________________________________________________
//__________________________________________________________________________
// check two strash schemes on equivalence (sequential (buggy on release))

// most powerful Dsec
int equiv_sec(Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2)
{
	Fra_Sec_t SecPar, *pSecPar = &SecPar;

	// set defaults
	Fra_SecSetDefaultParams(pSecPar);
	pSecPar->TimeLimit = 0;
	pSecPar->fSilent = 1;
	// perform verification

	return Abc_NtkDarSec(pNtk1, pNtk2, pSecPar);;

}


// Dcec - not so fast (only combinational)
int equiv_Dcec(Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2)
{
	int fSat;
	int fVerbose;
	int nSeconds;
	int nConfLimit;
	int nInsLimit;
	int fPartition;
	int fMiter;

	// set defaults
	fSat = 0;
	fVerbose = 0;
	nSeconds = 20;
	nConfLimit = 10000;
	nInsLimit = 0;
	fPartition = 0;
	fMiter = 0;

	// perform equivalence checking
	return Abc_NtkDarCec(pNtk1, pNtk2, nConfLimit, fPartition, fVerbose);
}


//__________________________________________________________________________
//__________________________________________________________________________
// check two bench schemes on equivalence
// checker type:    
//					4 - equiv_sec (SEQUENTIAL Dsec function in abc.exe)
//					5 - equiv_Dcec (COMBINATIONAL Dcec function in abc.exe)

int check_sch(char * pFileName1, char * pFileName2, int checker_type)
{
	int result;
	Abc_Ntk_t *sch1, *bench1, *logic1;
	Abc_Ntk_t *sch2, *bench2, *logic2;

	//READING BENCH
	//bench1 = My_ReadBench(pFileName1, 1);
	//bench2 = My_ReadBench(pFileName2, 1);
	struct circuit * ckt1;
	struct circuit * ckt2;
	ckt1 = read_initial_circuit(pFileName1);
	ckt2 = read_initial_circuit(pFileName2);
	bench1 = Cir_2_Ntk(ckt1);
	bench2 = Cir_2_Ntk(ckt2);
	free_circuit(ckt1);
	free_circuit(ckt2);

	//CONVERTING TO LOGIC
	logic1 = Abc_NtkToLogic(bench1);
	logic2 = Abc_NtkToLogic(bench2);
	//STRUCTURALLY HASHING
	sch1 = Abc_NtkStrash(logic1, 1, 1, 1);
	sch2 = Abc_NtkStrash(logic2, 1, 1, 1);
	//////////////////////////////////////////////////////////////////////////
	//EQUIVALENCE
	switch (checker_type) {
	case 4:
		result = equiv_sec(sch1, sch2);
		break;
	case 5:
		result = equiv_Dcec(sch1, sch2);
		break;
	default:
		result = equiv_Dcec(sch1, sch2);
		break;
	}

	// SWEEP OUT
	Abc_NtkDelete(sch1);
	Abc_NtkDelete(sch2);
	Abc_NtkDelete(bench1);
	Abc_NtkDelete(bench2);
	Abc_NtkDelete(logic1);
	Abc_NtkDelete(logic2);

	return result;

}


// find identical scheme pairs
int identical_sch_pairs(int sch_num, char * dir)
{
	// variables
	char pFileName1[128];
	char pFileName2[128];
	int i, j;
	int res;
	int faults = sch_num;
	int *a;  // указатель на массив
	a = (int*)malloc(faults * sizeof(int));
	for (i = 0; i<faults; i++)
	{
		a[i] = i + 1;
		//printf("a[%d] = %d\n", i, a[i]);
	}

	//printf("%d\n", faults);

	for (i = 0; i < faults; i++) {
		for (j = i + 1; j < faults; j++) {
			sprintf(pFileName1, "%s//%d.bench", dir, i + 1);
			sprintf(pFileName2, "%s//%d.bench", dir, j + 1);
			//printf("%d - (%d %d)\n", faults, i+1, j+1);
			res = check_sch(pFileName1, pFileName2, 4);
			if (res == 1) {
				printf(" %d  %d \n", a[i], a[j]);
				a[j] = a[i];
				if (a[j] != faults) {
					i++;
					j = i;
				}
			}
			//printf("%d ", res);
		}
	}

	return 0;
}


//__________________________________________________________________________
//__________________________________________________________________________
// check two bench schemes on equivalence
// checker type:    
//					4 - equiv_sec (SEQUENTIAL Dsec function in abc.exe)
//					5 - equiv_Dcec (COMBINATIONAL Dcec function in abc.exe)
int fim_checker(Abc_Ntk_t * sch1, Abc_Ntk_t * sch2, int checker_type)
{
	int result;
	
	switch (checker_type) {
	case 4:
		result = equiv_sec(sch1, sch2);
		break;
	case 5:
		result = equiv_Dcec(sch1, sch2);
		break;
	default:
		result = equiv_Dcec(sch1, sch2);
		break;
	}
	return result;
}


// Передается набор Ntk схем и их количество, а также массив индексов подгруппы
// Возвращается разбиение подгруппы по группам
int *fim_exhaust_group(int *group_ind, int group_len, Abc_Ntk_t **fcircNtk, int flnum, int * starting_index, int seq) {
	int *group_faults;
	int i, j;
	int res;
	int index = *starting_index;
	// Inititalization
	group_faults = calloc(group_len, sizeof(int));
	for (i = 0; i < group_len; i++)
		group_faults[i] = -1;


	//printf("%d\n", faults);

	for (i = 0; i < group_len; i++) {
		for (j = i + 1; j < group_len; j++) {
			if (group_faults[j] == -1)
			{
				if (seq == 0)
				{
					res = fim_checker(fcircNtk[group_ind[i]], fcircNtk[group_ind[j]], 5);
					//printf("COMBINATIONAL\n");
				}
				else
				{
					res = fim_checker(fcircNtk[group_ind[i]], fcircNtk[group_ind[j]], 4);
					//printf("SEQUENTIAL \n");
				}
				//printf(" %d  %d \n", group_ind[i] + 1, group_ind[j] + 1);
				if (res == 1) {
					//printf("EQUALITY %d  %d \n", group_ind[i] + 1, group_ind[j] + 1);
					if (group_faults[i] == -1)
					{
						group_faults[i] = index;
						group_faults[j] = index;
						index++;
					}
					else
					{
						group_faults[j] = group_faults[i];
					}
					if (j != group_len) { // ТУТ МОЖЕТ БЫТЬ ГЛЮК
						i++;
						j = i;
					}
				}
			}
		}
	}
	*starting_index = index;
	return group_faults;
}


// Передается набор Ntk схем и их количество, а также массив предразбиения
// Возвращается финальный массив c разбиениям по группам
int *gen_final_faults(int *pregroup_faults, Abc_Ntk_t **fcircNtk, int flnum, int grp_num, int seq) {
	int *final_faults;
	int *group_ind;
	int *group_faults;
	int i, j, k, group_len;
	int starting_index = grp_num;
	// Inititalization
	final_faults = (int *) calloc(flnum, sizeof(int));
	for (i = 0; i < flnum; i++)
		final_faults[i] = -1;

	group_ind = (int *) calloc(flnum, sizeof(int));

	for (i = 0; i < grp_num; i++)
	{
		k = 0;
		for (j = 0; j < flnum; j++)
		{
			if (pregroup_faults[j] == i)
			{
				// заполняем массив group_ind, и считаем group_len
				group_ind[k] = j;
				k++;
			}

		}
		group_len = k;
		// подаем на fim_exhaust_group 
		group_faults = fim_exhaust_group(group_ind, group_len, fcircNtk, flnum, &starting_index, seq);
		// переписываем индексы в final_faults для этой группы по group_ind
		for (j = 0; j < group_len; j++)
		{
			if (group_ind[j] >= flnum) {
				printf("Trying to set value out of range of final_faults (%d >= %d)", group_ind[j], flnum);
				exit(0);
			}
			final_faults[group_ind[j]] = group_faults[j];
		}
		free(group_faults);
	}

	free(group_ind);
	return final_faults;
}


// OpenMP Version
int *fim_exhaust_group_omp(int *group_ind, int group_len, Abc_Ntk_t **fcircNtk, int flnum, int seq) {
	int *group_faults;
	int i, j;
	int res;
	int index = 0;

	// Inititalization
	group_faults = calloc(group_len, sizeof(int));
	for (i = 0; i < group_len; i++)
		group_faults[i] = -1;


	//printf("%d\n", faults);

	for (i = 0; i < group_len; i++) {
		for (j = i + 1; j < group_len; j++) {
			if (group_faults[j] == -1)
			{
				if (seq == 0)
				{
					res = fim_checker(fcircNtk[group_ind[i]], fcircNtk[group_ind[j]], 5);
					//printf("COMBINATIONAL\n");
				}
				else
				{
					res = fim_checker(fcircNtk[group_ind[i]], fcircNtk[group_ind[j]], 4);
					//printf("SEQUENTIAL \n");
				}
				//printf(" %d  %d \n", group_ind[i] + 1, group_ind[j] + 1);
				if (res == 1) {
					//printf("EQUALITY %d  %d \n", group_ind[i] + 1, group_ind[j] + 1);
					if (group_faults[i] == -1)
					{
						group_faults[i] = index;
						group_faults[j] = index;
						index++;
					}
					else
					{
						group_faults[j] = group_faults[i];
					}
					if (j != group_len) { // ТУТ МОЖЕТ БЫТЬ ГЛЮК
						i++;
						j = i;
					}
				}
			}
		}
	}
	return group_faults;
}


int *gen_final_faults_omp(int *pregroup_faults, Abc_Ntk_t **fcircNtk, int flnum, int seq) {
	int *final_faults;
	int i;

	// Inititalization
	final_faults = (int *)calloc(flnum, sizeof(int));
	for (i = 0; i < flnum; i++)
		final_faults[i] = -1;

	for (i = 0; i < flnum; i++)
	{
		int j, group_len;
		int *group_faults;
		int *group_ind;
		int k = 0;
		group_ind = (int *)calloc(flnum, sizeof(int));

		for (j = 0; j < flnum; j++)
		{
			if (pregroup_faults[j] == i)
			{
				// заполняем массив group_ind, и считаем group_len
				group_ind[k] = j;
				k++;
			}
		}
		group_len = k;
		// подаем на fim_exhaust_group 
		group_faults = fim_exhaust_group_omp(group_ind, group_len, fcircNtk, flnum, seq);
		// переписываем индексы в final_faults для этой группы по group_ind
		for (j = 0; j < group_len; j++)
		{
			if (group_ind[j] >= flnum) {
				printf("Trying to set value out of range of final_faults (%d >= %d)", group_ind[j], flnum);
				exit(0);
			}
			final_faults[group_ind[j]] = group_faults[j];
		}
		free(group_faults);
		free(group_ind);
	}

	// renaming groups
	int max = 1;
	int new_max = 0;
	int *group_ind = (int *)calloc(flnum, sizeof(int));

	for (i = 0; i < flnum; i++)
	{
		int j, group_len;
		int k = 0;
		for (j = 0; j < flnum; j++)
		{
			if (pregroup_faults[j] == i)
			{
				// заполняем массив group_ind, и считаем group_len
				group_ind[k] = j;
				k++;
			}

		}
		group_len = k;

		for (j = 0; j < group_len; j++)
		{
			if (final_faults[group_ind[j]] > new_max)
			{
				new_max = final_faults[group_ind[j]];
			}
			if (final_faults[group_ind[j]] != -1)
				final_faults[group_ind[j]] = final_faults[group_ind[j]] + max;
		}
		max = new_max + max + 1;
	}

	free(group_ind);
	return final_faults;
}


// В final_faults копируются obvious_faults (простейшая функция на несколько строчек)
void merge_obvious_and_final_faults(int *obvious_faults, int *final_faults, int flnum) {
	int i, index;
	int grp_lbl = -2;
	for (i = 0; i < flnum; i++) {
		if (obvious_faults[i] == -1)
			continue;
		index = obvious_faults[i];
		if (index >= flnum) {
			printf("Error: index out of memory! (%d >= %d)\n", index, flnum);
			exit(0);
		}
		if (final_faults[i] != -1) {
			printf("Warning: Final faults array already contain value at place %d, which is strange! Overwrite it with place %d\n", i, index);
		}
		if (final_faults[index] != -1) // add to existing group
			final_faults[i] = final_faults[index];
		else // create new group
		{
			//printf("%d - %d \n", i + 1, index + 1);
			final_faults[i] = grp_lbl;
			final_faults[index] = grp_lbl;
			grp_lbl--;
		}
	}
	return;
}
