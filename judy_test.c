#include <stdio.h>

#include "judy_wrapper.h"

void printme(Word_t index, Word_t value, void* user)
{
	//printf("%u -> %u\n", (unsigned int)index, (unsigned int)value);
}

#include <time.h>

int main()
{
	Pvoid_t judy = 0;
	int i;

	clock_t t_0 = clock();

	for (i = 0; i < 10000000; i++) {
		if (!judy_w_insert(&judy, i * 10, i + 1)) {
			printf("error A\n");
			exit(EXIT_FAILURE);
		}
	}

	clock_t t_1 = clock();

	for (i = 0; i < 10000000; i++) {
		if (!judy_w_contains(&judy, i * 10)) {
			printf("error A\n");
			exit(EXIT_FAILURE);
		}
	}

	clock_t t_2 = clock();

	judy_w_walk(judy, printme, 0);

	clock_t t_3 = clock();

	printf("insert: %lf\n", (double)(t_1 - t_0) / CLOCKS_PER_SEC);
	printf("contains: %lf\n", (double)(t_2 - t_1) / CLOCKS_PER_SEC);
	printf("walk: %lf\n", (double)(t_3 - t_2) / CLOCKS_PER_SEC);

	if (!judy_w_destroy(&judy)) {
		printf("error B\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
