#include "WorkspaceBuilder.h"

using namespace std;

int main() {
#pragma region Code example: Parse Workflow 
    try {

        vector<string> workflowLines;

        string fileName = "teste.wksp";

        workflowLines = WorkspaceBuilder::SupportFunctions::GetLinesFromFile(fileName);

        WorkspaceBuilder::Structs::Workflow workflow = WorkspaceBuilder::Functions::ParseWorkflow(workflowLines, true);

    }
    catch (const std::exception& e) {
        cout << "Error: " << e.what() << '\n';
    }
#pragma endregion

    return 0;
}