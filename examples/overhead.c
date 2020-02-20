#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#define PERFSTUBS_USE_TIMERS
#include "perfstubs_api/timer.h"

#ifndef ID
#define ID -1
#endif

__attribute__((weak)) void inst_reg(int* pid, const char* f, const char* t) { *pid = ID; }
__attribute__((weak)) void inst_val(int id, int v) {}

#ifdef NOINST
#define INST(name,val)
#else
#define INST(name,val)                          \
 do { static int id = { 0 };                    \
   if (id >= 0) {                               \
     if (!id) inst_reg(&id, __func__, name);    \
     inst_val(id, val);                         \
   } } while(0)
#endif

#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

void set_thread_affinity(void) {
#if !defined(__APPLE__) && !defined(_MSC_VER)
    unsigned int s, j;
    cpu_set_t cpuset;
    pthread_t thread;
    thread = pthread_self();

    /* Set affinity mask to the last physical core */
    CPU_ZERO(&cpuset);
    unsigned int cores = sysconf(_SC_NPROCESSORS_ONLN) >> 2;
    j = cores - 1;
    j = 0;
    CPU_SET(j, &cpuset);
    printf("Pinned to core %u\n", j);

    s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (s != 0) handle_error_en(s, "pthread_setaffinity_np");
#endif
}

double wtime()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return (double)now.tv_sec + 0.000001 * now.tv_usec;
}

// returns number of instrumentations triggered
int mm(int s, double* a, double* b, double* c)
{
    int i,j,k;
    for(i = 0; i < s; i++) {
        for(j = 0; j < s; j++) {
            for(k = 0; k < s; k++)
                a[i*s+j] += b[i*s+k] * c[k*s+j];
        }
    }
    return s * s;
}

// returns number of instrumentations triggered
int mm_inst(int s, double* a, double* b, double* c)
{
    PERFSTUBS_TIMER_START_FUNC(_timer);
    int i,j,k;
    for(i = 0; i < s; i++) {
        for(j = 0; j < s; j++) {
            //INST("tag", j);
            PERFSTUBS_TIMER_START(_timer, "tag");
            for(k = 0; k < s; k++)
                a[i*s+j] += b[i*s+k] * c[k*s+j];
            PERFSTUBS_TIMER_STOP(_timer);
        }
    }
    PERFSTUBS_TIMER_STOP_FUNC(_timer);
    return s * s;
}

int main(int argc, char* argv[])
{
    int s = 0;
    int imax = 0;
    if (argc > 1) s = atoi(argv[1]);
    if (argc > 2) imax = atoi(argv[2]);
    if (s == 0) s = 100;
    if (imax == 0) imax = 10000;
    set_thread_affinity();

    PERFSTUBS_INITIALIZE();
    printf("Running %dx a MM on %d x %d\n", imax, s,s);
    double* a = malloc(s*s*sizeof(double));
    double* b = malloc(s*s*sizeof(double));
    double* c = malloc(s*s*sizeof(double));
    int i,j;
    for(i = 0; i < s; i++)
        for(j = 0; j < s; j++)
            a[i*s+j] = b[i*s+j] = c[i*s+j] = 1.0;

    double t1 = wtime();
    long inst_count = 0;
    int iter;
    for(iter = 0; iter < imax; iter++)
        inst_count += mm(s, a, b, c);
    double tdiff = wtime() - t1;

    printf("Warmup Time: %.3f s, instcount: %ld => rate: %.3f MI/s\n",
           tdiff, inst_count, ((double) inst_count) / tdiff *.000001 );

    t1 = wtime();
    inst_count = 0;
    for(iter = 0; iter < imax; iter++)
        inst_count += mm(s, a, b, c);
    tdiff = wtime() - t1;

    printf("Uninstrumented Time: %.3f s, instcount: %ld => rate: %.3f MI/s\n",
           tdiff, inst_count, ((double) inst_count) / tdiff *.000001 );

    t1 = wtime();
    inst_count = 0;
    for(iter = 0; iter < imax; iter++)
        inst_count += mm_inst(s, a, b, c);
    double tdiff2 = wtime() - t1;

    printf("Instrumented Time: %.3f s, instcount: %ld => rate: %.3f MI/s\n",
           tdiff2, inst_count, ((double) inst_count) / tdiff2 *.000001 );

    printf("Estimated overhead per call: %.3f nanoseconds per PerfStubs call.\n",
           ((tdiff2 - tdiff) / ((double) inst_count)) * 1.0e9);

    PERFSTUBS_FINALIZE();
    return 0;
}
