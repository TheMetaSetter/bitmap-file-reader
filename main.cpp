#define _CRT_SECURE_NO_WARNINGS
#pragma pack(1)

#include <bits/stdc++.h>
#include <sstream>

using namespace std;


struct BMP_Header
{
	char signature[2];
	uint32_t Filesize;
	uint16_t Reserved1;
	uint16_t Reserved2;
	uint32_t PiArrayOffset;
};

struct BMP_DIB
{
	uint32_t DIB_size;
	int32_t IMG_Width;
	int32_t IMG_Height;
	uint16_t Color_Planes;
	uint16_t PixelSize;
	uint32_t Compression;
	uint32_t PiArraySize;
	int32_t HorizontalResolution;
	int32_t VerticalResolution;
	int32_t NumOfColor;
	int32_t ImportanColor;
	int32_t PaletteSize; 
};

struct Color
{
	uint8_t blue;
	uint8_t green;
	uint8_t red;
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

string getFileName(int argc, char* argv[])
{
    if (argc == 1) return "default.bmp";
    else if (argc == 2) return (string)argv[1];
    else
    {
        cerr << "Error: Invalid argument !!";
        exit(1);
    }
}

void readBMPFile(string filename, BMP& bmp)
{
    ifstream Fin(filename, ios::binary);
    if (!Fin.is_open())
    {
        cout << "Error: Unable to open file image " << endl;
        exit(1);
    }

    // read header
    Fin.read((char*)&bmp.header, sizeof(bmp.header));

    // read dib
    Fin.read((char*)&bmp.dib.DIB_size, sizeof(bmp.dib.DIB_size));
    Fin.read((char*)&bmp.dib.IMG_Width, sizeof(bmp.dib.IMG_Width));
    Fin.read((char*)&bmp.dib.IMG_Height, sizeof(bmp.dib.IMG_Height));
    Fin.read((char*)&bmp.dib.Color_Planes, sizeof(bmp.dib.Color_Planes));
    Fin.read((char*)&bmp.dib.PixelSize, sizeof(bmp.dib.PixelSize));
    Fin.read((char*)&bmp.dib.Compression, sizeof(bmp.dib.Compression));
    Fin.read((char*)&bmp.dib.PiArraySize, sizeof(bmp.dib.PiArraySize));
    Fin.read((char*)&bmp.dib.HorizontalResolution, sizeof(bmp.dib.HorizontalResolution));
    Fin.read((char*)&bmp.dib.VerticalResolution, sizeof(bmp.dib.VerticalResolution));
    Fin.read((char*)&bmp.dib.NumOfColor, sizeof(bmp.dib.NumOfColor));
    Fin.read((char*)&bmp.dib.ImportanColor, sizeof(bmp.dib.ImportanColor));

    // read color table
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

    // read pixel array
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
    ofstream Fout("palette.txt");

    if (!Fout.is_open())
    {
        cerr << "Error: Unable to open file for writing palette" << endl;
        exit(1);
    }

    if (colortable.CLSize > 0)
    {
        Fout << colortable.CLSize << endl; // Writing the number of colors in the palette

        for (int i = 0; i < colortable.CLSize; i++)
        {
            Fout << "#" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(colortable.color[i].red) << ""
                << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(colortable.color[i].green) << ""
                << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(colortable.color[i].blue) << endl;
        }
    }
    Fout.close();
}

void writePixelArrayToFile (BMP& bmp) 
{
    ofstream fout("pixel.txt");

    if (!fout.is_open()) 
    {
        cerr << "Error: Unable to open file pixel.txt " << endl;
        return;
    }

    fout << bmp.dib.IMG_Width << " x " << bmp.dib.IMG_Height << endl;

    if (bmp.dib.PixelSize == 8) 
    {
        for (int i = 0; i < bmp.pixelarray.Row; i++)
        {
            for (int j = 0; j < bmp.pixelarray.Col; j++)
            {
                fout << static_cast<int>(bmp.pixelarray.indices[i][j]) << " ";
            }
            fout << endl;
        }
    }
    else
    {
        for (int i = 0; i < bmp.pixelarray.Row; i++)
        {
            for (int j = 0; j < bmp.pixelarray.Col; j++) 
            {
                fout << "#" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(bmp.pixelarray.pixel[i][j].red) << ""
                    << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(bmp.pixelarray.pixel[i][j].green) << ""
                    << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(bmp.pixelarray.pixel[i][j].blue) << " ";
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
    string filename = getFileName(argc, argv);
    readBMPFile(filename, bmp);

    cerr << filename << endl;
    cerr << "Bit depth: " << bmp.dib.PixelSize << "-bit" << endl;
    cerr << "Width: " << bmp.dib.IMG_Width << " pixels" << endl;
    cerr << "Height: " << bmp.dib.IMG_Height << " pixels" << endl;
    if (bmp.colortable.CLSize > 0)
    {
        cout << "Color Palette: Yes" << endl;
    }
    else cout << "Color Palette: No" << endl;


    writePaletteToFile(bmp.colortable);
    writePixelArrayToFile(bmp);

    deleteBMP(bmp);

    return 0;
}
