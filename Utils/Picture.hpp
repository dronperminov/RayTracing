#pragma once

#include <vector>
#include <fstream>
#include "../Geometry/Vec.hpp"

class Picture {
    struct Pixel {
        unsigned char b, g, r;
    };

    int width;
    int height;
    std::vector<Pixel> pixels; // пиксели

    void Read(const std::string& filename);
    int Clamp(double v) const;
public:
    Picture(int width, int height);
    Picture(const std::string& filename);
    Picture(std::istream &is);

    int Width() const;
    int Height() const;

    Vec GetPixel(int x, int y);
    void SetPixel(int x, int y, const Vec& vec);

    void Save(const std::string &filename);
};

Picture::Picture(int width, int height) {
    this->width = width;
    this->height = height;
    this->pixels.resize(width * height);
}

Picture::Picture(const std::string& filename) {
    Read(filename);
}

Picture::Picture(std::istream &is) {
    std::string filename;
    is >> filename;
    Read(filename);
}

int Picture::Width() const {
    return width;
}

int Picture::Height() const {
    return height;
}

Vec Picture::GetPixel(int x, int y) {
    Pixel p = pixels[(height - 1 - y) * width + x];
    return Vec(p.r / 255.0, p.g / 255.0, p.b / 255.0);
}

void Picture::SetPixel(int x, int y, const Vec& vec) {
    Pixel &p = pixels[(height - 1 - y) * width + x];
    p.r = Clamp(vec.x * 255);
    p.g = Clamp(vec.y * 255);
    p.b = Clamp(vec.z * 255);
}

int Picture::Clamp(double v) const {
    if (v < 0)
        return 0;

    if (v > 255)
        return 255;

    return v;
}

void Picture::Read(const std::string& filename) {
    std::ifstream f(filename, std::ios::in | std::ios::binary);

    if (!f)
        throw std::runtime_error("unable to open bmp file");
    
    unsigned char bmpfileheader[14];
    unsigned char bmpinfoheader[40];

    f.read((char *) bmpfileheader, 14);
    f.read((char *) bmpinfoheader, 40);

    unsigned char w0 = bmpinfoheader[4];
    unsigned char w1 = bmpinfoheader[5];
    unsigned char w2 = bmpinfoheader[6];
    unsigned char w3 = bmpinfoheader[7];

    unsigned char h0 = bmpinfoheader[8];
    unsigned char h1 = bmpinfoheader[9];
    unsigned char h2 = bmpinfoheader[10];
    unsigned char h3 = bmpinfoheader[11];

    width = (w3 << 24) | (w2 << 16) | (w1 << 8) | w0;
    height = (h3 << 24) | (h2 << 16) | (h1 << 8) | h0;
    
    int paddedsize = (width*height) * sizeof(Pixel);

    pixels.resize(width * height);
    f.read((char*)pixels.data(), paddedsize);
    f.close();
}

void Picture::Save(const std::string &filename) {
    int paddedsize = (width*height) * sizeof(Pixel);

    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};

    bmpfileheader[ 2] = (unsigned char)(paddedsize    );
    bmpfileheader[ 3] = (unsigned char)(paddedsize>> 8);
    bmpfileheader[ 4] = (unsigned char)(paddedsize>>16);
    bmpfileheader[ 5] = (unsigned char)(paddedsize>>24);

    bmpinfoheader[ 4] = (unsigned char)(width    );
    bmpinfoheader[ 5] = (unsigned char)(width>> 8);
    bmpinfoheader[ 6] = (unsigned char)(width>>16);
    bmpinfoheader[ 7] = (unsigned char)(width>>24);
    bmpinfoheader[ 8] = (unsigned char)(height    );
    bmpinfoheader[ 9] = (unsigned char)(height>> 8);
    bmpinfoheader[10] = (unsigned char)(height>>16);
    bmpinfoheader[11] = (unsigned char)(height>>24);

    std::ofstream out(filename.c_str(), std::ios::out | std::ios::binary);
    out.write((const char*)bmpfileheader, 14);
    out.write((const char*)bmpinfoheader, 40);
    out.write((const char*)pixels.data(), paddedsize);
    out.flush();
    out.close();
}
