#pragma once
#include<math.h>
#include<algorithm>
#include<vector>
#include<list>
#include<stdio.h>
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include "WatershedSegmenter.h"
using namespace std;
using namespace cv;

class WaterShad {
public:
    WaterShad(void);
    ~WaterShad(void);
    vector<pair<pair<int, int>, vector<cv::Point >> > ultimateContainer;
    vector<int> indexes;

    CvSeq* getContoursequence(Mat& objects) {

        IplImage* imageOb = &IplImage(objects);
        CvMemStorage* storage = cvCreateMemStorage(0);
        CvSeq* seq = 0;
        cvFindContours(imageOb, storage, &seq, sizeof (CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        return seq;
    }

    int minLocation(vector<cv::Point> contains, char c) {
        int min = 50000;
        int max = -1;
        for (int i = 0; i < contains.size(); i++) {
            if (c == 'm') {
                if (min > contains.at(i).x)
                    min = contains.at(i).x;
            } else
                if (max < contains.at(i).x)
                max = contains.at(i).x;
        }
        if (c == 'm')
            return min;
        else
            return max;
    }

    void drawContours(Mat Images) {
        ultimateContainer.reserve(2500);
        ultimateContainer.resize(2500);
        Mat mask = (Images == 255);
        Mat getTemp;
        CvMemStorage* memory = cvCreateMemStorage(0);
        vector<vector<cv::Point> > contoursPoint;
        vector<cv::Point> contourP;
        getTemp.create(mask.rows, mask.cols, CV_8UC1);
        mask.copyTo(getTemp);
        //cv::threshold(mask,mask,150,255,CV_THRESH_BINARY);
        //imshow("tempShow",getTemp);
        //waitKey();
        //contours.clear();
        CvSeq* sequence = getContoursequence(mask);
        vector<double> areas;
        CvSeq* maxSequence = NULL;
        vector<CvSeq*> seqHolder;
        double maxArea = 0;
        for (unsigned int ai = 0; sequence != 0/*ai < contours.size()*/; sequence = sequence->h_next, ai++) {
            //Mat _pts(contours.at(ai));
            //Scalar mp = mean(_pts);
            //areas[ai] = contourArea(_pts);
            //bias score according to distance from center face
            contourP.resize(sequence->total);
            printf("The number of Points %d\n", sequence->total);
            for (int i1 = 0; i1 < sequence->total; i1++) {
                CvPoint* cvPoints = (CvPoint*) cvGetSeqElem(sequence, i1);
                contourP[i1] = Point(cvPoints->x, cvPoints->y);
            }
            contoursPoint.push_back(contourP);
            contourP.clear();
            areas.push_back(cvContourArea(sequence)); //contourArea(Mat(contours[ai]))/* * bias.at<double>(mp.val[1],mp.val[0])*/;
            seqHolder.push_back(sequence);
            if (maxArea < areas.at(ai)) {
                maxArea = areas.at(ai);
                maxSequence = sequence;
            }
            //cvDrawContours(&IplImage(mask),sequence,Scalar(255),Scalar(255),-1,CV_FILLED);
            printf("area Size %lf\n", areas.at(ai));
        }

        //find largest connected component
        double max;
        Point maxLoc, minLoc;
        minMaxLoc(Mat(areas), 0, &max, 0, &maxLoc);
        //draw back on mask
        mask.setTo(Scalar(0)); //clear...
        //cvDrawContours(&mask,sequence,Scalar(255),Scalar(255),-1,CV_FILLED);
        try {
            //	printf("%d\n",maxSequence->total);
            //ultimateContainer.clear();
            int k = 0;
            for (int i = 0; i < seqHolder.size(); i++) {
                int min = minLocation(contoursPoint.at(i), 'm');
                int max = minLocation(contoursPoint.at(i), 'M');
                int mid = (min + max) / 2;
                for (k = min; k <= max; k++) {
                    if (ultimateContainer.at(k).second.size() != 0) {
                        if (ultimateContainer.at(k).first.first < mid && ultimateContainer.at(k).first.second > mid) {
                            mid = k;
                            k = -1;
                            break;
                        }
                    }
                }
                if (k != -1) {
                    ultimateContainer.at(mid).first.first = min;
                    ultimateContainer.at(mid).first.second = max;
                }
                for (int j = 0; j < contoursPoint.at(i).size(); j++)
                    ultimateContainer.at(mid).second.push_back(contoursPoint.at(i).at(j));
                if (k != -1)
                    indexes.push_back(mid);
                //cvDrawContours(&IplImage(mask),seqHolder.at(i),Scalar(255),Scalar(0),0);
            }
            for (int i = 0; i < indexes.size(); i++) {
                for (int j = 0; j < ultimateContainer.at(indexes.at(i)).second.size(); j++)
                    cvDrawContours(&IplImage(mask), seqHolder.at(i), Scalar(255), Scalar(0), 0);
                imshow("tempShow", mask);
                cvWaitKey();
            }
            printf("number of Characters %d", indexes.size());
            //imshow("tempShow",mask);
            //waitKey();
            //cv::drawContours(mask,contoursPoint,maxLoc.y,Scalar(255),CV_FILLED);
        } catch (Exception ex) {
            printf("%s\n", ex.err);
        }
    }

    int waterShade(void) {
		
			 //  cv::Mat image = cv::imread("D:/Transfer_research/Blind_Navigation/ImageInput/font1.jpg");
        cv::Mat image = cv::imread("D:/font1.png");
        cv::Mat binary; // = cv::imread(argv[2], 0);
        cv::cvtColor(image, binary, CV_BGR2GRAY);
        cv::threshold(binary, binary, 100, 255, THRESH_BINARY_INV);

        imshow("originalimage", image);
        imshow("originalbinary", binary);

        // Eliminate noise and smaller objects
        cv::Mat fg;
        std::auto_ptr<int> memory_manager(new int);
        binary.copyTo(fg);
        //cv::dilate(binary,fg,cv::Mat(),cv::Point(-1,-1),2);
        imshow("fg", fg);

        // Identify image pixels without objects
        cv::Mat bg;
        //binary.copyTo(bg);
        cv::dilate(binary, bg, cv::Mat(), cv::Point(-1, -1), 2);
        cv::threshold(bg, bg, 100, 128, cv::THRESH_BINARY_INV);
        imshow("bg", bg);

        // Create markers image
        cv::Mat markers(binary.size(), CV_8U, cv::Scalar(0));
        markers = fg + bg;
        imshow("markers", markers);
        // Create watershed segmentation object
        WatershedSegmenter segmenter;
        segmenter.setMarkers(markers);
        cv::Mat result = segmenter.process(image);
        imshow("final_result", result);
        drawContours(result);

        cv::waitKey(0);

        return 0;
    }


};

