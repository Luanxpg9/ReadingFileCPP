#include <iostream>
#include <windows.h>
#include <vector>
#include <fstream>

using namespace std;

wstring ExePath() {
    // Get the execution path
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    wstring::size_type pos = wstring(buffer).find_last_of(L"\\/");

    return wstring(buffer).substr(0, pos+1);
}

vector<string> ReturnVectorTest() {
    vector<string> test;

    test.push_back("test1");
    test.push_back("test2");
    test.push_back("test3");

    return test;
}

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

vector<string> ExplorerFolder(string path) {
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


/*
int main()
{
    #pragma region Compare String
    /*
    string input1;
    string input2;

    cout << "Please enter a string to compare: \n";
    cin >> input1;

    cout << "Please enter another string to compare: \n";
    cin >> input2;

    CompareString(input1, input2);
    */
    #pragma endregion

    #pragma region Return vector
    /*
    vector<string> test = ReturnVectorTest();
    cout << "Returned vector:" << endl;

    for (int i = 0; i < test.size(); i++) {
        cout << test[i] << endl;
    }*/
    #pragma endregion

    #pragma region Get File Path
    /*
    wstring path = ExePath();
    string pathS(path.begin(), path.end());
    
    //cout << "The current execution path is: \n" << pathS;
    #pragma endregion
    
    #pragma region Folder explorer - Windows
    vector<string> fileNames;
    
    fileNames = ExplorerFolder(pathS);

    for (int i = 0; i < fileNames.size(); i++) {
        cout << i << ": " << fileNames[i] << endl;
    }

    #pragma endregion

    #pragma region Read text file
    string file;
    fstream myFile;

    myFile.open("teste.txt", ios::in);

    if (!myFile) {
        cout << "Error reading file";
    }
    else {
        char ch;
        fstream infile;
        string line;



        while (1) {
            myFile >> ch;
            if (myFile.eof())
                break;

            //myFile.getline(line, STRING);
        }
    }

    myFile.close();


    #pragma endregion

    return 1;
}

*/