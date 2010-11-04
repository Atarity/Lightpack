#ifndef RGB_H
#define RGB_H

struct sRGB
{
    unsigned char r;
    unsigned char g;
    unsigned char b;

    sRGB(){
        this->r = 0;
        this->g = 0;
        this->b = 0;
    }

    sRGB(unsigned char r, unsigned char g, unsigned char b){
        this->r = r;
        this->g = g;
        this->b = b;
    }

    unsigned char operator[] (int colorIndex){
        switch(colorIndex){
        case R: return this->r;
        case G: return this->g;
        case B: return this->b;
        }
        return -1;
    }
};

#endif // RGB_H
