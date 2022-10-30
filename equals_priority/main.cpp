#include <iostream>
#include<pthread.h>
#include<semaphore.h>
#include <stdio.h>
using namespace std;

sem_t writer_lock;
pthread_mutex_t reader_mutex, cout_mutex, equality_mutex;
int no_of_readers = 0;
// int shared_var=1;

void *reader(void *read_num)
{
    pthread_mutex_lock(&equality_mutex);
    pthread_mutex_lock(&reader_mutex); // lock critical section to check no. of readers
    no_of_readers++;
    if (no_of_readers == 1)
    {
        sem_wait(&writer_lock); // lock the writer semaphore if u r the first reader
    }
    pthread_mutex_unlock(&equality_mutex);
    pthread_mutex_unlock(&reader_mutex); // unlock critical sec for no. of readers

    // code for reading the file
    // critical section

    FILE *fp;
    fp = fopen("DCS_file_shared.txt", "r");
    char read_data[50];

    if (fp == NULL)
    {
        pthread_mutex_lock(&cout_mutex);
        cout << "Reader Number " << *((int *)read_num) << " failed to open file " << endl;
        pthread_mutex_unlock(&cout_mutex);
    }
    else
    {
        fseek(fp, 0, SEEK_SET);
        if (fgets(read_data, 50, fp))
        {
            pthread_mutex_lock(&cout_mutex);

            cout << "Reader Number " << *((int *)read_num) << " reads: "<<read_data<<endl;

            pthread_mutex_unlock(&cout_mutex);
        }
    }
    fclose(fp);

    // code for reading the file
    // critical section

    /*pthread_mutex_lock(&cout_mutex);
    cout<<"Reader Number "<<*((int*)read_num)<<" reads shared variable as "<<shared_var<<endl;
    pthread_mutex_unlock(&cout_mutex);*/

    pthread_mutex_lock(&reader_mutex); // lock critical section to check no. of readers
    no_of_readers--;
    if (no_of_readers == 0)

    {

        sem_post(&writer_lock);
    } // if the only reader is quitting critical section,unlock writer mutex
    pthread_mutex_unlock(&reader_mutex);//unlock critical sec for readcnt
}

void *writer(void *write_num)
{
    pthread_mutex_lock(&equality_mutex);
    sem_wait(&writer_lock); // lock the critical section to prohibit other readers and writers from accessing the file

    // critical section
    // code for writing into a file
    FILE *fp = fopen("DCS_file_shared.txt", "a");

    if (!fp)
    {
        pthread_mutex_lock(&cout_mutex);
        cout << "Writer Number " << *((int *)write_num) << "failed to open file" << endl;
        pthread_mutex_unlock(&cout_mutex);
    }
    else
    {
        fputs("1", fp);
        pthread_mutex_lock(&cout_mutex);
        cout << "Writer Number " << *((int *)write_num) << " has written" << endl;
        pthread_mutex_unlock(&cout_mutex);
        // Closing the file using fclose()

        fclose(fp);
    }

    // critical section
    // code for writing into a file
    // shared_var+=10;
    /*pthread_mutex_lock(&cout_mutex); cout<<"Writer Number "<<*((int*)write_num)<<endl; pthread_mutex_unlock(&cout_mutex);*/

    pthread_mutex_unlock(&equality_mutex);
    sem_post(&writer_lock); // unlock the critical section to allow other readers and writers to access the file
}

int main()
{
    int readers[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int writers[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int i;
    pthread_t reader_thread[10], writer_thread[10];
    pthread_mutex_init(&reader_mutex, NULL);
    pthread_mutex_init(&cout_mutex, NULL);
    pthread_mutex_init(&equality_mutex, NULL);
    sem_init(&writer_lock, 0, 1); // initialising writer_lock semaphore with a value of "1"

    for (i = 0; i < 5; i++)
    {
        pthread_create(&writer_thread[i], NULL, writer, (void *)(&writers[i]));
    }

    for (i = 0; i < 5; i++)
    {
        pthread_create(&reader_thread[i], NULL, reader, (void *)(&readers[i]));
    }

    for (i = 5; i < 10; i++)
    {
        pthread_create(&reader_thread[i], NULL, reader, (void *)(&readers[i]));
    }
    for (i = 5; i < 10; i++)
    {
        pthread_create(&writer_thread[i], NULL, writer, (void *)(&writers[i]));
    }

    for (i = 0; i < 10; i++)
    {
        pthread_join(reader_thread[i], NULL);
    }
    for (i = 0; i < 10; i++)
    {
        pthread_join(writer_thread[i], NULL);
    }

    pthread_mutex_destroy(&reader_mutex);
    sem_destroy(&writer_lock);
}
