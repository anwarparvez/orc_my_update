#pragma once
#include<opencv/cv.h>
#include<iostream>
#include<vector>
#include<set>
#include<algorithm>
#include<math.h>
#include<opencv/highgui.h>
#include<stack>
using namespace std;
#define BLOCKSIZE 50
#define BLOCKMAX 150
#define DFSWHITE -1
#define DFSBLACK 1

/*Region of interest Extractor   */
class ROIExtractor {
public:

    typedef pair<int, int> pairing;
    typedef vector<pairing> nodes;

    struct setStruct {
        int X1, X2;
        int Y1, Y2;
        CvPoint pp;
        int oddEvenX;
        int oddEvenY;
        nodes Node;
        int degree;

        setStruct(CvPoint p) : pp(p), degree(0) {
            //this->X1 = this->X2 = this->Y1 = this->Y2 = 0;		
            this->X1 = 0; //400;
            this->Y1 = 0; //600;
        }

        int dataFill(CvPoint p1) {
            int distX = abs(this->pp.x - p1.x);
            int distY = abs(this->pp.y - p1.y);
            printf("distX %d distY %d\n", distX, distY);
            int meetX = 1;
            int meetY = 1;
            if (distX > 125 && distX <= 240 && distY < 25 && this->X1 < distX) {
                this->X1 = distX;
                meetY = 0;
            } else if (distY > 330 && distY < 450 && distX < 25 && this->Y1 < distY) {
                this->Y1 = distY;
                meetX = 0;
            } else if (distY > 330 && distY < 450 && distX > 125 && distX <= 240) {
                this->Y1 = distY;
                this->X1 = distX;
            } else {
                meetY = 0;
                meetX = 0;
            }
            if (meetX == 1 && meetY == 1) {
                printf("Got the hello out of here\n");
                return 2;
            }
            if (meetX == 1)
                return 1;
            if (meetY == 1)
                return 1;

            return 0;
        }

    };
    int dfs_low[BLOCKMAX];
    int dfs_number[BLOCKMAX];
    int dfs_parent[BLOCKMAX];
    int dfs_index;
    int visited[200][200];
    typedef vector<int> indexing;
    vector<indexing> indexHoldering;
    vector<nodes> graph;
    set<int> dfs_set;
    int cycleIndex;
    stack<pair<int, int > > dfs_stack;

    bool isSameline(CvPoint p1, CvPoint p2, vector<CvPoint*> &lineContainer);
    void mergeSet(int X, int Y, vector<pair<int, pair<int, setStruct > > > &contains, CvPoint p1, CvPoint p2);
    int findSet(int X, vector<pair<int, pair<int, setStruct > > > &contains);
    float lineLenght(CvPoint* p);
    void initGraph(void);
    void SSC(int node);
    vector<vector<pair<CvPoint, int >> > imageROIExtractor(vector<CvPoint*>&lineContainer, vector<CvPoint> cornerPoints, IplImage *img);



    bool find_if_Contain(vector<pairing>::iterator first, vector<pairing>::iterator last, int val);
};
