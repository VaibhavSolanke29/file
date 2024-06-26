#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#define NUM_FILES 3

void *process_file(void *arg) {
    char *filename = (char *)arg;
    int fd;
    char *data;
    struct stat statbuf;

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        pthread_exit(NULL);
    }

    if (fstat(fd, &statbuf) < 0) {
        perror("Error getting file size");
        close(fd);
        pthread_exit(NULL);
    }
    data = mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        pthread_exit(NULL);
    }

    printf("Contents of %s:\n", filename);
    write(STDOUT_FILENO, data, statbuf.st_size);
    printf("\n");

    munmap(data, statbuf.st_size);
    close(fd);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != NUM_FILES + 1) {
        fprintf(stderr, "Usage: %s <file1> <file2> <file3>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pthread_t threads[NUM_FILES];
    int i;

    for (i = 1; i <= NUM_FILES; i++) {
        if (pthread_create(&threads[i - 1], NULL, process_file, argv[i]) != 0) {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < NUM_FILES; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Error joining thread");
            exit(EXIT_FAILURE);
        }
    }

    printf("All files processed.\n");

    return 0;
}
