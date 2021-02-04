#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define GAMMA_STEP                      256
#define GAMMA_STEP_HALF                 (GAMMA_STEP/2)
#define GAMMA_RGB                       3
#define READ_SYSFS_FILE_MAX_LINE_LEN    100
#define CSV_FILE                        "./table.csv"

#define BIN                             2
#define OCT                             8
#define DEC                             10
#define HEX                             16
//data_x data_y means array's size(x * y)
static int data_x = 0;
static int data_y = 0;

FILE *fopen_file(char* file_name) {
    FILE *fp = NULL;
    fp = fopen(file_name,"r");
    if (fp == NULL) {
        printf("%s, open file %s error.\n", __FUNCTION__, file_name);
    }
    return fp;
}

int load_csvdate(FILE *fp, int data_buf[][GAMMA_STEP + 1]) {
    if (fp == NULL) {
        printf("fp is null\n");
        return 0;
    }
    char r_line_buf[READ_SYSFS_FILE_MAX_LINE_LEN] = {'\0'};
    char *result = NULL;
    char delims[] = ",";
    int a = 0;
    while(fgets(r_line_buf, READ_SYSFS_FILE_MAX_LINE_LEN, fp)) {
        result = strtok(r_line_buf, delims);
        while (result != NULL) {
            data_buf[data_x][data_y] = atoi(result);
            //a  = atoi(result);
            //printf("%d\n",a);
            data_x++;
            result = strtok(NULL, delims);
        }
        //reset to 0;
        data_x = 0;
        data_y++;
    }
    return data_y;
}

int writeIntDataToFile(char *file_name, int data_buf[], int data_size, int flag) {
    FILE *fp = NULL;
    fp = fopen(file_name, "w");
    if (fp == NULL) {
        printf("%s, fp is NULL\n", __FUNCTION__);
        return 0;
    }

    for(int i = 0; i < data_size; i++) {
        fprintf(fp, "%d", data_buf[i]);
        if ((i + 1) < data_size) 
            fprintf(fp, "%c", ',');
    }
    fprintf(fp, "%c",'\n');
    return data_size;
}

//flag: bin oct dec hex 0 1 2 3
int appendIntDataToFile(char *file_name, int data_buf[], int data_size, int flag) {
    FILE *fp = NULL;
    fp = fopen(file_name, "a");
    if (fp == NULL) {
        printf("%s, fp is NULL\n", __FUNCTION__);
        return 0;
    }

    for(int i = 0; i < data_size; i++) {
        if (flag == 16 )
            fprintf(fp, "0x%03x", data_buf[i]);
        else
            fprintf(fp, "%d", data_buf[i]);
        if ((i + 1) < data_size) 
            fprintf(fp, "%c", ',');
    }
    fprintf(fp, "%c",'\n');
    fclose(fp);
    return data_size;
}

int appendCharToFile(char *file_name, char word) {
    FILE *fp = NULL;
    fp = fopen(file_name, "a");
    if (fp == NULL) {
        printf("%s, fp is NULL\n", __FUNCTION__);
        return 0;
    }

    fprintf(fp, "%c", word);
    fclose(fp);
    return 1;
}

int appendStringToFile(char *file_name, char *string) {

    FILE *fp = NULL;
    fp = fopen(file_name, "a");
    if (fp == NULL) {
        printf("%s, fp is NULL\n", __FUNCTION__);
        return 0;
    }
    fprintf(fp, "%s", string);
    fclose(fp);
    return strlen(string) + 1;
}

void show_data_dec(int data_buf[][GAMMA_STEP + 1], int x, int y) {
    for (int i = 0; i < x; i++) {
        printf("line_%d: ", i);
        for (int j = 0; j < x; j++) {
            printf("%d\t", data_buf[j][i]);
        }
        printf("\n");
    }
    for (int i = 0; i < x; i++) {
        printf("line_%d: ", y - x + i);
        for (int j = 0; j < x; j++) {
            printf("%d\t", data_buf[j][y - x + i]);
        }
        printf("\n");
    }
}

void get_gamma_data(int data_gamma[][GAMMA_STEP], int data_buf[][GAMMA_STEP + 1], int base) {
    for (int i = 0; i < GAMMA_RGB; i++) {
        for (int j = 0; j < GAMMA_STEP; j++) {
            data_gamma[i][j] = data_buf[i + 1][j + 1] * base;
        }
    }
}

void split_gamma_to_oebuf(int data_oe_gamma[][GAMMA_STEP_HALF], int data_gamma[][GAMMA_STEP]) {
    for (int i = 0; i < GAMMA_RGB; i++) {
        for (int j = 0; j < GAMMA_STEP; j++) {
            if (j%2) {
                data_oe_gamma[i*2 + 1][j/2] = data_gamma[i][j];
            }
            else
                data_oe_gamma[i*2][j/2] = data_gamma[i][j];
        }
    }
}

int getOutputFilename(char *input_name, char symbol, char *output_name){
    char buf[20] = {'\0'};
    int len = 0;
    for (int i = 0; i < strlen(input_name); i++) {
        if (input_name[i] != symbol)
            output_name[i] = input_name[i];
        else {
            len = i;
            break;
        }
    }
    memcpy(output_name+len, ".output", 11);
    //printf("%s\n",output_name);
    return 0;
}

int usage(int argc, char* argv[]) {
    if (argc < 2) {
        printf("usage: %s file.csv\n", argv[0]);
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]){

    if (usage(argc, argv)) {
        return 1;
    }

    printf("Start GAMMA\n");

    char output_file[30] = {'\0'};
    getOutputFilename(argv[1], '.', output_file);
    FILE *fp = NULL;
    int data_buf[5][GAMMA_STEP + 1];
    int data_gamma[GAMMA_RGB][GAMMA_STEP];
    int data_oe_gamma[GAMMA_RGB*2][GAMMA_STEP_HALF];
    fp = fopen_file(argv[1]);
    if (fp == NULL)
        return 1;

    //load csv data to buf
    int line = load_csvdate(fp, data_buf);
    fclose(fp);

    //show_data_dec(data_buf, 4, line);
    
    //get r g b data from csv data_buf
    get_gamma_data(data_gamma, data_buf, 2);

    split_gamma_to_oebuf(data_oe_gamma, data_gamma);

    appendStringToFile(output_file, "REN:\n");
    appendIntDataToFile(output_file, data_oe_gamma[0], GAMMA_STEP_HALF, HEX);
    appendIntDataToFile(output_file, data_oe_gamma[1], GAMMA_STEP_HALF, HEX);
    appendCharToFile(output_file, '\n');
    appendStringToFile(output_file, "GREEN:\n");
    appendIntDataToFile(output_file, data_oe_gamma[2], GAMMA_STEP_HALF, HEX);
    appendIntDataToFile(output_file, data_oe_gamma[3], GAMMA_STEP_HALF, HEX);
    appendCharToFile(output_file, '\n');
    appendStringToFile(output_file, "BLUE:\n");
    appendIntDataToFile(output_file, data_oe_gamma[4], GAMMA_STEP_HALF, HEX);
    appendIntDataToFile(output_file, data_oe_gamma[5], GAMMA_STEP_HALF, HEX);
    appendCharToFile(output_file, '\n');

    printf("Output: %s\n", output_file);
    printf("Finish GAMMA\n");
    return 0;
}

