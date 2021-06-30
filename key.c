#include <stdio.h>
#include <math.h>//只需要在main中使用幂pow()，不用math库可以考虑手动实现

#define     N       8//在这里设置总层数
#define     column  5//column=2log2(N)-1
#define     max_layer   3//max_layer=log2(N)


#define     up          1
#define     down        2
#define     set         1
#define     unset       0

#define     parallar    0
#define     permute     1
#define     any         8
#define     error       9

int INPUT[N][N + 2] = { 0 };
int OUTPUT[N][N + 2] = { 0 };
int INPUT_N[N][N + 2] = { 0 };
int SWICH[N / 2][column] = { 0 };
int SET[N][N] = { 0 };

int UD(int M);
int findcouple(int i);
void get_map();
int toused(int(*port)[N + 2], int(*elseport)[N + 2], int m);
int routing(int(*port)[N + 2], int(*elseport)[N + 2], int M, int i, int flag);//i is port number,flag==1 means from left to right
int next_layer(int layer);
void EN_routing();
void get_next(int total);
void N_flush();
void UPDATE();



int main()
{
    int i, j;
    extern int SWICH[N / 2][column];
    int layer = 0;
    get_map();
    while (1)
    {
        layer++;
        EN_routing();
        next_layer(layer);
        N_flush();
        get_next(N / pow(2, (layer-1)));
        UPDATE();
        if (layer == max_layer)
        {
            for (i = 0; i < N / 2; i++)//打印
            {
                for (j = 0; j < column; j++)
                {
                    printf("%d\x20", SWICH[i][j]);
                }
                printf("\n");
            }
            break;
        }
    }
}

void EN_routing()
{
    extern int INPUT[N][N + 2];
    extern int OUTPUT[N][N + 2];
    int M;
    int k;
    for (k = 0; k < N; k++)
    {
        if ((toused(INPUT, OUTPUT, k)))//从0开始寻找需要但是还没有被分配的输入端口,k为行号
        {
            if (INPUT[findcouple(k)][N] == up)
            {
                M = down;
            }
            else
            {
                M = up;
            }
            routing(INPUT, OUTPUT, M, k, 0);
        }
    }
}


int toused(int(*port)[N + 2], int(*elseport)[N + 2], int m)
{
    int i;
    for (i = 0; i < N; i++)
    {
        if ((port[m][i] == set ) & (elseport[i][N + 1] == unset ))
            return 1;
    }
    return 0;
}

int routing(int(*port)[N + 2], int(*elseport)[N + 2], int M, int i, int flag)//i is port number,flag==1 means from left to right
{
    int j, k, couple;

    if (port[i][N + 1] == unset )
    {
        port[i][N] = M;
        port[i][N + 1] = set ;
    }

    for (k = 0; k < N; k++)//设置
    {
        if (port[i][k] == set )
        {
            if (elseport[k][N + 1] == unset )
            {
                elseport[k][N] = M;
                elseport[k][N + 1] = set ;
            }

        }
    }

    for (j = 0; j < N; j++)
    {
        if (port[i][j] == 1)
        {
            couple = findcouple(j);
            if (elseport[couple][N + 1] != set )
            {
                for (k = 0; k < N; k++)
                {
                    if ((elseport[couple][k] == set ) & (elseport[couple][N + 1] == unset ))//如果同switch的另一个输出有输入，且还未被安排
                    {

                        //M = UD(M);//指向互补中间级
                        flag = !flag;
                        flag = routing(elseport, port, UD(M), couple, flag);//递归
                        if (flag == 0)
                        {
                            return flag;
                        }
                    }
                }
            }
        }
    }
    return flag;
}

int next_layer(int layer)
{
    extern int SWICH[N / 2][column];
    extern int SET[N][N];
    extern int INPUT[N][N + 2];
    extern int OUTPUT[N][N + 2];
    int k = 0;
    printf("INPUT MIDDLE SEL \n");
    for (k = 0; k < N; k++)
    {
        SET[k][layer - 1] = INPUT[k][N];

        printf("%d\n", SET[k][layer - 1]);
    }
    printf("OUTPUT MIDDLE SEL \n");
    for (k = 0; k < N; k++)
    {
        SET[k][N - layer] = OUTPUT[k][N];
        printf("%d\n", SET[k][N - layer]);
    }

    for (k = 0; k < N / 2; k++)
    {
        if ((SET[2 * k][layer - 1] == unset ) & (SET[2 * k + 1][layer - 1] == unset ))
            SWICH[k][layer - 1] = any ;
        if ((SET[2 * k][layer - 1] == unset ) & (SET[2 * k + 1][layer - 1] == up ))
            SWICH[k][layer - 1] = permute ;
        if ((SET[2 * k][layer - 1] == unset ) & (SET[2 * k + 1][layer - 1] == down ))
            SWICH[k][layer - 1] = parallar ;
        if ((SET[2 * k][layer - 1] == up ) & (SET[2 * k + 1][layer - 1] == unset ))
            SWICH[k][layer - 1] = parallar ;
        if ((SET[2 * k][layer - 1] == up ) & (SET[2 * k + 1][layer - 1] == up ))
            SWICH[k][layer - 1] = error ;
        if ((SET[2 * k][layer - 1] == up ) & (SET[2 * k + 1][layer - 1] == down ))
            SWICH[k][layer - 1] = parallar;
        if ((SET[2 * k][layer - 1] == down ) & (SET[2 * k + 1][layer - 1] == unset ))
            SWICH[k][layer - 1] = permute;
        if ((SET[2 * k][layer - 1] == down ) & (SET[2 * k + 1][layer - 1] == up ))
            SWICH[k][layer - 1] = permute;
        if ((SET[2 * k][layer - 1] == down ) & (SET[2 * k + 1][layer - 1] == down ))
            SWICH[k][layer - 1] = error ;
    }

    for (k = 0; k < N / 2; k++)
    {
        if ((SET[2 * k][N - layer] == unset ) & (SET[2 * k + 1][N - layer] == unset ))
            SWICH[k][column - layer] = any ;
        if ((SET[2 * k][N - layer] == unset ) & (SET[2 * k + 1][N - layer] == up ))
            SWICH[k][column - layer] = permute ;
        if ((SET[2 * k][N - layer] == unset ) & (SET[2 * k + 1][N - layer] == down))
            SWICH[k][column - layer] = parallar ;
        if ((SET[2 * k][N - layer] == up ) & (SET[2 * k + 1][N - layer] == unset ))
            SWICH[k][column - layer] = parallar ;
        if ((SET[2 * k][N - layer] == up) & (SET[2 * k + 1][N - layer] == up ))
            SWICH[k][column - layer] = error ;
        if ((SET[2 * k][N - layer] == up) & (SET[2 * k + 1][N - layer] == down))
            SWICH[k][column - layer] = parallar ;
        if ((SET[2 * k][N - layer] == down ) & (SET[2 * k + 1][N - layer] == unset ))
            SWICH[k][column - layer] = permute ;
        if ((SET[2 * k][N - layer] == down ) & (SET[2 * k + 1][N - layer] == up ))
            SWICH[k][column - layer] = permute ;
        if ((SET[2 * k][N - layer] == down ) & (SET[2 * k + 1][N - layer] == down))
            SWICH[k][column - layer] = error ;
    }

    printf("INPUT SWITCH SET\n");
    for (k = 0; k < N / 2; k++)
    {
        printf("%d\n", SWICH[k][layer - 1]);
    }

    printf("OUTPUT SWITCH SET\n");
    for (k = 0; k < N / 2; k++)
    {
        printf("%d\n", SWICH[k][column - layer]);
    }


    printf("done \n");
    return 0;
}


void get_next(int total)
{
    extern int INPUT[N][N + 2];
    extern int OUTPUT[N][N + 2];
    extern int INPUT_N[N][N + 2];
    int i;
    int j;
    int si;
    int sj;
    int ii;
    int jj;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            si = i - total * ((int)(i / total));
            sj = j - total * ((int)(j / total));//将ij归一化为ks
            if (INPUT[i][j]== set )//提取有值ij
            {
                    if (INPUT[i][N] == up)
                    {
                        ii = total * ((int)(i / total)) + si / 2;
                    }
                    else
                    {
                        ii = total * ((int)(i / total)) + ((int)(si / 2)) + total / 2;
                    }
                    if (OUTPUT[j][N] == up)
                    {
                        jj = total * ((int)(j / total)) + sj / 2;
                    }
                    else 
                    {
                        jj = total * ((int)(j / total)) + ((int)(sj / 2)) + total / 2;
                    }
                INPUT_N[ii][jj] = set;
            }
        }
    }
}


void N_flush()
{
    extern int INPUT_N[N][N + 2];
    int i;
    int j;
    for (i = 0; i < N; i++)//清零
    {
        for (j = 0; j < N + 2; j++)
        {
            INPUT_N[i][j] = 0;
        }
    }
}

void UPDATE()
{
    extern int INPUT[N][N + 2];
    extern int OUTPUT[N][N + 2];
    extern int INPUT_N[N][N + 2];
    int i;
    int j;
    for (i = 0; i < N; i++)//清零
    {
        for (j = 0; j < N + 2; j++)
        {
            INPUT[i][j] = 0;
            OUTPUT[i][j] = 0;
        }
    }

    for (i = 0; i < N; i++)//更新
    {
        for (j = 0; j < N; j++)
        {
            INPUT[i][j] = INPUT_N[i][j];
        }
    }

    for (i = 0; i < N; i++)//转置
    {
        for (j = 0; j < N; j++)
        {
            OUTPUT[j][i] = INPUT[i][j];
        }
    }
}

int UD(int M)
{
    if (M == 1)
    {
        M = 2;
        return M;
    }
    if (M == 2)
    {
        M = 1;
        return M;
    }

    return M;

}

int findcouple(int i)
{
    if (i % 2)//0,2,4,6
    {
        return i - 1;
    }
    else
        return i + 1;
}

void get_map()
{
    int key;
    int i = 0;
    int j = 0;
    printf("Setup mapping \n");
    while (1)
    {

        printf("Enter input port %d's outport once a time, input number %d to end. \n", i, N);
        scanf_s("%d", &key);
        if (key < N)
        {
            INPUT[i][key] = 1;
            i = i + 1;
        }
        else if (key >= N)
        {
            printf("Error: out of range \n");
            break;
        }
        if (i == N)
        {
            printf("SET! \n");
            break;
        }
    }
    printf("Input map: \n");
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            printf("%d\x20", INPUT[i][j]);
            OUTPUT[j][i] = INPUT[i][j];
        }
        printf("\n");
    }
    printf("Output map: \n");
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            printf("%d\x20", OUTPUT[i][j]);
        }
        printf("\n");
    }
    return;
}