#include <iostream>
#include <cstdint>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <vector>

using namespace std;

#define COLOR_PAINT {0x000000, 0x7F7F7F, 0x880015, 0xED1C24, 0xFF7F27, 0xFFF200, 0x22B14C, 0x00A2E8, 0x3F48CC, 0xA349A4, 0xFFFFFF, 0xC3C3C3, 0xB97A57, 0xFFAEC9, 0xFFC90E, 0xEFE4B0, 0xB5E61D, 0x99D9EA, 0x7092BE, 0xC8BFE7}
#define COLORS {0x000000, 0x666666, 0x004FCD, 0xFFFFFF, 0xAAAAAA, 0x25C9FF, 0x027420, 0x980000, 0x964112, 0x662A25, 0xFF0014, 0xFF7828, 0xAF7018, 0x9A004F, 0xCB5A57, 0xFFC227, 0xFF0090, 0xFEB0A6}
#define TAILLE 18


int colors[] = COLORS;

#pragma pack(push, 1)
struct BMPHeader{
    uint16_t fileType;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offsetData; //adress of the byte where the pixel array is
};

struct DIBHeader{
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t nbColorPlanes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t imgSize;
    int32_t xPixelsPerMeter; 
    int32_t yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
};
#pragma pack(pop)


struct Pixel{
    uint8_t b, g, r;
};


struct Pixel4{
    uint8_t b, g, r, a;
};


void swap(Pixel *a1, Pixel* a2);
void convertToGray(Pixel * a);
string* split(string s, char split);
int puissance(int a, int b);
void converToGartic(Pixel* pixel);
void converToGartic(Pixel4* pixel);
int minPos(int arr[], int taille);

int main(int argc, char* argv[]){

    //Checking the command line arguments
    if(argc == 1){
        cout << "Please Enter the path of the file!";
        return 1;
    }
    else if (argc > 2){
        cout << "Too many arguments!" ;
        return 1;
    }

    string fileName = *(argv + 1);
    cout << "filePath is: " << fileName << endl;

    ifstream img(fileName, ios::binary);

    //Cheking if the filePath is correct
    if(!img){
        cout << "Can't open the file: " << fileName << endl;
        return 1;
    }


    cout << "File opened succesfully" << endl;
    
    //stocking the buffer into two separate entities
    BMPHeader bmpHeader;
    DIBHeader dibHeader;

    img.read(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));
    img.read(reinterpret_cast<char*>(&dibHeader), sizeof(dibHeader));


    //Just printing the info of the file
    cout << bmpHeader.fileType << endl;
    printf("%x\n", bmpHeader.fileType);
    cout << dibHeader.size << endl;
    cout << "starting adress of the bitmap : " << bmpHeader.offsetData << endl;
    cout << "nb pixels per bits: " << dibHeader.bitCount << endl;
    cout << "Width: " << dibHeader.width << ", Height: " << dibHeader.height << std::endl;
    cout << "Bits per Pixel: " << dibHeader.bitCount << std::endl;

    int bit = dibHeader.bitCount / 8;

    unsigned char bmpPad[bit];

    for(int i = 0; i < bit; i++){
        bmpPad[i] = 0;
    }

    const int paddingAmount = ((4 - (dibHeader.width * 3) % 4) % 4);

    cout << "paddingAmount: " << paddingAmount << endl;

    int taille = dibHeader.width * dibHeader.height;

    //Making a new File to Filter

    string * temp = split(fileName, '.');
    string newFileName = temp[0] + "Gartic" + temp[1];

    
    ofstream copy1(newFileName, ios::binary);
    
    copy1.write(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));
    copy1.write(reinterpret_cast<char*>(&dibHeader), sizeof(dibHeader));
    
    if(bit == 3){
        Pixel* pixel = new Pixel[taille];

        img.seekg(bmpHeader.offsetData, ios_base::beg);
        if(img.read(reinterpret_cast<char*>(pixel), (taille * bit))){
            cout << "succeed";
        }

        //Setting the position of the stream in the adress of the bitmap
        copy1.seekp(bmpHeader.offsetData, ios::beg);

        //Copying the pixel array and pasting into the bitmap
    
        for(int y = 0; y < dibHeader.height; y++){
            for(int x = 0; x < dibHeader.width; x++){
                    
                //swap(pixel[i], pixel[(row - 1) - i]);
                //convertToGray(&pixel[x + (dibHeader.width * y)]);
                converToGartic(&pixel[x + (dibHeader.width * y)]);
                    
                copy1.write(reinterpret_cast<char*>(&pixel[x + (dibHeader.width * y)]), bit);
            }
            copy1.write(reinterpret_cast<char*>(bmpPad), paddingAmount);
        }
        delete[] pixel;
    }

    else if(bit == 4){
        Pixel4* pixel = new Pixel4[taille];

        img.seekg(bmpHeader.offsetData, ios_base::beg);
        if(img.read(reinterpret_cast<char*>(pixel), (taille * bit))){
            cout << "succeed";
        }

        //Setting the position of the stream in the adress of the bitmap
        copy1.seekp(bmpHeader.offsetData, ios::beg);

        //Copying the pixel array and pasting into the bitmap
    
        for(int y = 0; y < dibHeader.height; y++){
            for(int x = 0; x < dibHeader.width; x++){
                    
                //swap(pixel[i], pixel[(row - 1) - i]);
                //convertToGray(&pixel[x + (dibHeader.width * y)]);
                converToGartic(&pixel[x + (dibHeader.width * y)]);
                    
                copy1.write(reinterpret_cast<char*>(&pixel[x + (dibHeader.width * y)]), bit);
            }
            copy1.write(reinterpret_cast<char*>(bmpPad), paddingAmount);
        }
        delete[] pixel;
    }
    
    copy1.close();
    img.close();

    


    return 0;
}

void swap(Pixel *a1, Pixel *a2){
    Pixel * temp;

    temp->r = a1->r;
    temp->g = a1->g;
    temp->b = a1->b;

    a1->r = a2->r;
    a1->g = a2->g;
    a1->b = a2->b;

    a2->r = temp->r;
    a2->g = temp->g;
    a2->b = temp->b;
}

void converToGartic(Pixel* pixel){
    int r, g, b;
    int sum[TAILLE];

    for(int i = 0; i < TAILLE; i++){
        r = colors[i] / puissance(2, 16);
        g = (colors[i] / puissance(2, 8)) - ((colors[i] / puissance(2, 16)) * puissance(2, 8));
        b = colors[i] - (colors[i] / puissance(2, 8)) * puissance(2, 8);

        sum[i] = sqrt(puissance((pixel->r - r), 2) + puissance((pixel->g - g), 2) + puissance((pixel->b - b), 2));
    }

    int pos = minPos(sum, TAILLE);

    pixel->r = colors[pos] / puissance(2, 16);
    pixel->g = (colors[pos] / puissance(2, 8)) - ((colors[pos] / puissance(2, 16)) * puissance(2, 8));
    pixel->b = colors[pos] - (colors[pos] / puissance(2, 8)) * puissance(2, 8);

}

void converToGartic(Pixel4* pixel){
    int r, g, b, a;
    int sum[20];

    for(int i = 0; i < 20; i++){
        r = colors[i] / puissance(2, 16);
        g = (colors[i] / puissance(2, 8)) - ((colors[i] / puissance(2, 16)) * puissance(2, 8));
        b = colors[i] - (colors[i] / puissance(2, 8)) * puissance(2, 8);

        sum[i] = sqrt(puissance((pixel->r - r), 2) + puissance((pixel->g - g), 2) + puissance((pixel->b - b), 2));
    }

    int pos = minPos(sum, TAILLE);

    pixel->r = colors[pos] / puissance(2, 16);
    pixel->g = (colors[pos] / puissance(2, 8)) - ((colors[pos] / puissance(2, 16)) * puissance(2, 8));
    pixel->b = colors[pos] - (colors[pos] / puissance(2, 8)) * puissance(2, 8);
    pixel->a = 255;
}

int puissance(int a, int b){
    int temp = a;
    for(int i = 1; i < b; i++){
        a *= temp;
    }
    return a;
}

int minPos(int arr[], int taille){
    int pos = 0;
    int temp = arr[0];

    for(int i = 0; i < taille; i++){
        if(abs(temp) > abs(arr[i])){
            temp = arr[i];
            pos = i;
        }
    }
    return pos;
}

void convertToGray(Pixel * a){
    a->r = (int)(a->r * 0.299) + (int)(a->g * 0.587) + (int)(a->b * 0.114);
    a->g = (int)(a->r * 0.299) + (int)(a->g * 0.587) + (int)(a->b * 0.114);
    a->b = (int)(a->r * 0.299) + (int)(a->g * 0.587) + (int)(a->b * 0.114);
}

string* split(string s, char split){
    string* temp = new string[2];
    int posSplit = -1;
    
    for(int i = 0; i < s.size(); i++){
        if(s[i] == split){
            posSplit = i;
        }
    }
    if(posSplit == -1){
        cout << "Couldn't find the split in the split function";
        exit;
    }

    for(int i = 0; i < posSplit; i++){
        temp[0] += s[i];
    }

    for(int i = posSplit; i < s.size(); i++){
        temp[1] += s[i];
    }

    return temp;
}
