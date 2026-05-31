#include <stdio.h>

int count_digits(char *str) {
    int count = 0;
    while (*str != '\0') {
        if (*str >= '0' && *str <= '9') {
            count++;
        }
        str++;
    }
    return count;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        printf("Digit count: %d\n", count_digits(argv[1]));
    }
    return 0;
}
