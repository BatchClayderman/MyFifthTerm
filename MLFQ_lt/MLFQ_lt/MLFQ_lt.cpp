#include <iostream>
#include <Windows.h>
#define MAX 100
int timer;//计时器
int n = 10;//进程数量

typedef struct PCB
{
    int pid;
    char state;
    int priority;
    int arrivalTime;
    int neededTime;
    int totalWaitTime;
    struct PCB* next;
}PCB;

PCB* initLink() {
    srand((unsigned)time(NULL));//选取种子文件
    //进程初始化
    PCB* head = (PCB*)malloc(sizeof(PCB));//创建头结点
    PCB* p = head;
    if (p == NULL) {
        return ERROR;
    }
    for (int i = 1; i < n + 1; i++) {
        int a = rand() % 20 + 1;
        int b = rand() % 41 + 10;
        PCB* node = (PCB*)malloc(sizeof(PCB));
        if (node != NULL) {
            node->pid = i;
            node->state = 'w';
            node->arrivalTime = a;
            node->neededTime = b;
            node->totalWaitTime = 0;
            node->next = NULL;
            printf("进程编号 到达时间 所需运行时间\n");
            printf("%d      %d      %d\n", node->pid, node->arrivalTime, node->neededTime);
            p->next = node;
            p = node;
        }
    }
    return head;
}

void Run(PCB* p) {
    p->state = 'r';
    int startTime = timer;
    p->totalWaitTime = timer - p->arrivalTime;
    timer += p->neededTime;
    printf("进程编号 到达时间 开始时间 运行时间 等待时间\n");
    printf("   %d       %d       %d       %d       %d\n", p->pid, p->arrivalTime, startTime, p->neededTime, p->totalWaitTime);
}

void sort(PCB* head) {
    //按到达时间排序
    PCB* p = head->next;
    PCB* q = head;
    for (int i = 0; i < n - 1; i++) {
        p = head->next;
        q = head;
        for (int j = 0; j < n - 1 - i; j++) {
            if (p->arrivalTime > p->next->arrivalTime) {
                PCB* temp = p->next;
                p->next = temp->next;
                temp->next = q->next;//交换节点数据
                q->next = temp;
                q = q->next;
            }
            else {
                q = q->next;
                p = p->next;
            }
        }
    }
    //按所需时间排序
    for (int i = 0; i < n - 1; i++) {
        p = head->next;
        q = head;
        for (int j = 0; j < n - 1 - i; j++) {
            if (p->arrivalTime == p->next->arrivalTime) {
                if (p->neededTime > p->next->neededTime) {
                    PCB* temp = p->next;
                    p->next = temp->next;
                    temp->next = q->next;//交换节点数据
                    q->next = temp;
                    q = q->next;
                }
            }
            else {
                q = q->next;
                p = p->next;
            }
        }
    }
    PCB* a = head->next;
    while (a != NULL) {
        printf("进程编号 到达时间 所需运行时间\n");
        printf("%d      %d      %d\n", a->pid, a->arrivalTime, a->neededTime);
        a = a->next;
    }
}

PCB* order(PCB* perP, PCB* curP, int count) {
    for (int i = 0; i < count - 1; i++) {
        PCB* p = curP;
        PCB* q = perP;
        for (int j = 0; j < count - 1 - i; j++) {
            if (p != NULL)
            {
                if (p->neededTime > p->next->neededTime) {
                    PCB* temp = p->next;
                    p->next = temp->next;
                    temp->next = q->next;//交换节点数据
                    q->next = temp;
                    q = q->next;
                }
                else {
                    q = q->next;
                    p = p->next;
                }
            }
        }
    }
    return(curP);
}

void SJF() {
    PCB* head = initLink();
    sort(head);
    PCB* t = head;//指向p的前一个节点
    PCB* p = head->next;//指向当前节点
    timer = head->next->arrivalTime;
    while (p != NULL) {
        if (p->state == 'w') {//就绪状态
            if (p == head->next || p->arrivalTime == head->next->arrivalTime) {
                Run(p);
            }
            else if (p->arrivalTime < timer || p->arrivalTime == timer) {
                PCB* q = (PCB*)malloc(sizeof(PCB));
                q = p;
                int num = 0;
                while (q != NULL && (q->arrivalTime == timer || q->arrivalTime < timer) && num < n) {//统计到达时间小于前一个进程运行结束时间的进程
                    q = q->next;
                    num++;
                }
                PCB* r = order(t, p, num);
                Run(r);
            }
            else Run(p);
        }
        t = t->next;
        p = p->next;
    }
}

int main()
{
    SJF();
    return 0;
}