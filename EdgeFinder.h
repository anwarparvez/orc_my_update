#pragma once
#include<algorithm>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <iostream> 
#include <vector>
#include <opencv\cv.h>
#include "Morphological.h"
#include "RansacProcessor.h"

using namespace std;
using namespace cv;

class EdgeFinder {
    Mat src;
    IplImage* img;
    IplImage* imgOnwork;
    IplImage* out;
    IplImage* finalOut;
    vector<CvPoint*> verticalLineholder;
    vector<CvPoint*> otherLines;
    int* imageMap;
    bool isVertical;

public:

    EdgeFinder(void);
    ~EdgeFinder(void);
    EdgeFinder(IplImage* imgfile);

    vector<CvPoint*> getVeticalLineHolder() {
        return verticalLineholder;
    }

    vector<CvPoint*> getOtherLineHolder() {
        return otherLines;
    }

    EdgeFinder(Mat &src) {
        this->src = src;

        isVertical = false;
        img = &(IplImage) src;

        imgOnwork = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
        cvCopyImage(img, imgOnwork);
        out = cvCreateImage(cvGetSize(imgOnwork), imgOnwork->depth, imgOnwork->nChannels);
        cvCopyImage(imgOnwork, out);

        finalOut = cvCreateImage(cvGetSize(imgOnwork), imgOnwork->depth, 1);
        cvCvtColor(imgOnwork, finalOut, CV_RGB2GRAY);

    }

    /*filter out gray edge Image
        
     */
    void getSobelDerivatives(Mat & gray, Mat &grad_x, Mat &grad_y) {
        Mat _tmp, _tmp1, gray32f, res, gray3;

        gray.convertTo(gray32f, CV_32FC1, 1.0 / 255.0);

        /*A Gaussian blur (also known as Gaussian smoothing) is 
         * the result of blurring an image by a Gaussian function.*/
        GaussianBlur(gray32f, gray32f, Size(11, 11), 0.75);

        /*OpenCV function Sobel to calculate the derivatives from an image.
            src – input image.
            dst – output image of the same size and the same number of channels as src .
            ddepth –

            output image depth; the following combinations of src.depth() and ddepth are supported:
                    src.depth() = CV_8U, ddepth = -1/CV_16S/CV_32F/CV_64F
                    src.depth() = CV_16U/CV_16S, ddepth = -1/CV_32F/CV_64F
                    src.depth() = CV_32F, ddepth = -1/CV_32F/CV_64F
                    src.depth() = CV_64F, ddepth = -1/CV_64F

            when ddepth=-1, the destination image will have the same depth as the source; in the case of 8-bit input images it will result in truncated derivatives.
            xorder – order of the derivative x.
            yorder – order of the derivative y.
            ksize – size of the extended Sobel kernel; it must be 1, 3, 5, or 7.
            scale – optional scale factor for the computed derivative values; by default, no scaling is applied (see getDerivKernels() for details).
            delta – optional delta value that is added to the results prior to storing them in dst.
            borderType – pixel extrapolation method (see borderInterpolate() for details).
         * http://docs.opencv.org/doc/tutorials/imgproc/imgtrans/sobel_derivatives/sobel_derivatives.html
         */

        /// Generate grad_x
        Sobel(gray32f, _tmp, -1, 1, 0, 3);

        _tmp1 = abs(_tmp);
        _tmp1.copyTo(res, (_tmp1 > 0.15));
        res.convertTo(grad_y, CV_32FC1);


        /// Generate grad_y
        Sobel(gray32f, _tmp, -1, 0, 1, 3);
        _tmp1 = abs(_tmp);
        res.setTo(Scalar(0));
        _tmp1.copyTo(res, (_tmp1 > 0.15));
        res.convertTo(grad_x, CV_32FC1);



        for (int i = 0; i < grad_x.rows; i++) {
            float *temp = grad_x.ptr<float>(i);
            for (int j = 0; j < grad_x.cols; j++)
                temp[j] = fabs(temp[j]);
        }
        return;

    }

    void doNormalize(Mat bImage) {
        /*Parameters:	
            src – input array.
            dst – output array of the same size as src .
            alpha – norm value to normalize to or the lower range boundary in case of the range normalization.
            beta – upper range boundary in case of the range normalization; it is not used for the norm normalization.
            normType – normalization type (see the details below).
            dtype – when negative, the output array has the same type as src; otherwise, it has the same number of channels as src and the depth =CV_MAT_DEPTH(dtype).
            mask – optional operation mask.
         */
        cvNormalize(&(IplImage) bImage, &(IplImage) bImage, 0.0, 255.0, CV_MINMAX);


        cvSmooth(&(IplImage) bImage, &(IplImage) bImage, CV_GAUSSIAN, 3, 3);
        /*
        Smooths the image in one of several ways.
        Parameters:	

        src – The source image
        dst – The destination image
        smoothtype –CV_GAUSSIAN
        param1 – The first parameter of the smoothing operation, the aperture width. Must be a positive odd number (1, 3, 5, ...)
        param2 – The second parameter of the smoothing operation, 
         */

        return;
    }

    int findVerticallines(vector<int> *indexHolder, CvSeq * linez) {

        int ux[2] = {1, 0};
        int u[2];
        int sum = 0;
        for (int i = 0; i < indexHolder->size(); i++) {
            CvPoint* points = (CvPoint*) cvGetSeqElem(linez, indexHolder->at(i));
            u[0] = abs(points[0].x - points[1].x);
            u[1] = abs(points[0].y - points[1].y);
            sum += u[0] * ux[0] + u[1] * ux[1];
        }
        printf("The sum is :%d", sum);
        return sum < indexHolder->size()*10;
    }

    Mat preprocess(Mat &img) {
    }

    CvSeq* getHoughLines(Mat mfinalOut, CvMemStorage* store) {
        //Mat mfinalOut=(Mat)finalOut;
        imshow("Init gray", mfinalOut);

        doNormalize(mfinalOut);
        imshow("normalization and sooth", mfinalOut);
        waitKey();

        Mat grad_x, grad_y, gray1;
        getSobelDerivatives(mfinalOut, grad_x, grad_y);
        imshow("grad_x grayInt1", grad_x);
        imshow("grad_y grayInt", grad_y);
        waitKey();

        cv::add(grad_x, grad_y, gray1);
        gray1.convertTo(gray1, CV_8U, 255);

        printf("getSobelDerivatives");
        finalOut = &((IplImage) gray1);

        Morphological morph;
        finalOut = morph.closing(finalOut, 3, 3, 0, 1);
        printf(" Morphological");
        /*
         * Finds lines in a binary image using a Hough transform.
         * http://opencv.willowgarage.com/documentation/feature_detection.html
         */
        CvSeq* lines = cvHoughLines2(finalOut, store, CV_HOUGH_PROBABILISTIC, 1.0, CV_PI / 180, 100, 40.0, 20.0);
        return lines;
    }

    void doProcess() {
        CvMemStorage* store = cvCreateMemStorage(0);
        CvSeq* lines = getHoughLines((Mat) finalOut, store);
       // CvSeq* lines = linez;

        //draw lines
        for (int i = 0; i < lines->total; i++) {
            CvPoint* line = (CvPoint*) cvGetSeqElem(lines, i);
            cvLine(imgOnwork, line[0], line[1], CV_RGB(255, 0, 0), 3, 8);
        }
        imshow("normalization and sooth", (Mat) img);
        waitKey();

        if (lines == NULL || lines->total < 0) {
            printf("No Line Found Programe is exiting\n");
            exit(0);
        }

        int size = lines->total;

        printf("linez size %d\n", lines->total);
        //cvReleaseImage(&finalOut);

        imageMap = new int[lines->total];
        for (int i = 0; i < lines->total; i++)
            imageMap[i] = 0;

        int trailRunner = 0;
        int colorVal = 255;
        isVertical = false;

        vector<int> *indexHolder;
        RansacProcessor ransecpro;
        while (trailRunner++ < 3) {
            printf("Start Ransac\n");
            indexHolder = ransecpro.initiateVanishing(lines, imageMap);

            if (findVerticallines(indexHolder, lines)) {
                printf("Found the vertical lines\n");
                for (int i = 0; i < indexHolder->size(); i++) {
                    CvPoint* pointtoHold = new CvPoint[2];
                    CvPoint* tempHold = (CvPoint*) cvGetSeqElem(lines, indexHolder->at(i));
                    pointtoHold[0] = tempHold[0];
                    pointtoHold[1] = tempHold[1];
                    verticalLineholder.push_back(pointtoHold);
                    cvLine(imgOnwork, verticalLineholder.at(verticalLineholder.size() - 1)[0], verticalLineholder.at(verticalLineholder.size() - 1)[1], cvScalar(255, 0, colorVal), 1, CV_AA, 0);
                }

                isVertical = true;
            } else {
                for (int i = 0; i < indexHolder->size(); i++) {

                    CvPoint* tempVarPoint = new CvPoint[2];
                    CvPoint* points = (CvPoint*) cvGetSeqElem(lines, indexHolder->at(i));
                    tempVarPoint[0] = points[0];
                    tempVarPoint[1] = points[1];
                    otherLines.push_back(tempVarPoint);

                    if (trailRunner == 1)
                        cvLine(imgOnwork, points[0], points[1], cvScalar(0, 255, 0), 1, CV_AA, 0);
                    else if (trailRunner == 2)
                        cvLine(imgOnwork, points[0], points[1], cvScalar(255, 0, colorVal), 1, CV_AA, 0);
                    else
                        cvLine(imgOnwork, points[0], points[1], cvScalar(0, 127, colorVal), 1, CV_AA, 0);



                }
            }
            for (int i = 0; i < indexHolder->size(); i++)
                imageMap[indexHolder->at(i)] = 1;
            cvShowImage("Ransac", imgOnwork);
            cvWaitKey(0);
        }

        cout << "size of other lines " << otherLines.size() << "\n";
        cout << "size of vertical lines " << verticalLineholder.size() << "\n";
        cvReleaseMemStorage(&store);
        cvReleaseImage(&out);

        return;
    }
};
