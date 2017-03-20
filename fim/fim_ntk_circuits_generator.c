/*
	Comments
*/

// #include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef FIM_H
#include "fim.h"
#endif
#include "base/io/ioAbc.h"


// function to convert circuit into abc Ntk format
Abc_Ntk_t * Cir_2_Ntk(struct circuit *p)
{
	// Vec_Ptr_t * vTokens;
	Abc_Ntk_t * pNtk;
	Abc_Obj_t * pNode;
	char inname[10], outname[10], in1[10], in2[10], out[10];
	int i, type;
	// struct cell * temp_cell;


	// allocate the empty network
	pNtk = Abc_NtkStartRead("temp");

	// go through inputs

	for (i = 0; i < p->inNum; i++)
	{
		//itoa(p->inputs[i], inname, 10);
		snprintf(inname, sizeof(inname), "%d", p->inputs[i]);
		Io_ReadCreatePi(pNtk, inname); // СОЗДАЁМ ПЕРВИЧНЫЙ ВХОД
	}

	for (i = 0; i < p->outNum; i++)
	{
		//itoa(p->outputs[i], outname, 10);
		snprintf(outname, sizeof(outname), "%d", p->outputs[i]);
		Io_ReadCreatePo(pNtk, outname); // СОЗДАЁМ ПЕРВИЧНЫЙ ВЫХОД
	}

	for (i = 0; i < p->elemNum; i++)
	{
		type = p->elem[i].type;
		//itoa(p->elem[i].input1, in1, 10);
		//itoa(p->elem[i].input2, in2, 10);
		//itoa(p->elem[i].output, out, 10);
		snprintf(in1, sizeof(in1), "%d", p->elem[i].input1);
		snprintf(in2, sizeof(in2), "%d", p->elem[i].input2);
		snprintf(out, sizeof(out), "%d", p->elem[i].output);
		char *inputs[] = { in1, in2 };

		if (type == 9)
		{
			pNode = Io_ReadCreateLatch(pNtk, in1, out); // ДОБАВЛЯЕМ DFF
			Abc_LatchSetInit0(pNode);
		}
		else if (type == 1)
		{
			pNode = Io_ReadCreateNode(pNtk, out, inputs, 1);
			Abc_ObjSetData(pNode, Abc_SopCreateBuf((Mem_Flex_t *)pNtk->pManFunc)); // ДОБАВЛЯЕМ BUFF
																				   //printf("%s = BUFF(%s)\n", out, inputs[0]);
		}
		else if (type == 2)
		{
			pNode = Io_ReadCreateNode(pNtk, out, inputs, 1);
			Abc_ObjSetData(pNode, Abc_SopCreateInv((Mem_Flex_t *)pNtk->pManFunc)); // ДОБАВЛЯЕМ NOT
																				   //printf("%s = NOT(%s)\n", out, inputs[0]);
		}
		else if (type == 3)
		{
			pNode = Io_ReadCreateNode(pNtk, out, inputs, 2);
			Abc_ObjSetData(pNode, Abc_SopCreateAnd((Mem_Flex_t *)pNtk->pManFunc, 2, NULL)); // ДОБАВЛЯЕМ AND
																							//printf("%s = AND(%s, %s)\n", out, inputs[0], inputs[1]);
		}
		else if (type == 4)
		{
			pNode = Io_ReadCreateNode(pNtk, out, inputs, 2);
			Abc_ObjSetData(pNode, Abc_SopCreateNand((Mem_Flex_t *)pNtk->pManFunc, 2)); // ДОБАВЛЯЕМ NAND
																					   //printf("%s = NAND(%s, %s)\n", out, inputs[0], inputs[1]);
		}
		else if (type == 5)
		{
			pNode = Io_ReadCreateNode(pNtk, out, inputs, 2);
			Abc_ObjSetData(pNode, Abc_SopCreateOr((Mem_Flex_t *)pNtk->pManFunc, 2, NULL)); // ДОБАВЛЯЕМ OR
																						   //printf("%s = OR(%s, %s)\n", out, inputs[0], inputs[1]);
		}
		else if (type == 6)
		{
			pNode = Io_ReadCreateNode(pNtk, out, inputs, 2);
			Abc_ObjSetData(pNode, Abc_SopCreateNor((Mem_Flex_t *)pNtk->pManFunc, 2)); // ДОБАВЛЯЕМ NOR
																					  //printf("%s = NOR(%s, %s)\n", out, inputs[0], inputs[1]);
		}
		else if (type == 7)
		{
			pNode = Io_ReadCreateNode(pNtk, out, inputs, 2);
			Abc_ObjSetData(pNode, Abc_SopCreateXor((Mem_Flex_t *)pNtk->pManFunc, 2)); // ДОБАВЛЯЕМ XOR
																					  //printf("%s = XOR(%s, %s)\n", out, inputs[0], inputs[1]);
		}
		else if (type == 8)
		{
			pNode = Io_ReadCreateNode(pNtk, out, inputs, 2);
			Abc_ObjSetData(pNode, Abc_SopCreateNxor((Mem_Flex_t *)pNtk->pManFunc, 2)); // ДОБАВЛЯЕМ NXOR
																					   //printf("%s = NXOR(%s, %s)\n", out, inputs[0], inputs[1]);
		}
		else
		{
			printf("Io_ReadBenchNetwork(): Cannot determine gate type \"%d\" in line .\n", type);
			Abc_NtkDelete(pNtk);
			return NULL;
		}

	}

	Abc_NtkFinalizeRead(pNtk);
	return pNtk;
}

// Здесь нужен Димин код по генерации Ntk схем из fcirc массива
// на выходе Ntk массив fcircNtk
// не генерировать их для очевидных ошибок (obvious_faults)
// Места где obvious_faults == True, оставляем схему равной NULL, что бы пропускать такие в следующих функциях
Abc_Ntk_t **generate_ntk_circuits_array(struct circuit **fcirc, int flnum, int *obvious_faults) {
	int i;
	Abc_Ntk_t **ntk_array;

	ntk_array = (Abc_Ntk_t**) malloc(flnum * sizeof(Abc_Ntk_t *));
	for (i = 0; i < flnum; i++)
	{
		Abc_Ntk_t *bench, *logic;
		if (obvious_faults[i] != -1)
		{
			ntk_array[i] = NULL;
		}
		else 
		{
			//CONVERTING TO NETWORK
			bench = Cir_2_Ntk(fcirc[i]);
			//CONVERTING TO LOGIC
			logic = Abc_NtkToLogic(bench);
			//STRUCTURALLY HASHING
			ntk_array[i] = Abc_NtkStrash(logic, 1, 1, 1);		
			// SWEEP OUT
			Abc_NtkDelete(bench);
			Abc_NtkDelete(logic);
			//Abc_NtkDelete(ntk_array[i]);
		}
	}
	return ntk_array;
}


// Функция которая почистит память
void free_ntk_circuits(Abc_Ntk_t **fcircNtk, int flnum) {
	int i;
	for (i = 0; i < flnum; i++) {
		Abc_NtkDelete(fcircNtk[i]);
	}
	free(fcircNtk);
	return;
}