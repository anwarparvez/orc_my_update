#pragma once
#include<math.h>
#include<algorithm>
#include<vector>
#include<list>
#include<stdio.h>
#include "opencv\cv.h"
#include "opencv\highgui.h"

class WatershedSegmenter {
public:
    WatershedSegmenter(void);
    ~WatershedSegmenter(void);
    cv::Mat markers;
    void setMarkers(cv::Mat& markerImage) {
        markerImage.convertTo(markers, CV_32S);
    }

    cv::Mat process(cv::Mat &image) {
        cv::watershed(image, markers);
        markers.convertTo(markers, CV_8U);
        return markers;
    }
};

