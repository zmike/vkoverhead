/*
 * Copyright (C) 2009  VMware, Inc.  All Rights Reserved.
 * Copyright (C) 2021  Advanced Micro Devices, Inc.
 * Copyright 2022 Valve Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * VMWARE BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * Common perf code.  This should be re-usable with other tests.
 */

#include "os_time.h"
#include "common.h"
#include <stdio.h>

static double
measure_rate(perf_rate_func f, double duration, unsigned initial_iterations,
	     double (*measure_time)(perf_rate_func f, unsigned iterations))
{
	unsigned iterations = initial_iterations;

	while (1) {
		double t = measure_time(f, iterations);

		/* Allow iterations to go 5% over. */
		double time_adjusted = t - t * 0.05;
		double multiplier = time_adjusted ? duration / time_adjusted : 0;

		bool accepted = t >= duration;
		bool converging = (unsigned)(iterations * multiplier) > iterations;

		if (0) {
			printf("\niterations %7u, time %f, multiplier %f%s", iterations, t, multiplier,
			       accepted ? " - accepted" : (converging ? "" : " - stuck"));
		}

		/* Find the time it takes to run the test at duration. */
		if (!accepted) {
			if (converging) {
				iterations *= multiplier;
			} else {
				/* Force convergence. */
				if (iterations > 20)
					iterations *= iterations / 20;
				iterations += 5;
			}
			continue;
		}

		/* Return iterations per second. */
		return iterations / t;
	}
}

static double
measure_cpu_time(perf_rate_func f, unsigned iterations)
{
	intptr_t t0 = os_time_get_nano();
	f(iterations); /* call the rendering function */
	return (os_time_get_nano() - t0) * 0.000000001;
}
#if 0
static double
measure_gpu_time(perf_rate_func f, unsigned iterations)
{
	/* Run a few iterations to wake up power management - this helps AMD. */
	f(iterations);
	f(iterations);

	/* Measure the execution time. */
	GLuint query;
	glGenQueries(1, &query);
	glBeginQuery(GL_TIME_ELAPSED, query);

	f(iterations);

	int64_t nsecs;
	glEndQuery(GL_TIME_ELAPSED);
	glGetQueryObjecti64v(query, GL_QUERY_RESULT, &nsecs);
	glDeleteQueries(1, &query);

	return nsecs * 0.000000001;
}


double
perf_measure_gpu_rate(perf_rate_func f, double duration)
{
	return measure_rate(f, duration, 5, measure_gpu_time);
}
#endif

/**
 * Return iterations/second for the duration.
 * Use a longer duration if you want more precision.
 */
double
perf_measure_cpu_rate(perf_rate_func f, double duration)
{
	return measure_rate(f, duration, 1, measure_cpu_time);
}


/**
 * Return iterations/second for a fixed number of iterations
 * Use a larger iteration count if you want more precision.
 */
double
perf_measure_cpu_fixed(perf_rate_func f, int iterations)
{
    return iterations / measure_cpu_time(f, iterations);
}
