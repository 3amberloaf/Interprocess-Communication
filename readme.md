# Table of Contents
- [Introduction](#introduction)
- [Execution](#execution)
- [Foundation](#foundation)
- [Function](#function)
   - [Part I](#part-i)
   - [Part II](#part-ii)
   - [Part III](#part-iii)
   - [Part IV](#part-iv)
   - [Part V](#part-v)
   - [Part VI](#part-vi)
   - [Part VII](#part-vii)
- [Project Requirements](#project-requirements)
- [Conclusion](#conclusion)
   - [Key Takeaways](#key-takeways)

# Introduction

This C program demonstrates how to establish interprocess communication between a parent and child process through a shared memory region. This design allows for seamless communication and synchronization between the two processes, along with efficient error-handling mechanisms and cleanup practices to ensure proper performance.

# Execution

Navigate to the source file directory by using the `cd` command. 

```c
   cd /Users/ambersautner/Programming/AmberSautner_Project
   ```

Run the following code to compile the program.

   ```c
   gcc -o AmberSautner_SharedMemory AmberSautner_MiniProject.c
   ```

After compiling the program, execute the code through the terminal.

   ```c
   ./AmberSautner_SharedMemory
   ```

The expected outcome should be:

```c
The child wrote 0xDEADBEEF
   ```

# Foundation

**Define the necessary header files that will be used in the program**

   - `<stdio.h>`: Standard I/O functions
      - `printf()`: Display messages to the console
   - `<stdlib.h>`: Standard library functions
      - `malloc`: Memory allocation
      - `free`: Memory deallocation
      - `exit`: Program termination
   - `<unistd.h>`: POSIX operating system services
      - `fork()`: Create child processes
      - `waitpid()`: Wait for child processes to finish
   - `<sys/types.h>`: Data types used in system calls
      - `pid_t`: Process_ID used
   - `<sys/mman.h>`: Memory management functions
      - `mmap`: Maps memory
      - `PROT_READ`: Constant to specify memory protection
      - `PROT_WRITE`: Constant to specify memory protection
   - `<sys/wait.h>`: Functions for process synchronization
      - `waitpid`: Wait for child process to finish
   - `<fcntl.h>`: File control options
      - `shm_open`: Helps manage shared memory

**Define the Constants**

   - `shared_memory_object` `"/AmberSautner_SharedMemory"`:
     - `shared_memory_object` is the name of the shared memory region that the program will create and access throughout
     - `"/AmberSautner_SharedMemory"` is a unique identifier for the shared memory region
     - When the program is compiled, all instances of `shared_memory_object` is replaced with `"/AmberSautner_SharedMemory"`

   - `shared_memory_size` `sizeof(int)`:
     - The size of the shared memory region is set to the size of an integer (`sizeof(int)`)
     - In a C environment, an integer is typically 4 bytes
     - This ensures that the correct amount of memory is always allocated to the shared memory region and prevents memory shortage

# Function 

The function will be broken down into eight key parts:
1. Declare key variables
2. Create a shared memory region
3. Set the size of the shared memory region
4. Map the shared memory into the address space of the current process
5. Create and verify the child process
6. Suspend the parent process and read the value from shared memory
7. Unmap and unlink from shared memory

There are various functions throughout the code to check for errors, as well as proper cleanup at the end. 

## Part I

**Declare key variables**

```c
int* shared_memory;
int value = 0;
int pid;
int status;
```

   - `int* shared memory`: * is used to declare a pointer variable in C, while `shared_memory` is used to access and manipulate the data in the shared memory region
   - `value`: An integer variable to store the value read from shared memory
   - `pid`: Process ID of the child process
   - `status`: A variable to store the exit status of the child process

## Part II

**Create the shared memory region**

```c
int shared_mem_file_descriptor = shm_open(shared_memory_object, O_CREAT | O_RDWR, 0666);
```

- `shared_mem_file_descriptor`: An integer variable that will store a file descriptor for the shared memory region
   - File descriptors are used to represent files, sockets, pipes, and other input/output resources
   - In this scenario, it is used to help set the size of the shared memory and map it into the address space of the process
- `Shm_open`: A POSIX function used to create a shared memory object
   - If it does not exit, it will create one
   - If it does exit, it will open it for both reading and writing
   - The function will return a file descriptor (`shared_mem_file_descriptor`) that can be used to access the shared memory later
- `shared_memory_object`: A constant for the shared memory region
- `O_CREAT`: A flag used to control the behavior of the shared memory
   - It will create the shared memory region if it doesn't exist by calling `shm_open`
   - If it does exist, this flag will have no effect
   - Used in conjunction with `O_RDWR`
- `O_RDWR`: A flag used to control the behavior of the shared memory
   - Indicates that the shared memory region should be opened for both reading and writing access
   - The combination of `O_CREAT` | `O_RDWR` means that the shared memory object will be created if it doesn't exist, and if it does exist it will be opened with read and write access
- `0666`: A permission mode value for the shared memory region
   - This gives read and write access for the owner and group, as well as read-only access for others


**Project Requirement 1:** This line of code fulfills project requirement 1 by creating a shared memory object (`shared_memory_object`) with read and write permissions for all users, as well as returning a file descriptor (`shared_mem_file_descriptor`) for further operations.

```c
if (shared_mem_file_descriptor == -1) {
    perror("shm_open");
    exit(1);
```

- `if (shared_mem_file_descriptor == -1)`
   - Checks whether the `shm_open` function call was successful
   - If the function failed, it would return -1
      - This means there was an error
- `perror("shm_open")`
   - If the function fails, the `perror` function is used to print a description for the last error that occurred to the console
- `exit(1);`
   - The `exit` function is called to terminate the program
   - This tells us that an error occurred

**Project Requirement 7:** This block of code fulfills project requirement 7 because it define the `perror` function to print an error message and exit the program in case of an error.
 
## Part III

**Set the size of the shared memory region**

```c
if (ftruncate(shared_mem_file_descriptor, shared_memory_size) == -1) {
        perror("ftruncate");
        exit(1);
    }    
```

- `ftruncate`: A function that sets the size of a file (i.e. `shared_memory_object`)
   - The if condition checks if there was any error when setting the size of `shared_memory_object`
   - `ftruncate` returns 0 if successeful
   - `ftruncate` returns -1 if it encountered an error
      - If an error occurred, the remaining code will execute to print an error message and terminate the program
- The value of the shared memory object will be kept inside `shared_memory_size`

## Part IV
**Map the shared memory into the address space of the current process**

```c
shared_memory = (int*)mmap(NULL, shared_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem_file_descriptor, 0)

 if (shared_memory == (int*)-1) {
        perror("mmap");
        exit(1);
    }
```

- `shared_memory`: A pointer that will be assigned the starting address of the mapped region
- `mmap`: Used to map files or devices into memory
   - `NULL`: Lets the OS choose the most suitable starting address for the mapping
   - `shared_memory_size`: The size (in bytes) of the mapping
   - `PROT_READ | PROT_WRITE`: Allows both reading and writing to the mapped memory
   - `MAP_SHARED`: Confirms that updates to the mapping are visible to other processes that have mapped the same region
      - Vital in establishing a shared memory region that can be used for interprocess communication
   - `shared_mem_file_descriptor`: The file descriptor of the shared memory object obtained from shm_open
   - `0`: The offset within the file at which the mapping starts. It starts at the beginning of the file.
- If the `mmap` function failed, the remaining code will execute to print an error message and terminate the program

**Project Requirement 2/3:** This line of code fulfills project requirements 2 and 3 by allocating memory into the shared memory region (`shared_memory`) and mapping memory into the proccess's address space.

## Part V

**Create a child process**

```c
   pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);}
```
- `fork`: A function used to create child processes from a parent process through duplication
- `pid`: The process ID to identify a process
   - The return value of `fork` is a `pid` for the parent and 0 for the child
- The condition statement states that if the return value is -1, an error occurred
- `perror`: A function will be called with "fork" as an argument, so the error message will similar to `fork: [error description]`
- Finally, the program is terminated

**Verify the child process and write a value to the shared memory**

```c
   else if (pid == 0) { 
        *shared_memory = 0xDEADBEEF;
        exit(0);
    }
```
- The next condition checks if the process is actually a child process
   - A successfuly fork function will return a value of 0, it's the child process
   - If it is, the following code is executed to write the value `0xDEADBEEF` into the `shared_memory` variable 
- This demonstrates the shared memory is communicating an integer value (`0xDEADBEEF`) from the child process to the parent process

**Project Requirement 4:** This block of code fulfills project requirement 4 because after the `fork` function is called successfully, the child process writes the value `0xDEADBEEF` to the shared memory.

## Part VI

**Suspend the parent process and wait for the child to finish**

```c
waitpid(pid, &status, 0);
```

- `waitpid`: A function is called to suspend the execution of the calling process until the child process is finished executing
   - It takes three arguements (`pid`, `&status`, `0`)
      - `pid`: The process ID of the child process
      - `&status`: An integer variable that will keep track of the exit status of the child process
      - `0`: Tells the `waitpid` function that the parent process will wait for any child process whose process ID matches the one specified in the pid parameter
- The exit status of the terminated child process is stored in the status variable and can describe how the child process terminated (e.g., normal exit, termination by a signal, etc.)

**Project Requirement 5:** This block of code fulfills project requirement 5 because it demonstrates the parent process waiting for the child process to finish executing before reading the value
from the shared memory region.

**Read the value from the shared memory**

```c
   value = *shared_memory;

        printf("The child wrote 0x%X\n", value);
```

- `Shared_memory`: A a pointer that was set to point to the beginning of the shared memory region using `mmap`
   - We must retrieving the value stored in the shared memory region by dereferencing the pointer through `*`
   - Now we can retrieve the value that the pointer is pointing to and assign it to `value`
- Finally, a message is printed to the console indicating that the parent process has read a value from the shared memory along with the value

**Project Requirement 5:** This block of code fulfills project requirement 5 because it demonstrates the parent process waiting for the child process to finish executing before reading the value from the shared memory region.

## Part VII

**Unmap and close the shared memory**

```c
   munmap(shared_memory, shared_memory_size);
        close(shared_mem_file_descriptor);
```
- `munmap`: A system call function used to used to release a range of memory that was previously mapped
   - Takes two arguements to release
      - `shared_memory`: The starting address of the mapped memory region.
      - `shared_memory_size`: The size of the mapped memory region
- After this call, the shared memory is no longer accessible from within the process
- Close is a system call used to close a file descriptor
   - `shared_mem_file_descriptor`: The file descriptor associated with the shared memory object, obtained from `shm_open`

**Unlink the shared memory region**

```c
   shm_unlink(shared_memory_object);}
```

- `shm_unlink`: A function is used to remove the shared memory object from the system, allowing it to be deallocated
   - `shared_memory_object`: The shared memory region used

**Project Requirement 6:** This block of code fulfills project requirement 6 because the shared memory is being unmapped, closed, and unlinked. 


**Finish**

```c
   return 0; }
```

- Indicates that the program is exiting with a status code of 0 and terminated successfully.

# Project Requirements

In this section, each project requirement is matched with the corresponding code that satisfies them.


1. In the main function, a shared memory region is set up by creating a shared memory
object.

  ```c
   int shared_mem_file_descriptor = shm_open(shared_memory_object, O_CREAT | O_RDWR, 0666);
   ```

2. Memory is allocated within the shared region.

  ```c
   shared_memory = (int*)mmap(NULL, shared_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem_file_descriptor, 0);
  ```
3. The shared memory region is mapped into the process's address space.

 ```c
shared_memory = (int*)mmap(NULL, shared_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem_file_descriptor, 0);
 ```

4. After forking a child process, the child process writes a specific value 0xDEADBEEF to the
shared memory.

 ```c
   else if (pid == 0) {
    *shared_memory = 0xDEADBEEF;
    exit(0);
}
 ```

5. The parent process waits for the child process to complete and then reads the value
from the shared memory region.

 ```c
   else {
    // Suspend parent process and wait for the child to finish
    waitpid(pid, &status, 0);

    // Read the value from the shared memory
    value = *shared_memory;

    printf("The child wrote 0x%X\n", value);
}
 ```

6. Finally, the shared memory is cleaned up by un-mapping it and removing the shared
memory object.

 ```c
    // Unmap and close the shared memory
    munmap(shared_memory, shared_memory_size);
    close(shared_mem_file_descriptor);

    // Unlink the shared memory segment
    shm_unlink(shared_memory_object);
 ```

7. Also, define a function that prints the error message and exits the program in case of an
error.

 ```c
   if (shared_mem_file_descriptor == -1) {
      perror("shm_open");
      exit(1);
   }
 ```

# Conclusion 
This project explored the concept of interprocess communication (IPC) using shared memory in a parent-child process scenario. Shared memory allows for two processes to exchange data efficiently by mapping a common region of memory into their respective address spaces. This code example demonstrates how to create, access, and manage shared memory in a simple C program.

## Key Takeways

**Shared Memory:** Shared memory allows processes to share data by providing a shared region of memory.

**System Calls:** The code utilized various system calls, including `shm_open`, `ftruncate`, and `mmap`, to create and manage shared memory.

**Parent-Child Communication:** The code showcases a parent process and a child process communicating by sharing an integer value through shared memory.

**Process Synchronization:** We used the `fork` and `waitpid` system calls to create child processes and wait for them to complete their tasks.

**Cleanup and Unlinking:** Proper cleanup of shared memory is an important aspect of clean code. In this program, we unmapped, closed file descriptors, and unlinked the shared memory region to properly end the program.