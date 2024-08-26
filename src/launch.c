#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "loader.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <ELF Executable>\n", argv[0]);
        exit(1);
    }

    // Load the shared library
    void* handle = dlopen("./lib_simpleloader.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Error loading shared library: %s\n", dlerror());
        exit(1);
    }

    // Get function pointers
    void (*load_and_run_elf)(char**) = dlsym(handle, "load_and_run_elf");
    void (*loader_cleanup)() = dlsym(handle, "loader_cleanup");

    if (!load_and_run_elf || !loader_cleanup) {
        fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(handle);
        exit(1);
    }

    // Call the loader function
    load_and_run_elf(&argv[1]);

    // Call the cleanup function
    loader_cleanup();

    // Close the shared library
    dlclose(handle);

    return 0;
}
