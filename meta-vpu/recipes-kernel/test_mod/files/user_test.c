#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#define EDU_DEVICE             "/dev/edu"

#define DMA_START              0x01
#define DMA_RAM2EDU            0x0
#define DMA_EDU2RAM            0x02
#define DMA_IRQ                0x04
#define DMA_IRQ_VALUE          0x100

#define EDU_FACT_CALC          0x08
#define EDU_DMA_SRC_ADDRESS    0x80
#define EDU_DMA_DST_ADDRESS    0x88
#define EDU_DMA_COUNT          0x90
#define EDU_DMA_CMD            0x98

#define EDU_DMA_GET            0x1234

#define EDU_DMA_RAM2EDU        0x00
#define EDU_DMA_EDU2RAM        0x02

#define EDU_BUFFER_ADDRESS     0x40000

#define DMA_BUFFER_SIZE        1024 * 1024
#define EDU_MEMORY_REGION      8 * 1024 * 1024

#define EDU_MAGIC              'e'
#define EDU_IOCTL_GET_DMA_ADDR _IOR(EDU_MAGIC, 1, uint64_t)
#define EDU_IOCTL_SET_DMA_DIR  _IOW(EDU_MAGIC, 1, uint32_t)

#define SET_DMA_USER(value64, pos)                                             \
    do {                                                                       \
        /* 1. Get a pointer to the base register as a volatile 32-bit type */  \
        volatile uint32_t* reg_addr                                            \
        = (volatile uint32_t*)((uint8_t*)(bar_ptr) + (pos));                   \
                                                                               \
        /* 2. Write the lower 32 bits */                                       \
        reg_addr[0] = (uint32_t)((uint64_t)(value64) & 0xFFFFFFFF);            \
                                                                               \
        /* 3. Write the upper 32 bits to the next 4-byte offset */             \
        reg_addr[1] = (uint32_t)(((uint64_t)(value64) >> 32) & 0xFFFFFFFF);    \
    } while (0)

int fd;
char read_buffer[100];
const char* write_buffer
= "This is a content to test QEMU EDU device DMA function.";
void *dma_buffer_ptr, *bar_ptr;

void signal_handler(int signum, siginfo_t* info, void* context)
{
    printf("Received SIGUSR1, DMA transfer complete!\n");

    if (info->si_int == EDU_DMA_EDU2RAM) {
        if (strcmp(write_buffer, (char*)dma_buffer_ptr) == 0) {
            printf("DMA test pass!\n");
        } else {
            printf("DMA test failed!\n");
        }
    }
}

int main()
{
    int ret;
    uint32_t read_value32, write_value32;
    uint64_t read_value64, write_value64;
    ssize_t bytes_write;
    uintptr_t buffer_address;
    fd = open(EDU_DEVICE, O_RDWR);
    if (fd < 0) {
        printf("Failed to open the device");
        return errno;
    }

    // ============== Test Factorial Computation =============

    write_value32 = 12;
    if ((bytes_write
         = pwrite(fd, &write_value32, sizeof(write_value32), EDU_FACT_CALC))
        == sizeof(write_value32)) {
        printf(
        "Write Value: %d, Write Size: %zd\n", write_value32, bytes_write);
    } else {
        printf("Failed to write the device");
        close(fd);
        return errno;
    }

    // Get factorial result
    if (pread(fd, &read_value32, sizeof(read_value32), EDU_FACT_CALC)
        == sizeof(read_value32)) {
        printf("Result: %d\n", read_value32);
    } else {
        printf("Failed to read the device");
        close(fd);
        return errno;
    }

    // Register the SIGUSR1 signal handler
    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Failed to register SIGUSR1 handler");
        close(fd);
        return errno;
    }

    // ============== Test DMA: RAM to EDU =============

    // mmaps
    bar_ptr
    = mmap(NULL, EDU_MEMORY_REGION, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (bar_ptr == (void*)-1) {
        printf("Failed to map bar memory");
        return errno;
    }

    dma_buffer_ptr = mmap(NULL,
                          DMA_BUFFER_SIZE,
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED,
                          fd,
                          EDU_MEMORY_REGION);

    if (dma_buffer_ptr == (void*)-1) {
        printf("Failed to map dma buffer memory");
        return errno;
    }

    // Set source address
    // buffer_address = (uintptr_t)write_buffer;
    // if (pwrite(fd, &buffer_address, sizeof(buffer_address),
    // EDU_DMA_SRC_ADDRESS)
    //     != sizeof(buffer_address)) {
    //     printf("Failed to set DMA source address");
    //     return errno;
    // }

    // get phsy addr of dma bus addr
    uint64_t dma_bus_addr;
    ioctl(fd, EDU_IOCTL_GET_DMA_ADDR, &dma_bus_addr);

    SET_DMA_USER(dma_bus_addr, EDU_DMA_SRC_ADDRESS);

    // Set data
    memcpy(dma_buffer_ptr, write_buffer, strlen(write_buffer) + 1);

    // Set destination address
    write_value64 = EDU_BUFFER_ADDRESS;
    // if (pwrite(fd, &write_value, sizeof(write_value), EDU_DMA_DST_ADDRESS)
    //     != sizeof(write_value)) {
    //     printf("Failed to set DMA destination address");
    //     return errno;
    // }
    SET_DMA_USER(write_value64, EDU_DMA_DST_ADDRESS);

    // Set transfer count
    write_value64 = strlen(write_buffer) + 1; // 1 means '\0'
    // if (pwrite(fd, &write_value, sizeof(write_value), EDU_DMA_COUNT)
    //     != sizeof(write_value)) {
    //     printf("Failed to set DMA count");
    //     return errno;
    // }
    SET_DMA_USER(write_value64, EDU_DMA_COUNT);

    // Start DMA transfer
    // write_value = EDU_DMA_RAM2EDU;
    // if (pwrite(fd, &write_value, sizeof(write_value), EDU_DMA_CMD)
    //     != sizeof(write_value)) {
    //     printf("Failed to start DMA transfer");
    //     return errno;
    // }

    write_value32 = EDU_DMA_RAM2EDU;
    ioctl(fd, EDU_IOCTL_SET_DMA_DIR, &write_value32);
    write_value64 |= DMA_START | DMA_IRQ;
    SET_DMA_USER(write_value64, EDU_DMA_CMD);

    // Wait for signal
    printf("Wait for signal\n");
    pause();

    // ============== Test DMA: EDU to RAM =============

    // Set source address
    write_value64 = EDU_BUFFER_ADDRESS;
    // if (pwrite(fd, &write_value, sizeof(write_value), EDU_DMA_SRC_ADDRESS)
    //     != sizeof(write_value)) {
    //     printf("Failed to set DMA source address");
    //     return errno;
    // }
    SET_DMA_USER(write_value64, EDU_DMA_SRC_ADDRESS);

    // Set destination address
    // buffer_address = (uintptr_t)read_buffer;
    // if (pwrite(fd, &buffer_address, sizeof(buffer_address),
    // EDU_DMA_DST_ADDRESS)
    //     != sizeof(buffer_address)) {
    //     printf("Failed to set DMA destination address");
    //     return errno;
    // }
    SET_DMA_USER(dma_bus_addr, EDU_DMA_DST_ADDRESS);

    // There is no need to reset transfer count, as it is the same as before

    // Start DMA transfer
    // write_value = EDU_DMA_EDU2RAM;
    // if (pwrite(fd, &write_value, sizeof(write_value), EDU_DMA_CMD)
    //     != sizeof(write_value)) {
    //     printf("Failed to start DMA transfer");
    //     return errno;
    // }

    write_value32 = EDU_DMA_EDU2RAM;
    ioctl(fd, EDU_IOCTL_SET_DMA_DIR, &write_value32);
    write_value64 = EDU_DMA_EDU2RAM | DMA_START | DMA_IRQ;
    SET_DMA_USER(write_value64, EDU_DMA_CMD);

    // Wait for signal
    printf("Wait for signal\n");
    pause();

    close(fd);

    return 0;
}
