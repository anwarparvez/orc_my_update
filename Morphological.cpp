#include "Morphological.h"

Morphological::Morphological(void) {
}

Morphological::~Morphological(void) {
}

IplImage* Morphological::closing(IplImage *image, int rows, int cols, int shape, int iter) {
    int structure = 0;
    IplImage* dest = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);
    if (shape == 0)
        structure = CV_SHAPE_RECT;
    else if (shape == 1)
        structure = CV_SHAPE_CROSS;
    else
        structure = CV_SHAPE_ELLIPSE;
    IplConvKernel *kernel = cvCreateStructuringElementEx(rows, cols, rows / 2, cols / 2, CV_SHAPE_RECT, NULL);
    cvDilate(image, dest, kernel, iter);
    cvErode(dest, image, kernel, iter);
  //  cvShowImage("tempOpeningShow", image);
   // cvWaitKey(0);
    return image;
}

IplImage* Morphological::opening(IplImage *image, int rows, int cols, int shape, int iter) {

    int structure = 0;
    cvNamedWindow("tempOpeningShow");
    cvShowImage("tempOpeningShow", image);
    cvWaitKey(0);
    IplImage* dest = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);
    if (shape == 0)
        structure = CV_SHAPE_RECT;
    else if (shape == 1)
        structure = CV_SHAPE_CROSS;
    else
        structure = CV_SHAPE_ELLIPSE;
    IplConvKernel *kernel = cvCreateStructuringElementEx(rows, cols, rows / 2, cols / 2, CV_SHAPE_RECT, NULL);

    cvErode(image, dest, kernel, iter);
    cvShowImage("tempOpeningShow", dest);
    cvWaitKey(0);
    cvDilate(dest, image, kernel, iter);
    return image;

}
