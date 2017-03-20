/*
	Comments
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef FIM_H
#include "fim.h"
#endif
#include "base/io/ioAbc.h"
#include "proof/fra/fra.h"
#include "proof/fraig/fraig.h"

extern void Abc_NtkCecFraig(Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int nSeconds, int fVerbose);
extern void Abc_NtkVerifyReportError(Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int * pModel);

int My_NtkCecFraig(Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int nSeconds, int fVerbose)
{
	Prove_Params_t Params, *pParams = &Params;
	//    Fraig_Params_t Params;
	//    Fraig_Man_t * pMan;
	Abc_Ntk_t * pMiter, *pTemp;
	Abc_Ntk_t * pExdc = NULL;
	int RetValue;

	if (pNtk1->pExdc != NULL || pNtk2->pExdc != NULL)
	{
		if (pNtk1->pExdc != NULL && pNtk2->pExdc != NULL)
		{
			//printf("Comparing EXDC of the two networks:\n");
			Abc_NtkCecFraig(pNtk1->pExdc, pNtk2->pExdc, nSeconds, fVerbose);
			//printf("Comparing networks under EXDC of the first network.\n");
			pExdc = pNtk1->pExdc;
		}
		else if (pNtk1->pExdc != NULL)
		{
			//printf("Second network has no EXDC. Comparing main networks under EXDC of the first network.\n");
			pExdc = pNtk1->pExdc;
		}
		else if (pNtk2->pExdc != NULL)
		{
			//printf("First network has no EXDC. Comparing main networks under EXDC of the second network.\n");
			pExdc = pNtk2->pExdc;
		}
		else assert(0);
	}

	// get the miter of the two networks
	pMiter = Abc_NtkMiter(pNtk1, pNtk2, 1, 0, 0, 0);
	if (pMiter == NULL)
	{
		printf("Miter computation has failed.\n");
		return -1;
	}
	// add EXDC to the miter
	if (pExdc)
	{
		assert(Abc_NtkPoNum(pMiter) == 1);
		assert(Abc_NtkPoNum(pExdc) == 1);
		pMiter = Abc_NtkMiter(pTemp = pMiter, pExdc, 1, 0, 1, 0);
		Abc_NtkDelete(pTemp);
	}
	// handle trivial case
	RetValue = Abc_NtkMiterIsConstant(pMiter);
	if (RetValue == 0)
	{
		//printf("Networks are NOT EQUIVALENT after structural hashing.\n");
		// report the error
		pMiter->pModel = Abc_NtkVerifyGetCleanModel(pMiter, 1);
		Abc_NtkVerifyReportError(pNtk1, pNtk2, pMiter->pModel);
		ABC_FREE(pMiter->pModel);
		Abc_NtkDelete(pMiter);
		return RetValue;
	}
	if (RetValue == 1)
	{
		//printf("Networks are equivalent after structural hashing.\n");
		Abc_NtkDelete(pMiter);
		return RetValue;
	}

	// solve the CNF using the SAT solver
	Prove_ParamsSetDefault(pParams);
	pParams->nItersMax = 5;
	//    RetValue = Abc_NtkMiterProve( &pMiter, pParams );
	//    pParams->fVerbose = 1;
	RetValue = Abc_NtkIvyProve(&pMiter, pParams);
	Abc_NtkDelete(pMiter);
	return RetValue;
}

// reading bench files
Abc_Ntk_t * My_ReadBenchNetwork(Extra_FileReader_t * p)
{
	ProgressBar * pProgress;
	Vec_Ptr_t * vTokens;
	Abc_Ntk_t * pNtk;
	Abc_Obj_t * pNode;
	Vec_Str_t * vString;
	char * pType, ** ppNames;
	int iLine, nNames;

	// allocate the empty network
	pNtk = Abc_NtkStartRead("temp");

	// go through the lines of the file
	vString = Vec_StrAlloc(100);
	pProgress = Extra_ProgressBarStart(stdout, Extra_FileReaderGetFileSize(p));
	for (iLine = 0; (vTokens = (Vec_Ptr_t *)Extra_FileReaderGetTokens(p)); iLine++)
	{
		Extra_ProgressBarUpdate(pProgress, Extra_FileReaderGetCurPosition(p), NULL);

		if (vTokens->nSize == 1)
		{
			printf("%s: Wrong input file format.\n", Extra_FileReaderGetFileName(p));
			Vec_StrFree(vString);
			Abc_NtkDelete(pNtk);
			return NULL;
		}

		// get the type of the line
		if (strncmp((char *)vTokens->pArray[0], "INPUT", 5) == 0)
		{
			Io_ReadCreatePi(pNtk, (char *)vTokens->pArray[1]);   // СОЗДАЁМ ПЕРВИЧНЫЙ ВХОД
		}
		else if (strncmp((char *)vTokens->pArray[0], "OUTPUT", 5) == 0)
		{
			Io_ReadCreatePo(pNtk, (char *)vTokens->pArray[1]);   // СОЗДАЁМ ПЕРВИЧНЫЙ ВЫХОД
		}
		else
		{
			// get the node name and the node type
			pType = (char *)vTokens->pArray[1];

			if (strncmp(pType, "DFF", 3) == 0)
			{
				pNode = Io_ReadCreateLatch(pNtk, (char *)vTokens->pArray[2], (char *)vTokens->pArray[0]); // ДОБАВЛЯЕМ DFF
				Abc_LatchSetInit0(pNode);
			}
			else
			{
				// create a new node and add it to the network
				ppNames = (char **)vTokens->pArray + 2;
				nNames = vTokens->nSize - 2;
				pNode = Io_ReadCreateNode(pNtk, (char *)vTokens->pArray[0], ppNames, nNames);
				// assign the cover
				if (strcmp(pType, "AND") == 0)
					Abc_ObjSetData(pNode, Abc_SopCreateAnd((Mem_Flex_t *)pNtk->pManFunc, nNames, NULL));
				else if (strcmp(pType, "OR") == 0)
					Abc_ObjSetData(pNode, Abc_SopCreateOr((Mem_Flex_t *)pNtk->pManFunc, nNames, NULL));
				else if (strcmp(pType, "NAND") == 0)
					Abc_ObjSetData(pNode, Abc_SopCreateNand((Mem_Flex_t *)pNtk->pManFunc, nNames));
				else if (strcmp(pType, "NOR") == 0)
					Abc_ObjSetData(pNode, Abc_SopCreateNor((Mem_Flex_t *)pNtk->pManFunc, nNames));
				else if (strcmp(pType, "XOR") == 0)
					Abc_ObjSetData(pNode, Abc_SopCreateXor((Mem_Flex_t *)pNtk->pManFunc, nNames));
				else if (strcmp(pType, "NXOR") == 0 || strcmp(pType, "XNOR") == 0)
					Abc_ObjSetData(pNode, Abc_SopCreateNxor((Mem_Flex_t *)pNtk->pManFunc, nNames));
				else if (strncmp(pType, "BUF", 3) == 0)
					Abc_ObjSetData(pNode, Abc_SopCreateBuf((Mem_Flex_t *)pNtk->pManFunc));
				else if (strcmp(pType, "NOT") == 0)
					Abc_ObjSetData(pNode, Abc_SopCreateInv((Mem_Flex_t *)pNtk->pManFunc));
				else if (strncmp(pType, "gnd", 3) == 0)
					Abc_ObjSetData(pNode, Abc_SopRegister((Mem_Flex_t *)pNtk->pManFunc, " 0\n"));
				else if (strncmp(pType, "vdd", 3) == 0)
					Abc_ObjSetData(pNode, Abc_SopRegister((Mem_Flex_t *)pNtk->pManFunc, " 1\n"));
				else
				{
					printf("Io_ReadBenchNetwork(): Cannot determine gate type \"%s\" in line %d.\n", pType, Extra_FileReaderGetLineNumber(p, 0));
					Vec_StrFree(vString);
					Abc_NtkDelete(pNtk);
					return NULL;
				}
			}
		}
	}
	Extra_ProgressBarStop(pProgress);
	Vec_StrFree(vString);
	Abc_NtkFinalizeRead(pNtk);


	return pNtk;
}

Abc_Ntk_t * My_ReadBench(char * pFileName, int fCheck)
{
	Extra_FileReader_t * p;
	Abc_Ntk_t * pNtk;

	// start the file
	p = Extra_FileReaderAlloc(pFileName, "#", "\n\r", " \t,()=");
	if (p == NULL)
		return NULL;

	// read the network
	pNtk = My_ReadBenchNetwork(p);
	Extra_FileReaderFree(p);
	if (pNtk == NULL)
		return NULL;

	// make sure that everything is okay with the network structure
	if (fCheck && !Abc_NtkCheckRead(pNtk))
	{
		printf("My_ReadBench: The network check has failed.\n");
		Abc_NtkDelete(pNtk);
		return NULL;
	}
	return pNtk;
}


//verify functional equivalence of two circuits
int verify(int sc1, int sc2, char * dir, int tst_num)
{
	Abc_Ntk_t *tmp1, *tmp2, *tmp3, *tmp4, *sch1, *sch2;
	char pFileName1[20];
	char pFileName2[20];
	int *out1;
	int *out2;
	int outputs;
	int i, seed;
	sprintf(pFileName1, "%s//%d.bench", dir, sc1);
	sprintf(pFileName2, "%s//%d.bench", dir, sc2);
	//READING BENCH
	//tmp1 = My_ReadBench(pFileName1, 1);
	//tmp2 = My_ReadBench(pFileName2, 1);
	struct circuit * ckt1;
	struct circuit * ckt2;
	ckt1 = read_initial_circuit(pFileName1);
	ckt2 = read_initial_circuit(pFileName2);
	tmp1 = Cir_2_Ntk(ckt1);
	tmp2 = Cir_2_Ntk(ckt2);
	free_circuit(ckt1);
	free_circuit(ckt2);

	//CONVERTING TO LOGIC
	tmp3 = Abc_NtkToLogic(tmp1);
	tmp4 = Abc_NtkToLogic(tmp2);
	//STRUCTURALLY HASHING
	sch1 = Abc_NtkStrash(tmp3, 1, 1, 1);
	sch2 = Abc_NtkStrash(tmp4, 1, 1, 1);
	//Abc_NtkDelete(tmp);
	Abc_NtkDelete(tmp1);
	Abc_NtkDelete(tmp2);
	Abc_NtkDelete(tmp3);
	Abc_NtkDelete(tmp4);
	//Check outputs number
	outputs = sch1->vPos->nSize;
	srand(time(NULL));
	seed = rand();
	out1 = simulate(sch1, tst_num, outputs, seed);
	out2 = simulate(sch2, tst_num, outputs, seed);
	for (i = outputs; i < outputs*tst_num; i++) {
		if (out1[i] != out2[i]) {
			printf("False");
			return 0;
		}
	}
	printf("True");
	return 0;
}

