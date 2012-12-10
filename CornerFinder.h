#pragma once
#include<algorithm>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <iostream> 
#include <vector>
#include <opencv\cv.h>
#include "Voxel.h"
//#include "RansacProcessor.h"

class CornerFinder {
    vector<CvPoint*> verticalLineholder;
    vector<CvPoint*> otherLines;
    Voxel *voxelGrid;
public:

    CornerFinder() {
    }

    ~CornerFinder() {
    }

    vector<CvPoint*> getVeticalLineHolder() {
        return verticalLineholder;
    }

    vector<CvPoint*> getOtherLineHolder() {
        return otherLines;
    }

    Voxel * getVoxelGrid() {
        return voxelGrid;

    }

    void setVeticalLineHolder(vector<CvPoint*> lineholder) {
        verticalLineholder = lineholder;
    }

    void setOtherLineHolder(vector<CvPoint*> lineholder) {
        otherLines = lineholder;
    }

    int getCornerpoints(IplImage* img) {
        IplImage* cornerOut = NULL;
        IplImage* imgOnwork = NULL;
        IplImage* R = NULL;
        IplImage* G = NULL;
        IplImage* B = NULL;
        IplImage* R2 = NULL;
        IplImage* G2 = NULL;
        IplImage* B2 = NULL;
        IplImage* sourceFrame = NULL;
        CvScalar harris;

        //cvNamedWindow("SourceImage");
       /* if (img->height > 500 || img->width > 650) {

            imgOnwork = cvCreateImage(cvSize(img->width / 2, img->height / 2), img->depth, img->nChannels);
            cvPyrDown(img, imgOnwork); //resize

        } else {*/
            imgOnwork = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
            cvCopyImage(img, imgOnwork);
       // }
        printf("image depth: %d\n", imgOnwork->depth);

        IplImage* result = cvCreateImage(cvGetSize(imgOnwork), imgOnwork->depth, imgOnwork->nChannels);
        IplImage* result2 = cvCreateImage(cvGetSize(imgOnwork), imgOnwork->depth, imgOnwork->nChannels);
        R = cvCreateImage(cvGetSize(imgOnwork), imgOnwork->depth, 1);
        G = cvCreateImage(cvGetSize(imgOnwork), imgOnwork->depth, 1);
        B = cvCreateImage(cvGetSize(imgOnwork), imgOnwork->depth, 1);
        R2 = cvCreateImage(cvGetSize(imgOnwork), imgOnwork->depth, 1);
        G2 = cvCreateImage(cvGetSize(imgOnwork), imgOnwork->depth, 1);
        B2 = cvCreateImage(cvGetSize(imgOnwork), imgOnwork->depth, 1);



        cornerOut = cvCreateImage(cvGetSize(imgOnwork), imgOnwork->depth, 1);
        sourceFrame = cvCreateImage(cvGetSize(imgOnwork), imgOnwork->depth, imgOnwork->nChannels);
        cvCopyImage(imgOnwork, sourceFrame);
        cvSplit(imgOnwork, B, G, R, NULL);
        B2 = laplace(B);
        G2 = laplace(G);
        R2 = laplace(R);
        cvMerge(R2, G2, B2, NULL, result);
        cvAdd(sourceFrame, result, result2, NULL);
        cvCvtColor(result2, cornerOut, CV_RGB2GRAY);

        Mat gray1, gray2, gray3;
        vector<Mat> matCollector;
        getEdges((Mat) cornerOut, gray1, gray2);
        printf("Hello got it\n");

        cv::add(gray1, gray2, gray3);

        gray3.convertTo(gray1, CV_32FC1);

        printf("%d\n", gray1.depth());
        cornerOut = &((IplImage) gray1);

        printf("this is point is okay\n");
        CvMat* mat = cvCreateMat(sourceFrame->height, sourceFrame->width, CV_32FC1);
        cvSmooth(cornerOut, cornerOut, CV_GAUSSIAN, 11, 11);
        cvCornerHarris(cornerOut, mat, 3);
        printf("number of vertical lines %d\n", verticalLineholder.size());
        vector<CvPoint> pointsCol;
        if (verticalLineholder.size() != 0)
            pointsCol = getFilteredpoints(mat, verticalLineholder);
        else
            pointsCol = getFilteredpoints(mat, otherLines);
        printf("right before the voxel creation %d\n", pointsCol.size());
        voxelGrid = new Voxel(&pointsCol, 25, 25, 0, pointsCol.size(), 2, 10);
        voxelGrid->maxminFinder();
        voxelGrid->gridCreation();
        voxelGrid->histogramGeneration();
        voxelGrid->voxelFiltering();
        printf("size of the filtered corner: %d\n", pointsCol.size());
        printf("size of the listOfPoints %d\n", voxelGrid->listofPoints.size());
        //  sort(voxelGrid->listofPoints.begin(), voxelGrid->listofPoints.end(), sortOut);
        for (int i = 0; i < voxelGrid->listofPoints.size()/*pointsCol.size()*/; i++) {
            printf("point x = %d y = %d\n", voxelGrid->listofPoints.at(i).x, voxelGrid->listofPoints.at(i).y);
            cvCircle(result, cvPoint(voxelGrid->listofPoints.at(i).x, voxelGrid->listofPoints.at(i).y), fRound(3), cvScalar(25, 0, 255), 1);
        }

        cout << "seize of the other lines " << otherLines.size() << "\n";
        verticalLineholder.insert(verticalLineholder.end(), otherLines.begin(), otherLines.end());

        printf("Total lines in the system are : %d\n", verticalLineholder.size());
        cvNamedWindow("SharpedImage");
        cvShowImage("SharpedImage", result);

        cvWaitKey(0);
        cvReleaseImage(&result);
        cvReleaseImage(&result2);

        cvReleaseImage(&R);
        cvReleaseImage(&G);
        cvReleaseImage(&B);
        return 0;

    }
    //! Round float to nearest integer

    inline int fRound(float flt) {
        return (int) floor(flt + 0.5f);
    }

    bool sortOut(CvPoint &p1, CvPoint &p2) {
        return p1.x == p2.x ? p1.y < p2.y : p1.x < p2.x;
    }

    IplImage* laplace(IplImage* channel) {

        CvSize size = cvSize(channel->width, channel->height);

        IplImage* temp = cvCreateImage(size, channel->depth, 1);

        IplImage* lapl = cvCreateImage(size, channel->depth, 1);

        int width = size.width;

        int height = size.height;

        cvConvertScale(channel, temp, 1.0);

        CvMat* ker = cvCreateMat(3, 3, CV_32FC1);

        cvSet(ker, cvScalarAll(-1.0));

        cvSet2D(ker, 1, 1, cvScalarAll(8.0));

        printf("this is been executed\n");

        cvFilter2D(temp, lapl, ker);

        cvReleaseMat(&ker);

        double maxv = -1.79769e-308;

        float maxFloat = 1.79769e+308;

        double minv = maxFloat;

        cvMinMaxLoc(lapl, &minv, &maxv);


        for (int i = 0; i < width * height; i++) {

            double lap_val = cvGet1D(lapl, i).val[0];

            int v = (int) ((255.0 * lap_val / maxv) + 0.5); // this calculation does nothing particularly

            cvSet1D(channel, i, cvScalarAll(v));
        }

        maxv = 0.0;

        cvMinMaxLoc(channel, &minv, &maxv);

        for (int i = 0; i < width * height; i++) {

            double val = cvGet1D(channel, i).val[0];
            int v = (int) ((255.0 * val / maxv) + 0.5);
            cvSet1D(channel, i, cvScalarAll(v));
        }

        cvReleaseImage(&temp);
        cvReleaseImage(&lapl);
        cvReleaseMat(&ker);

        return channel;
    } // end of function

    void getEdges(Mat& gray, Mat& grayInt, Mat& grayInt1) {
        Mat _tmp, _tmp1, gray32f, res;

        gray.convertTo(gray32f, CV_32FC1, 1.0 / 255.0);

        GaussianBlur(gray32f, gray32f, Size(11, 11), 0.75);

        Sobel(gray32f, _tmp, -1, 1, 0, 3); //sobel for dx
        //Sobel(gray32f,_tmp1,-1,1,0,3,-1.0);	//sobel for -dx
        //_tmp = abs(_tmp) + abs(_tmp1);
        //_tmp.copyTo(_tmp,(_tmp > 0.0));
        //_tmp1.copyTo(_tmp1,(_tmp1 > 0.0));
        _tmp1 = abs(_tmp); // + (_tmp1 == 0.0);
        _tmp1.copyTo(res, (_tmp1 > 0.15)); //0.2
        //res = -res + 1.0;

        imshow("tmp", res);

        double maxVal, minVal;
        minMaxLoc(_tmp, &minVal, &maxVal);

        cv::log(/*(_tmp - minVal) / (maxVal - minVal)*/res, _tmp);
        _tmp = -_tmp * 0.17;
        //_tmp.convertTo(grayInt1,CV_32SC1);
        res.convertTo(grayInt1, CV_32FC1);
        Sobel(gray32f, _tmp, -1, 0, 1, 3); //sobel for dy
        //Sobel(gray32f,_tmp1,-1,0,2,3,-1.0);	//sobel for -dy
        //_tmp = abs(_tmp) + abs(_tmp1);
        //_tmp = (_tmp + _tmp1 + 2.0) / 4.0;
        _tmp1 = abs(_tmp);
        res.setTo(Scalar(0));
        _tmp1.copyTo(res, (_tmp1 > 0.15)); //0.2
        //res = -res+1.0;
        imshow("tmp1", res);
        waitKey();
        minMaxLoc(_tmp, &minVal, &maxVal);
        cv::log(/*(_tmp - minVal) / (maxVal - minVal)*/res, _tmp);

        _tmp = -_tmp * 0.17;
        //_tmp.convertTo(grayInt,CV_32SC1);
        res.convertTo(grayInt, CV_32FC1);

        for (int i = 0; i < grayInt.rows; i++) {
            float *temp = grayInt.ptr<float>(i);
            for (int j = 0; j < grayInt.cols; j++)
                temp[j] = fabs(temp[j]);
        }

    }

    vector<CvPoint> getFilteredpoints(CvMat* mat, vector<CvPoint*> verticalLineholder) {

        CvScalar harris;
        vector<CvPoint> pointHolder;
        int counter = 0;
        printf("seize of the vertiacal line Holder %d\n", verticalLineholder.size());
        printf("No of Rows %d\n", mat->rows);
        printf("No of Cols %d\n", mat->cols);
        for (int i = 0; i < mat->rows; i++)
            for (int j = 0; j < mat->cols; j++) {
                harris = cvGet2D(mat, i, j);
                if (harris.val[0] > 10e-6) {

                    counter++;
                    if (isVerticalline(verticalLineholder, cvPoint(j, i)))
                        pointHolder.push_back(cvPoint(j, i));
                }
            }
        printf("No of times entered the system %d\n", pointHolder.size());
        return pointHolder;
    }

    bool isVerticalline(vector<CvPoint*> verticalLineholder, CvPoint points) {

        double distance1, distance2;

        for (int i = 0; i < verticalLineholder.size(); i++) {
            CvPoint* pointsget = verticalLineholder.at(i);
            if (getLineLenght(pointsget[0], pointsget[1]))
                continue;
            double val1 = (double) ((pointsget[0].x - points.x)*(pointsget[0].x - points.x) + (pointsget[0].y - points.y)*(pointsget[0].y - points.y));
            double val2 = (double) ((pointsget[1].x - points.x)*(pointsget[1].x - points.x) + (pointsget[1].y - points.y)*(pointsget[1].y - points.y));
            //printf("val1 = %lf val2 = %lf\n",val1,val2);
            distance1 = sqrt(val1);
            distance2 = sqrt(val2);
            //printf("%lf %lf\n",distance1,distance2);
            if (distance1 < 20 || distance2 < 20)
                return true;

        }
        return false;
    }

    bool getLineLenght(CvPoint p1, CvPoint p2) {
        return sqrt((double) ((p1.x - p2.x)*(p1.x - p2.x)+(p1.y - p2.y)*(p1.y - p2.y))) < 200;
    }
};


