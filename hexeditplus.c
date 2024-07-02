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
    int display_mode;
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

    FILE *fd;
    if ((fd = fopen(s->file_name, "rb")) < 0)
    {
        perror("Error opening file");
        return;
    }
    printf("Enter <location> <length>:\n");
    char input[100];
    fgets(input, sizeof(input), stdin);
    unsigned int location;
    int length;
    sscanf(input, "%x %d", &location, &length);
    if (s->debug_mode)
    {
        printf("Debug: file_name=%s, location=0x%x, length=%d\n", s->file_name, location, length);
    }

    if (fseek(fd, location, SEEK_SET) < 0)
    {
        perror("Error seeking file");
        fclose(fd);
        return;
    }

    int bytes_to_read = length * s->unit_size;
    if (bytes_to_read > sizeof(s->mem_buf))
    {
        fprintf(stderr, "Error: requested length exceeds buffer size.\n");
        fclose(fd);
        return;
    }
    size_t bytes_read = fread(s->mem_buf, s->unit_size, length, fd);
    if (bytes_read < length)
    {
        perror("Could not read the expected number of units.");
        fclose(fd);
    }
    s->mem_count = bytes_read; // maybe need to multiply by unit_size
    printf("read %d units into memory.\n", length);

    fclose(fd);
}

void toggleDisplayMode(state *s)
{
    s->display_mode = !s->display_mode;
    printf("Display flag now %s, %s representation\n", s->display_mode ? "on" : "off", s->display_mode ? "hexadecimal" : "decimal");
}

static char *hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
static char *dec_formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};

void memoryDisplay(state *s)
{
    unsigned int addr; // address to start displaying from (if addr !=0)
    int u;             // number of units to display
    char input[100];

    printf("Enter <address> <length>\n");
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %d", &addr, &u);
    unsigned char *start_addr;
    if (addr == 0)
    {
        start_addr = s->mem_buf;
    }
    else
    {
        start_addr = (unsigned char *)addr;
    }

    if (s->display_mode)
    {
        printf("Hexadecimal\n===========\n");
        for (int i = 0; i < u; ++i)
        {
            unsigned int n = 0;
            memcpy(&n, start_addr + i * s->unit_size, s->unit_size);
            printf(hex_formats[s->unit_size - 1], n);
        }
    }
    else
    {
        printf("Decimal\n=======\n");
        for (int i = 0; i < u; ++i)
        {
            unsigned int n = 0;
            memcpy(&n, start_addr + i * s->unit_size, s->unit_size);
            printf(dec_formats[s->unit_size - 1], n);
        }
    }
}

void saveIntoFile(state *s)
{
    printf("Enter <source-address> <target-location> <length>\n");
    char input[100];
    fgets(input, sizeof(input), stdin);
    unsigned int source_address;
    unsigned int target_location;
    int length;
    unsigned char* sourceAdr;
    sscanf(input, "%x %x %d", &source_address, &target_location, &length);
    if (source_address == 0)
    {
        sourceAdr = s->mem_buf;
    }
    else
    {
        sourceAdr = (unsigned char *)source_address;
    }
    if (s->file_name == "")
    {
        printf("Error: File name is not set.\n");
        return;
    }
    FILE *file = fopen(s->file_name, "r+b");
    if (file == NULL)
    {
        printf("Error: Failed to open file %s\n", s->file_name);
        return;
    }

   if(fseek(file, target_location, SEEK_SET) != 0) {
        perror("Could not open file.");
        fclose(file);
        return;
   }

//need to add a check if wrote all 
    fwrite(sourceAdr, s->unit_size, length, file);
    fclose(file);

    if (s->debug_mode)
    {
        printf("Wrote %d units from memory to file %s at offset 0x%x.\n", length, s->file_name, target_location);
    }
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
    state s = {0, "", 1, {0}, 0, 0};
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
        }
        printMenu();
    }
    return 0;
}