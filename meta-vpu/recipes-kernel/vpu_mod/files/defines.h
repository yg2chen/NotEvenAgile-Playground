#define DRIVER_NAME        "qemu_edu"
#define BAR                0
#define BASEMINOR          0
#define DEVICE_COUNT       1

#define EDU_ID             0x00
#define EDU_LIVE_CHECK     0x04
#define EDU_FACT_CALC      0x08
#define EDU_STATUS         0x20
#define EDU_IRQ_STATUS     0x24
#define EDU_IRQ_RAISE      0x60
#define EDU_IRQ_ACK        0x64
#define EDU_DMA_SRC_ADDR   0x80
#define EDU_DMA_DST_ADDR   0x88
#define EDU_DMA_COUNT      0x90
#define EDU_DMA_CMD        0x98
#define EDU_BUFFER_ADDRESS 0x40000

// Custom Parameters
#define EDU_DMA_GET        0x1234

#define STATUS_COMPUTING   0x01
#define STATUS_IRQFACT     0x80

#define DMA_START          0x01
#define DMA_RAM2EDU        0x0
#define DMA_EDU2RAM        0x02
#define DMA_IRQ            0x04
#define DMA_IRQ_VALUE      0x100

#define FACT_IRQ           0x01

#define BUFFER_SIZE        4096

#define DMA_BUFFER_SIZE    1024 * 1024

#define SET_DMA(value64, pos)                                                  \
    do {                                                                       \
        iowrite32((uint32_t)((value64) & 0xFFFFFFFF),                          \
                  edu_dev->mmio_base + (pos));                                 \
        iowrite32((uint32_t)(((value64) >> 32) & 0xFFFFFFFF),                  \
                  edu_dev->mmio_base + (pos) + 4);                             \
    } while (0)
