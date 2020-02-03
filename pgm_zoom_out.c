#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define HI(num)	(((num) & 0x0000FF00) << 8)
#define LO(num)	((num) & 0x000000FF)

typedef struct _PGMdata{
    int row;
    int col;
    int max_gray;
    int **matrix;
} PGMdata;

int **allocate_dynamic_matrix(int row, int col){
    int **ret_val;
    int i;

    ret_val = (int **)malloc(sizeof(int *) * row);
    if(ret_val==NULL){
        printf("Error! memory allocation failure");
        exit(1);
    }

    for(i=0;i<row;i++){
        ret_val[i] = (int *)malloc(sizeof(int) * col);
        if(ret_val[i]==NULL){
            printf("Error! memory allocation failure");
            exit(1);
        }
    }
    return ret_val;
}

void deallocate_dynamic_matrix(int **matrix, int row){
    int i;
 
    for (i = 0; i < row; ++i) {
        free(matrix[i]);
    }
    free(matrix);
}

void skip_comments(FILE *fptr){
    char ch;
    ch = fgetc(fptr);
    
    while(ch=='#'){
        ch = getc(fptr);
        while (ch!='\n'){
            ch = fgetc(fptr);
        }
    }
    fseek(fptr, -1, SEEK_CUR);
}

void pgm_zoom(const char *filename, PGMdata* data, PGMdata* zoomed_data){
    FILE *fptr;
    char type[3];
    int i=0,j=0;
    int low, high;

    fptr = fopen(filename, "rb");
    if(fptr==NULL){
        printf("Error! cannot open file");
        exit(1);
    }

    fgets(type, sizeof(type), fptr);
    
    if(!strcmp(type, "P5\n")){
        printf("Wrong file type");
        exit(1);
    }
    char ch = getc(fptr);
    while(ch!='\n');

    skip_comments(fptr);
    fscanf(fptr, "%d %d", &data->row, &data->col);
    skip_comments(fptr);
    fscanf(fptr, "%d", &data->max_gray);
    fgetc(fptr);

    
    data->matrix = allocate_dynamic_matrix(data->row, data->col);
    //read image pixels and store in matrix
    for (i = 0; i < data->row; ++i) {
        for (j = 0; j < data->col; ++j) {
            low = fgetc(fptr);
            data->matrix[i][j] = low;
        }
    }

    zoomed_data->row = 2*(data->row);
    zoomed_data->col = 2*(data->col);
    zoomed_data->max_gray = (data->max_gray);
    zoomed_data->matrix = allocate_dynamic_matrix(zoomed_data->row, zoomed_data->col);

    int pixel=0;
    for(i=0;i<data->row;i++){
        for(j=0;j<data->col;j++){
            pixel = data->matrix[i][j];
            zoomed_data->matrix[2*i][2*j] = pixel;
            zoomed_data->matrix[2*i+1][2*j] = pixel;
            zoomed_data->matrix[2*i][2*j+1] = pixel;
            zoomed_data->matrix[2*i+1][2*j+1] = pixel;
        }
    }
    
    fclose(fptr);
    deallocate_dynamic_matrix(data->matrix, data->row);
}

void write_pgm(const char *filename, PGMdata* data){
    FILE *fptr;
    int low, i, j;

    fptr = fopen(filename,"wb");
    if(fptr==NULL){
        printf("Error opening file...");
        exit(1);
    }

    fprintf(fptr, "P5 \n");
    fprintf(fptr, "%d %d \n", data->row, data->col);
    fprintf(fptr, "%d ",data->max_gray);
  
    for (i = 0; i < data->row; i++) {
        for (j = 0; j < data->col; j++) {
            low = LO(data->matrix[i][j]);
            fwrite(&low,1,1, fptr);
        }
    }
    fclose(fptr);
}


int main(){
    PGMdata pgmdata;
    PGMdata* data = &pgmdata;
    PGMdata zoomed_pgm;
    PGMdata* zoomed_data = &zoomed_pgm;

    const char *filename = "images/lena256.pgm";
    //calculate zoomed in pgm data
    pgm_zoom(filename,data, zoomed_data);

    //write the zoomed in pgm image
    const char *output_filename = "output/lena256_zoomed.pgm";
    write_pgm(output_filename,zoomed_data);
    return 0;
}
