#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef FIM_H
#include "fim.h"
#endif

// Получить массив очевидных пар идентичных ошибок
// В позиции i хранится индекс j - позиция fault'а с которой данная ошибка идентична.
// В остальных позициях хранится -1
int *get_obvious_pairs(struct fault_descr *fl, int flnum, struct circuit *circ)
{
	// Inititalization
	int *obvious;
	int cnt = 0;
	int i, j, k;
	obvious = calloc(flnum, sizeof(int));
	int cases = 0;
	
/////////////////////////////////////1////////////////////////////////////////
	//AND=RDOB_NAND|NEG
	int cnt_and_nand_neg = 0;
	int *good_pairs_and_nand_neg;
	int ready_and_nand_neg, err1_and_nand_neg, err2_and_nand_neg;
	good_pairs_and_nand_neg = calloc(circ->elemNum, sizeof(int));

	//NAND=RDOB_AND|NEG
	int cnt_nand_and_neg = 0;
	int *good_pairs_nand_and_neg;
	int ready_nand_and_neg, err1_nand_and_neg, err2_nand_and_neg;
	good_pairs_nand_and_neg = calloc(circ->elemNum, sizeof(int));	

	//OR=RDOB_NOR|NEG
	int cnt_or_nor_neg = 0;
	int *good_pairs_or_nor_neg;
	int ready_or_nor_neg, err1_or_nor_neg, err2_or_nor_neg;
	good_pairs_or_nor_neg = calloc(circ->elemNum, sizeof(int));

	//NOR=RDOB_OR|NEG
	int cnt_nor_or_neg = 0;
	int *good_pairs_nor_or_neg;
	int ready_nor_or_neg, err1_nor_or_neg, err2_nor_or_neg;
	good_pairs_nor_or_neg = calloc(circ->elemNum, sizeof(int));

	//XOR=RDOB_NXOR|NEG
	int cnt_xor_nxor_neg = 0;
	int *good_pairs_xor_nxor_neg;
	int ready_xor_nxor_neg, err1_xor_nxor_neg, err2_xor_nxor_neg;
	good_pairs_xor_nxor_neg = calloc(circ->elemNum, sizeof(int));

	//NXOR=RDOB_XOR|NEG
	int cnt_nxor_xor_neg = 0;
	int *good_pairs_nxor_xor_neg;
	int ready_nxor_xor_neg, err1_nxor_xor_neg, err2_nxor_xor_neg;
	good_pairs_nxor_xor_neg = calloc(circ->elemNum, sizeof(int));

	//BUFF=RDOB_NOT|NEG
	int cnt_buff_not_neg = 0;
	int *good_pairs_buff_not_neg;
	int ready_buff_not_neg, err1_buff_not_neg, err2_buff_not_neg;
	good_pairs_buff_not_neg = calloc(circ->elemNum, sizeof(int));

	//NOT=RDOB_BUFF|NEG
	int cnt_not_buff_neg = 0;
	int *good_pairs_not_buff_neg;
	int ready_not_buff_neg, err1_not_buff_neg, err2_not_buff_neg;
	good_pairs_not_buff_neg = calloc(circ->elemNum, sizeof(int));

	//SA0_NAND_SA1
	int *good_pairs_SA0_NAND_SA1;
	int *in1_good_SA0_NAND_SA1;
	int *in2_good_SA0_NAND_SA1;
	good_pairs_SA0_NAND_SA1 = calloc(circ->elemNum, sizeof(int));
	in1_good_SA0_NAND_SA1 = calloc(circ->elemNum, sizeof(int));
	in2_good_SA0_NAND_SA1 = calloc(circ->elemNum, sizeof(int));

	//SA1_OR_SA1
	int *good_pairs_SA1_OR_SA1;
	int *in1_good_SA1_OR_SA1;
	int *in2_good_SA1_OR_SA1;
	good_pairs_SA1_OR_SA1 = calloc(circ->elemNum, sizeof(int));
	in1_good_SA1_OR_SA1 = calloc(circ->elemNum, sizeof(int));
	in2_good_SA1_OR_SA1 = calloc(circ->elemNum, sizeof(int));

	//SA0_AND_SA0
	int *good_pairs_SA0_AND_SA0;
	int *in1_good_SA0_AND_SA0;
	int *in2_good_SA0_AND_SA0;
	good_pairs_SA0_AND_SA0 = calloc(circ->elemNum, sizeof(int));
	in1_good_SA0_AND_SA0 = calloc(circ->elemNum, sizeof(int));
	in2_good_SA0_AND_SA0 = calloc(circ->elemNum, sizeof(int));

	//SA1_NOR_SA0
	int *good_pairs_SA1_NOR_SA0;
	int *in1_good_SA1_NOR_SA0;
	int *in2_good_SA1_NOR_SA0;
	good_pairs_SA1_NOR_SA0 = calloc(circ->elemNum, sizeof(int));
	in1_good_SA1_NOR_SA0 = calloc(circ->elemNum, sizeof(int));
	in2_good_SA1_NOR_SA0 = calloc(circ->elemNum, sizeof(int));

	//NEG_XOR_RDOBNXOR
	int *good_pairs_NEG_XOR_RDOBNXOR;
	int *in1_good_NEG_XOR_RDOBNXOR;
	int *in2_good_NEG_XOR_RDOBNXOR;
	good_pairs_NEG_XOR_RDOBNXOR = calloc(circ->elemNum, sizeof(int));
	in1_good_NEG_XOR_RDOBNXOR = calloc(circ->elemNum, sizeof(int));
	in2_good_NEG_XOR_RDOBNXOR = calloc(circ->elemNum, sizeof(int));

	//NEG_NXOR_RDOBXOR
	int *good_pairs_NEG_NXOR_RDOBXOR;
	int *in1_good_NEG_NXOR_RDOBXOR;
	int *in2_good_NEG_NXOR_RDOBXOR;
	good_pairs_NEG_NXOR_RDOBXOR = calloc(circ->elemNum, sizeof(int));
	in1_good_NEG_NXOR_RDOBXOR = calloc(circ->elemNum, sizeof(int));
	in2_good_NEG_NXOR_RDOBXOR = calloc(circ->elemNum, sizeof(int));

	//SA0_INV_SA1
	int *good_pairs_SA0_INV_SA1;
	int *in1_good_SA0_INV_SA1;
	int *in2_good_SA0_INV_SA1;
	good_pairs_SA0_INV_SA1 = calloc(circ->elemNum, sizeof(int));
	in1_good_SA0_INV_SA1 = calloc(circ->elemNum, sizeof(int));
	in2_good_SA0_INV_SA1 = calloc(circ->elemNum, sizeof(int));

	//SA0_BUFF_SA0
	int *good_pairs_SA0_BUFF_SA0;
	int *in1_good_SA0_BUFF_SA0;
	int *in2_good_SA0_BUFF_SA0;
	good_pairs_SA0_BUFF_SA0 = calloc(circ->elemNum, sizeof(int));
	in1_good_SA0_BUFF_SA0 = calloc(circ->elemNum, sizeof(int));
	in2_good_SA0_BUFF_SA0 = calloc(circ->elemNum, sizeof(int));


//////////////////////////////////////////////////////////////////////////////


	// Get the type of element
	for (j = 0; j < circ->elemNum; j++)
	{	
/////////////////////////////////////2////////////////////////////////////////
		//AND=RDOB_NAND|NEG
		if (circ->elem[j].type == AND){good_pairs_and_nand_neg[cnt_and_nand_neg] = circ->elem[j].output; cnt_and_nand_neg++;}
		//NAND=RDOB_AND|NEG
		if (circ->elem[j].type == NAND){ good_pairs_nand_and_neg[cnt_nand_and_neg] = circ->elem[j].output; cnt_nand_and_neg++;}
		//OR=RDOB_NOR|NEG
		if (circ->elem[j].type == OR){ good_pairs_or_nor_neg[cnt_or_nor_neg] = circ->elem[j].output; cnt_or_nor_neg++; }
		//NOR=RDOB_OR|NEG
		if (circ->elem[j].type == NOR){ good_pairs_nor_or_neg[cnt_nor_or_neg] = circ->elem[j].output; cnt_nor_or_neg++; }
		//XOR=RDOB_NXOR|NEG
		if (circ->elem[j].type == XOR){ good_pairs_xor_nxor_neg[cnt_xor_nxor_neg] = circ->elem[j].output; cnt_xor_nxor_neg++; }
		//NXOR=RDOB_XOR|NEG
		if (circ->elem[j].type == NXOR){ good_pairs_nxor_xor_neg[cnt_nxor_xor_neg] = circ->elem[j].output; cnt_nxor_xor_neg++; }
		//BUFF=RDOB_NOT|NEG
		if (circ->elem[j].type == BUFF){ good_pairs_buff_not_neg[cnt_buff_not_neg] = circ->elem[j].output; cnt_buff_not_neg++; }
		//NOT=RDOB_BUFF|NEG
		if (circ->elem[j].type == NOT){ good_pairs_not_buff_neg[cnt_not_buff_neg] = circ->elem[j].output; cnt_not_buff_neg++; }
		//SA0_NAND_SA1
		in1_good_SA0_NAND_SA1[j] = -1; in2_good_SA0_NAND_SA1[j] = -1; good_pairs_SA0_NAND_SA1[j] = -1;
		//SA1_OR_SA1
		in1_good_SA1_OR_SA1[j] = -1; in2_good_SA1_OR_SA1[j] = -1; good_pairs_SA1_OR_SA1[j] = -1;
		//SA0_AND_SA0
		in1_good_SA0_AND_SA0[j] = -1; in2_good_SA0_AND_SA0[j] = -1; good_pairs_SA0_AND_SA0[j] = -1;
		//SA1_NOR_SA0
		in1_good_SA1_NOR_SA0[j] = -1; in2_good_SA1_NOR_SA0[j] = -1; good_pairs_SA1_NOR_SA0[j] = -1;
		//NEG_XOR_RDOBNXOR
		in1_good_NEG_XOR_RDOBNXOR[j] = -1; in2_good_NEG_XOR_RDOBNXOR[j] = -1; good_pairs_NEG_XOR_RDOBNXOR[j] = -1;
		//NEG_NXOR_RDOBXOR
		in1_good_NEG_NXOR_RDOBXOR[j] = -1; in2_good_NEG_NXOR_RDOBXOR[j] = -1; good_pairs_NEG_NXOR_RDOBXOR[j] = -1;
		//SA0_INV_SA1
		in1_good_SA0_INV_SA1[j] = -1; in2_good_SA0_INV_SA1[j] = -1; good_pairs_SA0_INV_SA1[j] = -1;
		//SA0_BUFF_SA0
		in1_good_SA0_BUFF_SA0[j] = -1; in2_good_SA0_BUFF_SA0[j] = -1; good_pairs_SA0_BUFF_SA0[j] = -1;


//////////////////////////////////////////////////////////////////////////////
	}
	// Fill the obvious
	for (i = 0; i < circ->elemNum; i++)
	{
/////////////////////////////////////3////////////////////////////////////////
		//AND=RDOB_NAND|NEG
		ready_and_nand_neg = 0;
		//NAND=RDOB_AND|NEG
		ready_nand_and_neg = 0;
		//OR=RDOB_NOR|NEG
		ready_or_nor_neg = 0;
		//NOR=RDOB_OR|NEG
		ready_nor_or_neg = 0;
		//XOR=RDOB_NXOR|NEG
		ready_xor_nxor_neg = 0;
		//NXOR=RDOB_XOR|NEG
		ready_nxor_xor_neg = 0;
		//BUFF=RDOB_NOT|NEG
		ready_buff_not_neg = 0;
		//NOT=RDOB_BUFF|NEG
		ready_not_buff_neg = 0;
//////////////////////////////////////////////////////////////////////////////
		for (j = 0; j < flnum; j++)
		{
			if (obvious[j] == 0) obvious[j] = -1;
/////////////////////////////////////4////////////////////////////////////////
			//AND=RDOB_NAND|NEG
			if (fl[j].type == RDOB_NAND){if (good_pairs_and_nand_neg[i] == fl[j].node){ready_and_nand_neg++;err1_and_nand_neg = j;if (ready_and_nand_neg == 2){obvious[j] = err2_and_nand_neg;cases++;} } } 
			if (fl[j].type == NEG){ if (good_pairs_and_nand_neg[i] == fl[j].node){ ready_and_nand_neg++; err2_and_nand_neg = j; if (ready_and_nand_neg == 2) { obvious[j] = err1_and_nand_neg ; cases++; } } }
			//NAND=RDOB_AND|NEG
			if (fl[j].type == RDOB_AND){ if (good_pairs_nand_and_neg[i] == fl[j].node){ ready_nand_and_neg++; err1_nand_and_neg = j; if (ready_nand_and_neg == 2) { obvious[j] = err2_nand_and_neg; cases++; } } }
			if (fl[j].type == NEG){ if (good_pairs_nand_and_neg[i] == fl[j].node){ ready_nand_and_neg++; err2_nand_and_neg = j; if (ready_nand_and_neg == 2) { obvious[j] = err1_nand_and_neg; cases++; } } }
			//OR=RDOB_NOR|NEG
			if (fl[j].type == RDOB_NOR){ if (good_pairs_or_nor_neg[i] == fl[j].node){ ready_or_nor_neg++; err1_or_nor_neg = j; if (ready_or_nor_neg == 2) { obvious[j] = err2_or_nor_neg; cases++; } } }
			if (fl[j].type == NEG){ if (good_pairs_or_nor_neg[i] == fl[j].node){ ready_or_nor_neg++; err2_or_nor_neg = j; if (ready_or_nor_neg == 2) { obvious[j] = err1_or_nor_neg; cases++; } } }
			//NOR=RDOB_OR|NEG
			if (fl[j].type == RDOB_OR){ if (good_pairs_nor_or_neg[i] == fl[j].node){ ready_nor_or_neg++; err1_nor_or_neg = j; if (ready_nor_or_neg == 2) { obvious[j] = err2_nor_or_neg; cases++; } } }
			if (fl[j].type == NEG){ if (good_pairs_nor_or_neg[i] == fl[j].node){ ready_nor_or_neg++; err2_nor_or_neg = j; if (ready_nor_or_neg == 2) { obvious[j] = err1_nor_or_neg; cases++; } } }
			//XOR=RDOB_NXOR|NEG
			if (fl[j].type == RDOB_NXOR){ if (good_pairs_xor_nxor_neg[i] == fl[j].node){ ready_xor_nxor_neg++; err1_xor_nxor_neg = j; if (ready_xor_nxor_neg == 2) { obvious[j] = err2_xor_nxor_neg; cases++; } } }
			if (fl[j].type == NEG){ if (good_pairs_xor_nxor_neg[i] == fl[j].node){ ready_xor_nxor_neg++; err2_xor_nxor_neg = j; if (ready_xor_nxor_neg == 2){ obvious[j] = err1_xor_nxor_neg; cases++; } } }
			//NXOR=RDOB_XOR|NEG
			if (fl[j].type == RDOB_XOR){ if (good_pairs_nxor_xor_neg[i] == fl[j].node){ ready_nxor_xor_neg++; err1_nxor_xor_neg = j; if (ready_nxor_xor_neg == 2) { obvious[j] = err2_nxor_xor_neg; cases++; } } }
			if (fl[j].type == NEG){ if (good_pairs_nxor_xor_neg[i] == fl[j].node){ ready_nxor_xor_neg++; err2_nxor_xor_neg = j; if (ready_nxor_xor_neg == 2) { obvious[j] = err1_nxor_xor_neg; cases++; } } }
			//BUFF=RDOB_NOT|NEG
			if (fl[j].type == RDOB_NOT){ if (good_pairs_buff_not_neg[i] == fl[j].node){ ready_buff_not_neg++; err1_buff_not_neg = j; if (ready_buff_not_neg == 2) { obvious[j] = err2_buff_not_neg; cases++; } } }
			if (fl[j].type == NEG){ if (good_pairs_buff_not_neg[i] == fl[j].node){ ready_buff_not_neg++; err2_buff_not_neg = j; if (ready_buff_not_neg == 2) { obvious[j] = err1_buff_not_neg; cases++; } } }
			//NOT=RDOB_BUFF|NEG
			if (fl[j].type == RDOB_BUFF){ if (good_pairs_not_buff_neg[i] == fl[j].node){ ready_not_buff_neg++; err1_not_buff_neg = j; if (ready_not_buff_neg == 2) { obvious[j] = err2_not_buff_neg; cases++; } } }
			if (fl[j].type == NEG){ if (good_pairs_not_buff_neg[i] == fl[j].node){ ready_not_buff_neg++; err2_not_buff_neg = j; if (ready_not_buff_neg == 2) { obvious[j] = err1_not_buff_neg; cases++; } } }
//////////////////////////////////////////////////////////////////////////////
		}
	}
	
//////////////////////////////////////////////////////////////////////////////

for (i = 0; i < flnum; i++)
{	
	if (fl[i].type == SA1)
	{
		//SA0_NAND_SA1
		for (j = 0; j < circ->elemNum; j++)
		{
			if (circ->elem[j].type == NAND)
			{
				if (fl[i].node == circ->elem[j].output){ if (fl[i].node != -1){ in1_good_SA0_NAND_SA1[j] = circ->elem[j].input1; for (k = 0; k < circ->elemNum; k++){ if ((in1_good_SA0_NAND_SA1[j] == circ->elem[k].input1) | (in1_good_SA0_NAND_SA1[j] == circ->elem[k].input2)){ cnt++; if (cnt>1){ in1_good_SA0_NAND_SA1[j] = -1; break; } } }cnt = 0; if (in1_good_SA0_NAND_SA1[j] != -1){ good_pairs_SA0_NAND_SA1[j] = fl[i].id - 1; } } }if (fl[i].node == circ->elem[j].output){ if (fl[i].node != -1){ in2_good_SA0_NAND_SA1[j] = circ->elem[j].input2; for (k = 0; k < circ->elemNum; k++){ if ((in2_good_SA0_NAND_SA1[j] == circ->elem[k].input1) | (in2_good_SA0_NAND_SA1[j] == circ->elem[k].input2)){ cnt++; if (cnt>1){ in2_good_SA0_NAND_SA1[j] = -1; break; } } }cnt = 0; if (in2_good_SA0_NAND_SA1[j] != -1){ good_pairs_SA0_NAND_SA1[j] = fl[i].id - 1; } } }
			}
		}
		//SA1_OR_SA1
		for (j = 0; j < circ->elemNum; j++)
		{
			if (circ->elem[j].type == OR)
			{
				if (fl[i].node == circ->elem[j].output){ if (fl[i].node != -1){ in1_good_SA1_OR_SA1[j] = circ->elem[j].input1; for (k = 0; k < circ->elemNum; k++){ if ((in1_good_SA1_OR_SA1[j] == circ->elem[k].input1) | (in1_good_SA1_OR_SA1[j] == circ->elem[k].input2)){ cnt++; if (cnt>1){ in1_good_SA1_OR_SA1[j] = -1; break; } } }cnt = 0; if (in1_good_SA1_OR_SA1[j] != -1){ good_pairs_SA1_OR_SA1[j] = fl[i].id - 1; } } }if (fl[i].node == circ->elem[j].output){ if (fl[i].node != -1){ in2_good_SA1_OR_SA1[j] = circ->elem[j].input2; for (k = 0; k < circ->elemNum; k++){ if ((in2_good_SA1_OR_SA1[j] == circ->elem[k].input1) | (in2_good_SA1_OR_SA1[j] == circ->elem[k].input2)){ cnt++; if (cnt>1){ in2_good_SA1_OR_SA1[j] = -1; break; } } }cnt = 0; if (in2_good_SA1_OR_SA1[j] != -1){ good_pairs_SA1_OR_SA1[j] = fl[i].id - 1; } } }
			}
		}
		//SA0_INV_SA1
		for (j = 0; j < circ->elemNum; j++)
		{
			if (circ->elem[j].type == NOT)
			{
				if (fl[i].node == circ->elem[j].output){ if (fl[i].node != -1){ in1_good_SA0_INV_SA1[j] = circ->elem[j].input1; for (k = 0; k < circ->elemNum; k++){ if ((in1_good_SA0_INV_SA1[j] == circ->elem[k].input1) | (in1_good_SA0_INV_SA1[j] == circ->elem[k].input2)){ cnt++; if (cnt>1){ in1_good_SA0_INV_SA1[j] = -1; break; } } }cnt = 0; if (in1_good_SA0_INV_SA1[j] != -1){ good_pairs_SA0_INV_SA1[j] = fl[i].id - 1; } } }
			}
		}

	}

	if (fl[i].type == SA0)
	{
		//SA0_AND_SA0
		for (j = 0; j < circ->elemNum; j++)
		{
			if (circ->elem[j].type == AND)
			{
				if (fl[i].node == circ->elem[j].output){ if (fl[i].node != -1){ in1_good_SA0_AND_SA0[j] = circ->elem[j].input1; for (k = 0; k < circ->elemNum; k++){ if ((in1_good_SA0_AND_SA0[j] == circ->elem[k].input1) | (in1_good_SA0_AND_SA0[j] == circ->elem[k].input2)){ cnt++; if (cnt>1){ in1_good_SA0_AND_SA0[j] = -1; break; } } }cnt = 0; if (in1_good_SA0_AND_SA0[j] != -1){ good_pairs_SA0_AND_SA0[j] = fl[i].id - 1; } } }if (fl[i].node == circ->elem[j].output){ if (fl[i].node != -1){ in2_good_SA0_AND_SA0[j] = circ->elem[j].input2; for (k = 0; k < circ->elemNum; k++){ if ((in2_good_SA0_AND_SA0[j] == circ->elem[k].input1) | (in2_good_SA0_AND_SA0[j] == circ->elem[k].input2)){ cnt++; if (cnt>1){ in2_good_SA0_AND_SA0[j] = -1; break; } } }cnt = 0; if (in2_good_SA0_AND_SA0[j] != -1){ good_pairs_SA0_AND_SA0[j] = fl[i].id - 1; } } }
			}
		}

		//SA1_NOR_SA0
		for (j = 0; j < circ->elemNum; j++)
		{
			if (circ->elem[j].type == NOR)
			{
				if (fl[i].node == circ->elem[j].output){ if (fl[i].node != -1){ in1_good_SA1_NOR_SA0[j] = circ->elem[j].input1; for (k = 0; k < circ->elemNum; k++){ if ((in1_good_SA1_NOR_SA0[j] == circ->elem[k].input1) | (in1_good_SA1_NOR_SA0[j] == circ->elem[k].input2)){ cnt++; if (cnt>1){ in1_good_SA1_NOR_SA0[j] = -1; break; } } }cnt = 0; if (in1_good_SA1_NOR_SA0[j] != -1){ good_pairs_SA1_NOR_SA0[j] = fl[i].id - 1; } } }if (fl[i].node == circ->elem[j].output){ if (fl[i].node != -1){ in2_good_SA1_NOR_SA0[j] = circ->elem[j].input2; for (k = 0; k < circ->elemNum; k++){ if ((in2_good_SA1_NOR_SA0[j] == circ->elem[k].input1) | (in2_good_SA1_NOR_SA0[j] == circ->elem[k].input2)){ cnt++; if (cnt>1){ in2_good_SA1_NOR_SA0[j] = -1; break; } } }cnt = 0; if (in2_good_SA1_NOR_SA0[j] != -1){ good_pairs_SA1_NOR_SA0[j] = fl[i].id - 1; } } }
			}
		}

		//SA0_BUFF_SA0
		for (j = 0; j < circ->elemNum; j++)
		{
			if (circ->elem[j].type == BUFF)
			{
				if (fl[i].node == circ->elem[j].output){ if (fl[i].node != -1){ in1_good_SA0_BUFF_SA0[j] = circ->elem[j].input1; for (k = 0; k < circ->elemNum; k++){ if ((in1_good_SA0_BUFF_SA0[j] == circ->elem[k].input1) | (in1_good_SA0_BUFF_SA0[j] == circ->elem[k].input2)){ cnt++; if (cnt>1){ in1_good_SA0_BUFF_SA0[j] = -1; break; } } }cnt = 0; if (in1_good_SA0_BUFF_SA0[j] != -1){ good_pairs_SA0_BUFF_SA0[j] = fl[i].id - 1; } } }
			}
		}

	}	

	//NEG_XOR_RDOBNXOR
	if (fl[i].type == RDOB_NXOR)
	{
		for (j = 0; j < circ->elemNum; j++)
		{
			if (circ->elem[j].type == XOR)
			{
				if (fl[i].node == circ->elem[j].output){ if (fl[i].node != -1){ in1_good_NEG_XOR_RDOBNXOR[j] = circ->elem[j].input1; for (k = 0; k < circ->elemNum; k++){ if ((in1_good_NEG_XOR_RDOBNXOR[j] == circ->elem[k].input1) | (in1_good_NEG_XOR_RDOBNXOR[j] == circ->elem[k].input2)){ cnt++; if (cnt>1){ in1_good_NEG_XOR_RDOBNXOR[j] = -1; break; } } }cnt = 0; if (in1_good_NEG_XOR_RDOBNXOR[j] != -1){ good_pairs_NEG_XOR_RDOBNXOR[j] = fl[i].id - 1; } } }if (fl[i].node == circ->elem[j].output){ if (fl[i].node != -1){ in2_good_NEG_XOR_RDOBNXOR[j] = circ->elem[j].input2; for (k = 0; k < circ->elemNum; k++){ if ((in2_good_NEG_XOR_RDOBNXOR[j] == circ->elem[k].input1) | (in2_good_NEG_XOR_RDOBNXOR[j] == circ->elem[k].input2)){ cnt++; if (cnt>1){ in2_good_NEG_XOR_RDOBNXOR[j] = -1; break; } } }cnt = 0; if (in2_good_NEG_XOR_RDOBNXOR[j] != -1){ good_pairs_NEG_XOR_RDOBNXOR[j] = fl[i].id - 1; } } }
			}
		}
	}

	//NEG_NXOR_RDOBXOR
	if (fl[i].type == RDOB_XOR)
	{
		for (j = 0; j < circ->elemNum; j++)
		{
			if (circ->elem[j].type == NXOR)
			{
				if (fl[i].node == circ->elem[j].output){ if (fl[i].node != -1){ in1_good_NEG_NXOR_RDOBXOR[j] = circ->elem[j].input1; for (k = 0; k < circ->elemNum; k++){ if ((in1_good_NEG_NXOR_RDOBXOR[j] == circ->elem[k].input1) | (in1_good_NEG_NXOR_RDOBXOR[j] == circ->elem[k].input2)){ cnt++; if (cnt>1){ in1_good_NEG_NXOR_RDOBXOR[j] = -1; break; } } }cnt = 0; if (in1_good_NEG_NXOR_RDOBXOR[j] != -1){ good_pairs_NEG_NXOR_RDOBXOR[j] = fl[i].id - 1; } } }if (fl[i].node == circ->elem[j].output){ if (fl[i].node != -1){ in2_good_NEG_NXOR_RDOBXOR[j] = circ->elem[j].input2; for (k = 0; k < circ->elemNum; k++){ if ((in2_good_NEG_NXOR_RDOBXOR[j] == circ->elem[k].input1) | (in2_good_NEG_NXOR_RDOBXOR[j] == circ->elem[k].input2)){ cnt++; if (cnt>1){ in2_good_NEG_NXOR_RDOBXOR[j] = -1; break; } } }cnt = 0; if (in2_good_NEG_NXOR_RDOBXOR[j] != -1){ good_pairs_NEG_NXOR_RDOBXOR[j] = fl[i].id - 1; } } }
			}
		}
	}
}

for (i = 0; i < flnum; i++)
{
	if (fl[i].type == SA0)
	{
		//SA0_NAND_SA1
		for (j = 0; j < circ->elemNum; j++){ if (in1_good_SA0_NAND_SA1[j] != -1)if (in1_good_SA0_NAND_SA1[j] == fl[i].node){ for (k = 0; k < flnum; k++){ if (k == good_pairs_SA0_NAND_SA1[j]){ if (i > k){ obvious[i] = k; cases++;  } else{ obvious[k] = i; cases++;  } } } }if (in2_good_SA0_NAND_SA1[j] != -1)if (in2_good_SA0_NAND_SA1[j] == fl[i].node){ for (k = 0; k < flnum; k++){ if (k == good_pairs_SA0_NAND_SA1[j]){ if (i > k){ obvious[i] = k; cases++;  } else{ obvious[k] = i; cases++;  } } } } }
		//SA0_AND_SA0
		for (j = 0; j < circ->elemNum; j++){ if (in1_good_SA0_AND_SA0[j] != -1)if (in1_good_SA0_AND_SA0[j] == fl[i].node){ for (k = 0; k < flnum; k++){ if (k == good_pairs_SA0_AND_SA0[j]){ if (i > k){ obvious[i] = k; cases++;  } else{ obvious[k] = i; cases++;  } } } }if (in2_good_SA0_AND_SA0[j] != -1)if (in2_good_SA0_AND_SA0[j] == fl[i].node){ for (k = 0; k < flnum; k++){ if (k == good_pairs_SA0_AND_SA0[j]){ if (i > k){ obvious[i] = k; cases++;  } else{ obvious[k] = i; cases++;  } } } } }
		//SA0_INV_SA1
		for (j = 0; j < circ->elemNum; j++){ if (in1_good_SA0_INV_SA1[j] != -1)if (in1_good_SA0_INV_SA1[j] == fl[i].node){ for (k = 0; k < flnum; k++){ if (k == good_pairs_SA0_INV_SA1[j]){ if (i > k){ obvious[i] = k; cases++; } else{ obvious[k] = i; cases++; } } } } }
		//SA0_BUFF_SA0
		for (j = 0; j < circ->elemNum; j++){ if (in1_good_SA0_BUFF_SA0[j] != -1)if (in1_good_SA0_BUFF_SA0[j] == fl[i].node){ for (k = 0; k < flnum; k++){ if (k == good_pairs_SA0_BUFF_SA0[j]){ if (i > k){ obvious[i] = k; cases++; } else{ obvious[k] = i; cases++; } } } } }
		
	}

	if (fl[i].type == SA1)
	{
		//SA1_OR_SA1
		for (j = 0; j < circ->elemNum; j++){ if (in1_good_SA1_OR_SA1[j] != -1)if (in1_good_SA1_OR_SA1[j] == fl[i].node){ for (k = 0; k < flnum; k++){ if (k == good_pairs_SA1_OR_SA1[j]){ if (i > k){ obvious[i] = k; cases++;  } else{ obvious[k] = i; cases++;  } } } }if (in2_good_SA1_OR_SA1[j] != -1)if (in2_good_SA1_OR_SA1[j] == fl[i].node){ for (k = 0; k < flnum; k++){ if (k == good_pairs_SA1_OR_SA1[j]){ if (i > k){ obvious[i] = k; cases++;  } else{ obvious[k] = i; cases++;  } } } } }
		//SA1_NOR_SA0
		for (j = 0; j < circ->elemNum; j++){
			if (in1_good_SA1_NOR_SA0[j] != -1)if (in1_good_SA1_NOR_SA0[j] == fl[i].node){
				for (k = 0; k < flnum; k++){
					if (k == good_pairs_SA1_NOR_SA0[j]){
						if (i > k){
							obvious[i] = k; cases++; 
						}
						else{ obvious[k] = i; cases++;  }
					}
				}
			}if (in2_good_SA1_NOR_SA0[j] != -1)if (in2_good_SA1_NOR_SA0[j] == fl[i].node){ for (k = 0; k < flnum; k++){ if (k == good_pairs_SA1_NOR_SA0[j]){ if (i > k){ obvious[i] = k; cases++; } else{ obvious[k] = i; cases++;  } } } }
		}
	}

	if (fl[i].type == NEG)
	{
		//NEG_XOR_RDOBNXOR
		for (j = 0; j < circ->elemNum; j++){ if (in1_good_NEG_XOR_RDOBNXOR[j] != -1)if (in1_good_NEG_XOR_RDOBNXOR[j] == fl[i].node){ for (k = 0; k < flnum; k++){ if (k == good_pairs_NEG_XOR_RDOBNXOR[j]){ if (i > k){ obvious[i] = k; cases++; } else{ obvious[k] = i; cases++; } } } }if (in2_good_NEG_XOR_RDOBNXOR[j] != -1)if (in2_good_NEG_XOR_RDOBNXOR[j] == fl[i].node){ for (k = 0; k < flnum; k++){ if (k == good_pairs_NEG_XOR_RDOBNXOR[j]){ if (i > k){ obvious[i] = k; cases++;  } else{ obvious[k] = i; cases++; } } } } }
		//NEG_NXOR_RDOBXOR
		for (j = 0; j < circ->elemNum; j++){ if (in1_good_NEG_NXOR_RDOBXOR[j] != -1)if (in1_good_NEG_NXOR_RDOBXOR[j] == fl[i].node){ for (k = 0; k < flnum; k++){ if (k == good_pairs_NEG_NXOR_RDOBXOR[j]){ if (i > k){ obvious[i] = k; cases++; } else{ obvious[k] = i; cases++; } } } }if (in2_good_NEG_NXOR_RDOBXOR[j] != -1)if (in2_good_NEG_NXOR_RDOBXOR[j] == fl[i].node){ for (k = 0; k < flnum; k++){ if (k == good_pairs_NEG_NXOR_RDOBXOR[j]){ if (i > k){ obvious[i] = k; cases++;  } else{ obvious[k] = i; cases++;  } } } } }
	}
}

//////////////////////////////////////////////////////////////////////////////


//for (j = 0; j < flnum; j++) { obvious[j] = -1; }


/*
//show the result
for (i = 0; i < flnum; i++)
	begin
		if (obvious[i] != -1)
		printf("%d = %d\n", i+1, obvious[i]+1);
	end
*/

	printf("		... %d obvious faults detected\n", cases);

	free(good_pairs_and_nand_neg);
	free(good_pairs_nand_and_neg);
	free(good_pairs_or_nor_neg);
	free(good_pairs_nor_or_neg);
	free(good_pairs_xor_nxor_neg);
	free(good_pairs_nxor_xor_neg);
	free(good_pairs_buff_not_neg);
	free(good_pairs_not_buff_neg);
	free(good_pairs_SA0_NAND_SA1);
	free(in1_good_SA0_NAND_SA1);
	free(in2_good_SA0_NAND_SA1);
	free(good_pairs_SA1_OR_SA1);
	free(in1_good_SA1_OR_SA1);
	free(in2_good_SA1_OR_SA1);
	free(good_pairs_SA0_AND_SA0);
	free(in1_good_SA0_AND_SA0);
	free(in2_good_SA0_AND_SA0);
	free(good_pairs_SA1_NOR_SA0);
	free(in1_good_SA1_NOR_SA0);
	free(in2_good_SA1_NOR_SA0);
	free(good_pairs_NEG_XOR_RDOBNXOR);
	free(in1_good_NEG_XOR_RDOBNXOR);
	free(in2_good_NEG_XOR_RDOBNXOR);
	free(good_pairs_NEG_NXOR_RDOBXOR);
	free(in1_good_NEG_NXOR_RDOBXOR);
	free(in2_good_NEG_NXOR_RDOBXOR);
	free(good_pairs_SA0_INV_SA1);
	free(in1_good_SA0_INV_SA1);
	free(in2_good_SA0_INV_SA1);
	free(good_pairs_SA0_BUFF_SA0);
	free(in1_good_SA0_BUFF_SA0);
	free(in2_good_SA0_BUFF_SA0);

	//_sleep(8000);
	//exit(1);
	return obvious;
	
}



