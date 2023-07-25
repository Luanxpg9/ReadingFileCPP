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
    Image,
    Data
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
    // Output name
    string name;
    // Output type
    VariableType type;
};

// Input connection of a block
struct Input {
    // Input name
    string name;
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
    // hostMachine defines where will the block of code run (Under development: use localhost)
    string hostMachine;
    // The position in the 2d Grid
    Vector2 position;
    // Block variables like the input parameters
    vector<Variable> variables;
    // Input nodes in this block
    vector<Input> inputs;
    // Output nodes in this block
    vector<Output> outputs;
};

struct Connection {
    // Connection identificator
    int id;
    // The block in which the connection will begin
    int startBlock;
    // The output in which the connection will begin on that block
    string outputStartBlock;
    // The block in which the connection will finish
    int endBlock;
    // The input in which the connection will finish
    string inputEndBlock;
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

#pragma region Suport Functions

    #pragma region ToLower function
void ToLower(string& str) {
    for (int i = 0; i < str.size(); i++) {
        str[i] = tolower(str[i]);
    }
}

    #pragma endregion

    #pragma region StringToVariableType function
VariableType StringToVariableType(string type) {
    ToLower(type);

    if (type == "str" || type == "string")
        return VariableType::String;
    
    if (type == "int" || type == "integer")
        return VariableType::Integer;

    if (type == "double")
        return VariableType::Double;

    if (type == "img" || type == "image")
        return VariableType::Image;

    if (type == "data" || type == "byte")
        return VariableType::Data;

    return VariableType::String;
}
    #pragma endregion

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

    #pragma region ParseVariable function
Variable ParseVariable(string variable) {
    int separator = variable.find_first_of(' ');

    if (separator == 0) {
        throw std::runtime_error("Not a valid format. Format must be a string like 'MyVariable MyValue'");
    }

    Variable var;

    // Key and type will not be parsed
    var.key = variable.substr(0, separator);

    // Defines the variable type
    if (variable[separator + 1] == '\'') {
        var.type = VariableType::String;
    }
    else if (variable[separator + 1] == '[') {
        var.type = VariableType::Image;
    }
    else if (isdigit(variable[separator + 1])) {
        var.type = VariableType::Integer;
    }
    else {
        var.type = VariableType::String;
    }


    if (var.type == String) {
        // Start of value will be separator +2 so so remove the ' character we must add 1 and end up with three
        int endPosition = variable.size() - separator - 3;

        var.value = variable.substr(separator + 2, endPosition);
    }
    else {
        // Do not parse
        var.value = variable.substr(separator + 1, variable.size() - separator - 1);
    }

    return var;
}
#pragma endregion

    #pragma region ParseBlockLine function
Block ParseBlockLine(string line, bool verbose = false) {
    // Glyph line composition:
            //  Glyph tag > Lib > Function > hostmachine > Glyph Id > X position > Y position > args 

            // Init variables positions
    Vector2 functionNameIndex = { 0, 0 };
    Vector2 hostMachineIndex = { 0, 0 };
    Vector2 glyphIdIndex = { 0, 0 };
    Vector2 xPositionIndex = { 0, 0 };
    Vector2 yPositionIndex = { 0, 0 };
    Vector2 functionArgsIndex = { 0, 0 };
    int variableStart = 0;

    // Init variables
    int blockId = 0;
    string variableRaw;
    string blockType;
    string blockHost;
    Vector2 blockPosition = { 0, 0 };
    vector<Variable> blockVariables;

    int lineLength = line.size();


    for (int i = 13; i < lineLength; i++) {
        // Defines split pattern
        bool separator = line[i] == ':' && line[i - 1] != ':';
        bool doubleSeparator = line[i] == ':' && line[i+ 1] == ':';
        bool variableSeparator = line[i] == ' ' && line[i + 1] == '-';

        // Logs which character is being parsed
        if (verbose)
            cout << "Char " << i << ": " << line[i] << endl;

        // Start position of function name
        functionNameIndex.x = 13;

        // Defines Function name
        if (functionNameIndex.y == 0 && separator) {
            // End position of function name
            functionNameIndex.y = i;

            // The 13 number is the character count for 'Glyph:VGL_CL:'
            blockType = line.substr(functionNameIndex.x, functionNameIndex.y - 13);

            // Logs a detected function name
            if (verbose)
                cout << "\tFunction name>> " << blockType << endl;

            // Defines initial position for hostmachine
            if (doubleSeparator) {
                hostMachineIndex.x = i + 2;
            }
            else {
                hostMachineIndex.x = i + 1;
            }
        }
        // Defines hostmachine
        else if (hostMachineIndex.x != 0 && hostMachineIndex.y == 0 && separator) {
            // End position of hostmachine
            hostMachineIndex.y = i;

            blockHost = line.substr(hostMachineIndex.x, hostMachineIndex.y - hostMachineIndex.x);

            // Logs found hostname
            if (verbose)
                cout << "\tHostname>> " << blockHost << endl;

            // Defines initial position for GlyphId
            if (doubleSeparator) {
                glyphIdIndex.x = i + 2;
            }
            else {
                glyphIdIndex.x = i + 1;
            }
        }
        // Defines Glyph Id
        else if (glyphIdIndex.x != 0 && glyphIdIndex.y == 0 && separator) {
            // End position of GlyphId
            glyphIdIndex.y = i;

            // stoi convert string to int
            blockId = std::stoi(line.substr(glyphIdIndex.x, i - glyphIdIndex.x));

            // Logs found Glyph Id
            if (verbose)
                cout << "\tGlyph Id>> " << blockId << endl;

            // Defines initial position for X Position in grid
            if (doubleSeparator) {
                xPositionIndex.x = i + 2;
            }
            else {
                xPositionIndex.x = i + 1;
            }
        }
        // Defines Position X
        else if (xPositionIndex.x != 0 && xPositionIndex.y == 0 && separator) {
            // End position of X Position in grid
            xPositionIndex.y = i;

            blockPosition.x = std::stoi(line.substr(xPositionIndex.x, i - xPositionIndex.x));

            // Logs found X Position
            if (verbose)
                cout << "\tX Position>> " << blockPosition.x << endl;

            // Defines initial position for Y Position in grid
            if (doubleSeparator) {
                yPositionIndex.x = i + 2;
            }
            else {
                yPositionIndex.x = i + 1;
            }
        }
        // Defines Position Y
        else if (yPositionIndex.x != 0 && yPositionIndex.y == 0 && separator) {
            // End position of Y Position in grid
            yPositionIndex.y = i;

            blockPosition.y = std::stoi(line.substr(yPositionIndex.x, i - yPositionIndex.x));

            // Logs found Y Position on the grid
            if (verbose)
                cout << "\tY Position>> " << blockPosition.y << endl;
        }
        // Get block variables
        else if (line[i] == ' ' && line[i + 1] == '-' && variableStart == 0) {
            // Set the first variable start and used as a flag to warn that the following code will be variables
            variableStart = i + 2;
        }
        else if (variableSeparator && variableStart != 0) {
            // Set the first variable
            variableRaw = line.substr(variableStart, i - variableStart);

            // Add the found variable to the block variables list
            blockVariables.push_back(ParseVariable(variableRaw));

            // Logs the Function Variable
            if (verbose)
                cout << "\tFunction Variable>> " << line.substr(variableStart, i - variableStart) << endl;
            // Points the next variable start, if there is one
            variableStart = i + 2;
        }
        else if (i == lineLength - 1 && variableStart != 0) {
            variableRaw = line.substr(variableStart);

            // Add the las variable to the block variables list
            blockVariables.push_back(ParseVariable(variableRaw));

            // Logs the Last function variable
            if (verbose)
                cout << "\tFunction Variable>> " << line.substr(variableStart) << endl;
        }
    }

    vector<Input> unitializedInputs;
    vector<Output> unitializedOutputs;

    Block newBlock = {
        blockId,
        blockType,
        blockHost,
        blockPosition,
        blockVariables,
        unitializedInputs,
        unitializedOutputs
    };

    return newBlock;
}


#pragma endregion

    #pragma region ParseWorkflowBlocks function
vector<Block> ParseWorkflowBlocks(vector<string> workflowLines, bool verbose = false) {
    vector<Block> blocks;

    // Parsing workflow file 
    for (int i = 0; i < workflowLines.size(); i++) {
        if (workflowLines[i].substr(0, 5) == "Glyph") {

            if (verbose) 
                cout << "Found Glyph on line " << i + 1 << ">>> " << workflowLines[i] << endl;

            Block newBlock = ParseBlockLine(workflowLines[i], verbose);

            blocks.push_back(newBlock);
        }
    }

    return blocks;
}
#pragma endregion
 
    #pragma region ParseIOConnection functions
Output ParseOutput(string name, string variableType) {
    Output newOutput = {
        name,
        StringToVariableType(variableType)
    };

    return newOutput;
}

Input ParseInput(string name, string variableType) {
    Input newInput = {
        name,
        StringToVariableType(variableType)
    };

    return newInput;
}

#pragma endregion

    #pragma region ParseConnectionLine function
Connection ParseConnectionLine(string line, int id, vector<Block> blocks, bool verbose = false) {
    


    /* TODO
    * 1. Parse the line string and get the variables [X]
    * 2. Parse the variables into a Connection Node [X]
    * 3. Return the Connection [X]
    * 4. Insert into the blocks the inputs
    * 5. Insert into the blocks the outputs
    */

    Connection newConnection;

    int lineSize = line.size();

    char separator = ':';

    // Example NodeConnection:data:1:RETVAL:2:img
    // NodeConnection sinalizer >> Data type >> Glyph Id Output >> Glyph output name >> Glyph Id Input >> Glyph input name

    Vector2 dataType = { 0, 0 };
    Vector2 glyphOutId = { 0, 0 };
    Vector2 glyphOutput = { 0, 0 };
    Vector2 glyphInId = { 0, 0 };
    Vector2 glyphInput = { 0, 0 };

    // Connection variables
    string dataTypeConnection;
    string blockOutputIdString;
    int blockOutputId = 0;
    string blockOutputName;
    string blockInputIdString;
    int blockInputId = 0;
    string blockInputName;


    // 15 is where the variables start
    //  Example: NodeConnection:data:1:RETVAL:2:img and the 16 character is 'd'
    for (int i = 15; i < lineSize; i++) {
        bool isSeparator = line[i] == ':' || i == lineSize - 1;

        if (dataType.x == 0 && isSeparator) {
            dataType.x = 15;
            dataType.y = i;

            // Catch variable
            dataTypeConnection = line.substr(dataType.x, dataType.y - dataType.x);
            // To lower
            ToLower(dataTypeConnection);

            if (verbose)
                cout << "DataType = " << dataTypeConnection << endl;
            
        }
        else if (glyphOutId.x == 0 && isSeparator) {
            glyphOutId.x = dataType.y + 1;
            glyphOutId.y = i;

            // Catch string value
            blockOutputIdString = line.substr(glyphOutId.x, glyphOutId.y - glyphOutId.x);

            if (verbose)
                cout << "Glyph Output Id = " << blockOutputIdString << endl;

            // Convert to Integer
            blockOutputId = stoi(blockOutputIdString);
        }
        else if (glyphOutput.x == 0 && isSeparator) {
            glyphOutput.x = glyphOutId.y + 1;
            glyphOutput.y = i;

            blockOutputName = line.substr(glyphOutput.x, glyphOutput.y - glyphOutput.x);
            ToLower(blockOutputName);

            if (verbose)
                cout << "Glyph Output name = " << blockOutputName << endl;
        }
        else if (glyphInId.x == 0 && isSeparator) {
            glyphInId.x = glyphOutput.y + 1;
            glyphInId.y = i;

            blockInputIdString = line.substr(glyphInId.x, glyphInId.y - glyphInId.x);

            if (verbose)
                cout << "Glyph Input Id = " << blockInputIdString << endl;

            // Convert to Integer
            blockInputId = stoi(blockInputIdString);

        }
        else if (glyphInput.x == 0 && isSeparator) {
            glyphInput.x = glyphInId.y + 1;
            glyphInput.y = i;

            blockInputName = line.substr(glyphInput.x);
            ToLower(blockInputName);

            if (verbose)
                cout << "Glyph Input name = " << blockInputName << endl;
        }
    }

    // Parse connection
    newConnection = {
        id,
        blockOutputId,
        blockOutputName,
        blockInputId,
        blockInputName
    };

    return newConnection;
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
vector<Connection> ParseWorkflowConnections(vector<string> workflowLines, vector<Block> blocks, bool verbose = false) {

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
            Connection newConnection = ParseConnectionLine(workflowLines[i], connections.size(), blocks);

            connections.push_back(newConnection);

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

    #pragma region Code example: Parse variable
    /*string variableUnparsed = "winname 'demoConvolution'";

    Variable variableParsed = ParseVariable(variableUnparsed);

    cout << "Key>>" << variableParsed.key << "<<\nValue>>" << variableParsed.value << "<<\n";*/
    #pragma endregion

    #pragma region Code example: Parse Workflow Blocks
    try {
    
        vector<string> workflow;
    
        string fileName = "teste.wksp";
    
        workflow = GetLinesFromFile(fileName);
    
        vector<Block> blocks = ParseWorkflowBlocks(workflow);
    }
    catch (const std::exception& e) {
        cout << "Error: " << e.what() << '\n';
    }
    #pragma endregion

    #pragma region Code example: Parse Workflow 
    try {

        vector<string> workflow;

        string fileName = "teste.wksp";

        workflow = GetLinesFromFile(fileName);

        vector<Block> blocks = ParseWorkflowBlocks(workflow);

        // Debugging ParseConnectionLine
        //Connection connection = ParseConnectionLine("NodeConnection:data:1:RETVAL:2:img", 1, blocks, true);

        vector<Connection> connections = ParseWorkflowConnections(workflow, blocks);

        for (Connection c : connections) {
            cout << "Connection: " << c.id +1 << " | Line start: " << c.startBlock << " | Line end: " << c.endBlock << endl;
        }
    }
    catch (const std::exception& e) {
        cout << "Error: " << e.what() << '\n';
    }
    #pragma endregion

    return 0;
}

