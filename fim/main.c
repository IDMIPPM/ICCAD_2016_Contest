/*
	Основной файл для запуска с функцией main. Разбор параметров запуска и основной DataFlow
*/

// #include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef FIM_H
	#include "fim.h"
#endif


void run_overall(char *design_file, char *faults_file, int first_tst_num, int iter_num, int tst_num, int min_el, int max_el, int threshold) {
	struct fault_descr *fl = NULL;
	struct circuit *circ = NULL;
	struct circuit **fcirc = NULL;
	Abc_Ntk_t **fcircNtk = NULL;
	
	// int numthreads;
	int omp = 0;
	int maxNode;
	int flnum = 0;
	int design_type = -1;
	int i;
	int *obvious_faults = NULL;
	int *pregroup_faults = NULL;
	int *final_faults = NULL;
	char *out_file = "identical_fault_pairs.txt";
	clock_t t0 = clock();
	clock_t t1, t2;

	/*
	numthreads = omp_get_num_procs() - 1;
	if (numthreads < 1)
		numthreads = 1;
	*/

	printf("Reading faults list: %s\n", faults_file);
	read_fault_list(faults_file, &fl, &flnum);
	printf("Number of faults: %d\n", flnum);
	// flnum - содержит число ошибок, все последующие массивы имеют столько элементов
		
	
	printf("Reading design: %s\n", design_file);
	circ = read_initial_circuit(design_file);
	maxNode = get_max_node_number(circ);
	
	/*
	Abc_Ntk_t *etalon, *bench, *logic = NULL;
	bench = Cir_2_Ntk(circ);
	//CONVERTING TO LOGIC
	logic = Abc_NtkToLogic(bench);
	//STRUCTURALLY HASHING
	etalon = Abc_NtkStrash(logic, 1, 1, 1);
	// SWEEP OUT
	Abc_NtkDelete(bench);
	Abc_NtkDelete(logic);
	*/
	print_circuit_statistics(circ);
	int seq = get_design_type(design_file);
	t1 = clock();
	printf("Reading time: %.2lf sec\n", (double)(((double)t1 - (double)t0) / CLOCKS_PER_SEC));

	printf("Inject faults...\n");
	fcirc = calloc(flnum, sizeof(struct circuit *));
	// #pragma omp parallel num_threads(numthreads)
	for (i = 0; i < flnum; i++) {
		fcirc[i] = inject_fault(circ, fl[i], maxNode+1);
	}
	t2 = clock();
	printf("Inject faults time: %.2lf sec\n", (double)(((double)t2 - (double)t1) / CLOCKS_PER_SEC));
	// print_circuit_structure_in_file(fcirc[0], "debug.txt");
	// fcirc - массив со схемами и внедренной ошибкой. 


	printf("Create obvious pairs...\n");
	// Получить массив очевидных пар идентичных ошибок
	// В позиции i хранится индекс j - позиция fault'а с которой данная ошибка идентична.
	// В остальных позициях хранится -1
	obvious_faults = get_obvious_pairs(fl, flnum, circ);
	t1 = clock();
	printf("Create obvious pairs time: %.2lf sec\n", (double)(((double)t1 - (double)t2) / CLOCKS_PER_SEC));

	printf("Create Ntk circuits from our format...\n");
	// Здесь нужен Димин код по генерации Ntk схем из fcirc массива
	// на выходе Ntk массив fcircNtk
	// не генерировать их для очевидных ошибок (obvious_faults)
	// Места где obvious_faults == True, оставляем схему равной NULL, что бы пропускать такие в следующих функциях
	fcircNtk = generate_ntk_circuits_array(fcirc, flnum, obvious_faults);
	t2 = clock();
	printf("Create Ntk circuits time: %.2lf sec\n", (double)(((double)t2 - (double)t1) / CLOCKS_PER_SEC));
	
	//printf("Detect group of masked faults...\n");
	//masked_errors(fcircNtk, flnum, obvious_faults, etalon, seq);


	printf("Pregroup faults...\n");
	// Передается толкьо набор Ntk схем и их количество
	// pregroup_faults - массив в котором напротив каждой схемы указан ID: группы к которой схема относится (от 0 до N-1 - где N число групп)
	// для пропущенных схем указан -1
	int dima = 2;
	if (dima == 1)
	{
		int group_num = 0;
		pregroup_faults = gen_pregroup_faults(fcircNtk, flnum, 200, 310, &group_num);
	}
	else if (dima == 2)
	{
		// SMART GROUPING
		pregroup_faults = gen_pregroup_faults_iter(fcircNtk, flnum, first_tst_num, tst_num, iter_num, min_el, max_el, threshold);
		//int group_num = 0;
		//pregroup_faults = gen_pregroup_faults(fcircNtk, flnum, 100, 310, &group_num);

	}
	else
	{
		pregroup_faults = calloc(flnum, sizeof(int));
		for (i = 0; i < flnum; i++)
			pregroup_faults[i] = -1;
		matrix_pregroup_faults(fcircNtk, flnum, 400, pregroup_faults, 200);	
	}
	t1 = clock();
	printf("Pregroup faults time: %.2lf sec\n", (double)(((double)t1 - (double)t2) / CLOCKS_PER_SEC));


	//for (int i = 0; i < flnum; i++)
	//{
	//	printf("%d) %d ", i, pregroup_faults[i]);
	//}
	//printf("\n\n");
	printf("All faults...\n");
	// Передается набор Ntk схем и их количество, а также массив предразбиения
	// Возвращается финальный массив c разбиениям по группам
	if (omp == 1)
		final_faults = gen_final_faults_omp(pregroup_faults, fcircNtk, flnum, seq);
	else	
		final_faults = gen_final_faults(pregroup_faults, fcircNtk, flnum, flnum, seq);
	//for (int i = 0; i < flnum; i++)
	//{
	//	printf("%d) %d ", i, final_faults[i]);
	//}

	
	t2 = clock();
	printf("All faults time: %.2lf sec\n", (double)(((double)t2 - (double)t1) / CLOCKS_PER_SEC));

	printf("Merge obvious faults and final faults arrays...\n");
	// printf("Testing: %d %d\n", final_faults[217], final_faults[218]);
	// В final_faults копируются obvious_faults (простейшая функция на несколько строчек)
	merge_obvious_and_final_faults(obvious_faults, final_faults, flnum);
	t1 = clock();
	printf("Merge faults time: %.2lf sec\n", (double)(((double)t1 - (double)t2) / CLOCKS_PER_SEC));

	printf("Create fault pairs file...\n");
	// Записываем результат в файл
	create_faults_pair_file(out_file, final_faults, fl, flnum);
	t2 = clock();
	printf("Create file time: %.2lf sec\n", (double)(((double)t2 - (double)t1) / CLOCKS_PER_SEC));

	printf("Free data...\n");
	free_circuit(circ);
	for (i = 0; i < flnum; i++) {
		free_circuit(fcirc[i]);
	}
	free(obvious_faults);
	free(pregroup_faults);
	free(final_faults);
	free(fcirc);
	free(fl);
	free_ntk_circuits(fcircNtk, flnum);
	t1 = clock();
	printf("Overall run time: %.2lf sec\n", (double)(((double)t1 - (double)t0) / CLOCKS_PER_SEC));
}


int main(int argc, char * argv[])
{
	// МИСТИЧЕСКАЯ СТРОЧКА 
	//__________________________________
	Extra_ProgressBarStart(stdout, 101);
	//__________________________________
	//int res = check_sch("01_ckt//97.bench", "01_ckt//193.bench", 1);
	//printf("EQ :  %d\n", res);
	//run_overall("Case_05//design_05.isc", "Case_05//fault_description.txt", 50, 10, 100, 5, 30, 3);
	if (argc < 2)
	{
		printf("Wrong number of command-line arguments.\n");		
		return 1;
	}


	// DEFAULT RUN
	if (argc == 3) {
		char *design_file = argv[1];
		char *faults_file = argv[2];
		int first_tst_num = 50;  // число тестов для первого предразбиения
		int iter_num = 50;       // число итераций после которых завершается предгрупировка
		int tst_num = 50;       // число тестов для остальных итераций
		int min_el = 5;          // если число элементов в группе меньше этого значения, группа не рассматривается
		int max_el = 20;         // если все группы состоят из меньшего числа элементов - предгрупировка завершается
		int threshold = 3;       // если такое число итераций подряд группа не разбилась - группа больше не рассматривается
		run_overall(design_file, faults_file, first_tst_num, iter_num, tst_num, min_el, max_el, threshold);
		return 0;
	}
	// EXPLICIT RUN
	else if (argc == 9) {
		char *design_file = argv[1];
		char *faults_file = argv[2];
		int first_tst_num = atoi(argv[3]);  // число тестов для первого предразбиения
		int iter_num = atoi(argv[4]);       // число итераций после которых завершается предгрупировка
		int tst_num = atoi(argv[5]);       // число тестов для остальных итераций
		int min_el = atoi(argv[6]);          // если число элементов в группе меньше этого значения, группа не рассматривается
		int max_el = atoi(argv[7]);         // если все группы состоят из меньшего числа элементов - предгрупировка завершается 
		int threshold = atoi(argv[8]);       // если такое число итераций подряд группа не разбилась - группа больше не рассматривается
		run_overall(design_file, faults_file, first_tst_num, iter_num, tst_num, min_el, max_el, threshold);
		return 0;
	}
	else {

		//IDENTICAL FAULTS EXHAUSTIVE SEARCH
		if (atoi(argv[1]) == 0) {
			if (argc != 4)
			{
				printf("Wrong number of command-line arguments.\n");
				return 1;
			}
			int faults = atoi(argv[2]);
			char * dir = argv[3];
			identical_sch_pairs(faults, dir);
			return 0;
		}


		//SIGNATURES EVALUATION
		if (atoi(argv[1]) == 1) {
			if (argc != 5)
			{
				printf("Wrong number of command-line arguments.\n");
				return 1;
			}
			int faults = atoi(argv[2]);
			int tests = atoi(argv[3]);
			char * dir = argv[4];
			pregroup2(faults, tests, dir);
			return 0;
		}

		//VERIFICATION OF TWO CIRCUITS

		if (atoi(argv[1]) == 2) {
			if (argc != 6)
			{
				printf("Wrong number of command-line arguments.\n");
				return 1;
			}
			int sc1 = atoi(argv[2]);
			int sc2 = atoi(argv[3]);
			char * dir = argv[4];
			int tst_num = atoi(argv[5]);
			verify(sc1, sc2, dir, tst_num);
			return 0;
		}
	}
}

