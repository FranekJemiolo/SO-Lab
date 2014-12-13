// This program was written by Franciszek Jemioło, index number 346919.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"
#define BUFF_SIZE 1024
#define MAX_SIZE 10000
#define Read 0
#define Write 1
#define ParentRead parent_pipe[0]
#define ParentWrite child_pipe[1]
#define ChildRead child_pipe[0]
#define ChildWrite parent_pipe[1]
// parent_pipe is the pipe responsible for writing to child and reading from
// from the child. child_pipe is responsible for writing to parent and reading 
// from parent.

int main (int argc, char* argv[])
{
    pid_t pid;
    char buffer[BUFF_SIZE];
    // Our previous value, and actual value.
    int prev = 0, act = 0;
    // Which step we are calculating.
    int step = 0;
    int prevReaden = 0;
    int results[MAX_SIZE];
    // The arguments array for our process.
    char* args[4];
    // The number of line of pascal triangle that we are calculating.
    int n = atoi (argv[1]);
    // We are w(i).
    int i = atoi (argv[2]);
    int nexti = i+1;
    sprintf (buffer, "%d", nexti);
    args[0] = argv[0];
    args[1] = argv[1];
    args[2] = buffer;
    args[3] = argv[3];
    int child_pipe[2];
    int parent_pipe[2];
    if (pipe (child_pipe) < 0) 
        syserr ("Error in pipe w child_pipe\n");
    if (pipe (parent_pipe) < 0) 
        syserr ("Error in pipe w parent_pipe\n");
    if (n - i > 0) {
        switch (pid = fork())
        {
            case -1:
                syserr("Error in fork\n");
            case 0:
                // Redirecting standard i/o for child so that it reads from our
                // process (child_pipe) and writing to our pipe (parent_pipe).
                close (ParentWrite);
                close (ParentRead);
                dup2 (ChildRead, Read);
                dup2 (ChildWrite, Write);
                close (ChildRead);
                close (ChildWrite);
                // Running i+1-th process to calculate i+1-th factor.
                execv("W",args);
                syserr ("Error in execv\n");        
        }
    }
    
    int k = 0;
    
    // Reading which step we are calculating and previous value from W(i-1).
    for (k = i; k <= n; k++)
    {
        if (read (Read, &step, sizeof (int)) != sizeof(int)) 
            syserr ("Error in reading from parent\n");
        if (read (Read, &prevReaden, sizeof (int)) != sizeof(int)) 
            syserr ("Error in reading from parent\n");
        if (i == 1) {
            prev = 1;
            act = 1;
        }
        else if (i < step) {
            act = prev + prevReaden;
        }
        else if (i == step) {
            act = 1;
            prev = 1;
        }
        if (i < step) {
            if (write (ParentWrite, &step, sizeof (int)) != sizeof (int)) 
                syserr("Error in write w\n"); 
            if (write (ParentWrite, &prev, sizeof (int)) != sizeof (int)) 
                syserr("Error in write w\n");   
        }
        prev = act;
    }
    close (ParentWrite);
    close (ChildRead);
    // Redirecting standard input to ParentRead so that we can read from child.
    if (i < n) {
        if (dup2 (ParentRead, Read) < 0) 
            syserr ("Error in dup2\n");
        if (close (ParentRead) < 0) 
            syserr ("Error in close ParentRead\n");
    }
    // Recieving calculated factors of pascal triangle from w's children
    // and sending them to pascal.
    for (k = i; k < n; k++)
    {
        if (read (Read, &results[i], sizeof (int)) != sizeof (int)) 
            syserr ("Error in reading from child w(%d)\n", i);
        if (write (Write, &results[i], sizeof (int)) != sizeof (int)) 
            syserr ("Error in writing to parent while w(%d)\n", i);
    }
    if (write (Write, &act, sizeof (int)) != sizeof (int)) 
        syserr ("Error in writing to parent w(%d)\n", i);
    if (close (Write) < 0) 
        syserr ("Error in close Write\n");
    if (close (Read) < 0) 
        syserr ("Error in close Read\n");
    if (i < n)
        if (wait (0) < 0)
            syserr ("Error in wait w\n");
    exit (0);
    return 0;
    
}
