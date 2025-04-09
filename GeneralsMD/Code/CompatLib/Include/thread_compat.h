#pragma once

#include <pthread.h>
#include <stdint.h>
#include <unistd.h>  // For usleep
#include <sched.h>   // For sched_yield

typedef pthread_t THREAD_ID;
typedef uint32_t (*start_routine)(void *);

THREAD_ID GetCurrentThreadId();
void* CreateThread(void *lpSecure, size_t dwStackSize, start_routine lpStartAddress, void *lpParameter, unsigned long dwCreationFlags, unsigned long *lpThreadId);
unsigned long _beginthread(void (*start_address)(void*), unsigned stack_size, void* arg);
int TerminateThread(void *hThread, unsigned long dwExitCode);
unsigned GetCurrentThreadIdCompat();
void SwitchThread();
