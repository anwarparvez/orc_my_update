#include "EdgeFinder.h"

EdgeFinder::EdgeFinder(void) {
    isVertical = false;
}

EdgeFinder::EdgeFinder(IplImage* imgfile) {
    isVertical = false;
    img = imgfile;
    imgOnwork = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
    cvCopyImage(img, imgOnwork);
    out = cvCreateImage(cvGetSize(imgOnwork), imgOnwork->depth, imgOnwork->nChannels);
    cvCopyImage(imgOnwork, out);

    finalOut = cvCreateImage(cvGetSize(imgOnwork), imgOnwork->depth, 1);
    cvCvtColor(imgOnwork, finalOut, CV_RGB2GRAY);

}



EdgeFinder::~EdgeFinder(void) {
}
