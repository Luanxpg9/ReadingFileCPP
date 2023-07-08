#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

#pragma region Enums
enum VariableType {
    Integer,
    Double,
    String,
    Image
};
#pragma endregion

#pragma region Structs
// Variables can be global or inside a block
struct Variable {
    string key;
    string value;
    VariableType type;
};

struct Vector2 {
    float x;
    float y;
};

// Comments that are shown in the workflow file
struct Comment {
    // Line position in the workflow file
    int line;
    // The text that a commentary has
    string text;
    // Grid position in the User interface
    Vector2 position;
};

// Output connection of a block
struct Output {
    // Identificator
    int id;
    // Output type
    VariableType type;
};

// Input connection of a block
struct Input {
    // Identificator
    int id;
    // Input type
    VariableType type;
};
// Note that the input and output look the same but are used to identify diferent things


// A block defines a VGL function, Its parameters, variables, input and output
struct Block {
    // Identificator
    int id;
    // Used to define what VGL function it represents
    string type;
    // Block variables like the input parameters
    vector<Variable> variables;
    // The position in the 2d Grid
    Vector2 position;
    // Output node in this block
    vector<Output> outputs;
    vector<Input> inputs;
};

struct Connection {
    // Connection identificator
    int id;
    // The block in which the connection will begin
    int startBlock;
    // The output in which the connection will begin on that block
    int outputStartBlock;
    // The block in which the connection will finish
    int endBlock;
    // The input in which the connection will finish
    int inputEndBlock;
};

// Represents a workflow file in a struct pattern.
struct Workflow {
    // Variables that can be used in any function/block
    vector<Variable> globalVariables;
    // The blocks represents a VGL function that is called in the workflow
    vector<Block> blocks;
    // The connections between the blocks inside a workflow. The work like passing futher an variable to another block
    vector<Connection> connections;
    // A user can specify comments for a workflow that can be used to make a workflow more human readable
    vector<Comment> comments;
};

#pragma endregion

#pragma region Functions
#pragma region Get Execution Path function
// Get the execution path
wstring ExePath() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    wstring::size_type pos = wstring(buffer).find_last_of(L"\\/");

    return wstring(buffer).substr(0, pos+1);
}
#pragma endregion

#pragma region Test functon for return a vector
vector<string> ReturnVectorTest() {
    vector<string> test;

    test.push_back("test1");
    test.push_back("test2");
    test.push_back("test3");

    return test;
}
#pragma endregion

#pragma region CompareString function
// return 1 if strings are the same, return 0 if they are not
int CompareString(string in1, string in2) {
    int res = in1.compare(in2);

    if (res == 0) {
        cout << "The strings are the same";
        return 1;
    }
    else {
        cout << "Strings don't match";
        return 0;
    }
}
#pragma endregion

#pragma region ExploreFolder function
// Returns a list of paths inside that directory
vector<string> ExploreFolder(string path) {
    // Declare variables
    vector<string> names;
    string searchPath = path + "/*.*";
    WIN32_FIND_DATA findData;
    
    cout << "Target file is " << searchPath;

    // Convert string to LPCWSTR
    wstring searchPathW = wstring(searchPath.begin(), searchPath.end());
    LPCWSTR searchPathLPCW = searchPathW.c_str();
    
    // Explore given folder
    HANDLE hFind = FindFirstFileW(searchPathLPCW, &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        while (FindNextFileW(hFind, &findData)) {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                // Convert wstring to string
                wstring fileName = findData.cFileName;
                string fileNameSTR(fileName.begin(), fileName.end());

                // Push in the fileName list
                names.push_back(fileNameSTR);
            }
        }

        FindClose(hFind);
    }
    
    return names;
}
#pragma endregion

#pragma region GetLinesFromFile function
// Returns a list of lines in a given file
vector<string> GetLinesFromFile(string path) {
    ifstream myFile(path);
    vector<string> myLines;
    string myLine;

    // Tell the runtime that an error ocurred while opening the file
    if (myFile.fail()) {
        throw std::runtime_error("GetLinesFromFile error >> Unable to open file");
    }

    if (myFile.is_open()) {
        while (myFile) {
            myLine = "";
            std::getline(myFile, myLine);

            myLines.push_back(myLine);
        }
    }

    return myLines;
}
#pragma endregion
/**
 * Get workflow comment lines in a .wksp file
 *
 * @param workflowLines A vector of lines containing the workflow file's lines.
 * @verbose If true prints in the console the comments found in the file.
 * @return A vector of comments
 * 
 * @throws runtime_error if workflowLines is empty
 */
vector<Comment> ParseWorkflowComments(vector<string> workflowLines, bool verbose) {
    
    // Throw error if there is no line
    //      Prevents from initializing vectors and structs
    if (workflowLines.empty()) {
        throw std::runtime_error("ParseWorkflowComments error >> Workflow lines are empty");
    }
    

    vector<Comment> comments;

    Vector2 nullPosition = { 0.0, 0.0 };

    
    for (int i = 0; i < workflowLines.size(); i++) {
        if (workflowLines[i][0] == '#') {
            // Get Comment without the '#' character
            string comment = workflowLines[i].substr(1, workflowLines[i].length());
            Comment newComment = {
                i+1,
                comment,
                nullPosition
            };

            // Insert new comment into the comment vector
            comments.push_back(newComment);

            // Print function comments if verbose parameter is true
            if (verbose) {
                cout << "Comment detected: \n" << "\t-> Line: " << i+1 << "\n\t-> Comment: " << comment << '\n';
            }
            
        }
    }

    return comments;
}


#pragma endregion

int main()
{
    #pragma region Code example: Comparing two strings
    /*
    string input1;
    string input2;

    cout << "please enter a string to compare: \n";
    cin >> input1;

    cout << "please enter another string to compare: \n";
    cin >> input2;

    comparestring(input1, input2);
    */
    #pragma endregion

    #pragma region Code example: Get a list of vector from a function
    /*
    vector<string> test = returnvectortest();
    cout << "returned vector:" << endl;

    for (int i = 0; i < test.size(); i++) {
        cout << test[i] << endl;
    }*/
    #pragma endregion

    #pragma region Code example: Get current execution path
    /*wstring path = ExePath();
    string paths(path.begin(), path.end());
    
    cout << "the current execution path is: \n" << paths;*/
    #pragma endregion
    
    #pragma region Code example: List current files in execution path
    /*vector<string> filenames;
    
    filenames = ExploreFolder(paths);

    for (int i = 0; i < filenames.size(); i++) {
        cout << i << ": " << filenames[i] << endl;
    }*/
    #pragma endregion

    #pragma region Code example: Read lines in a file
    // Initializing variable
    //vector<string> file;
    //
    //try {
    //    // Calling the function that returns a list of line in a file
    //    file = GetLinesFromFile("teste.wksp");

    //    // Printing the list of lines
    //    for (int i = 0; i < file.size(); i++) {
    //        cout << file[i] << '\n';
    //    }
    //}
    //// Exception treatment
    //catch (const std::exception &e) {
    //    cout << "Error: " << e.what() << '\n';
    //}
    #pragma endregion

    #pragma region Code example: Parse Workflow Comments
    try {
        vector<string> workflow;

        string fileName = "teste.wksp";

        workflow = GetLinesFromFile(fileName);

        vector<Comment> comments = ParseWorkflowComments(workflow, true);

        cout << "Found " << comments.size() << " comments in '" << fileName << '\'';
    }
    catch (const std::exception& e) {
        cout << "Error:" << e.what() << '\n';
    }
    #pragma endregion

    #pragma region Code example: Parse Workflow Connections
    try {

    }
    catch (const std::exception& e) {
        cout << "Error:" << e.what() << '\n';
    }
    #pragma endregion

    return 0;
}

