/*
	Объявление структур и констант
*/

#ifndef FIM_H
#define FIM_H

#include "base/abc/abc.h"


#define XVS0   ABC_INIT_ZERO
#define XVS1   ABC_INIT_ONE
#define XVSX   ABC_INIT_DC

#define SA0 0
#define SA1 1
#define NEG 2
#define RDOB_AND 3
#define RDOB_NAND 4
#define RDOB_OR 5
#define RDOB_NOR 6
#define RDOB_XOR 7
#define RDOB_NXOR 8
#define RDOB_NOT 9
#define RDOB_BUFF 10

#define BUFF 1
#define NOT 2
#define AND 3
#define NAND 4
#define OR 5
#define NOR 6
#define XOR 7
#define NXOR 8
#define DFF 9


struct fault_descr {
	int id;
	int node;
	int type;
};


struct cell {
	int input1;
	int input2;
	int output;
	int type;
};


struct circuit {
	int *inputs;
	int inNum;
	int *outputs;
	int outNum;
	struct cell *elem;
	int elemNum;
	int type; // 0 - combinational, 1 - sequential
};

// fim_read_and_preprocess.c
extern void read_fault_list(char *faults_file, struct fault_descr **faults, int *fault_num);
extern struct circuit *read_initial_circuit(char *design_file);
extern int get_max_node_number(struct circuit *c);
extern void print_circuit_statistics(struct circuit *circ);
extern int get_design_type(char *design_file);
extern struct circuit *inject_fault(struct circuit *circ, struct fault_descr fl, int zero_replace);
extern void free_circuit(struct circuit *circ);

// fim_obvious_pairs.c
extern int *get_obvious_pairs(struct fault_descr *fl, int flnum, struct circuit *circ);

// fim_ntk_circuits_generator.c
extern Abc_Ntk_t **generate_ntk_circuits_array(struct circuit **fcirc, int flnum, int *obvious_faults);
extern void free_ntk_circuits(Abc_Ntk_t **fcircNtk, int flnum);
extern Abc_Ntk_t * Cir_2_Ntk(struct circuit *p);

// fim_pregroup_faults_generator.c
extern int *gen_pregroup_faults(Abc_Ntk_t **fcircNtk, int flnum, int tst_num, int seed, int *group_num);
extern int *gen_pregroup_faults_iter(Abc_Ntk_t **fcircNtk, int flnum, int first_tst_num, int tst_num, int iter_num, int min_el, int max_el, int threshold);
extern void matrix_pregroup_faults(Abc_Ntk_t **fcircNtk, int flnum, int tst_num, int *Out_cir, int seed);
extern void pregroup2(int sch_num, int tst_num, char * dir);
extern int * simulate(Abc_Ntk_t * pNtk, int nFrames, int outputs, int seed);

// fim_generate_final_faults.c
extern int *gen_final_faults(int *pregroup_faults, Abc_Ntk_t **fcircNtk, int flnum, int grp_num, int seq);
extern int *gen_final_faults_omp(int *pregroup_faults, Abc_Ntk_t **fcircNtk, int flnum, int seq);
extern void merge_obvious_and_final_faults(int *obvious_faults, int *final_faults, int flnum);
extern int identical_sch_pairs(int sch_num, char * dir);
extern int fim_checker(Abc_Ntk_t * sch1, Abc_Ntk_t * sch2, int checker_type);

// fim_write_result_in_file.c
extern void create_faults_pair_file(char *out_file, int *final_faults, struct fault_descr *fl, int flnum);

// fim_misc.c
extern int verify(int sc1, int sc2, char * dir, int tst_num);


#endif