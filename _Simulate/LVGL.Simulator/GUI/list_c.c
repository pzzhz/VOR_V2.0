#include "list_c.h"
#include <stdio.h>
#include <stdlib.h>
//�����нڵ�Ľṹ

//��ʼ������ĺ���
//�����������ĺ���

void link_Remove(link *p, int cnt, int TO_cnt)
{
    link *from_last, *To_last, *move;
    if (cnt < 1)
        return;
    To_last = link_find(p, TO_cnt);    //��ȡǰһ��
    from_last = link_find(p, cnt - 1); //��ȡǰһ��
    move = from_last->next;
    from_last->next = move->next;
    move->next = To_last->next;
    To_last->next = move;
}

void link_Del(link *p, int cnt)
{
    // link *from_last, *del, *move;
    // if (cnt < 1)
    //     return;
    // To_last = link_find(p, TO_cnt);    //��ȡǰһ��
    // from_last = link_find(p, cnt - 1); //��ȡǰһ��
    link_Remove(p,cnt,10);
    // memset(link_find(p,10),0,sizeof(link));
    // del = from_last->next;
    // from_last = from_last->next->next; // Last->Now->Next


    // move = from_last->next;
    // from_last->next = move->next;
    // move->next = To_last->next;
    // To_last->next = move;
}

// void link_Remove_one(link *p, int cnt, int TO_cnt)
//{
//     link *from_last, *To_last, *move;
//     if (cnt < 1)
//         return;
//     To_last = link_find(p, TO_cnt);    //��ȡǰһ��
//     from_last = link_find(p, cnt - 1); //��ȡǰһ��
//     move = from_last->next;
//     from_last->next = move->next;
//     move->next = To_last->next;
//     To_last->next = move;
// }

link *link_find(link *p, int num)
{
    int i;
    link *temp = p;
    for (i = 0; i < num; i++)
    {
        if (temp->next != NULL)
            temp = temp->next;
    }
    return temp;
}

link *initLink(int num)
{
    int i;
    link *p = NULL;                            //����ͷָ��
    link *temp = (link *)malloc(sizeof(link)); //������Ԫ�ڵ�
    //��Ԫ�ڵ��ȳ�ʼ��
    //    temp->elem = 1;
    temp->next = NULL;
    p = temp; //ͷָ��ָ����Ԫ�ڵ�
    for (i = 1; i < num + 1; i++)
    {
        link *a = (link *)malloc(sizeof(link));
        memset(a, 0, sizeof(link));
        //        a->elem = i;
        a->next = NULL;
        temp->next = a;
        temp = temp->next;
        a->cnt=i;
    }
    return p;
}
// void display(link *p)
//{
//     link *temp = p; //��tempָ������ָ��ͷ���
//     //ֻҪtempָ��ָ��Ľ���next����Null����ִ�������䡣
//     while (temp)
//     {
//         printf("%d ", temp->elem);
//         temp = temp->next;
//     }
//     printf("\n");
// }
