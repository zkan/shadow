#ifndef SHADOW_UTILS_H
#define SHADOW_UTILS_H

#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <sstream>
#include <math.h>

using namespace std;

template<class T> class Image {
private:
    IplImage* imgp;

public:
    Image( IplImage* img = 0 ) { imgp = img; }
    ~Image() { imgp = 0; }
    void operator=( IplImage* img ) { imgp = img; }
    inline T* operator[]( const int rowIndx ) {
        return ( ( T* )( imgp->imageData + rowIndx * imgp->widthStep ) );
    }
};

typedef struct{
    unsigned char b, g, r;
} RgbPixel;

typedef struct{
    float b, g, r;
} RgbPixelFloat;

typedef Image<RgbPixel>      RgbImage;
typedef Image<RgbPixelFloat> RgbImageFloat;
typedef Image<unsigned char> BwImage;
typedef Image<float>         BwImageFloat;

const float PI = 3.1415926;

IplImage* subtract_img(IplImage *img, IplImage *img_back, 
                       double threshold, double ap);

float cdf_gaussian_value(float x, float mean, float std);
float pdf_gaussian_value(float x, float mean, float std);
float pdf_exp_value(float x, float V_B, float lambda);
float get_z_score(float x, float mean, float std);
double string_to_double(string str);
vector<string> split(string str, string delimiters);

#endif

