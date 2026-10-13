#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define Total_Num 4

sem_t sem_p1;
sem_t sem_p2;
sem_t sem_correct;

char shared_buffer[256];
int error_count = 0;

const char *p1_dialogue[Total_Num] =
{
    "Hey! Nice to meet you. I really needed some iced coffee today for this hot weather.",
    "The weather is so shinny today. I always crave a large iced coffee.",
    "Did you try the new roast? It makes the best iced coffee!",
    "Totally agree! Nothing beats a fresh iced coffee on a warm afternoon."
};

const char *p2_dialogue[Total_Num] =
{
    "Hey! Same here! I was just thinking about getting a cold iced tea.",
    "Me too! A sweet iced tea is always refreshing.",
    "Not yet, but any brew turned into iced tea sounds amazing right now.",
    "Cheers to that! Let's order our iced tea now."
};

void replace_substring(char *str, const char *old_word, const char *new_word, int *count)
{
    char buffer[512];
    char *pos;

    while ((pos = strstr(str, old_word)) != NULL)
        {
        (*count)++;
        int index = pos - str;

        strncpy(buffer, str, index);
        buffer[index] = '\0';

        strcat(buffer, new_word);

        strcat(buffer, pos + strlen(old_word));

        strcpy(str, buffer);
    }
}

void* person1_thread(void* arg)
{
    for (int i = 0; i < Total_Num; i++)
        {
        sem_wait(&sem_p1);

        printf("[Person 1]: %s\n", p1_dialogue[i]);
        usleep(100000);

        sem_post(&sem_p2);
    }
    return NULL;
}

void* person2_thread(void* arg)
{
    for (int i = 0; i < Total_Num; i++)
        {
        sem_wait(&sem_p2);

        strcpy(shared_buffer, p2_dialogue[i]);

        sem_post(&sem_correct);
    }
    return NULL;
}

void* correction_thread(void* arg)
{
    for (int i = 0; i < Total_Num; i++)
        {
        sem_wait(&sem_correct);

        replace_substring(shared_buffer, "iced tea", "iced coffee", &error_count);

        printf("[Person 2 (Corrected)]: %s\n", shared_buffer);
        usleep(100000);

        sem_post(&sem_p1);
    }
    return NULL;
}

int main()
{
    pthread_t t1, t2, t_correct;

    sem_init(&sem_p1, 0, 1);
    sem_init(&sem_p2, 0, 0);
    sem_init(&sem_correct, 0, 0);

    printf("--- Conversation Started ---\n\n");


    pthread_create(&t1, NULL, person1_thread, NULL);
    pthread_create(&t2, NULL, person2_thread, NULL);
    pthread_create(&t_correct, NULL, correction_thread, NULL);


    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t_correct, NULL);

    printf("\n--- Conversation Ended ---\n");
    printf("Total errors detected and corrected: %d\n", error_count);


    sem_destroy(&sem_p1);
    sem_destroy(&sem_p2);
    sem_destroy(&sem_correct);

    return 0;
}
