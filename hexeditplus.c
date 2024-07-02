#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

typedef struct {
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

void toggleDebugMode(state* s) {
    s->debug_mode = !s->debug_mode;
    printf("Debug flag now %s\n", s->debug_mode ? "on" : "off");
}

void setFileName(state* s) {
    printf("Enter a new file name \n");
    char input[128];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';
    /*if(rename(curr_filename,  input) != 0){
        exit(1);
    }*/ 
    s->file_name = strdup(input); 
}

void quit(state* s) {
    free(s);
    exit(0);
}

void setUnitSize(state* s) {
    printf("Enter unit size (1, 2, or 4): ");
    int size;
    char buffer[100];
    if(sscanf(buffer, "%d", &size) != 1){
        printf("not within bounds. \n");
        exit(1);
    }
    if (size == 1 || size == 2 || size == 4) {
        s->unit_size = size;
        if (s->debug_mode) {
            fprintf(stderr, "Debug: set size to %d\n", s->unit_size);
        }
    } 
    else {
        printf("Invalid unit size\n");
    }
}




struct fun_desc {
char *name;
char (*fun)(char);
};

struct fun_desc menu[] = { { "Toggle Debug Mode", toggleDebugMode }, { "Set File Name", setFileName },  
    { "Set Unit Size", setUnitSize }, { "Load Into Memory" , decrypt}, { "Toggle Display Mode", xoprt },
    { "Memory Display", xoprt }, { "Save Into File", xoprt }, { "Memory Modify", xoprt },
    { "Quit", xoprt } { NULL, NULL } };

int printMenu(){
    printf("Select operation from the following menu (ctrl^D for exit): \n");
    int i = 0;
    while(menu[i].name != NULL){
        printf("%d) %s \n", i, menu[i].name);
        i++;
    }
    return 0;
}

int main(int argc, char **argv){
    char * carray = calloc(5, sizeof(char));
    char * temp;
    int size = sizeof(menu) / sizeof(menu[0]);
    int leftbound = 0;
    int rightbound = size-2;
    printMenu();
    char buffer[100];
    int num;
    while(fgets(buffer, 100, stdin) != NULL){
        if(sscanf(buffer, "%d", &num) != 1){
            printf("not within bounds. \n");
            exit(1);
        }
        if(leftbound <= num && num <= rightbound){
            printf("Within bounds. \n");
            temp = carray;
            carray = map(carray, 5, menu[num].fun);
            free(temp);
            printf("DONE. \n");
            printMenu();
        }
        else{
            printf("Not within bounds. \n");
            exit(1);
        } 
    }    
    return 0;
}