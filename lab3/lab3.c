#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
typedef unsigned char BYTE;
struct tagBITMAPFILEHEADER
{
    WORD bfType; //specifies the file type
    DWORD bfSize; //specifies the size in bytes of the bitmap file
    WORD bfReserved1; //reserved; must be 0
    WORD bfReserved2; //reserved; must be 0
    DWORD bfOffBits; //species the offset in bytes from the bitmapfileheader to the bitmap bits
};
struct tagBITMAPINFOHEADER
{
    DWORD biSize; //specifies the number of bytes required by the struct
    LONG biWidth; //specifies width in pixels
    LONG biHeight; //species height in pixels
    WORD biPlanes; //specifies the number of color planes, must be 1
    WORD biBitCount; //specifies the number of bit per pixel
    DWORD biCompression;//spcifies the type of compression
    DWORD biSizeImage; //size of image in bytes
    LONG biXPelsPerMeter; //number of pixels per meter in x axis
    LONG biYPelsPerMeter; //number of pixels per meter in y axis
    DWORD biClrUsed; //number of colors used by th ebitmap
    DWORD biClrImportant; //number of colors that are important
};

void fileRead(struct tagBITMAPFILEHEADER *fileHead, struct tagBITMAPINFOHEADER *fileInfo, FILE *fp)
{
    fread(&(fileHead -> bfType), sizeof(WORD), 1, fp);
    fread(&(fileHead -> bfSize), sizeof(DWORD), 1, fp);
    fread(&(fileHead -> bfReserved1), sizeof(WORD), 1, fp);
    fread(&(fileHead -> bfReserved2), sizeof(WORD), 1, fp);
    fread(&(fileHead -> bfOffBits), sizeof(DWORD), 1, fp);
    fread(fileInfo, sizeof(struct tagBITMAPINFOHEADER), 1, fp);
}

void fileWrite(struct tagBITMAPFILEHEADER *fileHead, struct tagBITMAPINFOHEADER *fileInfo, BYTE array[], DWORD size, FILE *fp)
{
    fwrite(&(fileHead -> bfType), sizeof(WORD), 1, fp);
    fwrite(&(fileHead -> bfSize), sizeof(DWORD), 1, fp);
    fwrite(&(fileHead -> bfReserved1), sizeof(WORD), 1, fp);
    fwrite(&(fileHead -> bfReserved2), sizeof(WORD), 1, fp);
    fwrite(&(fileHead -> bfOffBits), sizeof(DWORD), 1, fp);
    fwrite(fileInfo, sizeof(struct tagBITMAPINFOHEADER), 1, fp);
    fwrite(array, size, 1, fp);
}

void matrixMult(int x, int y, float angle, int* coords) {
    double sine = sin(-1 * angle);
    double cosine = cos(-1 * angle);

    double new_x = cosine * x - sine * y;
    double new_y = sine * x + cosine * y;

    coords[0] = new_x;
    coords[1] = new_y;
}

void rotate(BYTE* input, BYTE* output, LONG width, LONG trueWidth, LONG height, int N, float rad)
{
    for(int i = 0; i < N; ++i)
    {
        if(fork() == 0)
        {
            int *coords = (int*)malloc(2 * sizeof(int));
            for(int y = i * (height / N); y < (y + 1) * (height / N); ++y)
            {
                for(int x = 0; x < width; ++x)
                {
                    matrixMult(x - (.5 * width), y - (.5 * height), rad, coords);
                    int x_r = coords[0] + (.5 * width);
                    int y_r = coords[1] + (.5 * height);
                    if(x_r >= 0 && x_r < width && y_r >= 0 && y_r < height)
                    {
                        output[x*3 + y*trueWidth] = input[x_r*3 + y_r*trueWidth];
                        output[x*3+1 + y*trueWidth] = input[x_r*3+1 + y_r*trueWidth];
                        output[x*3+2 + y*trueWidth] = input[x_r*3+2 + y_r*trueWidth];
                    }
                }
            }
            free(coords);
        }
    }
    while (wait(NULL) > 0);
}

// void singleCoreRotate (BYTE* input, BYTE* output, LONG width, LONG trueWidth, LONG height, int N, float rad)
// {
//     int *coords = (int*)malloc(2 * sizeof(int));
//     for(int y = 0; y < height; ++y)
//     {
//         for(int x = 0; x < width; ++x)
//         {
//             matrixMult(x - (.5 * width), y - (.5 * height), rad, coords);
//             int x_r = coords[0] + (.5 * width);
//             int y_r = coords[1] + (.5 * height);
//             if(x_r >= 0 && x_r < width && y_r >= 0 && y_r < height)
//             {
//                 output[x*3 + y*trueWidth] = input[x_r*3 + y_r*trueWidth];
//                 output[x*3+1 + y*trueWidth] = input[x_r*3+1 + y_r*trueWidth];
//                 output[x*3+2 + y*trueWidth] = input[x_r*3+2 + y_r*trueWidth];
//             }
//         }
//     }
//     free(coords);
// }

int main(int argc, char *argv[])
{
    struct tagBITMAPFILEHEADER *fileHeader = (struct tagBITMAPFILEHEADER*)malloc(sizeof(struct tagBITMAPFILEHEADER));
    struct tagBITMAPINFOHEADER *infoHeader = (struct tagBITMAPINFOHEADER*)malloc(sizeof(struct tagBITMAPINFOHEADER));

    FILE *inputFP = fopen(argv[1], "rb");
    FILE *outputFP = fopen(argv[4], "wb");
    float angle = atof(argv[2]);
    int cores = atof(argv[3]);
    
    fileRead(fileHeader, infoHeader, inputFP);

    LONG pxlWidth = (infoHeader -> biWidth);
    int padding = (4 - ((pxlWidth*3) % 4)) % 4;
    LONG trueWidth = pxlWidth*3 + padding;
    DWORD trueSize = trueWidth * (infoHeader -> biHeight);
    BYTE inputArr[infoHeader -> biSizeImage];
    fread(inputArr, trueSize, 1, inputFP);
    fclose(inputFP);
    BYTE *outputArr = mmap(NULL, trueSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    //delete me
    memset(outputArr, 0, trueSize);

    clock_t start = clock();

    //singleCoreRotate(inputArr, outputArr, pxlWidth, trueWidth, infoHeader -> biHeight, cores, angle);
    rotate(inputArr, outputArr, pxlWidth, trueWidth, (infoHeader -> biHeight), cores, angle);

    clock_t end = clock();

    double time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\nTime spent : \n%2.6lf seconds\n\n", time);

    fileWrite(fileHeader, infoHeader, outputArr, trueSize, outputFP);
    munmap(outputArr, infoHeader->biSizeImage);

    fclose(outputFP);

    return 0;
}