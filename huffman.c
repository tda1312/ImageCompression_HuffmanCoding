#include<stdio.h>
#include<stdlib.h>

int main(){
    int i, j;
    char filename[] = "lena_gray.bmp";
    int data, offset, bpp, width, height, temp;
    long imgsize, imgdataoff;
    int** image;

    FILE* image_file;

    image_file = fopen(filename, "rb");
    if(image_file == NULL){
        printf("Error opening file! Please try again.");
        exit(1);
    } else{
        // Get size of BMP file
        // offset at 2 contains size of BMP file
        offset = 2;
        fseek(image_file, offset, SEEK_SET);
        fread(&imgsize, 4, 1, image_file);

        // Offset at 10 is the start of the pixel array
        // (according to BMP header)
        offset = 10;
        fseek(image_file, offset, SEEK_SET);
        fread(&imgdataoff, 4, 1, image_file);

        // Get width and height of the image at offset 18 and 22, respectively
        // each of 4 bytes
        offset = 18;
        fseek(image_file, offset, SEEK_SET);
        fread(&width, 4, 1, image_file);

        offset = 22;
        fseek(image_file, offset, SEEK_SET);
        fread(&height, 4, 1, image_file);

        // get number of bits per pixel
        offset = 2;
        fseek(image_file, offset, SEEK_CUR);
        fread(&bpp, 2, 1, image_file);

        // set offset to the start of the pixel data
        fseek(image_file, imgdataoff, SEEK_SET);

        // allocate space for Image array
        image = (int**)malloc(height * sizeof(int*));
        for(i = 0; i < height; i++){
            image[i] = (int*)malloc(width* sizeof(int));
        }

        // number of bytes in the Image array
        int num_bytes = (imgsize - imgdataoff) / 3;

        
    }
}