#include "WorkspaceBuilder.h"

namespace WorkspaceReader {
	namespace SupportFunctions {
		void ToLower(std::string& str) {
			for (int i = 0; i < str.size(); i++) {
				str[i] = tolower(str[i]);
			}
		}


        WorkspaceBuilder::Enums::VariableType StringToVariableType(std::string type) {
            ToLower(type);

            if (type == "str" || type == "string")
                return WorkspaceBuilder::Enums::VariableType::String;

            if (type == "int" || type == "integer")
                return WorkspaceBuilder::Enums::VariableType::Integer;

            if (type == "double")
                return WorkspaceBuilder::Enums::VariableType::Double;

            if (type == "img" || type == "image")
                return WorkspaceBuilder::Enums::VariableType::Image;

            if (type == "data" || type == "byte")
                return WorkspaceBuilder::Enums::VariableType::Data;

            return WorkspaceBuilder::Enums::VariableType::String;
        }

        int CompareString(const std::string& in1,const std::string& in2, bool verbose = false) {
            int res = in1.compare(in2);

            if (res == 0) {
                if (verbose) std::cout << "The strings are the same";
                return 1;
            }
            else {
                if (verbose) std::cout << "The strings don't match";
                return 0;
            }
        }

        std::vector<std::string> GetLinesFromFile(const std::string& path) {
            std::ifstream myFile(path);
            std::vector<std::string> myLines;
            std::string myLine;

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
	}

    namespace Functions {
        
        std::vector<WorkspaceBuilder::Structs::Comment> ParseWorkflowComments(const std::vector<std::string>& workflowLines, bool verbose = false) {
            // Throw error if there is no line
            //      Prevents from initializing vectors and structs
            if (workflowLines.empty()) {
                throw std::runtime_error("ParseWorkflowComments error >> Workflow lines are empty");
            }

            // Initialize vector of comments for return
            std::vector<WorkspaceBuilder::Structs::Comment> comments;
            // Initialize a position when the workflow file does not specifie where it should be in the Visual Workflow Editor
            WorkspaceBuilder::Structs::Vector2 nullPosition = { 0.0, 0.0 };

            // Loop for parsing the file
            for (int i = 0; i < workflowLines.size(); i++) {
                if (workflowLines[i][0] == '#') {
                    // Get Comment without the '#' character
                    std::string comment = workflowLines[i].substr(1, workflowLines[i].length());

                    // Initialize the Comment struct for appending in the vector of comments
                    WorkspaceBuilder::Structs::Comment newComment = {
                        i + 1,
                        comment,
                        nullPosition
                    };

                    // Insert new comment into the comment vector
                    comments.push_back(newComment);

                    // Print function comments if verbose parameter is true
                    if (verbose) {
                        std::cout << "Comment detected: \n" << "\t-> Line: " << i + 1 << "\n\t-> Comment: " << comment << '\n';
                    }
                }
            }

            return comments;
        }

        std::vector<WorkspaceBuilder::Structs::Variable> ParseWorkflowGlobalVariables(std::vector<std::string>& workflowLines, bool verbose = false) {
            // List of variables for return
            std::vector<WorkspaceBuilder::Structs::Variable> variables;
            // Flag active when the VariableBegin is found
            bool isVariableParserUp = false;

            // Loop for parsing file
            for (int i = 0; i < workflowLines.size(); i++) {

                // Line by line logging
                if (verbose) {
                    std::cout << "Workflow line being parsed> '" << workflowLines[i] << "'\n";
                }

                // Sinalizes that the search for variables has ended
                if (workflowLines[i] == "VariablesEnd:") {

                    // Finished function
                    if (verbose) {
                        std::cout << "Finished parsing global variables\n\n";
                    }
                    isVariableParserUp = false;
                    break;
                }

                // Formating variable
                if (isVariableParserUp && workflowLines[i][0] != '\n' && workflowLines[i][0] != ' ' && workflowLines[i][0] != '#' && workflowLines[i][0] != '\0') {

                    // To test: maybe a for will be faster than using find two times for getting indexes
                    std::string variableKey = workflowLines[i].substr(0, workflowLines[i].find_first_of(' '));
                    std::string variableValue = workflowLines[i].substr(workflowLines[i].find_first_of('=') + 2, workflowLines[i].size());

                    WorkspaceBuilder::Enums::VariableType type = WorkspaceBuilder::Enums::VariableType::Integer;

                    WorkspaceBuilder::Structs::Variable newVariable = {
                        variableKey,
                        variableValue,
                        type
                    };

                    // Found variable log
                    if (verbose) {
                        std::cout << "\t\tNew variable found: \n" << "\t\t\t->Key: '" << variableKey << "'\n" << "\t\t\t->Value: '" << variableValue << "'\n\n";
                    }

                    variables.push_back(newVariable);
                }

                // Sinalizes that the search for variables has begun
                // Must be in the end for the parser code doesn't catch the 'VariablesBegin:'
                if (workflowLines[i] == "VariablesBegin:") {
                    // Finished function
                    if (verbose) {
                        std::cout << "Starting parsing global variables\n\n";
                    }

                    isVariableParserUp = true;
                }
            }

            // Return found variables
            return variables;
        }

        WorkspaceBuilder::Structs::Variable ParseVariable(const std::string& variable) {
            int separator = variable.find_first_of(' ');

            if (separator == 0) {
                throw std::runtime_error("Parse Variable error >> Not a valid format. Format must be a string like 'MyVariable MyValue'");
            }

            WorkspaceBuilder::Structs::Variable var;

            // Key and type will not be parsed
            var.key = variable.substr(0, separator);

            // Defines the variable type
            if (variable[separator + 1] == '\'') {
                var.type = WorkspaceBuilder::Enums::VariableType::String;
            }
            else if (variable[separator + 1] == '[') {
                var.type = WorkspaceBuilder::Enums::VariableType::Image;
            }
            else if (isdigit(variable[separator + 1])) {
                var.type = WorkspaceBuilder::Enums::VariableType::Integer;
            }
            else {
                var.type = WorkspaceBuilder::Enums::VariableType::String;
            }


            if (var.type == WorkspaceBuilder::Enums::String) {
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

        WorkspaceBuilder::Structs::Block ParseBlockLine(const std::string& line, bool verbose = false) {
            // Glyph line composition:
                    //  Glyph tag > Lib > Function > hostmachine > Glyph Id > X position > Y position > args 

            // Init variables positions
            WorkspaceBuilder::Structs::Vector2 functionNameIndex = { 0, 0 };
            WorkspaceBuilder::Structs::Vector2 hostMachineIndex = { 0, 0 };
            WorkspaceBuilder::Structs::Vector2 glyphIdIndex = { 0, 0 };
            WorkspaceBuilder::Structs::Vector2 xPositionIndex = { 0, 0 };
            WorkspaceBuilder::Structs::Vector2 yPositionIndex = { 0, 0 };
            WorkspaceBuilder::Structs::Vector2 functionArgsIndex = { 0, 0 };
            int variableStart = 0;

            // Init variables
            int blockId = 0;
            std::string variableRaw;
            std::string blockType;
            std::string blockHost;
            WorkspaceBuilder::Structs::Vector2 blockPosition = { 0, 0 };
            std::vector<WorkspaceBuilder::Structs::Variable> blockVariables;

            int lineLength = line.size();


            for (int i = 13; i < lineLength; i++) {
                // Defines split pattern
                bool separator = line[i] == ':' && line[i - 1] != ':';
                bool doubleSeparator = line[i] == ':' && line[i + 1] == ':';
                bool variableSeparator = line[i] == ' ' && line[i + 1] == '-';

                // Logs which character is being parsed
                if (verbose)
                    std::cout << "Char " << i << ": " << line[i] << std::endl;

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
                        std::cout << "\tFunction name>> " << blockType << std::endl;

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
                        std::cout << "\tHostname>> " << blockHost << std::endl;

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
                        std::cout << "\tGlyph Id>> " << blockId << std::endl;

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
                        std::cout << "\tX Position>> " << blockPosition.x << std::endl;

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
                        std::cout << "\tY Position>> " << blockPosition.y << std::endl;
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
                        std::cout << "\tFunction Variable>> " << line.substr(variableStart, i - variableStart) << std::endl;
                    // Points the next variable start, if there is one
                    variableStart = i + 2;
                }
                else if (i == lineLength - 1 && variableStart != 0) {
                    variableRaw = line.substr(variableStart);

                    // Add the las variable to the block variables list
                    blockVariables.push_back(ParseVariable(variableRaw));

                    // Logs the Last function variable
                    if (verbose)
                        std::cout << "\tFunction Variable>> " << line.substr(variableStart) << std::endl;
                }
            }

            std::vector<WorkspaceBuilder::Structs::Input> unitializedInputs;
            std::vector<WorkspaceBuilder::Structs::Output> unitializedOutputs;

            WorkspaceBuilder::Structs::Block newBlock = {
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

        std::vector<WorkspaceBuilder::Structs::Block> ParseWorkflowBlocks(const std::vector<std::string>& workflowLines, bool verbose = false) {
            std::vector<WorkspaceBuilder::Structs::Block> blocks;

            // Parsing workflow file 
            for (int i = 0; i < workflowLines.size(); i++) {
                if (workflowLines[i].substr(0, 5) == "Glyph") {

                    if (verbose)
                        std::cout << "Found Glyph on line " << i + 1 << ">>> " << workflowLines[i] << std::endl;

                    WorkspaceBuilder::Structs::Block newBlock = ParseBlockLine(workflowLines[i], verbose);

                    blocks.push_back(newBlock);
                }
            }

            return blocks;
        }

        WorkspaceBuilder::Structs::Output ParseOutput(const std::string& name, std::string variableType) {
            WorkspaceBuilder::Structs::Output newOutput = {
                name,
                WorkspaceBuilder::SupportFunctions::StringToVariableType(variableType)
            };

            return newOutput;
        }

        WorkspaceBuilder::Structs::Input ParseInput(const std::string& name, std::string variableType) {
            WorkspaceBuilder::Structs::Input newInput = {
                name,
                WorkspaceBuilder::SupportFunctions::StringToVariableType(variableType)
            };

            return newInput;
        }

        WorkspaceBuilder::Structs::Connection ParseConnectionLine(const std::string& line, int id, const std::vector<WorkspaceBuilder::Structs::Block>& blocks, bool verbose = false) {
            WorkspaceBuilder::Structs::Connection newConnection;

            int lineSize = line.size();

            char separator = ':';

            // Example NodeConnection:data:1:RETVAL:2:img
            // NodeConnection sinalizer >> Data type >> Glyph Id Output >> Glyph output name >> Glyph Id Input >> Glyph input name

            WorkspaceBuilder::Structs::Vector2 dataType = { 0, 0 };
            WorkspaceBuilder::Structs::Vector2 glyphOutId = { 0, 0 };
            WorkspaceBuilder::Structs::Vector2 glyphOutput = { 0, 0 };
            WorkspaceBuilder::Structs::Vector2 glyphInId = { 0, 0 };
            WorkspaceBuilder::Structs::Vector2 glyphInput = { 0, 0 };

            // Connection variables
            std::string dataTypeConnection;
            std::string blockOutputIdString;
            int blockOutputId = 0;
            std::string blockOutputName;
            std::string blockInputIdString;
            int blockInputId = 0;
            std::string blockInputName;


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
                    WorkspaceBuilder::SupportFunctions::ToLower(dataTypeConnection);

                    if (verbose)
                        std::cout << "DataType = " << dataTypeConnection << std::endl;

                }
                else if (glyphOutId.x == 0 && isSeparator) {
                    glyphOutId.x = dataType.y + 1;
                    glyphOutId.y = i;

                    // Catch string value
                    blockOutputIdString = line.substr(glyphOutId.x, glyphOutId.y - glyphOutId.x);

                    if (verbose)
                        std::cout << "Glyph Output Id = " << blockOutputIdString << std::endl;

                    // Convert to Integer
                    blockOutputId = stoi(blockOutputIdString);
                }
                else if (glyphOutput.x == 0 && isSeparator) {
                    glyphOutput.x = glyphOutId.y + 1;
                    glyphOutput.y = i;

                    blockOutputName = line.substr(glyphOutput.x, glyphOutput.y - glyphOutput.x);
                    WorkspaceBuilder::SupportFunctions::ToLower(blockOutputName);

                    if (verbose)
                        std::cout << "Glyph Output name = " << blockOutputName << std::endl;
                }
                else if (glyphInId.x == 0 && isSeparator) {
                    glyphInId.x = glyphOutput.y + 1;
                    glyphInId.y = i;

                    blockInputIdString = line.substr(glyphInId.x, glyphInId.y - glyphInId.x);

                    if (verbose)
                        std::cout << "Glyph Input Id = " << blockInputIdString << std::endl;

                    // Convert to Integer
                    blockInputId = stoi(blockInputIdString);

                }
                else if (glyphInput.x == 0 && isSeparator) {
                    glyphInput.x = glyphInId.y + 1;
                    glyphInput.y = i;

                    blockInputName = line.substr(glyphInput.x);
                    WorkspaceBuilder::SupportFunctions::ToLower(blockInputName);

                    if (verbose)
                        std::cout << "Glyph Input name = " << blockInputName << std::endl;
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

        std::vector<WorkspaceBuilder::Structs::Connection> ParseWorkflowConnections(const std::vector<std::string>& workflowLines, const std::vector<WorkspaceBuilder::Structs::Block>& blocks, bool verbose = false) {
            // Throw error if there is no line
            //      Prevents from initializing vectors and structs
            if (workflowLines.empty()) {
                throw std::runtime_error("ParseWorkflowConnections error >> Workflow lines are empty");
            }

            // Initialize vector of comments for return
            std::vector<WorkspaceBuilder::Structs::Connection> connections;

            // Loop for parsing the file
            for (int i = 0; i < workflowLines.size(); i++) {
                if (workflowLines[i].substr(0, 14) == "NodeConnection") {

                    // Parssing connection
                    WorkspaceBuilder::Structs::Connection newConnection = ParseConnectionLine(workflowLines[i], connections.size(), blocks);

                    connections.push_back(newConnection);

                    // Print function comments if verbose parameter is true
                    if (verbose) {
                        std::cout << "Connection detected: " << workflowLines[i] << '\n';
                    }

                }
            }

            if (verbose && connections.size() == 0) {
                std::cout << "No connections where found";
            }

            return connections;
        }

        void DistributeInputAndOutput(const std::vector<WorkspaceBuilder::Structs::Connection>& connections, std::vector<WorkspaceBuilder::Structs::Block>& blocks) {

            // Percorrer a lista de connecçoes e para cara connecao adicionar a ligacao de input e output
            WorkspaceBuilder::Structs::Connection connection;

            for (WorkspaceBuilder::Structs::Connection connection : connections) {
                // adicionar o input no bloco correspondente

                std::string functionName = blocks.at(connection.startBlock).type;
                std::cout << "Start block: " << connection.startBlock << "\t VGL function: " << functionName << std::endl;

                functionName = blocks.at(connection.endBlock).type;
                std::cout << "End block: " << connection.endBlock << "\t VGL function: " << functionName << std::endl;
            }

        }
    }
}