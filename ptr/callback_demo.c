#include <stdio.h>
#include <stdbool.h>

#define NO_FAIL_CODE 0
#define INVALID_VAL 0xFF

typedef enum {
    A_ID = 0,
    B_ID,
    C_ID
} Id;

int g_testTable[3] = {0};

bool GetNumberById(Id myId)
{
    int idIndex = myId;
    return (g_testTable[idIndex] > 0U) ? true : false;
}

typedef bool (*getValFun)(Id myId);

typedef struct {
    int number;
    getValFun mycallback;
    Id myId;
} Cfg;

Cfg g_CfgTable[] = {
    {1, GetNumberById, A_ID},
    {10, GetNumberById, B_ID},
    {100, GetNumberById, C_ID}
};

const int g_table_len = sizeof(g_CfgTable) / sizeof(Cfg);

int Get_Number(void)
{
    int ret = NO_FAIL_CODE;
    static int oldRetIndex = INVALID_VAL;
    int i;
    bool isNumberExist;
    Id myId;

    do {
        isNumberExist = false;
        for (i = 0U; i < g_table_len; ++i) {
            myId = g_CfgTable[i].myId;
            if (g_CfgTable[i].mycallback(myId)) {
                isNumberExist = true;
                if ((oldRetIndex == INVALID_VAL) || (i > oldRetIndex)) {
                    oldRetIndex = i;
                    return g_CfgTable[oldRetIndex].number;
                }
            }
        }
        if (i >= (g_table_len - 1U)) {
            oldRetIndex = INVALID_VAL;
        }
    } while (isNumberExist);

    return ret;
}

void PrintFun(void)
{
    for (int i = 0; i < 20; ++i) {
        printf("%d\n", Get_Number());
    }
}

int main()
{
    printf("one element\n");
    g_testTable[2] = 1;
    PrintFun();

    printf("two elements\n");
    g_testTable[2] = 0;
    g_testTable[0] = 1;
    g_testTable[1] = 1;
    PrintFun();

    printf("zero element\n");
    g_testTable[2] = 0;
    g_testTable[0] = 0;
    g_testTable[1] = 0;
    PrintFun();

    return 0;
}
