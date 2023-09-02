#include "WorkspaceBuilder.h"

using namespace std;

int main() {
#pragma region Code example: Parse Workflow 
    try {

        vector<string> workflowLines;

        vector<string> workflowLinesReparsed;

        string fileName = "demo.wksp";

        workflowLines = WorkspaceBuilder::SupportFunctions::GetLinesFromFile(fileName);

        WorkspaceBuilder::Structs::Workflow workflow = WorkspaceBuilder::Functions::ParseWorkflow(workflowLines, false);

        for (WorkspaceBuilder::Structs::Block block : workflow.blocks) {
            cout << "Block: " << block.id << endl;
            for (WorkspaceBuilder::Structs::Variable var : block.variables) {
                cout << "\tValue: " << var.value << endl;
                cout << "\tType: " << var.type << endl;
            }
        }

        workflowLinesReparsed = WorkspaceBuilder::Functions::ConvertWorkflowToVectorString(workflow);
        
        int saved = WorkspaceBuilder::SupportFunctions::SaveWkspfile("teste.wksp", workflowLinesReparsed);
        
        
        if (saved) {
            cout << "The file was saved";
        }
        else {
            cout << "The fila wasn't saved";
        }
    }
    catch (const std::exception& e) {
        cout << "Error: " << e.what() << '\n';
    }
#pragma endregion

    return 0;
}