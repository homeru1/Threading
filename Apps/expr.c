#include <string>
#include <windows.h>
#include <time.h> 

CRITICAL_SECTION cs;

int answer = 0, N = 0;


int sum(int* number, int amount, int max) {
    int summ = 0;
    for (int i = 0; i <= amount; i++)summ += number[i];
    if (summ == max)return 1;
    else if (summ > max)return 2;
    return 0;
}

void dis(int n, int max, int* numbers, int round) {
    numbers[round] = n;
    if (sum(numbers, round, max) == 2)return;
    else if (sum(numbers, round, max) == 1) {
        EnterCriticalSection(&cs);
        answer++;
        LeaveCriticalSection(&cs);
        return;
    }
    for (int i = 1; i <= n; i++) {
        dis(i, max, numbers, round + 1);
    }
}



DWORD WINAPI thread_fun(LPVOID lpParam) {
    int *data= (int*)lpParam;
    int num = 0;
    EnterCriticalSection(&cs);
    num = N;
    N++;
    LeaveCriticalSection(&cs);
    int number[1000];
    memset(number, 0, 1000);
    for (int i = num; i < data[1]; i += data[0]) {
        dis(i, data[1], number, 0);
    }
    return 0;
}


int main()
{
    FILE* file;
    fopen_s(&file,"input.txt", "rb");
    if (file == NULL)return 0;
    char tmp[200];
    int data[3];
    fgets(tmp, 200, file);
    data[0] = atoi(tmp);
    fgets(tmp, 200, file);
    data[1] = atoi(tmp);
    InitializeCriticalSection(&cs);
    HANDLE t[10000];
    data[2] = data[1] / data[0]; //0 amount of threads 1// number //2 pace
    LPVOID ThreadsData = (LPVOID)(&data);
    clock_t start = clock();
    for (int i = 0; i < data[0]; i++) {
        t[i] = CreateThread(0, 0, thread_fun, ThreadsData, 0, 0);
        if (t[i] == NULL)return 0;
    }
    if (t == 0)return 0;
    while (1) {
        DWORD res = WaitForMultipleObjects(data[0],t,true, 1000);
        if (res != WAIT_TIMEOUT) break;
    }
    clock_t end = clock();
    for (int i = 0; i < data[0]; i++)CloseHandle(t[i]);
    DeleteCriticalSection(&cs);
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Answer is:%d\nit takes:%g", answer, seconds);
    fclose(file);
    fopen_s(&file, "output.txt", "wb");
    fprintf(file, "%d\n%d\n%d", data[0], data[1], answer);
    fclose(file);
    fopen_s(&file, "time.txt", "wb");
    fprintf(file, "%g",seconds);
    fclose(file);
}
