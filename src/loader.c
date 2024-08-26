#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;
void *mapped_memory = NULL;

void loader_cleanup() {
    if (mapped_memory) {
        munmap(mapped_memory, ehdr->e_phnum * sizeof(Elf32_Phdr));
    }
    if (fd != -1) {
        close(fd);
    }
    free(ehdr);
    free(phdr);
}

void load_and_run_elf(char** exe) {
    fd = open(exe[0], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }

    // Read ELF header
    ehdr = malloc(sizeof(Elf32_Ehdr));
    if (read(fd, ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)) {
        perror("Error reading ELF header");
        exit(1);
    }

    // Read program headers
    phdr = malloc(ehdr->e_phnum * sizeof(Elf32_Phdr));
    lseek(fd, ehdr->e_phoff, SEEK_SET);
    if (read(fd, phdr, ehdr->e_phnum * sizeof(Elf32_Phdr)) != ehdr->e_phnum * sizeof(Elf32_Phdr)) {
        perror("Error reading program headers");
        exit(1);
    }

    // Find PT_LOAD segment containing entry point
    Elf32_Phdr *load_segment = NULL;
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD && ehdr->e_entry >= phdr[i].p_vaddr &&
            ehdr->e_entry < (phdr[i].p_vaddr + phdr[i].p_memsz)) {
            load_segment = &phdr[i];
            break;
        }
    }

    if (!load_segment) {
        fprintf(stderr, "Could not find PT_LOAD segment containing entry point\n");
        exit(1);
    }

    // Map segment into memory
    mapped_memory = mmap(NULL, load_segment->p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mapped_memory == MAP_FAILED) {
        perror("Error mapping memory");
        exit(1);
    }

    // Copy segment content
    lseek(fd, load_segment->p_offset, SEEK_SET);
    if (read(fd, mapped_memory, load_segment->p_filesz) != load_segment->p_filesz) {
        perror("Error reading segment content");
        exit(1);
    }

    // Calculate entry point address
    void *entry_point = mapped_memory + (ehdr->e_entry - load_segment->p_vaddr);

    // Cast to function pointer and call
    int (*_start)() = (int (*)())entry_point;
    int result = _start();
    printf("User _start return value = %d\n", result);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <ELF Executable>\n", argv[0]);
        exit(1);
    }

    load_and_run_elf(&argv[1]);
    loader_cleanup();
    return 0;
}
