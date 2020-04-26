#include <stdio.h>
#include <time.h>
#include <BigInt.h>

#define FIB_LIMIT 5500
// #define OPTIMIZED

#ifdef OPTIMIZED
	#include <omp.h>
#endif


void fib(int n) {
	BigInt a = {0};
	BigInt b = {0, [BIGINT_LEN-1] = 1};
	BigInt c;

	for (int i = 1; i < n; i++) {
		BigIntAdd(a, b, c);
		BigIntAssign(a, b);
		BigIntAssign(b, c);
	}
}

int main(int argc, const char *argv[]) {
	if (argv[1] == NULL) {
		fprintf(stderr, "Usage: fibtest <output_file.csv>\n");
		return 1;
	}

	FILE *output = fopen(argv[1], "w");
	if (output == NULL) {
		fprintf(stderr, "Failed to open file %s\n", argv[1]);
		return 1;
	}

	fprintf(output, "sep=,\nNumber,CPU Time (s)\n");

#ifdef OPTIMIZED
	#pragma omp parallel for schedule(dynamic)
#endif
	for (int i = 500; i <= FIB_LIMIT; i += 500) {
		clock_t start = clock();
		fib(i);
		clock_t end = clock();

#ifdef OPTIMIZED
	#pragma omp critical
#endif
		fprintf(output, "%d,%G\n", i, (double)(end - start) / CLOCKS_PER_SEC);
	}

	fclose(output);
	return 0;
}
