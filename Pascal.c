// This program was written by Franciszek Jemioło, index number 346919.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"
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
    if (argc == 2)
    {
        pid_t pid;
        // The array for the result which is n-th line of pascal triangle.
        int results[MAX_SIZE];
        // The arguments array for our process.
        char* args[4];
        // The number of line in pascal triangle that we are calculating.
        int n;
        n = atoi (argv[1]);
        // Initializing args for child process.
        // w - stands for the name of our calculating process, in [1] we are 
        // sending how many process we are instatiating, and in [2] our iterator 
        // for process so that it will know it's number.
        args[0] = "W";
        args[1] = argv[1];
        args[2] = "1";
        args[3] = NULL;
        int child_pipe[2];
        int parent_pipe[2];
        if (pipe (child_pipe) < 0) 
            syserr ("Error in pipe w child_pipe\n");
        if (pipe (parent_pipe) < 0) 
            syserr ("Error in pipe w parent_pipe\n");
        switch (pid = fork ())
        {

            case -1:
                syserr ("Error in fork");
            case 0:
                // Redirecting default descriptor for reading and writing for  
                // our child. Child will read from standard input from parent's 
                // pipe and write to parent's pipe so that a two way 
                // communication is established.
                close (ParentRead);
                close (ParentWrite);
                // We are closing duplicated ends to send good descriptors for 
                // our child.
                dup2 (ChildRead, Read);
                dup2 (ChildWrite, Write);
                close (ChildRead);
                close (ChildWrite);
                execv ("W",args);
                syserr ("Error in execv");
            
        }
        int i = 0;
        int firstNum = 1;
        // Now we are getting first available descriptor to write to our process
        // the step of our calculation and previous value calculated in i-1-th 
        // step in i-1-th process.
        if (dup (ParentWrite) < 0) 
            syserr ("Error in dup ParentWrite\n");
        if (close (ChildRead) < 0) 
            syserr ("Error in closing ChildRead\n");
        for (i = 1; i <= n; i++)
        {
            if (write (ParentWrite, &i, sizeof (int)) != sizeof (int)) 
                syserr("Error in write pascal\n");
            if (write (ParentWrite, &firstNum, sizeof (int)) != sizeof (int)) 
                syserr("Error in write pascal\n");

        }
        // Closing the descriptor for writing and redirecting standard input to
        // our pipe which is reading from child. Then we are reading all i-th 
        // calculated factors and writing them.
        if (close (ParentWrite) < 0) 
            syserr ("Error in close ParentWrite\n");
        if (dup2 (ParentRead, Read) < 0) 
            syserr ("Error in dup2 ParentRead\n");
        if (close (ParentRead) < 0) 
            syserr ("Error in close ParentRead\n");
        for (i = 1; i <= n; i++)
        {
            if (read (Read, &results[i], sizeof (int)) != sizeof (int)) 
                syserr ("Error in reading from child pascal\n");
        }
        if (close (Read) < 0) 
            syserr ("Error in close Read\n");
        if (i > 0)
            printf ("%d-th line of Pascal triangle is :", n);
        for (i = 1; i <= n; i++)
        {
            printf (" %d", results[i]);
        }
        printf("\n");
        if (close (Write) < 0) 
            syserr ("Error in close Write\n");
        if (wait (0) < 0)
            syserr ("Error in wait pascal\n");
    }
    else {
        printf ("Wrong number of parameters\n");
    }
    return 0;
}
