#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

#pragma region Get Execution Path
// Get the execution path
wstring ExePath() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    wstring::size_type pos = wstring(buffer).find_last_of(L"\\/");

    return wstring(buffer).substr(0, pos+1);
}
#pragma endregion

#pragma region Test funciton for return a vector
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
        throw std::runtime_error("Unable to open file");
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
    vector<string> file;
    
    try {
        file = GetLinesFromFile("teste.wksp");

        cout << "File read successfully\n";

        for (int i = 0; i < file.size(); i++) {
            cout << file[i] << '\n';
        }
    }
    catch (const std::exception &e) {
        cout << "Error: " << e.what() << '\n';
    }
#pragma endregion

    return 0;
}

