#include <sys/types.h>   // for fork, wait
#include <sys/wait.h>    // for wait
#include <unistd.h>      // for fork, pipe, dup, close
#include <stdio.h>       // for NULL, perror
#include <stdlib.h>      // for exit

#include <iostream>      // for cout

using namespace std;

int main( int argc, char** argv ) {
    int fds[2][2];
    int pid;

    if ( argc != 2 ) {
        cerr << "Usage: processes command" << endl;
        exit( -1 );
    }

    // fork a child
    if ( ( pid = fork( ) ) < 0 ) {
        perror( "fork error" );
    }
    else if (pid == 0) {
        // I'm a child

        // Create a pipe for communication between the first child and the second child
        if (pipe(fds[0]) == -1) {
            perror("pipe error");
            exit(-1);
        }

        // Fork the second child
        if ((pid = fork()) < 0) {
            perror("fork error");
            exit(-1);
        } else if (pid == 0) {
            // I'm the second child process

            // Close the unused read end of the pipe
            close(fds[0][0]);

            // Redirect the standard output to the write end of the pipe
            dup2(fds[0][1], STDOUT_FILENO);

            // Close the write end of the pipe
            close(fds[0][1]);

            // Execute "ps -A" command
            execlp("ps", "ps", "-A", NULL);

            // The following code will only execute if execlp fails
            perror("exec error");
            exit(-1);
        } else {
            // I'm the first child process

            // Close the unused write end of the pipe
            close(fds[0][1]);

            // Redirect the standard input to the read end of the pipe
            dup2(fds[0][0], STDIN_FILENO);

            // Close the read end of the pipe
            close(fds[0][0]);

            // Create a pipe for communication between the second child and the third child
            if (pipe(fds[1]) == -1) {
                perror("pipe error");
                exit(-1);
            }

            // Fork the third child
            if ((pid = fork()) < 0) {
                perror("fork error");
                exit(-1);
            } else if (pid == 0) {
                // I'm the third child process

                // Close the unused read end of the pipe
                close(fds[1][0]);

                // Redirect the standard output to the write end of the pipe
                dup2(fds[1][1], STDOUT_FILENO);

                // Close the write end of the pipe
                close(fds[1][1]);

                // Execute "grep -c <search_term>" command
                const char* grep_args[] = {"grep", "-c", argv[1], NULL};
                execvp("grep", const_cast<char**>(grep_args));

                // The following code will only execute if execvp fails
                perror("exec error");
                exit(-1);
            } else {
                // I'm the second child process

                // Close the unused write end of the pipe
                close(fds[1][1]);

                // Redirect the standard input to the read end of the pipe
                dup2(fds[1][0], STDIN_FILENO);

                // Close the read end of the pipe
                close(fds[1][0]);

                // Execute "wc -l" command
                execlp("wc", "wc", "-l", NULL);

                // The following code will only execute if execlp fails
                perror("exec error");
                exit(-1);
            }
        }
    }

    else {
        // I'm a parent
        wait(NULL);
        cout << "commands completed" << endl;
    }

    return 0;
}
