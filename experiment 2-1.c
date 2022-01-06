#include<pthread/pthread.h>
#include<sys/sem.h>
#include<sys/types.h>
#include<stdio.h>

int semID; //semID

/*P 操作*/
void P(int semid,int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = 0;
    semop(semid,&sem,1);
    return;
}

/*V 操作*/
void V(int semid,int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = 1;
    sem.sem_flg = 0;
    semop(semid,&sem,1);
    return;
}

int a=0;

/*线程1，负责计算*/
void *subp1()
{ //负责将1加到100
    int i = 1;
    for(;i<=100;i++)
    {
        P(semID,0);
        printf("Thread 1 is adding! \n");
        a += i;
        V(semID,1);
    }
    return 0;
}

/*负责打印中间结果*/
void *subp2()
{
    int i;
    for(i=1;i<=100;i++){
        //打印出内容
        P(semID,1);
        printf("Thread 2 is printing:");
        printf("%d\n",a);
        V(semID,0);
    }
    return 0;
}

int main()
{
    key_t key = 1;
    /*创建信号灯：2个*/
    semID = semget(key,2,IPC_CREAT | 0666);
    
    //信号灯赋初值 1
    union semun arg1, arg2;
    arg1.val = 1;
    arg2.val = 0;
    
    semctl(semID,0,SETVAL,arg1);
    semctl(semID,1,SETVAL,arg2);
    
    //创建两个线程p1和p2
    pthread_t p1,p2;
    int get1,get2;
    get1 = pthread_create(&p1,NULL,(void*)subp1,NULL);
    get2 = pthread_create(&p2,NULL,(void*)subp2,NULL);
    
    //等特两个线程运行结果
    pthread_join(p1,NULL);
    pthread_join(p2,NULL);
    
    //删除信号灯
    int isDelete;
    isDelete = semctl(semID,1,IPC_RMID,arg1);
}

