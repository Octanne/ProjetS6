#ifndef __ST_BENCHMARK_H__
#define __ST_BENCHMARK_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>

#define ST_COLOR_RESET "\033[0m"
#define ST_COLOR_RED "\033[0;31m"
#define ST_COLOR_YELLOW "\033[0;33m"

#define ST_BENCHMARK_BETWEEN(ST_BENCH_buffer, ST_BENCH_f1, ST_BENCH_f2, ST_BENCH_f1_name, ST_BENCH_f2_name, ST_BENCH_testing_time) \
	{ \
	long ST_BENCH_countF1 = 0; \
	long ST_BENCH_countF2 = 0; \
	time_t ST_BENCH_end = time(NULL) + 1; \
	while (time(NULL) < ST_BENCH_end) {} \
	ST_BENCH_end = time(NULL) + ST_BENCH_testing_time; \
	while (time(NULL) < ST_BENCH_end) { \
		ST_BENCH_f1; \
		ST_BENCH_countF1 += 1; \
	} \
	ST_BENCH_end = time(NULL) + ST_BENCH_testing_time; \
	while (time(NULL) < ST_BENCH_end) { \
		ST_BENCH_f2; \
		ST_BENCH_countF2 += 1; \
	} \
	if (ST_BENCH_countF1 > ST_BENCH_countF2) { \
		sprintf(ST_BENCH_buffer, ST_COLOR_YELLOW "[BENCHMARK] " ST_COLOR_RED ST_BENCH_f1_name " > " ST_BENCH_f2_name " by " ST_COLOR_YELLOW "%f" ST_COLOR_RED " times with" ST_COLOR_YELLOW "\n[BENCHMARK] " ST_COLOR_RED ST_BENCH_f1_name " executed " ST_COLOR_YELLOW "%ld" ST_COLOR_RED " times and " ST_BENCH_f2_name " executed " ST_COLOR_YELLOW "%ld" ST_COLOR_RED " times\n" ST_COLOR_RESET, (double)ST_BENCH_countF1 / (double)ST_BENCH_countF2, ST_BENCH_countF1, ST_BENCH_countF2); \
	} else { \
		sprintf(ST_BENCH_buffer, ST_COLOR_YELLOW "[BENCHMARK] " ST_COLOR_RED ST_BENCH_f1_name " < " ST_BENCH_f2_name " by " ST_COLOR_YELLOW "%f" ST_COLOR_RED " times with" ST_COLOR_YELLOW "\n[BENCHMARK] " ST_COLOR_RED ST_BENCH_f1_name " executed " ST_COLOR_YELLOW "%ld" ST_COLOR_RED " times and " ST_BENCH_f2_name " executed " ST_COLOR_YELLOW "%ld" ST_COLOR_RED " times\n" ST_COLOR_RESET, (double)ST_BENCH_countF2 / (double)ST_BENCH_countF1, ST_BENCH_countF1, ST_BENCH_countF2); \
	} \
	}


#define ST_BENCHMARK_SOLO(ST_BENCH_buffer, ST_BENCH_f, ST_BENCH_f_name, ST_BENCH_count) \
	{ \
	struct timeval ST_BENCH_timeval, ST_BENCH_timeval2; \
	gettimeofday(&ST_BENCH_timeval, NULL); \
	unsigned long ST_BENCH_time = 1000000 * ST_BENCH_timeval.tv_sec + ST_BENCH_timeval.tv_usec; \
	long ST_BENCH_i = 0; \
	for (ST_BENCH_i = 0; ST_BENCH_i < ST_BENCH_count; ST_BENCH_i++) { \
		ST_BENCH_f; \
	} \
	gettimeofday(&ST_BENCH_timeval2, NULL); \
	ST_BENCH_time = 1000000 * ST_BENCH_timeval2.tv_sec + ST_BENCH_timeval2.tv_usec - ST_BENCH_time; \
	sprintf(ST_BENCH_buffer, ST_COLOR_YELLOW "[BENCHMARK] " ST_COLOR_RED ST_BENCH_f_name " executed " ST_COLOR_YELLOW "%d" ST_COLOR_RED " times in " ST_COLOR_YELLOW "%lf" ST_COLOR_RED "s\n" ST_COLOR_RESET, ST_BENCH_count, (double)ST_BENCH_time / 1000000.0); \
	}

#endif