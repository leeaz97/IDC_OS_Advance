
#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

// Port Usage:
//     0xE9 = 233 = print uint32_t as char32_t orignal kvm-hello-world
//     0xEB = 235 = print uint32_t as integer
//     0xED = 237 = print uint32_t as char* which is a null terminated string
//     0xEF = 239 = file system functionalities such as: open, close, read, write



// static void outb(uint16_t port, uint8_t value) {
//     asm("outb %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
// }

static inline void outb(uint16_t port, uint32_t value) {
    asm("out %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
}

static inline uint32_t inb(uint16_t port) {
    uint32_t ret;
    asm("in %1, %0" : "=a"(ret) : "Nd"(port) : "memory" );
    return ret;
}

void printInt(uint32_t val) {
    outb(235, val);
}

uint32_t exits() {
    return inb(235);
}

void print(const char *str) {
    outb(237, (uint32_t) ((intptr_t) str));
}

uint32_t FILE_IO_PORT = 239;

enum FileSystemOperationType {
    ENUM_FILE_OPEN = 3, ENUM_FILE_CLOSE = 5, ENUM_FILE_READ = 7, ENUM_FILE_WRITE = 9};

struct FileSystemOperation {
    enum FileSystemOperationType operation_type;

    const char *param_path;
    int param_fd;
    void *param_buf;
    size_t param_cnt;

    int return_result_int;
    size_t return_result_size_t;
};

struct FileSystemOperation file_hypercall(struct FileSystemOperation fso) {
    outb(FILE_IO_PORT, (intptr_t) (&fso));
    return fso;
}


int open(const char *Path) {
    struct FileSystemOperation fso = {
            .operation_type = ENUM_FILE_OPEN,
            .param_path = Path,
    };
    return file_hypercall(fso).return_result_int;
}

int close(int fd) {
    struct FileSystemOperation fso = {
            .operation_type = ENUM_FILE_CLOSE,
            .param_fd = fd
    };
    return file_hypercall(fso).return_result_int;
}

size_t read(int fd, void *buf, size_t cnt) {
    struct FileSystemOperation fso = {
            .operation_type = ENUM_FILE_READ,
            .param_fd = fd,
            .param_buf = buf,
            .param_cnt = cnt
    };
    return file_hypercall(fso).return_result_size_t;
}

size_t write(int fd, void *buf, size_t cnt) {
    struct FileSystemOperation fso = {
            .operation_type = ENUM_FILE_WRITE,
            .param_fd = fd,
            .param_buf = buf,
            .param_cnt = cnt
    };
    return file_hypercall(fso).return_result_size_t;
}

void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
    const char *p;

    for (p = "Hello, world!\n"; *p; ++p)
        outb(233, *p);

    uint32_t numExits = exits();  
    printInt(numExits);  
    print("GUEST: print from gust to screen via the hypervisor :)\n");  
    numExits = exits();  
    printInt(numExits); 
    
    int filefd1 = open("./t.txt");
    write(filefd1, "lee&adi\n", 8);
    close(filefd1);
    
    //Read from "t.txt"
    char buff[1024];
    int filefd2 = open("./t.txt");
    size_t cnt = read(filefd2, buff, 1024);
    close(filefd2);
    buff[cnt] = '\0';  // NOTE: this is necessary because read call will not add '\0' to the buffer
    print(buff);


    *(long *) 0x400 = 42;

    for (;;)
        asm("hlt" : /* empty */ : "a" (42) : "memory");  // total exits = 20
}
