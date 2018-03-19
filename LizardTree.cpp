
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <unordered_set>
#include <vector>
#include <stack>
#include <queue>

using namespace std;

class GlobalState{
public:
    int n;
    int p;
    vector<pair<int, int>> areas;
    unordered_set<int> treePositions;
};

GlobalState gstate;
bool searchStrategy;  // true = DFS .. false = BFS

class Node{
public:
    unordered_set<int> lizardPositions;
    int areaIndex;
    int lastLizardPos;
};

stack<Node> openListStack;
queue<Node> openListQueue;

int **board;

// input file
ifstream myInputfile ("/Users/Patel/Documents/AI/LizardAndTrees/LizardAndTrees/input.txt");
ofstream myOutputfile;

bool isOnRight(int pos){
    if(pos % gstate.n != gstate.n - 1)
        return false;
    else
        return true;
}
bool isOnTop(int pos){
    if(pos / gstate.n == 0)
        return true;
    else
        return false;
}
bool isOnLeft(int pos){
    if(pos % gstate.n == 0)
        return true;
    else
        return false;
}

bool isSafe(Node node){
    int curr = node.lastLizardPos;
    if(curr == -1)
        return true;
    
    // look diagonally right
    while((!isOnRight(curr)) && (!isOnTop(curr)))
    {
        curr = curr - gstate.n + 1;
        if(node.lizardPositions.find(curr) != node.lizardPositions.end()) // if there is a lizard
            return false;
        if(gstate.treePositions.find(curr) != gstate.treePositions.end()) // tree - stop
            break;
            
    }
    curr = node.lastLizardPos;
    // look diagonally left
    while((!isOnLeft(curr)) && (!isOnTop(curr)))
    {
        curr = curr - gstate.n - 1;
        if(node.lizardPositions.find(curr) != node.lizardPositions.end()) // if there is a lizard
            return false;
        if(gstate.treePositions.find(curr) != gstate.treePositions.end()) // tree - stop
            break;
        
    }
    curr = node.lastLizardPos;
    // look vertically up
    while(!isOnTop(curr))
    {
        curr = curr - gstate.n;
        if(node.lizardPositions.find(curr) != node.lizardPositions.end()) // if there is a lizard
            return false;
        if(gstate.treePositions.find(curr) != gstate.treePositions.end()) // tree - stop
            break;
        
    }

    return true;
}

bool isGoal(Node node){
    
    if(node.lizardPositions.size() != gstate.p)
        return false;
    if(node.lizardPositions.size() == gstate.p && isSafe(node))
        return true;
    
    return false;
}

void printBoard(Node node)
{
    
    for(auto it = node.lizardPositions.begin(); it != node.lizardPositions.end(); it++)
    {
        int pos = *it;
        board[pos/gstate.n][pos%gstate.n] = 1;
    }
    
    for(int i = 0; i < gstate.n; i++)
    {
        for(int j = 0; j < gstate.n; j++)
            myOutputfile << board[i][j];
        myOutputfile<<endl;
    }
    
    
}


int main(int argc, const char * argv[]) {
    
    
    // init the global state here ----------------
    string line;
    
    getline(myInputfile, line);
    if(line.find("DFS") != string::npos)
        searchStrategy = true;
    else
        searchStrategy = false;
    
    getline(myInputfile, line);
    gstate.n = atoi(line.c_str());
    getline(myInputfile,line);
    gstate.p = atoi(line.c_str());
    
    board = new int*[gstate.n];
    for(int i = 0; i < gstate.n; i++)
        board[i] = new int[gstate.n];

    
    int areaStart = 0;
    
    for (int i = 0; i < gstate.n; i++) {
        getline(myInputfile, line);
        int j;
        for (j = 0; j < gstate.n; j++) {
            int currPos = line[j] - '0';
            board[i][j] = 0;
            if(currPos == 2){
                board[i][j] = 2;
                gstate.treePositions.insert(i*gstate.n + j); // insert the pos of tree
                gstate.areas.push_back(make_pair(areaStart, i*gstate.n + j - 1));
                areaStart = i*gstate.n + j + 1;
            }
//            cout << currPos << " ";
        }
        gstate.areas.push_back(make_pair(areaStart, i*gstate.n + j - 1));
        areaStart = i*gstate.n + j;
//        cout << endl;
    }
    
    vector<pair<int, int>>::iterator it;
    for(it = gstate.areas.begin(); it != gstate.areas.end(); )
    {
        if(it->first > it ->second)
            gstate.areas.erase(it);
        else
            it++;
    }
//    for(it = gstate.areas.begin(); it != gstate.areas.end(); it++)
//    {
//        cout << it->first << " " << it->second << endl;
//    }
    
//    gstate.n = 10;
//    gstate.p = 5;
//    gstate.areas.push_back(make_pair(0, 0));
//    gstate.treePositions.insert(1);
//    cout << gstate.n << " " << gstate.p << endl;
    
    //--------------------------------------------
    Node startNode;
    // init the start node here ------------------
    startNode.areaIndex = -1;
    startNode.lastLizardPos = -1;
    
    //--------------------------------------------
    // Process starts ----------------------------
    if(searchStrategy){
        openListStack.push(startNode);
        bool result = false;
        Node res;
        
        while (openListStack.size() != 0) {
            Node curr = openListStack.top();
            openListStack.pop();
            
            if(isGoal(curr)){
                result = true;
                res = curr;
                break;
            }
            if(!isSafe(curr))
                continue;
            // create child of the current node and push in the tree
            
            int nextAreaIndex = curr.areaIndex + 1;
            if (nextAreaIndex >= gstate.areas.size()) {
                continue;
            }
            
            
            int nextAreaStart = gstate.areas[nextAreaIndex].first;
            int nextAreaEnd = gstate.areas[nextAreaIndex].second;
            
            Node temp;
            temp.areaIndex = nextAreaIndex;
            temp.lizardPositions = curr.lizardPositions;
            temp.lastLizardPos = -1;
            openListStack.push(temp);
            
            for (int i = nextAreaStart; i <=  nextAreaEnd ; i++) {
                Node temp;
                temp.areaIndex = nextAreaIndex;
                temp.lizardPositions = curr.lizardPositions;
                temp.lizardPositions.insert(i);
                temp.lastLizardPos = i;
                openListStack.push(temp);
            }
            
        }
        
        
        myOutputfile.open("/Users/Patel/Documents/AI/LizardAndTrees/LizardAndTrees/output.txt");
        
        if(result)
        {
            myOutputfile << "OK" << endl;
            printBoard(res);
        }
        else
            myOutputfile << "FAIL" << endl;
        
        myOutputfile.close();
        return 0;
    }
    else
    {
        openListQueue.push(startNode);
        bool result = false;
        Node res;
        
        while (openListQueue.size() != 0) {
            Node curr = openListQueue.front();
            openListQueue.pop();
            
            if(isGoal(curr)){
                result = true;
                res = curr;
                break;
            }
            if(!isSafe(curr))
                continue;
            // create child of the current node and push in the tree
            
            int nextAreaIndex = curr.areaIndex + 1;
            if (nextAreaIndex >= gstate.areas.size()) {
                continue;
            }
            
            
            int nextAreaStart = gstate.areas[nextAreaIndex].first;
            int nextAreaEnd = gstate.areas[nextAreaIndex].second;
            
            Node temp;
            temp.areaIndex = nextAreaIndex;
            temp.lizardPositions = curr.lizardPositions;
            temp.lastLizardPos = -1;
            openListQueue.push(temp);
            
            for (int i = nextAreaStart; i <=  nextAreaEnd ; i++) {
                Node temp;
                temp.areaIndex = nextAreaIndex;
                temp.lizardPositions = curr.lizardPositions;
                temp.lizardPositions.insert(i);
                temp.lastLizardPos = i;
                openListQueue.push(temp);
            }
            
        }
        
        
        myOutputfile.open("/Users/Patel/Documents/AI/LizardAndTrees/LizardAndTrees/output.txt");
        
        if(result)
        {
            myOutputfile << "OK" << endl;
            printBoard(res);
        }
        else
            myOutputfile << "FAIL" << endl;
        myOutputfile.close();
        
        return 0;
    }
}
