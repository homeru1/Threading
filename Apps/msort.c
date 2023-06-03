#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <string.h>
#include <time.h> 

int *numbers;
HANDLE event;
HANDLE mutex;
HANDLE wai;
int size=0;

void merge(int start, int end) {
    int amount = end - start;
    int i = start;
    int j = start + amount / 2;
    int runner = 0;
    int deadend = start + amount / 2;
    int *tmp = (int*)calloc(end-start, sizeof(int));
    while (1) {
        if (i == deadend) {
            memmove(&numbers[start], tmp, sizeof(int) * runner);
            free(tmp);
            return;
        }
        if (j == end) {
            memmove(&numbers[start + runner], &numbers[i], sizeof(int) * amount);
            memmove(&numbers[start], tmp, sizeof(int) * runner);
            free(tmp);
            return;
        }
        if (numbers[i] > numbers[j]) {
            tmp[runner] = numbers[j];
            runner++;
            j++;
            amount--;
        }
        else if (numbers[i] < numbers[j]) {
            tmp[runner] = numbers[i];
            runner++;
            i++;
            amount--;
        }
        else {
            tmp[runner] = numbers[i];
            runner++;
            tmp[runner] = numbers[j];
            runner++;
            i++;
            j++;
            amount -= 2;
        }
    }
}
int runner = 0;

DWORD WINAPI thread_fun(LPVOID lpParam) {
    int* data = (int*)lpParam;
    int rounds = 0;
    for (int i = data[1]; i > 0; i /= 2)rounds++;
    int amount = 2;
    for (int j = 0; j < rounds; j++) {
        for (int i = data[2]*amount; i < data[1]; i += (data[0]*amount)) {
            if (i+amount<data[1])merge(i, i+amount);
            else merge(i, data[1]);
        }
        amount *= 2;
        if ((data[2]+1) == data[0]) {
            while (1) {
                WaitForSingleObject(mutex, INFINITE);
                if (runner == data[0] - 1) {
                    ReleaseMutex(mutex);
                    break;
                }
                ReleaseMutex(mutex);
            }
            runner = 0;
            PulseEvent(event);
        }
        else {
            WaitForSingleObject(mutex, INFINITE);
            runner ++;
            ReleaseMutex(mutex);
            WaitForSingleObject(event, INFINITE);
        }
    }
    return 0;
}


int main()
{
    event = CreateEvent(NULL, TRUE, FALSE, NULL);
    mutex = CreateMutex(NULL, FALSE, NULL);
   // SetEvent(event);
    SetEvent(wai);
    FILE* file; 
    fopen_s(&file, "input.txt", "rb");
    if (file == NULL)return 0;
    char *tmp= (char*)calloc(1000,1);
    int data[2];
    int remeber = 0;
    fgets(tmp, 1000, file);
    data[0] = atoi(tmp);
    if (data[0] > 64) {
        remeber = data[0];
        data[0] = 64;
    }
    fgets(tmp, 200, file);
    data[1] = atoi(tmp);
    printf("%d\n", data[1]);
    size = data[1];
    for (; !(!(size == 0) && !(size & (size - 1))); size++) {
    }
    numbers = (int*)calloc(size, sizeof(int));
    for (int i = 0; i < size - data[1]; i++) {
        numbers[i] = 0;
    }
    printf("%d", size);
    char* token;
    int am = size - data[1];
    bool go = 1;
    int numer = 0;
    while (1) {
        while (1) {
            tmp[0] = fgetc(file);
            if (tmp[0] == ' ' || tmp[0] == EOF)break;
            numer *= 10;
            numer += tmp[0] - '0';
        }
        numbers[am] = numer;
        am++;
        numer = 0;
        if (tmp[0] == EOF)break;
        if (am == size)break;
    }
    HANDLE t[1000];
    int max = data[0];
    if (data[0] > size)max = size / 2;
    else if (data[0] > size / 2)max = size / 2;
    clock_t start = clock();
    int ThreadData[1000][3];
    for (int i = 0; i < max; i++) {
        ThreadData[i][0] = max;
        ThreadData[i][1] = size;
        ThreadData[i][2] = i;
        LPVOID ThreadsData = (LPVOID)(&ThreadData[i]);
        t[i] = CreateThread(0, 0, thread_fun, ThreadsData, 0, NULL);
        if (t[i] == NULL)return 0;
    }
    if (t == 0)return 0;
    while (1) {
        DWORD res = WaitForMultipleObjects(max, t, true, 1000);
        if (res != WAIT_TIMEOUT) break;
    }
    clock_t end = clock();
    for (int i = 0; i < max ; i++)CloseHandle(t[i]);
    CloseHandle(event);
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
   // for (int i = more; i < data[1]; i++)printf("%d ", numbers[i]);
    printf("it takes:%g", seconds);
    fclose(file);
    fopen_s(&file, "output.txt", "wb");
    if(remeber==0)fprintf(file, "%d\n%d\n", data[0], data[1]);
    else fprintf(file, "%d\n%d\n", remeber, data[1]);
    for (int i = size-data[1]; i < size; i++)fprintf(file, "%d ", numbers[i]);
    fclose(file);
    fopen_s(&file, "time.txt", "wb");
    fprintf(file, "%g", seconds);
    fclose(file);
}