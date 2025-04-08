# Nome dell'eseguibile
TARGET = main

# Compilatore
CC = gcc

# Flag di compilazione (Wall = mostra tutti i warning, pthread = libreria per thread)
CFLAGS = -Wall -pthread

# File sorgenti
SRCS = main.c computation.c

# File oggetto (convertiamo i .c in .o)
OBJS = $(SRCS:.c=.o)

# Regola principale per creare l'eseguibile
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Regola per compilare i file .c in .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Pulizia dei file compilati
clean:
	rm -f $(OBJS) $(TARGET)