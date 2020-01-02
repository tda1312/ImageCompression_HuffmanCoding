#include<stdio.h>
#include<stdlib.h>

// function to find fibonacci number
int find_fibo(int n){
    if(n <= 1){
        return n;
    }
    return find_fibo(n - 1) + find_fibo(n - 2);
}

int main(){
    int i, j;
    char filename[] = "lena_gray.bmp";
    int data, offset, bpp, width, height, temp;
    long imgsize, imgdataoff;
    int** image;

    // read input image file
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

        // read Image file into Image array
        for(i = 0; i < height; i++){
            for(j = 0; j < width; j++){
                fread(&temp, 3, 1, image_file);

                // this case the Image is a 24-bit BMP Image
                temp = temp & 0x0000FF;
                image[i][j] = temp;
            }
        }
    }

    // calculate histogram
    int histogram[256];
    for(i = 0; i < 256; i++){
        histogram[i] = 0;
    }
    for(i = 0; i < height; i++){
        for(j = 0; j < width; i++){
            histogram[image[i][j]] += 1;
        }
    }

    // find non-zero occurences
    int nodes = 0;
    for(i = 0; i < 256; i ++){
        if(histogram[i] != 0){
            nodes += 1;
        }
    }

    // find minimum probability
    float p = 1.0, ptemp;
    for(i = 0; i < 256; i ++){
        ptemp = (histogram[i] / (float)(height * width));
        if(ptemp > 0 && ptemp <= p){
            p = ptemp;
        }
    }

    // calculate max length of codeword
    i = 0;
    while((1 / p) > find_fibo(i)){
        i++;
    }
    int max_codelen = i - 3;
}