/*
	Comments
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifndef FIM_H
#include "fim.h"
#endif


// Записываем результат в файл
void create_faults_pair_file(char *out_file, int *final_faults, struct fault_descr *fl, int flnum) {
	FILE *out;
	int i, group, cur_point;
	int *fa;

	// Copy array to have ability to modify it in process
	fa = calloc(flnum, sizeof(int));
	memcpy(fa, final_faults, sizeof(int) * flnum);

	out = fopen(out_file, "w");
	if (out == NULL) {
		printf("Cant open file: %s for writing", out_file);
		exit(0);
	}

	// Print all groups in array
	// Worst speed is O(n^2)
	while (1) {

		// Find current group (if not found then finish)
		group = -1;
		cur_point = -1;
		for (i = 0; i < flnum; i++) {
			if (fa[i] == -1)
				continue;
			group = fa[i];
			cur_point = i;
			fa[i] = -1;
			break;
		}
		if (group == -1)
			break;

		// Output all elements of current group 
		for (i = 0; i < flnum; i++) {
			if (fa[i] == group) {
				fprintf(out, "%d %d\n", fl[cur_point].id, fl[i].id);
				fa[i] = -1;
			}
		}
	}

	free(fa);
	fclose(out);
	return;
}
