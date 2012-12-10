#include<iostream>
#include<math.h>
#include<stdlib.h>
#include<vector>
#include<set>
#include<map>
#include<cstdlib>
#include<opencv/cv.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;
using namespace cv;
#define INF 500000
bool sortOutPoints(const CvPoint, const CvPoint);

class ConvexHull {
public:
    int convexHull(vector<CvPoint> X, bool onEdge);
    int getCrossProduct(CvPoint, CvPoint, CvPoint);
    int getDotProduct(CvPoint, CvPoint);
    int perpLex(vector<CvPoint> X);

};

