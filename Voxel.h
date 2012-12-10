#pragma once
#include<math.h>
#include<algorithm>
#include<vector>
#include<list>
#include<stdio.h>
#include "opencv\cv.h"
using namespace std;
using namespace cv;

typedef struct {
    int x, y, z;
} pointsGet;

typedef struct {
    //double threshHold;
    pointsGet Center;
    double xlenght, ylenght, zlenght;
    int members;
    list<long> lists;

} cube;
bool compareOut(CvPoint, CvPoint);

class Voxel {
private:
    cube **voxelCube;
    int dx, dy, dz;
    double shiftXYZ[3];
    CvPoint maxMin[2][3];
    vector<CvPoint> *pointCloud;
    long cloudDim;
    int spaceDim;
    long xsize, ysize, zsize;
    double threshHold;
    //typedef vector<double> subCon;
    typedef vector<CvPoint> listPoints;

public:
    listPoints listofPoints;

    Voxel(vector<CvPoint> *pointData, int dx, int dy, int dz = 0, long lenght = 1, int spaceDim = 1, double th = 5) {

        this->cloudDim = lenght;
        this->spaceDim = spaceDim;
        this->pointCloud = pointData;
        maxMin[0][0].x = maxMin[0][0].y = 0;
        maxMin[0][1].x = maxMin[0][1].y = 0;
        maxMin[0][2].x = maxMin[0][2].y = 0;
        maxMin[1][0].x = maxMin[1][0].y = 64000;
        maxMin[1][1].x = maxMin[1][1].y = 64000;
        maxMin[1][2].x = maxMin[1][1].y = 64000;
        this->dx = dx;
        this->dy = dy;
        this->dz = 0;
        this->threshHold = th;
        shiftXYZ[0] = 0;
        shiftXYZ[1] = 0;
        shiftXYZ[2] = 0;
        std::sort(this->pointCloud->begin(), this->pointCloud->end(), compareOut);
    }

    void voxelInitiator(double **);
    void maxminFinder();
    void gridCreation();
    void histogramGeneration();
    void voxelFiltering();
    Voxel(void);
    ~Voxel(void);

};
