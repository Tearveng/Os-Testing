#include<pthread.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/wait.h>

int semID; //semID

/*P操作*/
void P(int semid,int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = 0;
    semop(semid,&sem,1);
    return;
}

/*V操作*/
void V(int semid, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = 1;
    sem.sem_flg = 0;
    semop(semid,&sem,1);
    return;
}

int a=0;
/*用于设置共亭内存*/
struct sharedItem
{
    int sumNum;
};

int main()
{
    key_t key = 1;

/*创建信号灯：2个*/
semID = semget(key,2,IPC_CREAT|0666);

//信号灯赋初值 1
semctl(semID,0,SETVAL,1);
semctl(semID,1,SETVAL,0);

/*创建两个字进程*/
int pid1,pid2;

key_t sKey = 0x2233;

int shID;
shID = shmget(sKey,sizeof(struct sharedItem),IPC_CREAT|0666);

if((pid1 = fork()) == 0)
{
    /*创建共亭内存*/
    struct sharedItem *sum = (struct sharedItem*)shmat(shID,NULL,0);
    sum->sumNum = 0;
    int i=1;
    
    for(;i<=100;i++)
    {
        P(semID,0);
        printf("Process 1 is adding now\n");
        sum->sumNum += i;
        V(semID,1);
    }
    
    shmdt(sum);
}
else
if((pid2 = fork()) == 0)
{
    struct sharedItem *sum = (struct sharedItem*)shmat(shID,NULL,0);
    int i = 1;
    
    for(;i<=100;i++)
    {
        P(semID,1);
        printf("Process 2 is printing: %d\n",sum->sumNum); //打印
        V(semID,0);
    }
    shmdt(sum);
}

waitpid(pid1,NULL,0);
waitpid(pid2,NULL,0);
//删除信号灯
shmctl(shID,IPC_RMID,NULL);
int isDelete;
isDelete = semctl(semID,1,IPC_RMID,0);
}

