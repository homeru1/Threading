#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include<malloc.h>
#include<windows.h>
#include <time.h> 

int answer = 0;
int **boards;
CRITICAL_SECTION cs;

void put(int** board, int n, int x, int y) {
    for (int i = 0; i < n; i++) {
        board[i][x] = -1;
        board[y][i] = -1;
    }
    for (int i = x, j = y; i < n && j < n; i++, j++) {
        board[j][i] = -1;
    }
    for (int i = x, j = y; i >= 0 && j < n; i--, j++) {
        board[j][i] = -1;
    }
    for (int i = x, j = y; i < n && j >= 0; i++, j--) {
        board[j][i] = -1;
    }
    for (int i = x, j = y; i >= 0 && j >= 0; i--, j--) {
        board[j][i] = -1;
    }
    board[y][x] = -2;
}

void chess(int** board, int n, int l, int count, int x, int y) {
   // PrintBoard(board, n);
    int** Newboard = (int**)calloc(n, sizeof(int*));
    for (int i = 0; i < n; i++) {
        Newboard[i] = (int*)calloc(n, sizeof(int));
        memcpy(Newboard[i], board[i], sizeof(int) * n);
    }
        put(Newboard, n, x, y);
        count++;
    if (l == count) {
        EnterCriticalSection(&cs);
        answer++;
        LeaveCriticalSection(&cs);
        return;
    }
    for (int j = y; j < n; j++) {
        for (int i = x; i < n; i++)
        {
            if (Newboard[j][i] == 0) {
                chess(Newboard, n, l, count, i, j);
            }
        }
        x = 0;
    }
}

void ClearBoard(int** board, int n) {
    for (int j = n - 1; j >= 0; j--) {
        for (int i = 0; i < n; i++) {
            board[j][i] = 0;
        }
    }
}

DWORD WINAPI thread_fun(LPVOID lpParam) {
    int* data = (int*)lpParam;
    int** board = (int**)calloc(data[1], sizeof(int*));
    for (int i = 0; i < data[1]; i++) {
        board[i] = (int*)calloc(data[1], sizeof(int));
        EnterCriticalSection(&cs);
        memcpy(board[i], boards[i], sizeof(int) * data[1]);
        LeaveCriticalSection(&cs);
    }
    int x = 0, y = 0;
    for (int i = data[2]; i < data[1] * data[1]; i += data[0]) {
        if (data[2] == 0)printf("x:%d and y:%d\n", i % data[1], i / data[1]);
        x = i % data[1];
        y = i / data[1];
        if(board[y][x]==0)chess(board, data[1], data[3], 0, x, y);
    }
    return 1;
}
int main() {

    int i, j, n, l, k, amount;
    FILE* file = fopen("input.txt", "r");
    fscanf(file,"%d", &amount);
    fscanf(file, "%d%d%d", &n, &l, &k);
    boards = (int**)calloc(n, sizeof(int*));
    for (int i = 0; i < n; i++)boards[i] = (int*)calloc(n, sizeof(int));
    while (k-- > 0) {
        fscanf(file, "%d %d", &i, &j);
        put(boards,n,i,j);
    }
    int ThreadData[1000][4];
    HANDLE t[1000];
    InitializeCriticalSection(&cs);
    clock_t start = clock();
    for (int i = 0; i < amount; i++) {
        ThreadData[i][0] = amount;
        ThreadData[i][1] = n;
        ThreadData[i][2] = i;
        ThreadData[i][3] = l;
        LPVOID ThreadsData = (LPVOID)(&ThreadData[i]);
        t[i] = CreateThread(0, 0, thread_fun, ThreadsData, 0, NULL);
        if (t[i] == NULL)return 0;
    }
    if (t == 0)return 0;
    while (1) {
        DWORD res = WaitForMultipleObjects(amount, t, true, 1000);
        if (res != WAIT_TIMEOUT) break;
    }
    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Answer:%d", answer);
    fclose(file);
    fopen_s(&file, "output.txt", "wb");
    fprintf(file, "%d",answer);
    fclose(file);
    fopen_s(&file, "time.txt", "wb");
    fprintf(file, "%g", seconds);
    fclose(file);

    return 0;
}
