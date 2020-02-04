#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>

typedef struct _PGMdata{
    int row;
    int col;
    int max_grey;
    int **matrix;
} PGMdata;

int **allocate_dynamic_matrix(int row, int col){
    int **ret_val;
    int i;

    ret_val = (int **)malloc(sizeof(int *)*row);
    if(ret_val==NULL){
        printf("Error! memory allocation failure");
        exit(1);
    }

    for(i=0; i<row; i++){
        ret_val[i] = (int *)malloc(sizeof(int)*col);
        if(ret_val[i]==NULL){
            printf("Error! Memory Allocation Failure");
            exit(1);
        }
    }
    return ret_val;
}

void skip_comments(FILE *fptr){

    char ch;
    ch = getc(fptr);

    while(ch=='#'){
        ch = getc(fptr);
        while(ch!='\n'){
            ch = getc(fptr);
        }
    }
    fseek(fptr, -1, SEEK_CUR);
}

int get_otsu_threshold(PGMdata *data){

    int hist[256]={0}, i, j, t=0;
    
    int total_pixels = data->row * data->col;

    //compute histogram i.e. frequecy of each pixel value
    for(i=0;i<data->row;i++){
        for(j=0; j<data->col; j++){
            hist[data->matrix[i][j]]++;
        }
    }
    //calculate probability of pixel values
    double prob[256];
    int threshold=0;
    double q0=0, q1=0, meansum0=0, meansum1=0, mean0=0, mean1=0;

    for(i=0; i<256; i++){
        prob[i] = (double)hist[i]/total_pixels;
    }
    for(i=0;i<256; i++){
        meansum1 += i*prob[i];
    }
    double mean_diff=0,between_class_var=0, max_var=0;

    for(t=0;t<256;t++){
        q0 += prob[t];
        q1 = 1 - q0;

        meansum0 += t*prob[t];
        meansum1 -= t*prob[t];

        mean0 = meansum0/q0;
        mean1 = meansum1/q1;

        mean_diff = mean0 - mean1;
        between_class_var = q0*(1.0-q0)*(pow(mean_diff,2));

        if(between_class_var>max_var){
            max_var = between_class_var;
            threshold = t;
        }
    }
    printf("Computed Threshold value is : %d\n",threshold);
    return threshold;
}

PGMdata* readPGM(const char *filename, PGMdata *data){
    FILE *fptr;
    char type[3];
    int i=0, j=0, low, high;

    fptr = fopen(filename, "rb");
    if(fptr==NULL){
        printf("Error! cannot open file");
        exit(1);
    }

    fgets(type, sizeof(type), fptr);
    if(!strcmp(type, "P5\n")){
        printf("Error! wrong file type");
        exit(1);
    }

    char ch = getc(fptr);
    while(ch!='\n');

    skip_comments(fptr);
    fscanf(fptr, "%d %d", &data->row, &data->col);
    skip_comments(fptr);
    fscanf(fptr, "%d", &data->max_grey);
    fgetc(fptr);

    data->matrix = allocate_dynamic_matrix(data->row, data->col);

    for(i=0; i<data->row; i++){
        for(j=0;j<data->col;j++){
            low = getc(fptr);
            data->matrix[i][j] = low;
        }
    }
    return data;
}

void writePGM(const char *filename, PGMdata *data, int threshold){

    FILE *fptr;
    int i, j, pixel;

    fptr = fopen(filename, "wb");
    if(fptr==NULL){
        printf("Error! cannot open file...");
        exit(1);
    }

    fprintf(fptr, "P5\n");
    fprintf(fptr, "%d %d\n", data->row, data->col);
    fprintf(fptr, "%d\n", data->max_grey);

    for(i=0;i<data->row;i++){
        for(j=0;j<data->col;j++){
            if(data->matrix[i][j]>threshold){
                pixel = 255;
            }else{
                pixel = 0;
            }
            fwrite(&pixel, 1, 1, fptr);
        }
    }
    fclose(fptr);
}
int main(){
    PGMdata pgmdata;
    PGMdata* data = &pgmdata;

    int i=0, j=0;
    const char *filename = "images/boats.pgm";
    data = readPGM(filename, data);

    int threshold = get_otsu_threshold(data);
    
    const char *output_file = "output/lighthouse.pgm";
    writePGM(output_file, data, threshold);
    return 0;
}