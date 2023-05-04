#include "os_threadpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/* === TASK === */

/* Creates a task that thread must execute */
os_task_t *task_create(void *arg, void (*f)(void *))
{
	os_task_t *newTask = malloc(sizeof(os_task_t));

	if (newTask == NULL) {
		printf("Task was not created\n");
		return NULL;
	}
	newTask->argument = arg;
	newTask->task = f;
	return newTask;
}

/* Add a new task to threadpool task queue */
void add_task_in_queue(os_threadpool_t *tp, os_task_t *t)
{
	os_task_queue_t *newTaskNode = malloc(sizeof(os_task_queue_t));

	if (newTaskNode == NULL) {
		printf("Task node was not created\n");
		return;
	}
	newTaskNode->task = t;
	newTaskNode->next = NULL;
	pthread_mutex_lock(&(tp->taskLock));
	if (tp->tasks == NULL) {
		tp->tasks = newTaskNode;
		pthread_mutex_unlock(&(tp->taskLock));
		return;
	} else {
		os_task_queue_t *curr = tp->tasks;

		while (curr->next != NULL) {
			curr = curr->next;
		}
		curr->next = newTaskNode;
		if (curr->next == NULL) {
			printf("Task was not added successfull\n");
			return;
		}
		pthread_mutex_unlock(&(tp->taskLock));
	}
}

/* Get the head of task queue from threadpool */
os_task_t *get_task(os_threadpool_t *tp)
{
	pthread_mutex_lock(&(tp->taskLock));
	if (tp->tasks != NULL) {
		os_task_t *curr = tp->tasks->task;

		tp->tasks = tp->tasks->next;
		pthread_mutex_unlock(&(tp->taskLock));
		return curr;
	}
	pthread_mutex_unlock(&(tp->taskLock));
	return NULL;
}

/* === THREAD POOL === */

/* Initialize the new threadpool */
os_threadpool_t *threadpool_create(unsigned int nTasks, unsigned int nThreads)
{
	os_threadpool_t *newThreadpool = malloc(sizeof(os_threadpool_t));

	if (pthread_mutex_init(&(newThreadpool->taskLock), NULL) != 0) {
		printf("Mutex initialization failed\n");
		return NULL;
	}
	newThreadpool->should_stop = 0;
	newThreadpool->num_threads = nThreads;
	newThreadpool->tasks = NULL;
	newThreadpool->threads = malloc(sizeof(pthread_t) * nThreads);
	for (int i = 0; i < nThreads; i++) {
		pthread_create(&newThreadpool->threads[i], NULL, thread_loop_function, newThreadpool);
	}

	return newThreadpool;
}

/* Loop function for threads */
void *thread_loop_function(void *args)
{
	os_threadpool_t *tp = (os_threadpool_t *)args;
	os_task_t *my_task;

	while (tp->should_stop == 0 || tp->tasks != NULL) {
		my_task = get_task(tp);
		if (my_task != NULL){
			my_task->task(my_task->argument);
			free(my_task);
		}
	}
}

/* Stop the thread pool once a condition is met */
void threadpool_stop(os_threadpool_t *tp, int (*processingIsDone)(os_threadpool_t *))
{
	while (processingIsDone(tp)) {
		continue;
	}
	tp->should_stop = 1;
	for (int i = 0; i < tp->num_threads; i++)
	{
		pthread_join(tp->threads[i], NULL);
	}
}
