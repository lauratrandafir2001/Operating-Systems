#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "os_graph.h"
#include "os_threadpool.h"
#include "os_list.h"

#define MAX_TASK 100
#define MAX_THREAD 4

int sum;
os_graph_t *graph;
os_threadpool_t *tp;
pthread_mutex_t my_mutex;

void processNode(void *nodeIdx)
{
	os_node_t *node = graph->nodes[(unsigned int)nodeIdx];

	pthread_mutex_lock(&(my_mutex));
	sum += node->nodeInfo;
	pthread_mutex_unlock(&(my_mutex));
	for (int i = 0; i < node->cNeighbours; i++) {
		pthread_mutex_lock(&(my_mutex));
		if (graph->visited[node->neighbours[i]] == 0) {
			graph->visited[node->neighbours[i]] = 1;
			os_task_t *task = task_create((void *)(node->neighbours[i]), processNode);

			add_task_in_queue(tp, task);
		}
		pthread_mutex_unlock(&(my_mutex));
	}
}

void traverse_graph(void)
{
	for (int i = 0; i < graph->nCount; i++) {
		pthread_mutex_lock(&(my_mutex));
		if (graph->visited[i] == 0) {
			graph->visited[i] = 1;
			os_task_t *task = task_create((void *)i, processNode);

			add_task_in_queue(tp, task);
		}

		pthread_mutex_unlock(&(my_mutex));
	}
}

int processingIsDone(void)
{
	for (int i = 0; i < graph->nCount; i++) {
		// it finds a node that is not visited so the function is not done
		// it returns 1
		if (graph->visited[i] == 0)
			return 1;
	}
	// processing is Done we return 0
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: ./main input_file\n");
		exit(1);
	}

	FILE *input_file = fopen(argv[1], "r");

	if (input_file == NULL) {
		printf("[Error] Can't open file\n");
		return -1;
	}

	graph = create_graph_from_file(input_file);

	if (graph == NULL) {
		printf("[Error] Can't read the graph from file\n");
		return -1;
	}


	if (pthread_mutex_init(&(my_mutex), NULL) != 0) {
		printf("Mutex initialization failed\n");
		return 1;
	}

	tp = threadpool_create(MAX_TASK, MAX_THREAD);
	if (tp == NULL) {
		printf("Threadpool creation failed\n");
		return 1;
	}
	traverse_graph();
	threadpool_stop(tp, processingIsDone);
	printf("%d", sum);
	pthread_mutex_destroy(&(my_mutex));
	pthread_mutex_destroy(&(tp->taskLock));
	return 0;
}
