{\rtf1\ansi\ansicpg1252\cocoartf2580
\cocoatextscaling0\cocoaplatform0{\fonttbl\f0\fswiss\fcharset0 Helvetica;}
{\colortbl;\red255\green255\blue255;}
{\*\expandedcolortbl;;}
\paperw11900\paperh16840\margl1440\margr1440\vieww11520\viewh8400\viewkind0
\pard\tx566\tx1133\tx1700\tx2267\tx2834\tx3401\tx3968\tx4535\tx5102\tx5669\tx6236\tx6803\pardirnatural\partightenfactor0

\f0\fs24 \cf0 #include <stdio.h>\
#include <signal.h>\
#include <stdlib.h>\
#include <unistd.h>\
#include <sys/wait.h>\
\
\
// set p1, p2 and pipe_id as global to make sure `my_func` have access to them when called\
int p1;\
int p2;\
int pipefd[2];\
\
// SIGINT signal handler for parent process\
void my_func(int signum) \{\
    signal(SIGINT, SIG_DFL);\
    kill(p1, SIGTERM);\
    kill(p2, SIGTERM);\
    \
\}\
\
void p1_killed() \{\
    printf("Child Process 2 is Killed by Parent!\\n");\
    exit(EXIT_SUCCESS);\
\}\
\
void p2_killed() \{\
    printf("Child Process 1 is Killed by Parent!\\n");\
    exit(EXIT_SUCCESS);\
\}\
\
int main() \{\
    // Ctrl + C will emit SIGINT signal\
    char buffer[BUFSIZ];\
    pipe(pipefd);\
    signal(SIGINT, my_func);\
    p1 = fork();\
    if (p1 == 0) \{\
        // Child 1 process\
\
        signal(SIGINT, SIG_IGN);\
        signal(SIGTERM, p1_killed);\
        int x = 1;\
        while(1) \{\
            sprintf(buffer, "I send you %d times\\n", x);\
            write(pipefd[1], buffer, BUFSIZ);\
            x++;\
            sleep(1);\
        \}\
    \} else \{\
        p2 = fork();\
        if (p2 == 0) \{\
            // Child 2 process\
\
            signal(SIGINT, SIG_IGN);\
            signal(SIGTERM, p2_killed);            \
            while(1) \{\
                read(pipefd[0], buffer, BUFSIZ);\
                printf("%s", buffer);\
            \}\
        \} else \{\
            // parent process\
            waitpid(p1, NULL, 0);\
            waitpid(p2, NULL, 0);\
            close(pipefd[0]);\
            close(pipefd[1]);\
            printf("Parent Process is Killed\\n");\
            return 0;\
        \}\
    \}\
\}\
\
}