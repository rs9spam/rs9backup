/*
 * the task-generation loop is put into a untied task
 * So when the thread running task-generation loop get preempted to
 * conduct the generated tasks,
 * the other threads can resume the task-generation task, which
 * is not tied to the original thread.
 */
#include <stdio.h>
#include <omp.h>

#define LARGE_NUMBER 10
//#define LARGE_NUMBER 10000000
double item[LARGE_NUMBER];
void process (double input)
{
  printf("processing %f by thread %d\n",input, omp_get_thread_num());
}
int
main ()
{
#pragma omp parallel
  {
#pragma omp single
    {
      int i;
      printf("Using %d threads.\n",omp_get_num_threads());
/*
 untied cannot be used with omp single
 So another level of task is needed to used untied here!!

 Explicit tasks: 
  * generated by #omp task
  * tied or untied

 Implicit tasks: 
  * generated by #omp parallel
  * always tied!!
*/   
#pragma omp task untied
// i is firstprivate, item is shared
      {
	for (i = 0; i < LARGE_NUMBER; i++)
	{
#pragma omp task if(1) 
	  process (item[i]);
	}
      }
    }
  }
  return 0;
}

