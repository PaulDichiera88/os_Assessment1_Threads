Note: The makefile has been configured to work on the titan server, if you wish to run it on your 
systems VScode you may need to remove '-lstdc++fs' from the make file copilations.

Once the project is copied to titan we need to enable c++17, please use the following commands

- scl enable devtoolset-8 zsh
- g++ --version "you should see version 8 or 9."

From here we are able to use make and make clean if needed below are the commands to our two copiers.

to run mmcopier:
./mmcopier 10 task1/source_dir task1/destination_dir

note: the number can be changed from 1 - 10 it will alter how many files are copied in the operation

to run mscopier:
./mscopier 10 task2/source.txt task2/destination.txt

note: the number can be changed from 1 - 10 it will alter how many threads work on the operation

1. Mutex Lock ('pthread_mutex_lock') and Unlock ('pthread_mutex_unlock'):

File: mscopier.cpp

Lines: 155, 172, 186, 200

Description:
    - Line 155: in the 'read_call' function, line 'pthread_mutex_lock(threadData->queueMutex); 
    Locks the mutex before the thread checks and potentially modifies the shared 'readData queue.

    - Line 172: 'pthread_mutex_unlock(threadData->queueMutex); 'unlocks the mutex after the 
    shared data has been accessed and modified.

    - Line 186: In the 'write_call' function. 'pthread_mutex_lock(threadData->queueMutex):' locks 
    the mutex before the thread checks and modifies the shared 'readData' queue.

    - Line 200: 'pthread_mutex_unlock(threadData->queueMutex);' unlocks mutex after the shared data
    has been accessed and modified.

2. Condition Variables ('pthread_cond_wait', 'pthread_cond_signal', and 'pthread_cond_broadcast'):

File: mscopier.cpp

Lines: 159, 160, 163, 173, 189, 190, 201

Description:
    - Line 159: In the 'read_call' function 'pthread_cond_broadcast(threadData->condQueueWrite);' 
    signals all waiting threads that they can continue.

    -Line 160: 'pthread_cond_wait(threadData->condQueueRead, threadData->queueMutex);' puts the 
    reading thread to sleep if the queue is full and waits for the writing thread to signal that space 
    is available.

    -Line 163: 'pthread_cond_broadcast(threadData->condQueueWrite);' again signals all waiting threads 
    after the reader has completed its task or when new data is available in the queue.

    -Line 173:  'pthread_cond_signal(threadData->condQueueWrite);' signals a specific waiting writing
    thread that it can proceed.

    -Line 189: In the write_call function, 'pthread_cond_broadcast(threadData->condQueueRead);' signals 
    all waiting reader threads that they can continue.

    -Line 190: 'pthread_cond_wait(threadData->condQueueWrite, threadData->queueMutex);' puts the writing 
    thread to sleep if the queue is empty and waits for the reading thread to signal that data is available 
    to be written.

    -Line 201: 'pthread_cond_signal(threadData->condQueueRead);' signals a specific waiting reading thread
     that it can proceed with more data.

3. Mutex and Condition Variable Initialization and Destruction:

File: mscopier.cpp

Lines: 81, 82, 83, 132, 133, 134

Description:
    - Line 81: 'pthread_cond_init(&condQueueRead, NULL);' initializes the condition variable 'condQueueRead'.

    - Line 82: 'pthread_cond_init(&condQueueWrite, NULL);' initializes the condition variable 'condQueueWrite'.

    - Line 83: 'pthread_mutex_init(&queueMutex, NULL);' initializes the mutex 'queueMutex'.

    - Line 132: 'pthread_mutex_destroy(threadData->queueMutex);' destroys the mutex queueMutex after all threads have completed.

    - Line 133: 'pthread_cond_destroy(threadData->condQueueRead);' destroys the condition variable condQueueRead after all threads have completed.

    - Line 134: 'pthread_cond_destroy(threadData->condQueueWrite);' destroys the condition variable condQueueWrite after all threads have completed.