/*
	Comments
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef FIM_H
#include "fim.h"
#endif
#include "base/abci/abcXsim.c"

static inline int  My_Abc_XsimRand2()
{
	    return (rand() & 1) ? XVS1 : XVS0;
	//return (Gia_ManRandom(20) & 1) ? XVS1 : XVS0;
}


int arrcmp(int * a, int * b, int len)
{
	int i;
	for (i = 0; i < len; i++)
	{
		if (a[i] != b[i])
		{
			return 1;
		}
	}
	return 0;
}

// simulate seq and comb circuit
int * simulate(Abc_Ntk_t * pNtk, int nFrames, int outputs, int seed)
{
	int *out = (int*) malloc(nFrames * outputs * sizeof(int));
	Abc_Obj_t * pObj;
	int i, f;
	assert(Abc_NtkIsStrash(pNtk));
	srand(seed);
	//Gia_ManRandom(20);

	// start simulation
	Abc_ObjSetXsim(Abc_AigConst1(pNtk), XVS1);

	Abc_NtkForEachPi(pNtk, pObj, i)
		Abc_ObjSetXsim(pObj, My_Abc_XsimRand2());

	Abc_NtkForEachLatch(pNtk, pObj, i)
		Abc_ObjSetXsim(Abc_ObjFanout0(pObj), Abc_LatchInit(pObj));

	// simulate and print the result
	//fprintf(stdout, "Frame : Inputs : Latches : Outputs\n");
	for (f = 0; f < nFrames; f++)
	{
		Abc_AigForEachAnd(pNtk, pObj, i)
			Abc_ObjSetXsim(pObj, Abc_XsimAnd(Abc_ObjGetXsimFanin0(pObj), Abc_ObjGetXsimFanin1(pObj)));
		Abc_NtkForEachCo(pNtk, pObj, i)
			Abc_ObjSetXsim(pObj, Abc_ObjGetXsimFanin0(pObj));


		// print out
		Abc_NtkForEachPo(pNtk, pObj, i)
			out[f*outputs + i] = Abc_ObjGetXsim(pObj) - 1;
			//snprintf(out[f*outputs + i], sizeof(int), "%d", (Abc_ObjGetXsim(pObj) - 1));
		
		//Abc_NtkForEachPo(pNtk, pObj, i)
		//	Abc_XsimPrint(stdout, Abc_ObjGetXsim(pObj));


		// assign input values
		Abc_NtkForEachPi(pNtk, pObj, i)
			Abc_ObjSetXsim(pObj, My_Abc_XsimRand2());
		// transfer the latch values
		Abc_NtkForEachLatch(pNtk, pObj, i)
			Abc_ObjSetXsim(Abc_ObjFanout0(pObj), Abc_ObjGetXsim(Abc_ObjFanin0(pObj)));
	}
	return out;
}


void pregroup(int sch_num, int tst_num, char * dir)
{
	// variables
	char pFileName[20];
	Abc_Ntk_t *tmp, *tmp1, *tmp2;
	int *out;
	int i, j, k;
	int inputs, outputs;
	int *input_pattern;  // указатель на массив
	srand(time(NULL));
	char sign;

	Abc_Ntk_t **ckts;  // array of circuits
	ckts = (Abc_Ntk_t**) malloc(sch_num * sizeof(Abc_Ntk_t *));
	//READING ARRAY OF CIRCUITS
	for (i = 0; i < sch_num; i++)
	{
		sprintf(pFileName, "%s//%d.bench", dir, i + 1);
		//READING BENCH
		//tmp1 = My_ReadBench(pFileName, 1);
		struct circuit * ckt1;
		ckt1 = read_initial_circuit(pFileName);
		tmp1 = Cir_2_Ntk(ckt1);
		free_circuit(ckt1);

		//CONVERTING TO LOGIC
		tmp2 = Abc_NtkToLogic(tmp1);
		//STRUCTURALLY HASHING
		tmp = Abc_NtkStrash(tmp2, 1, 1, 1);
		ckts[i] = tmp;
		//Abc_NtkDelete(tmp);
		Abc_NtkDelete(tmp1);
		Abc_NtkDelete(tmp2);
		//printf("a[%d] = %d\n", i, a[i]);
	}

	inputs = ckts[0]->vPis->nSize;
	outputs = ckts[0]->vPos->nSize;
	input_pattern = (int*)malloc(inputs * sizeof(int));
	//SIGNATURES ARRAY
	char **signs = (char **)malloc(outputs * tst_num * sizeof(char *));
	for (i = 0; i < outputs * tst_num; i++) {
		signs[i] = (char *)malloc(sch_num * sizeof(char));
	}

	for (j = 0; j < tst_num; j++) {
		//GENERATE INPUT PATTERN
		//fprintf(stdout, "\n TEST %d \n", j + 1);
		for (i = 0; i < inputs; i++) {
			input_pattern[i] = rand() % 2;
		}
		//FILL IN SIGN ARRAY
		for (i = 0; i < sch_num; i++) {
			out = Abc_NtkVerifySimulatePattern(ckts[i], input_pattern);
			fprintf(stdout, "\n SCHEME %d \n", i + 1);
			for (k = 0; k < outputs; k++) {
				fprintf(stdout, "%d", out[k]);
				sign = (char)out[k];
				signs[k + j*outputs][i] = sign;
			}
		}
	}
	//WRITE TO FILE
	FILE *file;
	if ((file = fopen("sign.txt", "w")) == NULL)
		printf("‘айл невозможно открыть или создать\n");
	else {
		for (j = 0; j < sch_num; j++) {
			for (i = 0; i < outputs*tst_num; i++) {
				fprintf(file, "%d", signs[i][j]);

			}
			fwrite("\n", sizeof(char), 1, file);
		}
	}
	fclose(file);

	//CLEAR MEMORY
	for (i = 0; i < outputs * tst_num; i++) {
		free(signs[i]);
	}
	free(signs);
}


void pregroup2(int sch_num, int tst_num, char * dir)
{
	// variables
	char pFileName[20];
	Abc_Ntk_t *tmp, *tmp1, *tmp2;
	int *out;
	int i, j;
	int inputs, outputs;
	srand(time(NULL));

	Abc_Ntk_t **ckts;  // array of circuits
	ckts = (Abc_Ntk_t**) malloc(sch_num * sizeof(Abc_Ntk_t *));
	//READING ARRAY OF CIRCUITS

	for (i = 0; i < sch_num; i++)
	{
		sprintf(pFileName, "%s//%d.bench", dir, i + 1);
		//READING BENCH
		//tmp1 = My_ReadBench(pFileName, 1);
		struct circuit * ckt1;
		ckt1 = read_initial_circuit(pFileName);
		tmp1 = Cir_2_Ntk(ckt1);
		free_circuit(ckt1);
		//CONVERTING TO LOGIC
		tmp2 = Abc_NtkToLogic(tmp1);
		//STRUCTURALLY HASHING
		tmp = Abc_NtkStrash(tmp2, 1, 1, 1);
		ckts[i] = tmp;
		//Abc_NtkDelete(tmp);
		Abc_NtkDelete(tmp1);
		Abc_NtkDelete(tmp2);

		//printf("a[%d] = %d\n", i, a[i]);
	}

	inputs = ckts[0]->vPis->nSize;
	outputs = ckts[0]->vPos->nSize;
	//WRITE TO FILE

	FILE *file;
	if ((file = fopen("sign.txt", "w")) == NULL)
		printf("Файл невозможно открыть или создать\n");
	else {
		for (j = 0; j < sch_num; j++) {
			out = simulate(ckts[j], tst_num, outputs, 20);
			for (i = outputs; i < outputs*tst_num; i++) {
				fprintf(file, "%d", out[i]);

			}
			fwrite("\n", sizeof(char), 1, file);
		}
	}
	fclose(file);
}


void masked_errors(Abc_Ntk_t **fcircNtk, int flnum, int *obvious_faults, Abc_Ntk_t *etalon, int seq)
{
	int j, res;
	int flag = 0;
	int masked = 0;
	//printf("WTF&&&\n");
	for (j = 0; j < flnum; j++)
	{
		if (fcircNtk[j] == NULL)
			continue;
		
		if (seq == 0)
		{
			res = fim_checker(etalon, fcircNtk[j], 5);
			//printf("COMBINATIONAL\n");
		}
		else
		{
			res = fim_checker(etalon, fcircNtk[j], 4);
			//printf("SEQUENTIAL \n");
		}
		if (res == 1)
		{
			if (flag == 0)
			{
				flag = j;
			}
			else
			{
				Abc_NtkDelete(fcircNtk[j]);
				fcircNtk[j] = NULL;
				obvious_faults[j] = flag;
				masked++;

			}
		}
	}
	printf("		... %d masked faults found \n", masked);
}


// Передается толкьо набор Ntk схем и их количество
// pregroup_faults - массив в котором напротив каждой схемы указан ID: группы к которой схема относится (от 0 до N-1 - где N число групп)
// для пропущенных схем указан -1
int *gen_pregroup_faults(Abc_Ntk_t **fcircNtk, int flnum, int tst_num, int seed, int *group_num) {
	int *pregroup;
	int * out;
	int i, j, outputs, uniq_cls, cls, eq;
	int *classes;  // массив классов

	// number of outputs
	outputs = fcircNtk[0]->vPos->nSize;
	
	// Inititalization
	int *temp = (int*)malloc(flnum * sizeof(int));
	classes = (int*)malloc(outputs * tst_num * flnum * sizeof(int));
	pregroup = calloc(flnum, sizeof(int));
	for (i = 0; i < flnum; i++)
	{
		pregroup[i] = -1;
		temp[i] = 0;
	}

	// first simulation
	
	out = simulate(fcircNtk[0], tst_num, outputs, seed);
	for (i = 0; i < outputs * tst_num; i++)
		classes[i] = out[i];
	// number of uniq classes
	uniq_cls = 1;
	pregroup[0] = 0;
	temp[0] = 1;
	for (j = 1; j < flnum; j++)
	{
		if (fcircNtk[j] == NULL)
			continue;
		out = simulate(fcircNtk[j], tst_num, outputs, seed);
		for (cls = 0; cls < uniq_cls; cls++)
		{
			eq = 0;
			for (i = 0; i < tst_num*outputs; i++)
			{
				if (classes[cls * outputs * tst_num + i] != out[i])
					eq = 1;
			}
			
			//eq = arrcmp(out, temp, tst_num*outputs);
			if (eq == 0)
			{
				pregroup[j] = cls;
				temp[cls]++;
				break;
			}
		}
		// create new uniq class
		if (cls == uniq_cls)
		{
			for (i = 0; i<tst_num*outputs; i++)
			{
				classes[cls*tst_num*outputs + i] = out[i];
			}
			pregroup[j] = cls;
			uniq_cls++;
			temp[cls] = 1;
		}

	}

	int max = 0;
	for (i = 0; i<flnum; i++)
	{
		if (max < temp[i])
			max = temp[i];
		if (temp[i] == 1)
		{
			for (j = 0; j < flnum; j++)
			{
				if (pregroup[j] == i)
				{
					pregroup[j] = -1;
					uniq_cls--;
					break;
				}
			}
		}		
	}

	printf("		... %d groups after pregrouping \n", uniq_cls);
	printf("		... biggest - %d \n", max);
	*group_num = uniq_cls;
	return pregroup;
}


// КАТИНО
int * gen_matrix(Abc_Ntk_t **fcircNtk, int flnum, int tst_num, int* M, int seed)
{
	int j, i, outputs;
	int * out;
	int * final_matrix;
	int ind = 0;

	outputs = fcircNtk[0]->vPos->nSize;
	*M = outputs*tst_num;
	final_matrix = (int*)malloc(outputs * tst_num * flnum * sizeof(int));

	for (j = 0; j < flnum; j++)
	{
		if (fcircNtk[j] == NULL)
			for (i = 0; i < outputs*tst_num; i++)
			{
				final_matrix[ind] = -1;
				ind++;
			}
		else
		{
			out = simulate(fcircNtk[j], tst_num, outputs, seed);
			for (i = 0; i < outputs*tst_num; i++)
			{
				final_matrix[ind] = out[i];
				ind++;
			}
		}

	}
	return final_matrix;
}


void Print(int** Groups, int G_row_count, int* G_col_count, int n)
{
	int i, j;
	for (i = 0; i < G_row_count; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (j < G_col_count[i])
			{
				printf("%d ", Groups[i][j]);
			}
			else
			{
				printf("x ");
			}

		}

		printf("\r\n");
	}
	printf("\r\n");
}


int Compare(int *row_1, int *row_2, int lenght)
{
	int i;
	for (i = 0; i < lenght; i++)
	{
		if (row_1[i] != row_2[i])
			return 0;
	}
	return 1;
}


void Grouping(int** A, int n, int m, int** Groups, int* G_row_count, int* G_col_count)
{
	int i, j;

	(*G_row_count) = 0;
	for (i = 0; i < n; i++) G_col_count[i] = 0;

	Groups[0][0] = 0;
	(*G_row_count)++;
	G_col_count[0] ++;

	for (i = 1; i < n; i++)
	{
		int find = 0;
		for (j = 0; j < (*G_row_count); j++)
		{
			if (1 == Compare(A[i], A[Groups[j][0]], m))
			{
				Groups[j][G_col_count[j]] = i;
				G_col_count[j] ++;
				find = 1;
				break;
			}
		}
		if (1 != find)
		{
			Groups[*G_row_count][0] = i;
			G_col_count[*G_row_count] = 1;
			(*G_row_count)++;
		}
	}
}


void Out_Groups(int** A, int n, int m, int** Groups, int* G_row_count, int* G_col_count, int* Out_cir)
{
	int i, j;
	int* ONE;
	ONE = calloc(m, sizeof(int));
	for (i = 0; i < m; i++) ONE[i] = -1;

	int* FLAG;
	FLAG = calloc((*G_row_count), sizeof(int));
	for (i = 0; i < (*G_row_count); i++) FLAG[i] = -1;


	for (i = 0; i < (*G_row_count); i++)
	{
		//printf("%d \n", Groups[i][0]);
		//printf("%d ", A[Groups[i][0]]);
		if (Compare(A[Groups[i][0]], ONE, m))
		{
			FLAG[i] = 1;
		}
		else
		{
			FLAG[i] = 0;
		}
	}

	for (i = 0; i < (*G_row_count); i++)
	{
		if (G_col_count[i] == 1 && FLAG[i] == 0)
		{
			Out_cir[Groups[i][0]] = -1;
		}
		else
		{
			for (j = 0; j < G_col_count[i]; j++)
			{
				if (FLAG[i] == 0)
				{
					Out_cir[Groups[i][j]] = i;
				}
				else
				{
					Out_cir[Groups[i][j]] = -1;
				}

			}
		}
	}
}


void Input_data(int *fcircNtk, int n, int m, int** A)
{
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
			A[i][j] = fcircNtk[i*m + j];
	}
}


void matrix_pregroup_faults(Abc_Ntk_t **fcircNtk, int flnum, int tst_num, int *Out_cir, int seed)
{
	int i;
	int * matrix;
	int ind = 0;
	int M;

	// Matrix generation
	matrix = gen_matrix(fcircNtk, flnum, tst_num, &M, seed);
	//printf("\r\n");
	//printf("%d M = ", M);
	//printf("\r\n");

	int** A;
	int** Groups;
	int G_row_count;
	int* G_col_count;
	//int* Out_cir;


	A = calloc(flnum, sizeof(int*));
	for (i = 0; i < flnum; i++)
		A[i] = calloc(M, sizeof(int));

	Groups = calloc(flnum, sizeof(int*));
	for (i = 0; i < flnum; i++)
		Groups[i] = calloc(M, sizeof(int));

	G_col_count = calloc(flnum, sizeof(int));
	//Out_cir = calloc(flnum, sizeof(int));

	Input_data(matrix, flnum, M, A);

	Grouping((int**)A, flnum, M, (int**)Groups, &G_row_count, G_col_count);
	//Print(Groups, G_row_count, G_col_count, flnum);

	Out_Groups(A, flnum, M, Groups, &G_row_count, G_col_count, Out_cir);

	//for (i = 0; i < flnum; i++)
	//{
	//	printf("%d : %d ", i, Out_cir[i]);
	//}
	//printf("\n");

	int max = 0;
	int n_groups = G_row_count;
	for (i = 0; i < flnum; i++)
	{
		free(A[i]);
		free(Groups[i]);
		if (G_col_count[i] > max)
			max = G_col_count[i];
		if (G_col_count[i] == 1)
			n_groups--;
	}
	free(A);
	free(Groups);
	free(G_col_count);
	//free(Out_cir);

	// Print matrix
	/*for (j = 0; j < flnum; j++)
	{
	if (fcircNtk[j] == NULL)
	continue;
	for (i = 0; i < M; i++)
	{
	printf("%d", matrix[ind]);
	ind++;
	}
	printf("\n");
	}
	*/

	// Print statistics
	printf("		... %d groups after pregrouping \n", n_groups);
	printf("		... biggest - %d \n", max);
}


// ИТЕРАТИВНОЕ

int * gen_matrix_iter(Abc_Ntk_t **fcircNtk, int flnum, int tst_num, int* M, int seed, int* group, int grp_len)
{
	int j, i, outputs;
	int * out;
	int * final_matrix;
	int ind = 0;

	outputs = fcircNtk[0]->vPos->nSize;
	*M = outputs*tst_num;
	final_matrix = (int*)malloc(outputs * tst_num * flnum * sizeof(int));

	for (j = 0; j < grp_len; j++)
	{
		out = simulate(fcircNtk[group[j]], tst_num, outputs, seed);
		//printf("%d ", group[j]);
		for (i = 0; i < outputs*tst_num; i++)
		{
			final_matrix[ind] = out[i];
			ind++;
		}
		free(out);
	}
	
	return final_matrix;
}


void Input_data_iter(int *matrix, int flnum, int m, int** A)
{
	int i, j;
	for (i = 0; i < flnum; i++)
	{
		for (j = 0; j < m; j++)
			A[i][j] = matrix[i*m + j];
	}
}

// разбиение конкретной подгруппы, задаваемой mask с заданным числом тестов
// update -ится матрица Groups, число G_row_count и массив G_col_count
int pregroup_iteration(Abc_Ntk_t **fcircNtk, int fl_num, int tst_num, int** Groups, int* G_row_count, int row, int* G_col_count, int seed)
{
	int i, j;
	int first_el; // первый элемент рассматриваемой группы
	int grp_len; // длина группы
	int * matrix; // матрица реакций на воздействия
	int * ind_arr;
	int** A; // матрица реакций на воздействия
	int m; // длина строчки матрицы
	int rows; // сколько изначально было строк
			  /*
			  for (i = 0; i < fl_num; i++) G_col_count[i] = 0;

			  Groups[0][0] = 0;
			  (*G_row_count)++;
			  G_col_count[0] ++;
			  */

			  // первый элемент рассматриваемой подгруппы
	first_el = Groups[row][0];
	// длина подгруппы
	grp_len = G_col_count[row];
	rows = (*G_row_count);
	// генерация матрицы реакций на воздействия
	matrix = gen_matrix_iter(fcircNtk, fl_num, tst_num, &m, seed, Groups[row], grp_len);
	A = calloc(grp_len, sizeof(int*));
	for (i = 0; i < grp_len; i++)
		A[i] = calloc(m, sizeof(int));
	Input_data_iter(matrix, grp_len, m, A);

	/*
	for (i = 0; i < grp_len; i++)
	{
		for (j = 0; j < m; j++)
			printf("%d", A[i][j]);
		printf("\n");
	}
	*/

	// создание массива индексов элементов подгруппы ()
	ind_arr = calloc(fl_num, sizeof(int));
	for (i = 0; i < grp_len; i++)
		ind_arr[Groups[row][i]] = i;

	// кладем первый элемент группы вместо всей группы
	G_col_count[row] = 1; // в рассматриваемой группе типа один элемент
	Groups[row][0];

	// начинаем сравнение 
	
	for (i = 1; i < grp_len; i++)
	{
		int find = 0;
		// сравнение с первым элементом (заполняем ту самую группу которую обрабатываем)
		if (1 == Compare(A[i], A[0], m))
		{
			Groups[row][G_col_count[row]] = Groups[row][i];
			G_col_count[row] ++;
			find = 1;
			//printf("	EQUAL TO FIRST GROUP \n");
		}
		else // сравнение с последующими (создаем новые подгруппы)
			for (j = rows; j < (*G_row_count); j++)
			{
				if (1 == Compare(A[i], A[ind_arr[Groups[j][0]]], m)) // КОСЯЯЯЯЯЯЯЯЯЯЯЯЯЯК
				{
					Groups[j][G_col_count[j]] = Groups[row][i];
					G_col_count[j] ++;
					find = 1;
					//printf("	EQUAL \n");
					//printf("%d - %d \n", Groups[row][i], Groups[j][0]);
					break;
				}
			}
		
		if (1 != find)
		{
			//printf(" NOT EQUAL \n create new group %d \n", Groups[row][i]);
			Groups[*G_row_count][0] = Groups[row][i];
			G_col_count[*G_row_count] = 1;
			(*G_row_count)++;

		}
	}
	// free memory
	for (i = 0; i < grp_len; i++)
		free(A[i]);
	free(A);
	free(matrix);
	free(ind_arr);
	if (rows == (*G_row_count))
		return 1;
	else
		return 0;
}


void Out_Groups_iter(int** Groups, int* G_row_count, int* G_col_count, int* pregroup)
{
	int i, j, ind=0;
	for (j = 0; j < (*G_row_count); j++)
	{
		if (G_col_count[j] == 1)
			continue;
		for (i = 0; i < G_col_count[j]; i++)
		{
			pregroup[Groups[j][i]] = Groups[j][0];
		}
	}
}


int *gen_pregroup_faults_iter(Abc_Ntk_t **fcircNtk, int flnum, int first_tst_num, int tst_num, int iter_num, int min_el, int max_el, int threshold) {
	int *pregroup_faults = NULL;
	int *groups_to_process = NULL;
	int i, j;
	int ind = 0;
	int** Groups;
	int G_row_count = 1;
	int* G_col_count;
	int* to_process;
	int n_groups, max_group;

	// initialization
	pregroup_faults = calloc(flnum, sizeof(int));
	for (i = 0; i < flnum; i++)
	{
		pregroup_faults[i] = -1;
	}

	Groups = calloc(flnum, sizeof(int*));
	for (i = 0; i < flnum; i++)
		Groups[i] = calloc(flnum, sizeof(int));
	for (i = 0; i < flnum; i++)
	{
		if (fcircNtk[i] != NULL)
		{
			Groups[0][ind] = i;
			ind++;
			//printf("%d ", i);
		}	
	}
	G_col_count = calloc(flnum, sizeof(int));
	G_col_count[0] = ind;

	// first iteration
	printf("    iteration 0 \n");
	pregroup_iteration(fcircNtk, flnum, first_tst_num, (int**)Groups, &G_row_count, 0, G_col_count, 0);
	n_groups = G_row_count;
	max_group = 0;
	int row = 0;
	for (i = 0; i < flnum; i++)
	{
		if (max_group < G_col_count[i])
		{
			max_group = G_col_count[i];
			row = i;
		}
		if (G_col_count[i] == 1)
			n_groups--;
	}
	printf("		... %d groups after pregrouping \n", n_groups);
	printf("		... biggest - %d \n\n", max_group);
	
	// последующие итерации
	to_process = calloc(flnum, sizeof(int)); // маркеры обработки	
	for (i = 0; i < iter_num; i++)
	{
		//считаем останов
		if (max_group < max_el)
			break;
		//цикл по всем группам
		for (j = 0; j < G_row_count; j++)
		{
			//проверка на величину группы
			if (G_col_count[j] < min_el)
				continue;
			//проверка на "хорошесть" группы
			if (to_process[j] >= threshold)
				continue;
			//основная итерация прегруппировки
			if (pregroup_iteration(fcircNtk, flnum, tst_num, (int**)Groups, &G_row_count, j, G_col_count, (i + 1)*(j + 1) + 10))
				to_process[j]++;
			else 
				to_process[j] = 0;
		}
		// подсчет/вывод результатов итерации
		printf("    iteration %d \n", i+1);
		n_groups = 0;
		max_group = 0;
		for (j = 0; j < G_row_count; j++)
		{
			//проверка на величину группы
			if (G_col_count[j] < 2)
				continue;
			//проверка на "хорошесть" группы
			if (to_process[j] >= threshold)
				continue;

			if (max_group < G_col_count[j])
			{
				max_group = G_col_count[j];
			}
			n_groups++;
		}
		printf("		... %d groups after pregrouping \n", n_groups);
		printf("		... biggest - %d \n\n", max_group);

	}
	
	//for (i = 0; i < flnum; i++)
	//{
	//	printf("  %d)  -  %d\n", Groups[i][0], to_process[i]);
	//}
	
	//Print(Groups, G_row_count, G_col_count, flnum);
		
	// Form pregroup_faults

	Out_Groups_iter(Groups, &G_row_count, G_col_count, pregroup_faults);

	// Free data
	free(to_process);
	free(G_col_count);
	for (i = 0; i < flnum; i++)
		free(Groups[i]);
	free(Groups);

	return pregroup_faults;
}
