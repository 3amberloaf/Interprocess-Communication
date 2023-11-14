#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>

#define shared_memory_object "/AmberSautner_MiniProject"
#define shared_memory_size sizeof(int)

int main() {

    // Declare key variables
    int* shared_memory;
    int value = 0;
    int pid;
    int status;

    // Create a shared memory region 
    int shared_mem_file_descriptor = shm_open(shared_memory_object, O_CREAT | O_RDWR, 0666);
    if (shared_mem_file_descriptor == -1) {
        perror("shm_open");
        exit(1);
    }

    // Set the size of the shared memory region
    if (ftruncate(shared_mem_file_descriptor, shared_memory_size) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // Map the shared memory into the address space of the current process
    shared_memory = (int*)mmap(NULL, shared_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem_file_descriptor, 0);
    if (shared_memory == (int*) -1) {
        perror("mmap");
        exit(1);
    }

    // Create a child process
    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    } 
        // Verify the child process and write a value to the shared memory
        else if (pid == 0) { 
        *shared_memory = 0xDEADBEEF;
        exit(0);
    } 
    else { 
        
        // Suspend parent process and wait for the child to finish
        waitpid(pid, &status, 0);

        // Read the value from the shared memory
        value = *shared_memory;

        printf("The child wrote 0x%X\n", value);

        // Unmap and close the shared memory
        munmap(shared_memory, shared_memory_size);
        close(shared_mem_file_descriptor);

        // Unlink the shared memory region
        shm_unlink(shared_memory_object);
    }

    return 0;
}