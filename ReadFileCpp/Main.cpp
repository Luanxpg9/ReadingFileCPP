#include "WorkspaceBuilder.h"

using namespace std;

int main() {
#pragma region Code example: Parse Workflow 
    try {

        vector<string> workflowLines;

        vector<string> workflowLinesReparsed;

        string fileName = "teste.wksp";

        workflowLines = WorkspaceBuilder::SupportFunctions::GetLinesFromFile(fileName);

        WorkspaceBuilder::Structs::Workflow workflow = WorkspaceBuilder::Functions::ParseWorkflow(workflowLines, false);

        workflowLinesReparsed = WorkspaceBuilder::Functions::ConvertWorkflowToVectorString(workflow);

        for (string line : workflowLinesReparsed) {
            cout << line << endl;
        }

    }
    catch (const std::exception& e) {
        cout << "Error: " << e.what() << '\n';
    }
#pragma endregion

    return 0;
}