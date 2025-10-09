#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

/*
    Changes the image's contrast
*/
void contrast(BYTE array[], double factor, LONG width, LONG trueWidth, LONG height)
{
    double currByte;
    double normalizedVal;
    int x;

    for(int y = 0; y < height; ++y) {
        for(x = 0; x < width; ++x)
        {
            currByte = (array[x + (y * trueWidth)]) / 255.0;
            normalizedVal = (pow(currByte, factor));
            array [x + (y * trueWidth)] = normalizedVal * 255;
        }
    }
};

/*
    Changes the image's saturation
*/
void saturation(BYTE array[], double factor, LONG width, LONG trueWidth, LONG height)
{
    double currByte;
    double normalizedVal;
    double avg;
    double normalizedAvg;
    int pxlCtr = 0;
    int x;

        for(int y = 0; y < height; ++y) {
            pxlCtr = 0;
            for(x = 0; x < width; ++x)
            {
                if(pxlCtr % 3 == 0)
                    normalizedAvg = ((array[x + (y * trueWidth)] + array[x + 1 + (y * trueWidth)] + array[x + 2 + (y * trueWidth)]) / 3.0) / 255.0;
                
                currByte = (array[x + (y * trueWidth)]) / 255.0;
                normalizedVal = currByte + (currByte - normalizedAvg) * factor;
                if(normalizedVal > 1)
                    normalizedVal = 1; 
                array[x + (y * trueWidth)] = normalizedVal * 255;
                ++pxlCtr;
            }
    }
}

/*
    Changes the image's lightness
*/
void lightness(BYTE array[], double factor, LONG width, LONG trueWidth, LONG height)
{
    double currByte;
    double normalizedVal;
    int x;

    for(int y = 0; y < height; ++y) {
        for(x = 0; x < width; ++x)
        {
            currByte = (array[x + (y * trueWidth)]) / 255.0;
            normalizedVal = currByte + factor;

            if(normalizedVal > 1)
                normalizedVal = 1;
            else if(normalizedVal < 0)
                normalizedVal = 0;

            array [x + (y * trueWidth)] = normalizedVal * 255;
        }
    }
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

int main(int argc, char *argv[])
{
    struct tagBITMAPFILEHEADER *fileHeader = (struct tagBITMAPFILEHEADER*)malloc(sizeof(struct tagBITMAPFILEHEADER));
    struct tagBITMAPINFOHEADER *infoHeader = (struct tagBITMAPINFOHEADER*)malloc(sizeof(struct tagBITMAPINFOHEADER));

    FILE *inputFP = fopen(argv[1], "rb");
    FILE *outputFP = fopen(argv[2], "wb");
    char operation[1000];
    strcpy(operation, argv[3]);
    double factor = atof(argv[4]);
    
    fileRead(fileHeader, infoHeader, inputFP);

    LONG byteWidth = (infoHeader -> biWidth) * 3;
    int padding = (4 - ((byteWidth) % 4)) % 4;
    LONG trueWidth = byteWidth + padding;
    DWORD trueSize = trueWidth * (infoHeader -> biHeight);
    BYTE arr[trueSize];
    fread(arr, trueSize, 1, inputFP);

    switch (operation[0])
    {
    case 'c':
        contrast(arr, factor, byteWidth, trueWidth, (infoHeader -> biHeight));
        break;
    
    case 's':
        saturation(arr, factor, byteWidth, trueWidth, (infoHeader -> biHeight));
        break;
    
    case 'l':
        lightness(arr, factor, byteWidth, trueWidth, (infoHeader -> biHeight));
        break;
    
    default:
        printf("\nOperation not recognized.\n");
        break;
    }

    fileWrite(fileHeader, infoHeader, arr, trueSize, outputFP);

    fclose(inputFP);
    fclose(outputFP);
    
    return 0;
}

// Compile and run with these :
// gcc lab2.c -o imgchange
// ./imgchange ./blend-images/jar.bmp ./result-images/jar-sat.bmp saturation .6666