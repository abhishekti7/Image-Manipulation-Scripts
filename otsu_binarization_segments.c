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

void writePGM(const char *filename, PGMdata *data, int segemented_threshold[]){
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
            if(i>=0 && i<=(data->row/2)-1 && j>=0 && j<=data->col/4-1){
                if(data->matrix[i][j]>segemented_threshold[0]){
                    pixel = 255;
                }else{
                    pixel = 0;
                }
            }
            if(i>=0 && i<=(data->row/2)-1 && j>=(data->col/4) && j<=2*(data->row/4)-1){
                if(data->matrix[i][j]>segemented_threshold[1]){
                    pixel = 255;
                }else{
                    pixel = 0;
                }
            }
            if(i>=0 && i<=(data->row/2)-1 && j>=2*(data->col/4) && j<=3*(data->row/4)-1){
                if(data->matrix[i][j]>segemented_threshold[2]){
                    pixel = 255;
                }else{
                    pixel = 0;
                }
            }
            if(i>=0 && i<=(data->row/2)-1 && j>=3*(data->col/4) && j<=4*(data->row/4)-1){
                if(data->matrix[i][j]>segemented_threshold[3]){
                    pixel = 255;
                }else{
                    pixel = 0;
                }
            }
            if(i>=data->row/2 && i<=data->row-1 && j>=0 && j<=(data->row/4)-1){
                if(data->matrix[i][j]>segemented_threshold[4]){
                    pixel = 255;
                }else{
                    pixel = 0;
                }
            }
            if(i>=data->row/2 && i<=data->row-1 && j>=(data->col/4) && j<=2*(data->row/4)-1){
                if(data->matrix[i][j]>segemented_threshold[5]){
                    pixel = 255;
                }else{
                    pixel = 0;
                }
            }
            if(i>=data->row/2 && i<=data->row-1 && j>=2*(data->col/4) && j<=3*(data->row/4)-1){
                if(data->matrix[i][j]>segemented_threshold[6]){
                    pixel = 255;
                }else{
                    pixel = 0;
                }
            }
            if(i>=data->row/2 && i<=data->row-1 && j>=3*(data->col/4) && j<=4*(data->row/4)-1){
                if(data->matrix[i][j]>segemented_threshold[7]){
                    pixel = 255;
                }else{
                    pixel = 0;
                }
            }
            fwrite(&pixel, 1, 1, fptr);
        }
    }
    fclose(fptr);
}

int get_otsu_threshold(PGMdata *data, int row1, int row2, int col1, int col2){

    int hist[256]={0}, i, j, t=0;
    
    int total_pixels = (row2-row1+1)*(col2-col1+1);

    //compute histogram i.e. frequecy of each pixel value
    for(i=row1;i<=row2;i++){
        for(j=col1; j<=col2; j++){
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

void otsu_localised_binarization(const char *output_file, PGMdata *data){

    int segmented_threshold[8] = {0}, i=0, j;
    int col_factor = data->col/4;
    int row_factor = data->row/2;
    int indx = 0;

    for(i=0;i<2;i++){
        for(j=0;j<4;j++){
            segmented_threshold[indx++] = get_otsu_threshold(data, i*row_factor,(i+1)*row_factor-1, j*col_factor, (j+1)*col_factor-1);
        }
    }
    writePGM(output_file, data, segmented_threshold);
}

int main(){
    PGMdata pgmdata;
    PGMdata* data = &pgmdata;

    int i=0, j=0;
    const char *filename = "images/boats.pgm";
    const char *output_file = "output/otsu_local_boats.pgm";
    data = readPGM(filename, data);

    otsu_localised_binarization(output_file, data);
    
    return 0;
}