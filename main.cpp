#define _CRT_SECURE_NO_WARNINGS
#pragma pack(1)

#include <bits/stdc++.h>
using namespace std;


struct BMP_Header
{
	char signature[2];
	int32_t Filesize;
	int16_t Reserved1;
	int16_t Reserved2;
	int32_t PiArrayOffset;
};

struct BMP_DIB
{
	int32_t DIB_size;
	int32_t IMG_Width;
	int32_t IMG_Height;
	int16_t Color_Planes;
	int16_t PixelSize;
	int32_t Compression;
	int32_t PiArraySize;
	int32_t HorizontalResolution;
	int32_t VerticalResolution;
	int32_t NumOfColor;
	int32_t ImportanColor;
	int32_t PaletteSize;
};

struct Color
{
	int8_t blue;
	int8_t green;
	int8_t red;
};

struct BMP_ColorTable
{
	Color* color;
	int32_t CLSize;
};

struct BMP_PixelArray
{
	Color** pixel;
	uint8_t** indices;
	int32_t Row;
	int32_t Col;

	int RawByte;
	int LineSize;
	int PaddingSize;
};

struct BMP
{
	BMP_Header header;
	BMP_DIB dib;
	BMP_ColorTable colortable;
	BMP_PixelArray pixelarray;
};

string getFilename(int argc, char* argv[])
{
    if (argc == 1) return "default.bmp";
    else if (argc == 2) return (string)argv[1];
    else
    {
        cerr << " Invalid argument !!";
        exit(1);
    }
}

void readBMPFile(string filename, BMP& bmp)
{
    cerr << filename << endl;
    ifstream Fin(filename, ios::binary);
    if (!Fin.is_open())
    {
        cout << "Error: Unable to open file image " << endl;
        exit(1);
    }

    Fin.read((char*)&bmp.header, sizeof(bmp.header));
    Fin.read((char*)&bmp.dib, sizeof(bmp.dib));
    bmp.dib.PaletteSize = 0;

    if (bmp.dib.PixelSize <= 8)
    {
        bmp.dib.PaletteSize = 1 << bmp.dib.PixelSize;
    }

    if (bmp.dib.PaletteSize > 0)
    {
        bmp.colortable.CLSize = bmp.dib.PaletteSize;
        bmp.colortable.color = new Color[bmp.colortable.CLSize];
        for (int i = 0; i < bmp.colortable.CLSize; i++)
        {
            Fin.read((char*)&bmp.colortable.color[i].blue, sizeof(uint8_t));
            Fin.read((char*)&bmp.colortable.color[i].green, sizeof(uint8_t));
            Fin.read((char*)&bmp.colortable.color[i].red, sizeof(uint8_t));
            if (bmp.dib.DIB_size == 40)
            {
                Fin.seekg(1, ios::cur);
            }
        }
    }

    bmp.pixelarray.Col = bmp.dib.IMG_Width;
    bmp.pixelarray.Row = abs(bmp.dib.IMG_Height);
    bmp.pixelarray.RawByte = ((bmp.dib.PixelSize * bmp.pixelarray.Col + 31) / 32) * 4;
    bmp.pixelarray.LineSize = bmp.pixelarray.RawByte;
    bmp.pixelarray.PaddingSize = bmp.pixelarray.LineSize - bmp.pixelarray.Col * (bmp.dib.PixelSize / 8);
    if (bmp.dib.PixelSize == 8)
    {
        bmp.pixelarray.indices = new uint8_t * [bmp.pixelarray.Row];
        for (int i = 0; i < bmp.pixelarray.Row; i++)
        {
            bmp.pixelarray.indices[i] = new uint8_t[bmp.pixelarray.Col];
            for (int j = 0; j < bmp.pixelarray.Col; j++)
            {
                Fin.read((char*)&bmp.pixelarray.indices[i][j], sizeof(uint8_t));
            }
            Fin.seekg(bmp.pixelarray.PaddingSize, ios::cur);
        }
    }
    else
    {
        bmp.pixelarray.pixel = new Color * [bmp.pixelarray.Row];
        for (int i = 0; i < bmp.pixelarray.Row; i++)
        {
            bmp.pixelarray.pixel[i] = new Color[bmp.pixelarray.Col];
            for (int j = 0; j < bmp.pixelarray.Col; j++)
            {
                Fin.read((char*)&bmp.pixelarray.pixel[i][j].blue, sizeof(uint8_t));
                Fin.read((char*)&bmp.pixelarray.pixel[i][j].green, sizeof(uint8_t));
                Fin.read((char*)&bmp.pixelarray.pixel[i][j].red, sizeof(uint8_t));
            }
            Fin.seekg(bmp.pixelarray.PaddingSize, ios::cur);
        }
    }
    Fin.close();
}

void writePaletteToFile(const BMP_ColorTable& colortable)
{
    //ofstream paletteFile("palette.txt");

    //if (!paletteFile.is_open())
    //{
    //    cerr << "Error: Unable to open file for writing palette" << endl;
    //    exit(1);
    //}

    cout << colortable.CLSize << endl; // Writing the number of colors in the palette

    for (int i = 0; i < colortable.CLSize; i++) 
    {
        cout << static_cast<int>(colortable.color[i].red) << " "
            << static_cast<int>(colortable.color[i].green) << " "
            << static_cast<int>(colortable.color[i].blue) << endl;
    }

    //paletteFile.close();
}

void writePixelArrayToFile(string filename, BMP& bmp) 
{
    ofstream fout(filename);
    if (!fout.is_open()) 
    {
        cout << "Error: Unable to open file " << filename << endl;
        return;
    }

    fout << bmp.pixelarray.Row << " " << bmp.pixelarray.Col << endl;

    if (bmp.dib.PixelSize == 8) {
        for (int i = 0; i < bmp.pixelarray.Row; i++)
        {
            for (int j = 0; j < bmp.pixelarray.Col; j++)
            {
                fout << static_cast<int>(bmp.pixelarray.indices[i][j]) << " ";
            }
            fout << endl;
        }
    }
    else {
        for (int i = 0; i < bmp.pixelarray.Row; i++)
        {
            for (int j = 0; j < bmp.pixelarray.Col; j++) 
            {
                fout << static_cast<int>(bmp.pixelarray.pixel[i][j].red) << " "
                    << static_cast<int>(bmp.pixelarray.pixel[i][j].green) << " "
                    << static_cast<int>(bmp.pixelarray.pixel[i][j].blue) << " ";
            }
            fout << endl;
        }
    }

    fout.close();
}

void deleteBMP(BMP& bmp) 
{
    if (bmp.dib.PaletteSize > 0) 
    {
        delete[] bmp.colortable.color;
    }

    if (bmp.dib.PixelSize == 8) 
    {
        for (int i = 0; i < bmp.pixelarray.Row; i++)
        {
            delete[] bmp.pixelarray.indices[i];
        }
        delete[] bmp.pixelarray.indices;
    }
    else 
    {
        for (int i = 0; i < bmp.pixelarray.Row; i++) 
        {
            delete[] bmp.pixelarray.pixel[i];
        }
        delete[] bmp.pixelarray.pixel;
    }
}


int main(int argc, char* argv[])
{
    BMP bmp;
    string filename = "C:\\Users\\HP\\Desktop\\snail.bmp";
    readBMPFile(filename, bmp);

    cerr << " Done " << endl;
    cout << bmp.dib.PixelSize << endl;
    cout << bmp.dib.IMG_Width << endl;
    cout << bmp.dib.IMG_Height << endl;

    writePaletteToFile(bmp.colortable);

    deleteBMP(bmp);

    return 0;
}
