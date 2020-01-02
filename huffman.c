#include<stdio.h>
#include<stdlib.h>

// calculate length of codeword
int cal_codeLen(char* code){
    int l = 0;
    while(*(code + l) != '\0'){
        l++;
    }
    
    return l;
}

// concatenate the codewords
void concat_code(char* str, char* code_parent, char add){
    int i = 0 ;
    while(*(code_parent + i) != '\0'){
        *(str + i) = *(code_parent + i);
        i++;
    }

    if(add != '2'){
        str[i] = add;
        str[i + 1] = '\0';
    } else{
        str[i] = '\0';
    }
}

// find fibonacci number
int find_fibo(int n){
    if(n <= 1){
        return n;
    }

    return find_fibo(n - 1) + find_fibo(n - 2);
}

int main(int argc, char** argv){
    int i, j;
    //char filename[] = "lena_gray.bmp";
    int data = 0, offset, bpp = 0, width, height, temp = 0;
    long imgsize = 0, imgdataoff = 0;
    int** image;

    // get input image filename
    printf("------------------Welcome to Huffman coding for Image Compression Program------------------\n\n\n");

    // read input image file
    FILE* image_file;
    image_file = fopen(argv[1], "rb");

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
        fseek(image_file, 18, SEEK_SET);
        fread(&width, 4, 1, image_file);

        offset = 22;
        //fseek(image_file, offset, SEEK_SET);
        fread(&height, 4, 1, image_file);

        // get number of bits per pixel
        offset = 2;
        fseek(image_file, 2, SEEK_CUR);
        fread(&bpp, 2, 1, image_file);

        // set offset to the start of the pixel data
        fseek(image_file, imgdataoff, SEEK_SET);

        // allocate space for Image array
        image = (int**)malloc(height * sizeof(int*));
        for(i = 0; i < height; i++){
            image[i] = (int*)malloc(width * sizeof(int));
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
        for(j = 0; j < width; j++){
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

    // define structure PixFreq
    typedef struct PixFreq{
        int pixel, larrloc, rarrloc;
        float freq;
        struct PixFreq *left, *right;
        char code[max_codelen];
    } PixFreq;

    // define structure HuffCode
    typedef struct HuffCode{
        int pixel, arr_location;
        float freq;
    } HuffCode;

    // declare structs
    PixFreq* pixFreq;
    HuffCode* huffCodes;
    int total_nodes = 2 * nodes - 1;
    pixFreq = (PixFreq*)malloc(sizeof(PixFreq) * total_nodes);
    huffCodes = (HuffCode*)malloc(sizeof(HuffCode) * nodes);

    // initialize
    j = 0;
    int total_pix = height * width;
    float temp_prob;
    for(i = 0; i < 256; i++){
        if(histogram[i] != 0){
            // pixel intensity value
            huffCodes[j].pixel = i;
            pixFreq[j].pixel = i;

            // location of the node in the pixfreq array
            huffCodes[j].arr_location = j;

            // histogram
            temp_prob = (float)histogram[i] / (float)total_pix;
            pixFreq[j].freq = temp_prob;
            huffCodes[j].freq = temp_prob;

            // declare the children of leaf node
            pixFreq[j].left = NULL;
            pixFreq[j].right = NULL;

            // initialize the codeword as EOL
            pixFreq[j].code[0] = '\0';
            j++;
        }
    }

    // sort the histogram
    HuffCode temp_huff;
    for(i = 0; i < nodes; i++){
        for(j = i + 1; j < nodes; j++){
            if(huffCodes[i].freq < huffCodes[j].freq){
                temp_huff = huffCodes[i];
                huffCodes[i] = huffCodes[j];
                huffCodes[j] = temp_huff;
            }
        }
    }

    // build Huffman Tree
    float sum_prob;
    int sum_pixel, n = 0, k = 0, node_next;
    node_next = nodes;

    // total number of nodes in Huffman Tree = 2 * nodes - 1
    while(n < (nodes - 1)){
        // add the 2 lowest probabilities
        sum_prob = huffCodes[nodes - n - 1].freq + huffCodes[nodes - n - 2].freq;
        sum_pixel = huffCodes[nodes - n - 1].pixel + huffCodes[nodes - n - 2].pixel;

        // append to the pixFreq Array
        pixFreq[node_next].pixel = sum_pixel;
        pixFreq[node_next].freq = sum_prob;
        pixFreq[node_next].left = &pixFreq[huffCodes[nodes - n - 2].arr_location];
        pixFreq[node_next].right = &pixFreq[huffCodes[nodes - n - 1].arr_location];
        pixFreq[node_next].code[0] = '\0';
        i = 0;

        // sort and update the huffCodes Array
        // get new position of the combined code
        while(sum_prob <= huffCodes[i].freq){
            i++;
        }

        // insert the new node to the huffCodes Array
        for(k = nodes; k >= 0; k--){
            if(k == i){
                huffCodes[k].pixel = sum_pixel;
                huffCodes[k].freq = sum_prob;
                huffCodes[k].arr_location = node_next;
            } else if(k > i){
                // shift the nodes below the new node by 1
                // insert the new node at the updated position k
                huffCodes[k] = huffCodes[k - 1];
            }
        }
        n += 1;
        node_next += 1;
    }

    // assign code through backtracking
    char left, right;
    int index;
    left = '0';
    right = '1';
    for(i = total_nodes - 1; i >= nodes; i--){
        if(pixFreq[i].left != NULL){
            concat_code(pixFreq[i].left->code, pixFreq[i].code, left);
        }
        if(pixFreq[i].right != NULL){
            concat_code(pixFreq[i].right->code, pixFreq[i].code, right);
        }
    }

    // encode the image
    int pix_val, l;

    // write the Huffman-encoded image to a text file
    FILE* imageHuff = fopen("encoded_image.txt", "wb");
    for(i = 0; i < height; i++){
        for(j = 0; j < width; j++){
            pix_val = image[i][j];
            for(l = 0; l < nodes; l++){
                if(pix_val == pixFreq[l].pixel){
                    fprintf(imageHuff, "%s", pixFreq[l].code);
                }
            }
        }
    }

    // display Huffman codewords
    printf("Huffman Codes:\n\n");
    printf("Pixel value -> Code\n\n");
    for(i = 0; i < nodes; i++){
        if(snprintf(NULL, 0, "%d", pixFreq[i].pixel) == 2){
            printf("%d -> %s\n", pixFreq[i].pixel, pixFreq[i].code);
        } else{
            printf("%d -> %s\n", pixFreq[i].pixel, pixFreq[i].code);
        }
    }

    // calculate Average Bit Length
    float avg_bitlen = 0;
    for(i = 0; i < nodes; i++){
        avg_bitlen += pixFreq[i].freq * cal_codeLen(pixFreq[i].code);
    }
    printf("Average number of bits: %f", avg_bitlen);
}