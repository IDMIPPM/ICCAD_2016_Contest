/*
	Чтение схемы, файла с faults и заполнение наших внутренних структур.
	Первичная инжекция ошибок.
	Функция для освобождения памяти.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#ifndef FIM_H
#include "fim.h"
#endif

// Trim string
void trim(char * s) {
	char * p = s;
	int l = strlen(p);

	while (isspace(p[l - 1])) {
		p[--l] = 0;
		if (l == 0)
			break;
	}

	while (*p && isspace(*p)) {
		++p;
		--l;
	}

	memmove(s, p, l + 1);
}


// Read initial circuit from file
// Return value: pointer on circuit
struct circuit *read_initial_circuit(char *design_file) {
	struct circuit *c;
	char buf[2048];
	char elem[16];
	int node, outNode, inNode1, inNode2, inNode3, inNode4, elemType;
	FILE *f = fopen(design_file, "r");
	if (f == NULL) {
		printf("Couldn't open file: %s with design!", design_file);
		exit(0);
	}
	c = (struct circuit *) malloc(sizeof(struct circuit));
	c->inputs = NULL;
	c->inNum = 0;
	c->outputs = NULL;
	c->outNum = 0;
	c->elem = NULL;
	c->elemNum = 0;
	c->type = 0;

	while (fgets(buf, 2048, f) != NULL) {
		trim(buf);
		if (strlen(buf) == 0)
			continue;
		if (buf[0] == '#')
			continue;
		// Input
		if (sscanf(buf, "INPUT(%d)", &node) == 1) {
			c->inputs = realloc(c->inputs, (c->inNum + 1) * sizeof(int));
			c->inputs[c->inNum] = node;
			c->inNum++;
		}
		// Output
		else if (sscanf(buf, "OUTPUT(%d)", &node) == 1) {
			c->outputs = realloc(c->outputs, (c->outNum + 1) * sizeof(int));
			c->outputs[c->outNum] = node;
			c->outNum++;
		}
		// 4 input element DFF
		else if (sscanf(buf, "%d = %[^(](%d, %d, %d, %d)", &outNode, elem, &inNode1, &inNode2, &inNode3, &inNode4) == 6) {
			c->elem = realloc(c->elem, (c->elemNum + 1) * sizeof(struct cell));
			if (!strcmp(elem, "DFF")) {
				elemType = DFF;
			}
			else {
				printf("Unknown element: %s. Skip!\n", elem);
				continue;
			}
			c->elem[c->elemNum].input1 = inNode4;
			c->elem[c->elemNum].input2 = -1;
			c->elem[c->elemNum].output = outNode;
			c->elem[c->elemNum].type = elemType;
			c->elemNum++;
			c->type = 1; // Set sequential to circuit
		}
		// 2 input elements
		else if (sscanf(buf, "%d = %[^(](%d, %d)", &outNode, elem, &inNode1, &inNode2) == 4) {
			c->elem = realloc(c->elem, (c->elemNum + 1) * sizeof(struct cell));
			if (!strcmp(elem, "AND")) {
				elemType = AND;
			}
			else if (!strcmp(elem, "NAND")) {
				elemType = NAND;
			}
			else if (!strcmp(elem, "OR")) {
				elemType = OR;
			}
			else if (!strcmp(elem, "NOR")) {
				elemType = NOR;
			}
			else if (!strcmp(elem, "XOR")) {
				elemType = XOR;
			}
			else if (!strcmp(elem, "NXOR")) {
				elemType = NXOR;
			}
			else {
				printf("Unknown element: %s. Skip!\n", elem);
				continue;
			}
			c->elem[c->elemNum].input1 = inNode1;
			c->elem[c->elemNum].input2 = inNode2;
			c->elem[c->elemNum].output = outNode;
			c->elem[c->elemNum].type = elemType;
			c->elemNum++;
		}
		// 1 input element
		else if (sscanf(buf, "%d = %[^(](%d)", &outNode, elem, &inNode1) == 3) {
			c->elem = realloc(c->elem, (c->elemNum + 1) * sizeof(struct cell));
			if (!strcmp(elem, "BUFF")) {
				elemType = BUFF;
			}
			else if (!strcmp(elem, "NOT")) {
				elemType = NOT;
			}
			else if (!strcmp(elem, "DFF")) {
				elemType = DFF;
			}
			else {
				printf("Unknown element: %s. Skip!\n", elem);
				continue;
			}
			c->elem[c->elemNum].input1 = inNode1;
			c->elem[c->elemNum].input2 = -1;
			c->elem[c->elemNum].output = outNode;
			c->elem[c->elemNum].type = elemType;
			c->elemNum++;
		}
		else {
			printf("Unknown element desciption: %s. Skip it!\n", buf);
		}

	}
	fclose(f);

	return c;
}


// Read fault description file
void read_fault_list(char *faults_file, struct fault_descr **faults, int *fault_num) {
	struct fault_descr *fl = NULL;
	int flnum = 0;
	int id, node;
	char buf[2048];
	FILE *f = fopen(faults_file, "r");
	if (f == NULL) {
		printf("Couldn't open file: %s with faults!", faults_file);
		exit(0);
	}

	while (fscanf(f, "%d %d %s", &id, &node, buf) != EOF) {
		fl = realloc(fl, (flnum + 1) * sizeof(struct fault_descr));
		fl[flnum].id = id;
		fl[flnum].node = node;
		if (!strcmp(buf, "SA0")) {
			fl[flnum].type = SA0;
		}
		else if (!strcmp(buf, "SA1")) {
			fl[flnum].type = SA1;
		}
		else if (!strcmp(buf, "NEG")) {
			fl[flnum].type = NEG;
		}
		else if (!strcmp(buf, "RDOB_AND")) {
			fl[flnum].type = RDOB_AND;
		}
		else if (!strcmp(buf, "RDOB_NAND")) {
			fl[flnum].type = RDOB_NAND;
		}
		else if (!strcmp(buf, "RDOB_OR")) {
			fl[flnum].type = RDOB_OR;
		}
		else if (!strcmp(buf, "RDOB_NOR")) {
			fl[flnum].type = RDOB_NOR;
		}
		else if (!strcmp(buf, "RDOB_XOR")) {
			fl[flnum].type = RDOB_XOR;
		}
		else if (!strcmp(buf, "RDOB_NXOR")) {
			fl[flnum].type = RDOB_NXOR;
		}
		else if (!strcmp(buf, "RDOB_NOT")) {
			fl[flnum].type = RDOB_NOT;
		}
		else if (!strcmp(buf, "RDOB_BUFF")) {
			fl[flnum].type = RDOB_BUFF;
		}
		else {
			printf("Unknown type %s in fault description!", buf);
			exit(0);
		}
		flnum++;
	}
	*fault_num = flnum;
	*faults = fl;
	fclose(f);
}


// 0 - combinational
// 1 - sequential

int get_design_type(char *design_file) {
	int i, len;
	char buf[2048];
	FILE *f = fopen(design_file, "r");
	if (f == NULL) {
		printf("Couldn't open file: %s with design!", design_file);
		exit(0);
	}

	while (fgets(buf, 2048, f) != NULL) {
		len = strlen(buf);
		if (len < 3)
			continue;
		for (i = 0; i < len - 3; i++) {
			if (buf[i] == 'D' && buf[i + 1] == 'F' && buf[i + 2] == 'F') {
				fclose(f);
				return 1;
			}
		}
	}
	fclose(f);
	return 0;
}


void print_circuit_statistics(struct circuit *circ) {
	if (circ->type == 0) {
		printf("Design type: combinational\n");
	}
	else {
		printf("Design type: sequential\n");
	}
	printf("Number of inputs: %d\n", circ->inNum);
	printf("Number of outputs: %d\n", circ->outNum);
	printf("Number of elements: %d\n", circ->elemNum);
}


// Get maximum node number
int get_max_node_number(struct circuit *c) {
	int i, maxNode;
	maxNode = -1;
	for (i = 0; i < c->inNum; i++) {
		if (c->inputs[i] > maxNode)
			maxNode = c->inputs[i];
	}
	for (i = 0; i < c->outNum; i++) {
		if (c->outputs[i] > maxNode)
			maxNode = c->outputs[i];
	}
	for (i = 0; i < c->elemNum; i++) {
		if (c->elem[i].input1 > maxNode)
			maxNode = c->elem[i].input1;
		if (c->elem[i].input2 > maxNode)
			maxNode = c->elem[i].input2;
		if (c->elem[i].output > maxNode)
			maxNode = c->elem[i].output;
	}
	return maxNode;
}


// Create new circuit with injected fault
struct circuit *inject_fault(
	struct circuit *circ,
	struct fault_descr fl,
	int zero_replace)
{
	int i;
	int fix_num = 0;
	int nodeType = 0;
	struct circuit *nc;

	// Alloc memory and copy initital circuit
	nc = calloc(1, sizeof(struct circuit));
	nc->inNum = circ->inNum;
	nc->outNum = circ->outNum;
	nc->elemNum = circ->elemNum;
	nc->inputs = calloc(nc->inNum, sizeof(int));
	nc->outputs = calloc(nc->outNum, sizeof(int));
	nc->elem = calloc(nc->elemNum, sizeof(struct cell));
	for (i = 0; i < nc->inNum; i++) {
		nc->inputs[i] = circ->inputs[i];
		if (nc->inputs[i] == fl.node) {
			nodeType = 1;
		}
	}
	for (i = 0; i < nc->outNum; i++) {
		nc->outputs[i] = circ->outputs[i];
		if (nc->outputs[i] == fl.node) {
			nodeType = 2;
		}
	}
	for (i = 0; i < nc->elemNum; i++) {
		nc->elem[i].input1 = circ->elem[i].input1;
		nc->elem[i].input2 = circ->elem[i].input2;
		nc->elem[i].output = circ->elem[i].output;
		nc->elem[i].type = circ->elem[i].type;
		if (nodeType == 0) {
			if (nc->elem[i].input1 == fl.node || nc->elem[i].input2 == fl.node || nc->elem[i].output == fl.node) {
				nodeType = 3;
			}
		}
	}

	// Check possible errors of data
	if (nodeType == 0) {
		printf("Unknown type for node: %d", fl.node);
		exit(0);
	}

	if (nodeType == 1 && fl.type != SA0 && fl.type != SA1 && fl.type != NEG) {
		printf("Unexpected fault type %d for input node", fl.node);
		exit(0);
	}

	// Inject error in elements
	for (i = 0; i < nc->elemNum; i++) {
		nc->elem[i].input1 = circ->elem[i].input1;
		nc->elem[i].input2 = circ->elem[i].input2;
		nc->elem[i].output = circ->elem[i].output;
		nc->elem[i].type = circ->elem[i].type;
		if (fl.type == SA0 || fl.type == SA1 || fl.type == NEG) {
			if (nc->elem[i].input1 == fl.node) {
				if (nc->elem[i].input1 == 0) {
					nc->elem[i].input1 = -zero_replace;
				}
				else {
					nc->elem[i].input1 = -nc->elem[i].input1;
				}
				fix_num += 1;
			}
			if (nc->elem[i].input2 == fl.node) {
				if (nc->elem[i].input2 == 0) {
					nc->elem[i].input2 = -zero_replace;
				}
				else {
					nc->elem[i].input2 = -nc->elem[i].input2;
				}
				fix_num += 1;
			}
			if (nodeType == 2) {
				if (nc->elem[i].output == fl.node) {
					if (nc->elem[i].output == 0) {
						nc->elem[i].output = -zero_replace;
					}
					else {
						nc->elem[i].output = -nc->elem[i].output;
					}
					fix_num += 1;
				}
			}
		}
		else if (fl.type == RDOB_AND && fl.node == nc->elem[i].output) {
			nc->elem[i].type = AND;
			fix_num += 1;
		}
		else if (fl.type == RDOB_NAND && fl.node == nc->elem[i].output) {
			nc->elem[i].type = NAND;
			fix_num += 1;
		}
		else if (fl.type == RDOB_OR && fl.node == nc->elem[i].output) {
			nc->elem[i].type = OR;
			fix_num += 1;
		}
		else if (fl.type == RDOB_NOR && fl.node == nc->elem[i].output) {
			nc->elem[i].type = NOR;
			fix_num += 1;
		}
		else if (fl.type == RDOB_XOR && fl.node == nc->elem[i].output) {
			nc->elem[i].type = XOR;
			fix_num += 1;
		}
		else if (fl.type == RDOB_NXOR && fl.node == nc->elem[i].output) {
			nc->elem[i].type = NXOR;
			fix_num += 1;
		}
		else if (fl.type == RDOB_NOT && fl.node == nc->elem[i].output) {
			nc->elem[i].type = NOT;
			fix_num += 1;
		}
		else if (fl.type == RDOB_BUFF && fl.node == nc->elem[i].output) {
			nc->elem[i].type = BUFF;
			fix_num += 1;
		}
	}

	// For some cases add additional element
	if (nodeType == 1 || nodeType == 3) {
		// Input or Internal
		if (fl.type == SA0) {
			nc->elem = realloc(nc->elem, (nc->elemNum + 1) * sizeof(struct cell));
			nc->elem[nc->elemNum].input1 = fl.node;
			nc->elem[nc->elemNum].input2 = fl.node;
			if (fl.node == 0) {
				nc->elem[nc->elemNum].output = -zero_replace;
			}
			else {
				nc->elem[nc->elemNum].output = -fl.node;
			}
			nc->elem[nc->elemNum].type = XOR;
			nc->elemNum += 1;
		}
		else if (fl.type == SA1) {
			nc->elem = realloc(nc->elem, (nc->elemNum + 1) * sizeof(struct cell));
			nc->elem[nc->elemNum].input1 = fl.node;
			nc->elem[nc->elemNum].input2 = fl.node;
			if (fl.node == 0) {
				nc->elem[nc->elemNum].output = -zero_replace;
			} 
			else {
				nc->elem[nc->elemNum].output = -fl.node;
			}
			nc->elem[nc->elemNum].type = NXOR;
			nc->elemNum += 1;
		}
		else if (fl.type == NEG) {
			nc->elem = realloc(nc->elem, (nc->elemNum + 1) * sizeof(struct cell));
			nc->elem[nc->elemNum].input1 = fl.node;
			if (fl.node == 0) {
				nc->elem[nc->elemNum].output = -zero_replace;
			}
			else {
				nc->elem[nc->elemNum].output = -fl.node;
			}
			nc->elem[nc->elemNum].type = NOT;
			nc->elemNum += 1;
		}
	}
	else if (nodeType == 2) {
		// Output
		if (fl.type == SA0) {
			nc->elem = realloc(nc->elem, (nc->elemNum + 1) * sizeof(struct cell));
			if (fl.node == 0) {
				nc->elem[nc->elemNum].input1 = -zero_replace;
				nc->elem[nc->elemNum].input2 = -zero_replace;
			}
			else {
				nc->elem[nc->elemNum].input1 = -fl.node;
				nc->elem[nc->elemNum].input2 = -fl.node;
			}
			nc->elem[nc->elemNum].output = fl.node;
			nc->elem[nc->elemNum].type = XOR;
			nc->elemNum += 1;
		}
		else if (fl.type == SA1) {
			nc->elem = realloc(nc->elem, (nc->elemNum + 1) * sizeof(struct cell));
			if (fl.node == 0) {
				nc->elem[nc->elemNum].input1 = -zero_replace;
				nc->elem[nc->elemNum].input2 = -zero_replace;
			}
			else {
				nc->elem[nc->elemNum].input1 = -fl.node;
				nc->elem[nc->elemNum].input2 = -fl.node;
			}
			nc->elem[nc->elemNum].output = fl.node;
			nc->elem[nc->elemNum].type = NXOR;
			nc->elemNum += 1;
		}
		else if (fl.type == NEG) {
			nc->elem = realloc(nc->elem, (nc->elemNum + 1) * sizeof(struct cell));
			if (fl.node == 0) {
				nc->elem[nc->elemNum].input1 = -zero_replace;
			}
			else {
				nc->elem[nc->elemNum].input1 = -fl.node;
			}
			nc->elem[nc->elemNum].output = fl.node;
			nc->elem[nc->elemNum].type = NOT;
			nc->elemNum += 1;
		}
	}
	if (fix_num == 0) {
		//printf("There was no fixes. Check it!");
		//exit(0);
	}
	return nc;
}

// Free circuit memory
void free_circuit(struct circuit *circ) {
	free(circ->inputs);
	free(circ->outputs);
	free(circ->elem);
}


// Print circuit in our format to file
void print_circuit_structure_in_file(struct circuit *circ, char *file_name) {
	int i;
	FILE *out;
	out = fopen(file_name, "w");
	fprintf(out, "# Circuit\n");
	fprintf(out, "# Type: %s\n", circ->type == 0 ? "combinational": "sequential");
	fprintf(out, "# Number of inputs: %d\n", circ->inNum);
	fprintf(out, "# Number of outputs: %d\n", circ->outNum);
	fprintf(out, "# Number of elements: %d\n", circ->elemNum);
	
	// fprintf(out, "# Inputs:\n");
	for (i = 0; i < circ->inNum; i++) {
		fprintf(out, "INPUT(%d)\n", circ->inputs[i]);
	}
	fprintf(out, "\n");
	
	// fprintf(out, "# Outputs:\n");
	for (i = 0; i < circ->outNum; i++) {
		fprintf(out, "OUTPUT(%d)\n", circ->outputs[i]);
	}
	fprintf(out, "\n");

	// fprintf(out, "# Elements:\n");
	for (i = 0; i < circ->elemNum; i++) {
		fprintf(out, "%d = ", circ->elem[i].output);
		if (circ->elem[i].type == BUFF) {
			fprintf(out, "%s", "BUFF");
		}
		else if (circ->elem[i].type == NOT) {
			fprintf(out, "%s", "NOT");
		}
		else if (circ->elem[i].type == AND) {
			fprintf(out, "%s", "AND");
		}
		else if (circ->elem[i].type == NAND) {
			fprintf(out, "%s", "NAND");
		}
		else if (circ->elem[i].type == OR) {
			fprintf(out, "%s", "OR");
		}
		else if (circ->elem[i].type == NOR) {
			fprintf(out, "%s", "NOR");
		}
		else if (circ->elem[i].type == XOR) {
			fprintf(out, "%s", "XOR");
		}
		else if (circ->elem[i].type == NXOR) {
			fprintf(out, "%s", "NXOR");
		}
		else if (circ->elem[i].type == DFF) {
			fprintf(out, "%s", "DFF");
		}
		else {
			printf("Unknown element type!!!\n");
			exit(1);
		}
		fprintf(out, "(%d", circ->elem[i].input1);
		if (circ->elem[i].type != BUFF && circ->elem[i].type != NOT && circ->elem[i].type != DFF) {
			fprintf(out, ", %d", circ->elem[i].input2);
		}
		fprintf(out, ")\n");

	}
	fprintf(out, "\n");
	fclose(out);
}

