#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5
#define ELEMENTS 15

// Buffer condiviso
int buffer[BUFFER_SIZE];

// Successiva posizione libera nel buffer
int in = 0;

// Prima posizione piena nel buffer
int out = 0;