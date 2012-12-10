#pragma once
#include <opencv/cv.h>
#include<iostream>
#include<opencv/highgui.h>
using namespace cv;

class Morphological {
public:
    Morphological(void);
    ~Morphological(void);
    IplImage* opening(IplImage *image, int rows, int cols, int shape, int iter);
    IplImage* closing(IplImage *image, int rows, int cols, int shape, int iter);
};

