#include <stdio.h>
#include <dlfcn.h>
#include "loader.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <ELF file>\n", argv[0]);
        return 1;
    }

    void* handle = dlopen("./lib_simpleloader.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return 1;
    }

    void (*load_and_run_elf)(char**) = dlsym(handle, "load_and_run_elf");
    if (!load_and_run_elf) {
        fprintf(stderr, "%s\n", dlerror());
        dlclose(handle);
        return 1;
    }

    load_and_run_elf(&argv[1]);

    dlclose(handle);
    return 0;
}
