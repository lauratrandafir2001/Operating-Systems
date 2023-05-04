# Parallel Graph

# About
Operating System course
https://ocw.cs.pub.ro/courses/so

May 2023

Student:  Trandafir Laura;

# Description

For this assignment, we will implement a generic thread pool, which we will then use to traverse a graph and compute the sum of the elements contained by the nodes.
You will be provided with a serial implementation of the graph traversal and with most of the data structures needed to implement the thread pool.
Your job is to write the thread pool routines and then use the thread pool to traverse the graph.

## Thread Pool Description

A thread pool contains a given number of active threads that simply wait to be given specific tasks.
The threads are created when the thread pool is created they poll a task queue until a task is available.
Once the tasks are put in the task queue, the threads start running the task.
A thread pool creates N threads when the thread pool is created and does not destroy (join) them throughout the lifetime of the thread pool.
That way, the penalty of creating and destroying threads ad hoc is avoided.
As such, you must implement the following functions (marked with `TODO` in the provided skeleton):

# Implementation:

# parallel.c
    traverse_graph: we use this function to iterate through all the nodes from the graph
                    we use a lock
                    we mark each node as visited, and then create a task with the processNode function
                    we add the task to the queue
                    we unlock the lock

    processNode: we use this function to get the node passed by traverse_graph 
                add it to the sum, and then use a lock to iterate through 
                the neighbors of the node
                we create a task for each neighbor with the function processNode
                we add the task to the queue
                we unlock the mutex

    processingisDone:
                we verify if all the nodes in the graph were visited

    main:
    We create the graph from the input file, then create the thread pool
    We use the traverse graph to iterate and then call threadpool_stop to
    see if any nodes were not visited and after that, we distroy the 
    mutexes


#  os_thread_pool.cs:
    task_create: we allocate memory for a new task, and verify if the memory was allocated right
                and then set the pointer to the function, that the task has, and the arguments
                that we need to call that function
    
    add_task_in_queue: we create a node to add to the queue and put the task inside of it,
                        initializing next pointer to NULL
                        we use a lock to verify if the queue was empty before
                        if it was empty we add the node as the head of the queue
                        if it was not empty we iterate through it and add it at the
                        end of the queue
                        we unlock the mutex

    get_task: we use a mutex in case 2 threads want to get the head at the same time
            we get the head of the task queue from threadpool and move the head to the next one

    threadpool_create: we allocate memory for the threadpool, set the should_Stop parameter to 0,
                        initialize the num_threads to the number of threads given, the tasks queue to NULL
                        and allocate memory for the threads and create them

    thread_loop_function: used by threads to iterate through the tasks and call the function inside of them
                        we free the task after, and we iterate as long as the should_stop condition is not meant and
                        the queue of tasks is not empty, otherwise we stop
    
    threadpool_stop: as long as the processing is not done we continue, 
                    if the processing is done then we put the should_Stop condition on 1
                    and join all the threads



References:
- ["Threads](https://open-education-hub.github.io/operating-systems/Lab/Compute/threads)
- [Syncronzation](https://open-education-hub.github.io/operating-systems/Lab/Compute/synchronization)

