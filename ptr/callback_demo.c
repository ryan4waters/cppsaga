#include <stdio.h>
#include <stdbool.h>

typedef enum {
    A_ID = 0,
    B_ID,
    C_ID,
    D_ID
} Id;

typedef enum {
    NO_FAIL_CODE = 0,
    INVALID_VAL = -1
} ErrorCode;

int g_testTable[4] = {0};

bool getNumberById(Id myId)
{
    return g_testTable[myId] > 0;
}

typedef bool (*getValFun)(Id myId);

typedef struct {
    int number;
    getValFun callback;
    Id id;
} Cfg;

Cfg g_CfgTable[] = {
    {1, getNumberById, A_ID},
    {2, getNumberById, B_ID},
    {3, getNumberById, C_ID},
    {4, getNumberById, D_ID}
};

const size_t g_table_len = sizeof(g_CfgTable) / sizeof(Cfg);

int Get_Number(void)
{
    static int oldRetIndex = INVALID_VAL;
    static int currentIndex = -1;

    for (size_t i = 0; i < g_table_len; ++i) {
        if (g_CfgTable[i].callback(g_CfgTable[i].id)) {
            if (i == currentIndex) { 
                oldRetIndex = i;
                currentIndex = (currentIndex + 1) % g_table_len;
                return g_CfgTable[oldRetIndex].number;
            }
        }
    }

    for (size_t i = 0; i < g_table_len; ++i) {
        size_t searchIndex = (oldRetIndex + 1 + i) % g_table_len;
        if (g_CfgTable[searchIndex].callback(g_CfgTable[searchIndex].id)) {
            oldRetIndex = searchIndex;
            currentIndex = (oldRetIndex + 1) % g_table_len;
            return g_CfgTable[oldRetIndex].number;
        }
    }

    oldRetIndex = INVALID_VAL;
    currentIndex = 0;
    return NO_FAIL_CODE;
}

/*
static int currentIndex = -1;

int Get_Number(void)
{
    for (size_t i = 0; i < g_table_len; ++i) {
        if (g_CfgTable[(currentIndex + 1 + i) % g_table_len].callback(
                g_CfgTable[(currentIndex + 1 + i) % g_table_len].id)) {
            currentIndex = (currentIndex + 1 + i) % g_table_len;
            return g_CfgTable[currentIndex].number;
        }
    }

    currentIndex = -1;
    return NO_FAIL_CODE;
}

int Get_Number(void)
{
    int ret = NO_FAIL_CODE;
    for (size_t i = 0; i < g_table_len; ++i) {
        if (g_CfgTable[(currentIndex + 1 + i) % g_table_len].callback(
                g_CfgTable[(currentIndex + 1 + i) % g_table_len].id)) {
            currentIndex = (currentIndex + 1 + i) % g_table_len, ret = g_CfgTable[currentIndex].number;
            return ret;
        }
    }

    currentIndex = -1;
    return ret;
}
*/

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
    g_testTable[0] = 1;
    g_testTable[2] = 0;
    g_testTable[1] = 1;
    g_testTable[3] = 1;
    PrintFun();

    printf("zero element\n");
    g_testTable[0] = 0;
    g_testTable[1] = 0;
    g_testTable[2] = 0;
    g_testTable[3] = 0;
    PrintFun();

    return 0;
}
