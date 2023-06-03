#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <cstdlib>
#include <fstream>


pthread_mutex_t mutex;
int *numbers;


struct ListOfQuest{
    int start;
    int end;
    struct ListOfQuest *next;
};
struct ListOfQuest* head = NULL;

void AddNeqQuest(int start, int end){
    struct ListOfQuest* tmp = (struct ListOfQuest*)calloc(1,sizeof(struct ListOfQuest));
    tmp->start = start;
    tmp->end = end;
    if(head!=NULL)tmp->next = head;
    else tmp->next = NULL;
    head = tmp;
}

void QickSort(int first, int last)
{
   // printf("First:%d\n",first);
    if (first < last)
    {
        int left = first, right = last, middle = numbers[(left + right) / 2];
        do
        {
            while (numbers[left] < middle) left++;
            while (numbers[right] > middle) right--;
            if (left <= right)
            {
                int tmp = numbers[left];
                numbers[left] = numbers[right];
                numbers[right] = tmp;
                left++;
                right--;
            }
        } while (left <= right);
       // printf("After qs:\n");
       // for(int i = 0; i<size;i++)printf("%d ",numbers[i]);
        //printf("\n");
       AddNeqQuest(first, right); // add new quest
       AddNeqQuest(left, last);
       return ;
    }
}
void TakeQuest(){
    if(head==NULL)return;
    int first = head->start;
    int last = head->end;
   // printf("Take quest: start %d end %d \n",first,last);
    struct ListOfQuest* tmp = head;
    head = head->next;
    free(tmp);
    pthread_mutex_unlock(&mutex);
    QickSort(first,last);

}
void* thread_proc(void* param)
{
    while(head!=NULL){
        pthread_mutex_lock(&mutex);
        TakeQuest();
    }
    return 0;
}

int main()
{
    FILE* file = fopen("input.txt", "rb");; 
    if (file == NULL)return 0;
    char *tmp= (char*)calloc(1000,1);
    int data[2];
    fgets(tmp, 1000, file);
    data[0] = atoi(tmp);
    fgets(tmp, 200, file);
    data[1] = atoi(tmp);
    printf("%d\n", data[1]);
    numbers = (int*)calloc(data[1], sizeof(int));
    int am = 0;
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
    }
    //printf("Before qs:\n");
    //for(int i = 0; i<size;i++)printf("%d ",numbers[i]);
    //printf("\n");
    head = (struct ListOfQuest*)calloc(1,sizeof(struct ListOfQuest));
    head->start = 0;
    head->end = data[1]-1;
    head->next = NULL;
    pthread_t threads[1000];
    memset(&threads, 0, sizeof(threads));
    pthread_mutex_init(&mutex, 0);
    // Создание потоков
    clock_t start = clock();
    for(int i = 0; i < data[0]; i++)
    pthread_create(&threads[i], 0, thread_proc, (char*) 0 + i);
    // Ожидание потоков
    for(int i = 0; i < data[0]; i++)
    pthread_join(threads[i], 0);
    clock_t end = clock();
    pthread_mutex_destroy(&mutex);
     double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    //printf("After qs:\n");
    //for(int i = 0; i<size;i++)printf("%d ",numbers[i]);
    //printf("\n");
    printf("it takes:%g", seconds);
    fclose(file);
    file = fopen("output.txt", "wb");
    fprintf(file, "%d\n%d\n", data[0], data[1]);
    for (int i = 0; i < data[1]; i++)fprintf(file, "%d ", numbers[i]);
    fclose(file);
    file = fopen("time.txt", "wb");
    fprintf(file, "%g", seconds);
    fclose(file);
    return 0;
}