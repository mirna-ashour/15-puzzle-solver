//
//  main.cpp
//  15-puzzle-algorithm
//
//  Created by Mirna Ashour and Rohan Bhalla on 11/5/22.
//

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
using namespace std;

// structure that represents each state of the board in the program
struct Node {
    vector <vector<int>> stateArray;    //  current state of puzzle
    int hValue = 0;     //  value of calculated heuristic
    double fnValue = 0;    //  calculated f(n) value
    int depth = 0;      // depth of node in tree
    char action = 'N';  //  action taken to achieve node
    Node* parent = nullptr;
    Node* upNode = nullptr;
    Node* downNode = nullptr;
    Node* leftNode = nullptr;
    Node* rightNode = nullptr;
};


//  global variables
float weight = 0.0;
vector<vector<int>> goalBoard {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};
string actionsString = "";
vector<double> fnValues;
vector<Node*> uniqueStates;
int depth = 0;


// function protoypes:

//  open file stream
void openFile(ifstream& ifs, string fileName);

//  read from file and fill in information
void readFile(ifstream& ifs, vector<vector<int>>& initialBoard);

//  write to a new file the required results of the search
void writeToFile(string fileNmae, vector<vector<int>> initialBoard, int depth);

//  generates new array for next node
vector<vector<int>> generateNextState(vector<vector<int>> currBoard, string direction);

//  f(n) calculator
double bestEstimateCost(int hValue, int currDepth);

//  h(n) heuristic calculator
int heursiticFunc(vector<vector<int>> currBoard);

//  returns the position of given tiles on a given board
pair<int,int> findCellOfValue(vector<vector<int>> board, int targetVal);

//  comparing two tiles (returns true if same, false otherwise)
bool compareBoards(vector<vector<int>> boardA, vector<vector<int>> boardB);

//  check if generated node is unqiue
bool isStateUnique(vector<vector<int>> currState);

//  search through generated tree of nodes and produces a solution path using A* weighted search
Node* searchTree(Node* headNode);

//  print board to screen
void printBoard(vector<vector<int>> board);

//  prints all boards on the solution path in reverse order
void printBoardSqeuence(Node* goal, Node* pointToHead);


int main(int argc, const char * argv[]) {
    cout << "AI PROJECT 1\n";
    //  intializing file stream and initialBoard array
    ifstream ifs;
    string fileName = "Input8.txt";
    vector<vector<int>> initialBoard {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    };
    
    //  checks if a file name was passed an argument, if not, it exits with an error
    if(argc == 2) {
        fileName = argv[1];
    }
    else {
        cout << "File name not provided." << endl;
        exit(1);
    }
    
    //  reading provided data from input file populating inital and goal board arrays
    openFile(ifs, fileName);
    readFile(ifs, initialBoard);
    cout << "Initial board:" << endl;
    printBoard(initialBoard);
    cout << "\n";
    cout << "Goal board:" << endl;
    printBoard(goalBoard);
    cout << "\n";
    
    //  creating initial head node
    Node* headNode = new Node();
    headNode->stateArray = initialBoard;
    Node* pointToHead = headNode;       // indicates beginning of linked list
    
    //  generating solution path from head node
    Node* goal = searchTree(headNode);
    printBoardSqeuence(goal, pointToHead);      //  printing boards in reverse order
    
    //  prints actions taken along solution path + num of nodes generated
    reverse(actionsString.begin(), actionsString.end());
    actionsString.erase(0,1);
    cout << "Actions: " << actionsString <<endl;
    cout << "No. of nodes generated: " << uniqueStates.size() << endl;
    
    writeToFile("Output8.txt", initialBoard, goal->depth);
    
    return 0;
}


//  open file stream
void openFile(ifstream& ifs, string fileName) {
    ifs.open(fileName);
    if(!ifs){   // prints error and exits if file is not found
        cout << "File cannot be opened." << endl;
        exit(1);
    }
}


//  read from file and fill in information
void readFile(ifstream& ifs, vector<vector<int>>& initialBoard) {
    int readInt;
    
    //  reads in weight value
    ifs >> weight;
    
    //  populates intital board
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++){
            ifs >> readInt;
            initialBoard[i][j] = readInt;
        }
    }
    //  populates goal board
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++){
            ifs >> readInt;
            goalBoard[i][j] = readInt;
        }
    }
    
    ifs.close();
}


//  write to a new file the required results of the search
void writeToFile(string fileName, vector<vector<int>> initialBoard, int depth) {
        
    ofstream ofs (fileName);   //  create file stream and file
                
    if (ofs.is_open()) {
        //  write initial board
        for(int i = 0; i < initialBoard.size(); i++){
            for(int j = 0; j < initialBoard.size(); j++) {
                ofs << initialBoard[i][j] << " ";
            }
            ofs << endl;
        }
        
        ofs << endl;
        
        //  write goal board
        for(int i = 0; i < initialBoard.size(); i++){
            for(int j = 0; j < initialBoard.size(); j++) {
                ofs << goalBoard[i][j] << " ";
            }
            ofs << endl;
        }
        
        //  write global variables
        ofs << endl << weight << endl << depth << endl << uniqueStates.size() << endl << actionsString << endl;
        
        //  write f(n) values for search
        reverse(fnValues.begin(), fnValues.end());
        for(int i = 0; i < fnValues.size(); i++) {
            ofs << fnValues[i] << " ";
        }
        
        ofs.close();
    }
    
    else { cout << "Unable to open file."; }
}


//  f(n) calculator
double bestEstimateCost(int hValue, int currDepth) {
    return (currDepth + (weight*hValue));
}


//  returns the position of given tiles on a given board
pair<int,int> findCellOfValue(vector<vector<int>> board, int targetVal) {
    int xPos = -1;
    int yPos = -1;
    //  loops through values in array, finds target value, and breaks
    for(int i=0; i < board.size(); ++i)
    {
        for(int j=0; j < board[i].size(); ++j)
        {
            if(board[i][j] == targetVal)
            {
                yPos = i;
                xPos = j;
                break;
            }
        }
    }
    return  make_pair(yPos, xPos);
}


//  h(n) heuristic calculator
int heursiticFunc(vector<vector<int>> currBoard) {
    int answer = 0;
    for(int i=0; i < currBoard.size(); ++i)
    {
        for(int j=0; j < currBoard[i].size(); ++j)
        {
            //  Find where the number in current cell is kept in the goal board
            if(currBoard[i][j] != 0)
            {
                pair<int,int> coordOfTarget = findCellOfValue(goalBoard, currBoard[i][j]);
                //Take absolute difference of x and y positions and add max to answer
                answer += max(abs(coordOfTarget.first-i),abs(coordOfTarget.second-j));
            }
            
        }
    }
    return answer;
}

//  comparing two tiles (returns true if same, false otherwise)
bool compareBoards(vector<vector<int>> boardA, vector<vector<int>> boardB) {
    bool areSame = true;
    for(size_t i = 0; i <boardA.size(); ++i)
    {
        for(size_t j=0; j < boardA[i].size(); ++j)
        {
            if(boardA[i][j] != boardB[i][j])
            {
                areSame = false;
                break;
            }
        }
    }
    return areSame;
}


//  check if generated node is unqiue
bool isStateUnique(vector<vector<int>> compareThis) {
    bool flag = true;
    for(size_t i=0; i < uniqueStates.size(); ++i)
    {
        if(compareBoards(uniqueStates[i]->stateArray, compareThis))
        {
            //  If the two boards are the same then it is not unique
            flag = false;
            break;
        }
    }
    return flag;
}


//  print board to screen
void printBoard(vector<vector<int>> board) {
    for(size_t i=0; i < board.size(); ++i)
    {
        for(size_t j=0; j < board[i].size(); ++j)
        {
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
}

//  prints all boards on the solution path in reverse order
void printBoardSqeuence(Node* goal, Node* pointToHead) {
    cout << "Printing sequence of boards in reverse order:\n" <<endl;
    while(goal != pointToHead)      //  loops until the head node from the goal node
    {
        printBoard(goal->stateArray);       //  prints board
        actionsString += goal->action;      //  adds action for board to action strings
        actionsString += ' ';
        cout << goal->fnValue << endl;      //  prints f(n) value for board
        fnValues.push_back(goal->fnValue);  //  inserts the node's f(n) value into the f(n) values array
        cout << endl << endl;
        goal = goal->parent;        //  iterates on next node's parent
    }
    fnValues.push_back(pointToHead->fnValue);      //  inserts the root node's f(n) value
}


//  generates new array for next node
vector<vector<int>> generateNextState(vector<vector<int>> currBoard, string direction) {
    vector<vector<int>> updatedBoard = currBoard;
    pair<int,int> zeroPos = findCellOfValue(currBoard, 0);        //  Find where 0 is kept in the current board
    if(direction == "up") {
        //Check to see we can go up
        if(zeroPos.first != 0) {
            //swap zero with its y-1 position cell value
            updatedBoard[zeroPos.first][zeroPos.second] = updatedBoard[zeroPos.first-1][zeroPos.second];
            updatedBoard[zeroPos.first-1][zeroPos.second] = 0;
        }
    }
    else if(direction == "down") {
        //Check to see we can go down
        if(zeroPos.first != 3) {
            //swap zero with its y+1 position cell value
            updatedBoard[zeroPos.first][zeroPos.second] = updatedBoard[zeroPos.first+1][zeroPos.second];
            updatedBoard[zeroPos.first+1][zeroPos.second] = 0;
        }
        
    }
    else if(direction == "right") {
        //Check to see we can go right
        if(zeroPos.second != 3) {
            //swap zero with its x+1 position cell value
            updatedBoard[zeroPos.first][zeroPos.second] = updatedBoard[zeroPos.first][zeroPos.second+1];
            updatedBoard[zeroPos.first][zeroPos.second+1] = 0;
            
        }
    }
    else if(direction == "left") {
        //Check to see we can go left
        if(zeroPos.second != 0) {
            //swap zero with its x-1 position cell value
            updatedBoard[zeroPos.first][zeroPos.second] = updatedBoard[zeroPos.first][zeroPos.second-1];
            updatedBoard[zeroPos.first][zeroPos.second-1] = 0;
        }
    }
    return updatedBoard;
}


//  search through generated tree of nodes and produces a solution path using A* weighted search
Node* searchTree(Node* currNode) {
    typedef pair<double, Node*> numNode;
    priority_queue<numNode, vector<numNode>, greater<numNode>> frontier; //Frontier DS (might be global)
    
    // Assign heuristic and f(n) value to current node
    currNode->hValue = heursiticFunc(currNode->stateArray);
    currNode->fnValue = bestEstimateCost(currNode->hValue, 0);
    
    // Add node to frontier
    frontier.push(make_pair(currNode->fnValue, currNode));
    
    // Initialize board arrays for all directions
    vector<vector<int>> upBoard;
    vector<vector<int>> downBoard;
    vector<vector<int>> rightBoard;
    vector<vector<int>> leftBoard;

    while(currNode->hValue != 0) {  // loops until we reach the goal node
        
        //Create next possible states for every direction for current node
        upBoard = generateNextState(currNode->stateArray, "up");
        downBoard = generateNextState(currNode->stateArray, "down");
        rightBoard = generateNextState(currNode->stateArray, "right");
        leftBoard = generateNextState(currNode->stateArray, "left");

        //UP ACTION
        if(!compareBoards(currNode->stateArray, upBoard)) { //  action is legal (board is not uchanged)
            if(isStateUnique(upBoard)) { // state is unique
               //   Create new node and assign its fields
               Node* upState = new Node();
               upState->stateArray = upBoard;
               upState->hValue = heursiticFunc(upBoard);
               currNode->upNode = upState;
               upState->parent = currNode;
               upState->depth = currNode->depth + 1;
               upState->fnValue = bestEstimateCost(upState->hValue, upState->depth);
               upState->action = 'U';
               //   Add to unique states
               uniqueStates.push_back(upState);
               //   Add to frontier
               frontier.push(make_pair(upState->fnValue, upState));
            }
        }
        
        //DOWN ACTION
        if(!compareBoards(currNode->stateArray, downBoard)) {
            if(isStateUnique(downBoard)) {
               Node* downState = new Node();
               downState->stateArray = downBoard;
               downState->hValue = heursiticFunc(downBoard);
               currNode->downNode = downState;
               downState->parent = currNode;
               downState->depth = currNode->depth + 1;
               downState->fnValue = bestEstimateCost(downState->hValue, downState->depth);
               downState->action = 'D';
               uniqueStates.push_back(downState);
               frontier.push(make_pair(downState->fnValue, downState));
            }
        }
        //RIGHT ACTION
        if(!compareBoards(currNode->stateArray, rightBoard)) {
            if(isStateUnique(rightBoard)) {
               Node* rightState = new Node();
               rightState->stateArray = rightBoard;
               rightState->hValue = heursiticFunc(rightBoard);
               currNode->rightNode = rightState;
               rightState->parent = currNode;
               rightState->depth = currNode->depth + 1;
               rightState->fnValue = bestEstimateCost(rightState->hValue, rightState->depth);
               rightState->action = 'R';
               uniqueStates.push_back(rightState);
               frontier.push(make_pair(rightState->fnValue, rightState));
            }
        }
        //LEFT ACTION
        if(!compareBoards(currNode->stateArray, leftBoard)) {
            if(isStateUnique(leftBoard)) {
               Node* leftState = new Node();
               leftState->stateArray = leftBoard;
               leftState->hValue = heursiticFunc(leftBoard);
               currNode->leftNode = leftState;
               leftState->parent = currNode;
               leftState->depth = currNode->depth + 1;
               leftState->fnValue = bestEstimateCost(leftState->hValue, leftState->depth);
               leftState->action = 'L';
               uniqueStates.push_back(leftState);
               frontier.push(make_pair(leftState->fnValue, leftState));
            }
        }
        //Select next minimum path to explore by popping (will loop on this chosen board)
        currNode = frontier.top().second;
        //cout << currNode->hValue << endl;
        frontier.pop();
    }
    return currNode;
}
