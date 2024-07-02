#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

typedef struct
{
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    /*
     .
     .
     Any additional fields you deem necessary
    */
} state;

void toggleDebugMode(state *s)
{
    s->debug_mode = !s->debug_mode;
    printf("Debug flag now %s\n", s->debug_mode ? "on" : "off");
}

void setFileName(state *s)
{
    printf("Enter file name: ");
    fgets(s->file_name, sizeof(s->file_name), stdin);
    s->file_name[strcspn(s->file_name, "\n")] = '\0'; // Remove newline character
    if (s->debug_mode)
    {
        fprintf(stderr, "Debug: file name set to '%s'\n", s->file_name);
    }
}

void quit(state *s)
{
    printf("Quitting.\n");
    exit(0);
}

void loadIntoMemory(state *s)
{
    if (strcmp(s->file_name, "") == 0)
    {
        fprintf(stderr, "Error: File name is empty.\n");
        return;
    }

    int fd = 0;
    if(fd = fopen(s->file_name, "rb") < 0) {
        perror("Error opening file");
        return;
    }
    printf("Enter <location> <length>:\n");
    char input[100];
    fgets(input, sizeof(input), stdin);
    unsigned int location;
    int length;
    sscanf(input, "%x %d", &location, &length);

    if (s->debug_mode) {
        printf("Debug: file_name=%s, location=0x%x, length=%d\n", s->file_name, location, length);
    }

    if (lseek(fd, location, SEEK_SET) < 0)
    {
        perror("Error seeking file");
        close(fd);
        return;
    }

    int bytes_to_read = length * s->unit_size;
    if (bytes_to_read > sizeof(s->mem_buf))
    {
        fprintf(stderr, "Error: requested length exceeds buffer size.\n");
        close(fd);
        return;
    }

    int bytes_read = read(fd, s->mem_buf, bytes_to_read);
    if (bytes_read < 0)
    {
        perror("Error reading file");
        close(fd);
        return;
    }

    s->mem_count = bytes_read / s->unit_size;

    if (s->debug_mode)
    {
        printf("Debug info: read %ld bytes into memory.\n", bytes_read);
    }

    close(fd);
}

void toggleDisplayMode(state *s)
{
    return;
}

void memoryDisplay(state *s)
{
    return;
}

void saveIntoFile(state *s)
{
    return;
}

void memoryModify(state *s)
{
    return;
}

void setUnitSize(state *s)
{
    printf("Enter unit size (1, 2, or 4): ");
    char buffer[100];
    if (fgets(buffer, 100, stdin))
    {
        int size = atoi(buffer);
        if (size == 1 || size == 2 || size == 4)
        {
            s->unit_size = size;
            if (s->debug_mode)
            {
                fprintf(stderr, "Debug: set size to %d\n", s->unit_size);
            }
        }
        else
        {
            printf("Invalid unit size\n");
        }
    }
    else
    {
        printf("not within bounds. \n");
        exit(1);
    }
}

struct fun_desc
{
    char *name;
    void (*fun)(state *s);
};

struct fun_desc menu[] = {{"Toggle Debug Mode", toggleDebugMode}, {"Set File Name", setFileName}, {"Set Unit Size", setUnitSize}, {"Load Into Memory", loadIntoMemory}, {"Toggle Display Mode", toggleDisplayMode}, {"Memory Display", memoryDisplay}, {"Save Into File", saveIntoFile}, {"Memory Modify", memoryModify}, {"Quit", quit}, {NULL, NULL}};

int printMenu()
{
    printf("Select operation from the following menu (ctrl^D for exit): \n");
    int i = 0;
    while (menu[i].name != NULL)
    {
        printf("%d) %s \n", i, menu[i].name);
        i++;
    }
    return 0;
}

int main(int argc, char **argv)
{
    char inputBuffer[100];
    int size = sizeof(menu) / sizeof(menu[0]) - 1;
    printMenu();
    state s = {0, "", 1, {0}, 0};
    while (fgets(inputBuffer, 100, stdin))
    {
        int input = atoi(inputBuffer);
        if (input < 0 || input >= size)
        {
            printf("Not within bounds\n");
            exit(1);
        }
        else
        {
            printf("Within Bounds\n");
            menu[input].fun(&s);
            printf("DONE.\n");
        }
        printMenu();
    }
    return 0;
}