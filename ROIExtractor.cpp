#include "ROIExtractor.h"
#include<math.h>
#include "ConvexHull.h"

void ROIExtractor::initGraph(){
  memset(dfs_low,-1,sizeof(dfs_low));
  memset(dfs_number,-1,sizeof(dfs_number));
  memset(visited,0,sizeof(visited));
  memset(dfs_parent,-1,sizeof(dfs_parent));
  dfs_index = 0;
  graph.reserve(200);
  graph.resize(200);

  return;
}
inline int fRounds(float flt)
{
  return (int) floor(flt+0.5f);
}
void ROIExtractor::SSC(int node){
	dfs_number[node] = dfs_low[node] = dfs_index++;
	dfs_stack.push(make_pair<int,int>(node,1));
	dfs_set.insert(node);
	vector<pairing>::iterator it;
	int index = 1;
	for(it = graph[node].begin(); it != graph[node].end();it++){
		dfs_stack.top().second +=index; 
		if(dfs_number[it->second] == DFSWHITE){
			dfs_parent[it->second] = node;
			SSC(it->second);
		}
		else if(it->second != dfs_parent[node]){
			if(dfs_set.find(it->second) != dfs_set.end()){
				stack<pair<int,int>> tmpStack;
				printf("%d ",it->second);
				indexHoldering[cycleIndex].push_back(it->second);
				while(dfs_stack.top().first != it->second){
					printf("%d ",dfs_stack.top().first);
					indexHoldering[cycleIndex].push_back(dfs_stack.top().first);
					//if(dfs_stack.top().second >= graph[dfs_stack.top().first].size())
					  // dfs_stack.pop();
					//else{
					   tmpStack.push(dfs_stack.top());
					   dfs_stack.pop();
				//	}
				}
				printf("%d\n",dfs_stack.top().first);
				indexHoldering[cycleIndex].push_back(dfs_stack.top().first);
				//if(dfs_stack.top().second >= graph[dfs_stack.top().first].size())
				//		dfs_stack.pop();
				//else{
						tmpStack.push(dfs_stack.top());
						dfs_stack.pop();
				//}
				while(tmpStack.size() != 0){
					dfs_stack.push(tmpStack.top());
					tmpStack.pop();
				}
				cycleIndex++;
			}
		}
		index++;
	}
	dfs_set.erase(dfs_set.find(node));
	if(dfs_stack.top().first == node)
		dfs_stack.pop();
	return;
}

int ROIExtractor::findSet(int X, vector<pair<int,pair<int,setStruct>>> &contains){
	if(X != contains[X].first) findSet(contains[X].first,contains);
	return contains[X].first;
}
bool ROIExtractor::find_if_Contain(vector<pairing>::iterator first,vector<pairing>::iterator last,int val){
  
	for(;first != last; first++){
		if(first->second == val)
			return false;
	}
	return true;
}
void ROIExtractor::mergeSet(int X,int Y, vector<pair<int,pair<int,setStruct>>> &contains,CvPoint p1,CvPoint p2){
   int x = findSet(X,contains);
   int y = findSet(Y,contains);
   if(x == y){
      if(find_if_Contain(contains[X].second.second.Node.begin(),contains[X].second.second.Node.end(),Y)){
				contains[Y].second.second.degree++;
				pairing makePairs = make_pair<int,int>(Y,X);
				contains[Y].second.second.Node.push_back(makePairs);
			    contains[X].second.second.degree++;
		    	pairing makePairs0 = make_pair<int,int>(X,Y);
				contains[X].second.second.Node.push_back(makePairs0);
	   }
	   return;
   }
   int tmpVal;
   if(contains[x].second.first < contains[y].second.first) {
	   if((tmpVal = contains[y].second.second.dataFill(p1)) != 0){
			contains[x].first = y; contains[y].second.first++;
			//contains[x].second.second.pp = contains[y].second.second.pp;
			//tmpVal = contains[Y].second.second.dataFill(contains[X].second.second.pp);
			if(find_if_Contain(contains[X].second.second.Node.begin(),contains[X].second.second.Node.end(),Y)){
				contains[Y].second.second.degree++;
				pairing makePairs = make_pair<int,int>(Y,X);
				contains[Y].second.second.Node.push_back(makePairs);
			    contains[X].second.second.degree++;
		    	pairing makePairs0 = make_pair<int,int>(X,Y);
				contains[X].second.second.Node.push_back(makePairs0);
			}
	   }
   }
   else{
	   if((tmpVal = contains[x].second.second.dataFill(p2)) != 0){
	    	contains[y].first = x; 
		    contains[x].second.first++; 
			//contains[y].second.second.pp = contains[x].second.second.pp;
			//tmpVal = contains[X].second.second.dataFill(contains[Y].second.second.pp);
			if(find_if_Contain(contains[X].second.second.Node.begin(),contains[X].second.second.Node.end(),Y)){
				contains[X].second.second.degree++;
				pairing makePairs = make_pair<int,int>(X,Y);
				contains[X].second.second.Node.push_back(makePairs);
				contains[Y].second.second.degree++;
				pairing makePairs0 = make_pair<int,int>(Y,X);
				contains[Y].second.second.Node.push_back(makePairs0);
			}
	   }
   }  
return;
}
float ROIExtractor::lineLenght(CvPoint *p){
	return sqrt((double)((p[0].x - p[1].x)*(p[0].x - p[1].x) + (p[0].y - p[1].y)*(p[0].y - p[1].y)));
}
bool isOnTheSameline(CvPoint p1, CvPoint p2, vector<CvPoint*> &lineContainer){
	bool track1 = false,track2 = false;
	for(int i =0; i < lineContainer.size(); i++){
		CvPoint* points = lineContainer.at(i);
		//if(lineLenght(points) < 150.0)
		//	continue;
		int blockX = abs(points[0].x - points[1].x);
		int blockY = abs(points[0].y - points[1].y);
		if(blockX < 25 && blockY < 300)
			continue;
		if(blockY < 25 && blockX < 120)
			continue;
		if(points[0].x == -1 && points[0].y  == -1 && points[1].x == -1 && points[1].y == -1)
			continue;
		if((abs(p1.x - points[0].x) + abs(p1.y - points[0].y)) <= BLOCKSIZE ){
			if((abs(p2.x - points[1].x) + abs(p2.y - points[1].y)) <= BLOCKSIZE){
			//  points[0].x = -1;
			 // points[0].y = -1;
			 // points[1].x = -1;
			//  points[1].y = -1;
			  track1 = true;
			}
		}
		if((abs(p2.x - points[0].x) + abs(p2.y - points[0].y)) <= BLOCKSIZE ){
			if((abs(p1.x - points[1].x) + abs(p1.y - points[1].y)) <= BLOCKSIZE){
			//  points[0].x = -1;
			//  points[0].y = -1;
			//  points[1].x = -1;
			//  points[1].y = -1;
			  track2 = true;
			}
		}
		if(track1 || track2)
			return true;
	}
 return false;
}
 bool sortOutPointX(pair<CvPoint,int> &p1, pair<CvPoint,int> &p2){
	 return p1.first.x == p2.first.x ? p1.first.y < p2.first.y : p1.first.x < p2.first.x ; 
 }
 bool sortOutPointY(pair<CvPoint,int> &p1, pair<CvPoint,int> &p2){
	 return p1.first.y == p2.first.y ? p1.first.x < p2.first.x : p1.first.y < p2.first.y ; 
 }
 bool sortOutPointsY(const CvPoint p1,const CvPoint p2){
	 return p1.y == p2.y ? p1.x < p2.x : p1.y < p2.y;
 }
 bool sortOutAnother(const pair<int,int> p1,const pair<int,int> p2){
	 return p1.second < p2.second;
 }

vector<vector<pair<CvPoint,int>>> ROIExtractor::imageROIExtractor(vector<CvPoint*> &lineContainer,vector<CvPoint> cornerContainer,IplImage *img){
    
	int i,j;
	
	vector<vector<pair<CvPoint,int > > > superContainer;
	vector<pair<int,pair<int,setStruct>>> containers;
	//IplImage *img = cvLoadImage(FILEPATH); 
	initGraph();
	//containers.resize(cornerContainer.size());
	for( i = 0; i < cornerContainer.size(); i++){
		vector<pair<CvPoint,int>> tempContainer;
		tempContainer.push_back(make_pair<CvPoint,int>(cornerContainer.at(i),i));
		superContainer.push_back(tempContainer);
		//containers[i].first  =  i;
		//containers[i].second = 1;
		setStruct structuredObject = setStruct(cornerContainer.at(i));
		pair<int,setStruct> pairs = make_pair<int,setStruct>(1,structuredObject);
		containers.push_back(make_pair<int,pair<int,setStruct>>(i,pairs));

    }
	for(i = 0; i < cornerContainer.size(); i++ ){		
	    for(j = 0; j < cornerContainer.size(); j++){
			if(i == j)
				continue;
			//if(containers.at(j).first != j)
			//	continue;
			if(isOnTheSameline(cornerContainer.at(i),cornerContainer.at(j),lineContainer)){
					printf("Merge called for\n");
					mergeSet(i,j,containers,cornerContainer.at(i),cornerContainer.at(j));
					//break;
			}
		}
		//graph.push_back(containers[i].second.second.Node);
	}
	for(i = 0; i < cornerContainer.size(); i++){
		containers.at(i).first = findSet(containers[i].first,containers);
		if(i != containers[i].first){
			superContainer.at(containers[i].first).push_back(make_pair<CvPoint,int>(cornerContainer.at(i),i));
		}
	}	
	for(i = 0; i < superContainer.size(); i++){
		CvPoint cvPoints[2];
		printf("Set #%d size of %d is %d\n",i,i,superContainer.at(i).size());	
		if(superContainer.at(i).size() >= 4){
			sort(superContainer.at(i).begin(),superContainer.at(i).end(),sortOutPointX);
			for(int j = 0 ; j < superContainer.at(i).size(); j++){
				cvCircle(img,cvPoint(superContainer.at(i).at(j).first.x,superContainer.at(i).at(j).first.y),fRounds(3),cvScalar(25,i*200,255),2);
				graph[superContainer.at(i).at(j).second] = containers[superContainer.at(i).at(j).second].second.second.Node;
				sort(graph[superContainer.at(i).at(j).second].begin(),graph[superContainer.at(i).at(j).second].end(),sortOutAnother);
				for(int k = 0; k < graph[superContainer.at(i).at(j).second].size(); k++){
					if(visited[graph[superContainer.at(i).at(j).second][k].first][graph[superContainer.at(i).at(j).second][k].second] == 0){
						//cvDrawLine(img,cornerContainer[graph[superContainer.at(i).at(j).second][k].first],cornerContainer[graph[superContainer.at(i).at(j).second][k].second],cvScalar(255,0,0),2);
						visited[graph[superContainer.at(i).at(j).second][k].first][graph[superContainer.at(i).at(j).second][k].second] = 1;
						visited[graph[superContainer.at(i).at(j).second][k].second][graph[superContainer.at(i).at(j).second][k].first] = 1;
					}
				}
			}
			//graph[i].push_back(containers[i].second.second.Node);
			indexHoldering.resize(graph.size());
			cycleIndex = 0;
			SSC(superContainer.at(i).at(0).second);
			printf("size of IndexHoldeing %d\n",indexHoldering.size());
			CvPoint pX;
			CvPoint pY;
			pX.x = INF;pX.y = INF; pY.x = -1; pY.y = -1;
			for(int l1 = 0; l1 < indexHoldering.size();l1++){
				vector<CvPoint> tmpContainerVec;
				if(indexHoldering.at(l1).size() != 0){
					for(int l2 = 0; l2 < indexHoldering.at(l1).size()-1; l2++){
					  //cvDrawLine(img,cornerContainer[indexHoldering.at(l1).at(l2)],cornerContainer[indexHoldering.at(l1).at(l2+1)],cvScalar(255,0,0),2);
						tmpContainerVec.push_back(cornerContainer.at(indexHoldering.at(l1).at(l2)));
					}
					printf("Size of the Temp Vector %d\n",tmpContainerVec.size());
					ConvexHull convexhull;
					int sizeVal = convexhull.convexHull(tmpContainerVec,true);
					int counterVal = convexhull.perpLex(tmpContainerVec);
					printf("CounterVal for Perpendicularity %d\n",counterVal);
					if((sizeVal - 2) == tmpContainerVec.size()&& counterVal == 1){
					 // for(int l2 = 0; l2 < indexHoldering.at(l1).size()-1; l2++)
						//cvDrawLine(img,cornerContainer[indexHoldering.at(l1).at(l2)],cornerContainer[indexHoldering.at(l1).at(l2+1)],cvScalar(255,0,0),2);
						sort(tmpContainerVec.begin(),tmpContainerVec.end(),sortOutPoints);
						if(pX.x > tmpContainerVec.at(0).x)
							pX.x = tmpContainerVec.at(0).x;
						if(pY.x < tmpContainerVec.at(tmpContainerVec.size() - 1).x)
							pY.x = tmpContainerVec.at(tmpContainerVec.size()-1).x;
						sort(tmpContainerVec.begin(),tmpContainerVec.end(),sortOutPointsY);
						if(pX.y > tmpContainerVec.at(0).y)
							pX.y = tmpContainerVec.at(0).y;
						if(pY.y < tmpContainerVec.at(tmpContainerVec.size() - 1).y)
							pY.y = tmpContainerVec.at(tmpContainerVec.size() - 1).y;

					}
				//	cvShowImage("cycle",img);
				//	 cvWaitKey(0);
					tmpContainerVec.clear();  
				}
				//break;
			}		
			cvDrawRect(img,pX,pY,cvScalar(255,0,0),2);
			cvWaitKey(0);
			indexHoldering.clear();
			indexHoldering.resize(200);
			cvPoints[0].x = superContainer.at(i).at(0).first.x;
			cvPoints[1].x = superContainer.at(i).at(superContainer.at(i).size()-1).first.x;
			sort(superContainer.at(i).begin(),superContainer.at(i).end(),sortOutPointY);
			cvPoints[0].y = superContainer.at(i).at(0).first.y;
			cvPoints[1].y = superContainer.at(i).at(superContainer.at(i).size()-1).first.y;
			//if(abs(cvPoints[0].x - cvPoints[1].x) > 120 && abs(cvPoints[0].y - cvPoints[1].y) > 350 )
			//	cvDrawRect(img,cvPoints[0],cvPoints[1],cvScalar(255,0,0),2);
			superContainer.at(i).clear();
			superContainer.at(i).push_back(make_pair<CvPoint,int>(cvPoints[0],0));
			superContainer.at(i).push_back(make_pair<CvPoint,int>(cvPoints[1],1));
			//for(j =0; j < superContainer.at(i).size(); j++)
			//	cvCircle(img,superContainer.at(i).at(j),3,cvScalar(25,0,255),1);
		}
	}
	cvNamedWindow("dontKnow");
	cvShowImage("dontKnow",img);
	cvWaitKey(0);
	return superContainer;
}