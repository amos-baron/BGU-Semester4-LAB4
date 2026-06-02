#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char debug_mode = 0;
char file_name[128] = "";
int unit_size = 1;
unsigned char mem_buf[10000];
size_t mem_count = 0;
int display_mode = 0;

char* hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
char* dec_formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};

struct fun_desc {
    char *name;
    char index;
    void (*fun)(void);
};

void toggle_debug_mode() {
    if (debug_mode == 0) {
        debug_mode = 1;
        fprintf(stderr, "Debug flag now on\n");
    } else {
        debug_mode = 0;
        fprintf(stderr, "Debug flag now off\n");
    }
}

void set_file_name() {
    printf("Please enter file name:\n");
    if (fgets(file_name, sizeof(file_name), stdin) != NULL) {
        file_name[strcspn(file_name, "\n")] = 0;
        if (debug_mode) {
            fprintf(stderr, "Debug: file name set to '%s'\n", file_name);
        }
    }
}

void set_unit_size() {
    char input[10];
    printf("Please enter unit size:\n");
    if (fgets(input, sizeof(input), stdin) != NULL) {
        int size = atoi(input);
        if (size == 1 || size == 2 || size == 4) {
            unit_size = size;
            if (debug_mode) {
                fprintf(stderr, "Debug: set size to %d\n", unit_size);
            }
        } else {
            printf("Invalid unit size\n");
        }
    }
}

void load_into_memory() {
    if (strcmp(file_name, "") == 0) {
        printf("Error: file name is empty\n");
        return;
    }

    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        printf("Error: could not open file\n");
        return;
    }

    printf("Please enter <location> <length>\n");
    char input[256];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        fclose(file);
        return;
    }

    unsigned int location;
    int length;
    if (sscanf(input, "%x %d", &location, &length) != 2) {
        printf("Error: invalid input\n");
        fclose(file);
        return;
    }

    if (debug_mode) {
        fprintf(stderr, "Debug: file_name=%s, location=%#x, length=%d\n", file_name, location, length);
    }

    fseek(file, location, SEEK_SET);
    size_t units_read = fread(mem_buf, unit_size, length, file);
    fclose(file);

    mem_count = units_read * unit_size;
    printf("Loaded %zu units into memory\n", units_read);
}

void toggle_display_mode() {
    if (display_mode == 0) {
        display_mode = 1;
        printf("Decimal display flag now on, decimal representation\n");
    } else {
        display_mode = 0;
        printf("Decimal display flag now off, hexadecimal representation\n");
    }
}

void memory_display() {
    printf("Enter address and length\n");
    char input[256];
    if (fgets(input, sizeof(input), stdin) == NULL) return;
    
    unsigned int addr;
    int length;
    if (sscanf(input, "%x %d", &addr, &length) != 2) {
        printf("Error: invalid input\n");
        return;
    }

    unsigned char* source = (addr == 0) ? mem_buf : (unsigned char*)addr;

    if (display_mode == 1) {
        printf("Decimal\n=======\n");
    } else {
        printf("Hexadecimal\n===========\n");
    }

    for (int i = 0; i < length; i++) {
        int val = 0;
        memcpy(&val, source + (i * unit_size), unit_size);
        
        if (display_mode == 1) {
            printf(dec_formats[unit_size - 1], val);
        } else {
            printf(hex_formats[unit_size - 1], val);
        }
    }
}

void save_into_file() {
    if (strcmp(file_name, "") == 0) {
        printf("Error: file name is empty\n");
        return;
    }

    printf("Please enter <source-address> <target-location> <length>\n");
    char input[256];
    if (fgets(input, sizeof(input), stdin) == NULL) return;
    
    unsigned int source_addr, target_loc;
    int length;
    if (sscanf(input, "%x %x %d", &source_addr, &target_loc, &length) != 3) {
        printf("Error: invalid input\n");
        return;
    }

    FILE *file = fopen(file_name, "rb+");
    if (file == NULL) {
        printf("Error: could not open file\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (target_loc > file_size) {
        printf("Error: target location exceeds file size\n");
        fclose(file);
        return;
    }

    unsigned char* source = (source_addr == 0) ? mem_buf : (unsigned char*)source_addr;

    fseek(file, target_loc, SEEK_SET);
    fwrite(source, unit_size, length, file);
    fclose(file);
}

void memory_modify() {
    printf("Please enter <location> <val>\n");
    char input[256];
    if (fgets(input, sizeof(input), stdin) == NULL) return;
    
    unsigned int location, val;
    if (sscanf(input, "%x %x", &location, &val) != 2) {
        printf("Error: invalid input\n");
        return;
    }

    if (debug_mode) {
        fprintf(stderr, "Debug: location=%#x, val=%#x\n", location, val);
    }

    if (location + unit_size > 10000) {
        printf("Error: location out of bounds\n");
        return;
    }

    memcpy(&mem_buf[location], &val, unit_size);
}

void quit() {
    if (debug_mode) {
        fprintf(stderr, "quitting\n");
    }
    exit(0);
}

int main() {
    struct fun_desc menu[] = {
        { "Toggle <D>ebug Mode", 'D', toggle_debug_mode },
        { "Set <F>ile Name", 'F', set_file_name },
        { "Set <U>nit Size", 'U', set_unit_size },
        { "<L>oad Into Memory", 'L', load_into_memory },
        { "<T>oggle Display Mode", 'T', toggle_display_mode },
        { "<M>emory Display", 'M', memory_display },
        { "<S>ave Into File", 'S', save_into_file },
        { "Memory Modif<y>", 'y', memory_modify },
        { "<Q>uit", 'Q', quit },
        { NULL, 0, NULL }
    };

    char input[100];
    bool valid_input;

    while (true) {
        valid_input = false;

        if (debug_mode) {
            fprintf(stderr, "unit_size: %d, file_name: %s, mem_count: %zu\n", unit_size, file_name, mem_count);
        }

        printf("Choose action:\n");
        for (int i = 0; menu[i].name != NULL; i++) {
            printf("%s\n", menu[i].name);
        }

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        for (int i = 0; menu[i].name != NULL; i++) {
            if (input[0] == menu[i].index || input[0] == menu[i].index + 32 || input[0] == menu[i].index - 32) {
                valid_input = true;
                menu[i].fun();
                break;
            }
        }

        if (!valid_input) {
            printf("Invalid selection\n");
        }
    }

    return 0;
}
