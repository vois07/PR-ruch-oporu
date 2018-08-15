#include <stdio.h>
#include <stdio.h>

void clearLogFile() {
    FILE *fp;
    fp = fopen("log.txt", "w");
    fclose(fp);
}

void printStatus(char text[]) {
    printf("%s", text);
    FILE *fp;
    fp = fopen("log.txt", "a");
    fprintf(fp, "%s", text);
    fclose(fp);
}

