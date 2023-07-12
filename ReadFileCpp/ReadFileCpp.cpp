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

#pragma region ParseWorkflowComments function
/**
 * Get workflow comment lines in a .wksp file
 *
 * @param workflowLines A vector of lines containing the workflow file's lines.
 * @verbose If true prints in the console the comments found in the file.
 * @return A vector of comments
 * 
 * @throws runtime_error if workflowLines is empty
 */
vector<Comment> ParseWorkflowComments(vector<string> workflowLines, bool verbose = false) {
    
    // Throw error if there is no line
    //      Prevents from initializing vectors and structs
    if (workflowLines.empty()) {
        throw std::runtime_error("ParseWorkflowComments error >> Workflow lines are empty");
    }

    // Initialize vector of comments for return
    vector<Comment> comments;
    // Initialize a position when the workflow file does not specifie where it should be in the Visual Workflow Editor
    Vector2 nullPosition = { 0.0, 0.0 };
    
    // Loop for parsing the file
    for (int i = 0; i < workflowLines.size(); i++) {
        if (workflowLines[i][0] == '#') {
            // Get Comment without the '#' character
            string comment = workflowLines[i].substr(1, workflowLines[i].length());

            // Initialize the Comment struct for appending in the vector of comments
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

#pragma region ParseWorkflowGlobalVariables function
/**
 * Get workflow global variables in a .wksp file
 *
 * @param workflowLines A vector of lines containing the workflow file's lines.
 * @verbose If true prints in the console the comments found in the file.
 * @return A vector of variables
 *
 * @throws invalid string position> if a variable is declared not folling the pattern 'key = value'
 */
vector<Variable> ParseWorkflowGlobalVariables(vector<string> workflowLines, bool verbose = false) {
    // List of variables for return
    vector<Variable> variables;
    // Flag active when the VariableBegin is found
    bool isVariableParserUp = false;

    // Loop for parsing file
    for (int i = 0; i < workflowLines.size(); i++) {

        // Line by line logging
        if (verbose) {
            cout << "Workflow line being parsed> '" << workflowLines[i] << "'\n";
        }

        // Sinalizes that the search for variables has ended
        if (workflowLines[i] == "VariablesEnd:") {

            // Finished function
            if (verbose) {
                cout << "Finished parsing global variables\n\n";
            }
            isVariableParserUp = false;
            break;
        }

        // Formating variable
        if (isVariableParserUp && workflowLines[i][0] != '\n' && workflowLines[i][0] != ' ' && workflowLines[i][0] != '#' && workflowLines[i][0] != '\0') {

            // To test: maybe a for will be faster than using find two times for getting indexes
            string variableKey = workflowLines[i].substr(0, workflowLines[i].find_first_of(' '));
            string variableValue = workflowLines[i].substr(workflowLines[i].find_first_of('=') + 2, workflowLines[i].size());

            VariableType type = Integer;

            Variable newVariable = {
                variableKey,
                variableValue,
                type
            };

            // Found variable log
            if (verbose) {
                cout << "\t\tNew variable found: \n" << "\t\t\t->Key: '" << variableKey << "'\n" << "\t\t\t->Value: '" << variableValue << "'\n\n";
            }

            variables.push_back(newVariable);
        }

        // Sinalizes that the search for variables has begun
        // Must be in the end for the parser code doesn't catch the 'VariablesBegin:'
        if (workflowLines[i] == "VariablesBegin:") {
            // Finished function
            if (verbose) {
                cout << "Starting parsing global variables\n\n";
            }

            isVariableParserUp = true;
        }
    }

    // Return found variables
    return variables;
}
#pragma endregion

#pragma region ParseWorkflowBlocks function
vector<Block> ParseWorkflowBlocks(vector<string> workflowLines, bool verbose = false) {
    vector<Block> blocks;

    // Parsing workflow file
    for (int i = 0; i < workflowLines.size(); i++) {
        if (workflowLines[i].substr(0, 5) == "Glyph") {

            cout << "Found Glyph on line " << i + 1 << ">>> " << workflowLines[i] << endl;

            // Glyph line composition:
            //  Glyph tag > Lib > Function > hostmachine > Glyph Id > X position > Y position > args 

            // Init 
            Vector2 functionNameIndex = { 0, 0 };
            Vector2 hostMachineIndex = { 0, 0 };
            Vector2 glyphIdIndex = { 0, 0 };
            Vector2 xPositionIndex = { 0, 0 };
            Vector2 yPositionIndex = { 0, 0 };
            Vector2 functionArgsIndex = { 0, 0 };
            
            int lastDivIndex = 13;

            int lineLength = workflowLines[i].size();

            // Parsing workflow line
            for (int j = 13; j < lineLength; j++) {
                bool separator = workflowLines[i][j] == ':';
                bool doubleSeparator = workflowLines[i][j] == ':' && workflowLines[i][j + 1] == ':';

                cout << "\tChar " << j << ": " << workflowLines[i][j] << endl;


                // Defines Function name
                
                functionNameIndex.x = 13;
                if (functionNameIndex.y == 0 && separator) {
                    functionNameIndex.y = j;
                    // The 13 number is the character count for 'Glyph:VGL_CL:'
                    cout << "\tFunction name>> " << workflowLines[i].substr(functionNameIndex.x, functionNameIndex.y-13) << endl;
                    
                    if (doubleSeparator) {
                        hostMachineIndex.x = j + 2;
                    }
                    else {
                        hostMachineIndex.x = j + 1;
                    }
                }
                // Defines hostmachine
                else if (hostMachineIndex.x != 0 && hostMachineIndex.y == 0 && separator && workflowLines[i][j-1] != ':') {
                    hostMachineIndex.y = j;
                    cout << "\tHostname>> " << workflowLines[i].substr(hostMachineIndex.x, hostMachineIndex.y - hostMachineIndex.x) << endl;
                    break;
                }

                /*
                // Defines Glyph Id
                else if (hostMachineIndex.y != 0 && glyphIdIndex.x == 0 && validSeparator) {
                    glyphIdIndex.x = j + 1;
                    cout << "\tGlyph Id>> " << workflowLines[i].substr(hostMachineIndex.y, j - glyphIdIndex.x) << endl;
                }*/

            }
        }
            
    }

    return blocks;
}
#pragma endregion

#pragma region ParseWorkflowConnections function
/**
 * Get workflow connections between blocks in a .wksp file
 *
 * @param workflowLines A vector of lines containing the workflow file's lines.
 * @verbose If true prints in the console the comments found in the file.
 * @return A vector of Connections
 *
 * @throws runtime_error if workflowLines is empty
 */
vector<Connection> ParseWorkflowConnections(vector<string> workflowLines, bool verbose) {

    // Throw error if there is no line
    //      Prevents from initializing vectors and structs
    if (workflowLines.empty()) {
        throw std::runtime_error("ParseWorkflowConnections error >> Workflow lines are empty");
    }

    // Initialize vector of comments for return
    vector<Connection> connections;

    // Loop for parsing the file
    for (int i = 0; i < workflowLines.size(); i++) {
        if (workflowLines[i].substr(0, 14) == "NodeConnection") {

            // Parssing connection


            // Print function comments if verbose parameter is true
            if (verbose) {
                cout << "Connection detected: " << workflowLines[i] << '\n';
            }

        }
    }

    if (verbose && connections.size() == 0) {
        cout << "No connections where found";
    }

    return connections;
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
    /*try {
        vector<string> workflow;


        string fileName = "teste.wksp";

        workflow = GetLinesFromFile(fileName);

        for (int i = 0; i < workflow.size(); i++) {
            cout << "Linha " << i << ": '" << workflow[i] << "'\n";
        }

        vector<Comment> comments = ParseWorkflowComments(workflow, false);

        cout << "Found " << comments.size() << " comments in '" << fileName << '\'';
    }
    catch (const std::exception& e) {
        cout << "Error:" << e.what() << '\n';
    }*/
    #pragma endregion

    #pragma region Code example: Parse Workflow Global Variables
    /*try {

        vector<string> workflow;

        string fileName = "teste.wksp";

        workflow = GetLinesFromFile(fileName);

        vector<Variable> variables = ParseWorkflowGlobalVariables(workflow, true);
    }
    catch (const std::exception& e) {
        cout << "Error: " << e.what() << '\n';
    }*/
    #pragma endregion

#pragma region Code example: Parse Workflow Blocks
    try {
    
        vector<string> workflow;
    
        string fileName = "teste.wksp";
    
        workflow = GetLinesFromFile(fileName);
    
        vector<Block> blocks = ParseWorkflowBlocks(workflow, true);
    }
    catch (const std::exception& e) {
        cout << "Error: " << e.what() << '\n';
    }
#pragma endregion

    return 0;
}

