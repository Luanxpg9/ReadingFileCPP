#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

namespace WorkspaceBuilder {
    #pragma region Enums
    namespace Enums {
        // Describes the variable type of a vgl variable. It can be used eather as a parameter or a variable type.
        enum VariableType {
            Integer,
            Double,
            String,
            Image,
            Data
        };
    }
    #pragma endregion

    #pragma region Structs
    namespace Structs {
        // Variables can be global or inside a block
        struct Variable {
            std::string key;
            std::string value;
            WorkspaceBuilder::Enums::VariableType type;
        };

        // Used to store 2: floats X and Y
        struct Vector2 {
            float x;
            float y;
        };

        // Comments that are shown in the workflow file
        struct Comment {
            // Line position in the workflow file
            int line;
            // The text that a commentary has
            std::string text;
            // Grid position in the User interface
            Vector2 position;
        };

        // Output connection of a block
        struct Output {
            // Output name
            std::string name;
            // Output type
            WorkspaceBuilder::Enums::VariableType type;
        };

        // Input connection of a block
        struct Input {
            // Input name
            std::string name;
            // Input type
            WorkspaceBuilder::Enums::VariableType type;
        };

        // A block defines a VGL function, Its parameters, variables, input and output
        struct Block {
            // Identificator
            int id;
            // Used to define what VGL function it represents
            std::string type;
            // hostMachine defines where will the block of code run (Under development: use localhost)
            std::string hostMachine;
            // The position in the 2d Grid
            Vector2 position;
            // Block variables like the input parameters
            std::vector<Variable> variables;
            // Input nodes in this block
            std::vector<Input> inputs;
            // Output nodes in this block
            std::vector<Output> outputs;
        };

        // Describes a connection beteween 2 nodes
        struct Connection {
            // Connection identificator
            int id;
            // The block in which the connection will begin
            int startBlock;
            // The output in which the connection will begin on that block
            std::string outputStartBlock;
            // The block in which the connection will finish
            int endBlock;
            // The input in which the connection will finish
            std::string inputEndBlock;
        };

        // Represents a workflow file in a struct pattern.
        struct Workflow {
            // Variables that can be used in any function/block
            std::vector<Variable> globalVariables;
            // The blocks represents a VGL function that is called in the workflow
            std::vector<Block> blocks;
            // The connections between the blocks inside a workflow. The work like passing futher an variable to another block
            std::vector<Connection> connections;
            // A user can specify comments for a workflow that can be used to make a workflow more human readable
            std::vector<Comment> comments;
        };
    }
    #pragma endregion

    #pragma region Support Functions
    namespace SupportFunctions {

        /**
        * Converts a string to It's lower case version
        *
        * @param str: String address of the variable to be converted
        */
        void ToLower(std::string& str);

        /**
        * Converts a string to a It's workflow enum type representative
        *
        * @param type: Address with the variable type string
        * @return A representation of the datatype as a VGL workspace VariableType
        */
        WorkspaceBuilder::Enums::VariableType StringToVariableType(std::string type);

        /**
        * Check if the strings are the same
        *
        * @param in1: First string
        * @param in2: Second string
        * @param verbose: If true prints in the console if the string is the same or not.
        * @return 1 - Strings are the same || 0 - Strings are different
        */
        int CompareString(const std::string& in1,const std::string& in2, bool verbose = false);

        /**
        * Separete each line in a vector of strings on a wksp file
        *
        * @param path: The string address of a valid file
        * @param verbose: If true prints in the console the comments found in the file. Default = false.
        * @return A vector of string containing each line on a wksp file
        *
        * @throws Unable to open file> if the given path is invalid
        */
        std::vector<std::string> GetLinesFromFile(const std::string& path);

        /**
        * Push back endline characters in the string vector.
        *
        * @param stringVector: The string vector where the endlines will be add
        * @param endlineCount: The number of endlines to be inserted.
        *
        */
        void PushEndline(std::vector<std::string>& stringVector, int endlineCount);
    }
    #pragma endregion

    namespace Functions {
        /**
        * Get workflow comment lines in a .wksp file
        *
        * @param workflowLines A vector of lines containing the workflow file's lines.
        * @verbose If true prints in the console the comments found in the file.
        * @return A vector of comments
        *
        * @throws runtime_error if workflowLines is empty
        */
        std::vector<WorkspaceBuilder::Structs::Comment> ParseWorkflowComments(const std::vector<std::string>& workflowLines, bool verbose = false);

        /**
        * Get workflow global variables in a .wksp file
        *
        * @param workflowLines: A vector of lines containing the workflow file's lines.
        * @param verbose: If true prints in the console the comments found in the file. Default = false
        * @return A vector of comments
        *
        * @throws invalid string position> if a variable is declared not folling the pattern 'key value'
        */
        std::vector<WorkspaceBuilder::Structs::Variable> ParseWorkflowGlobalVariables(const std::vector<std::string>& workflowLines, bool verbose = false);

        /**
        * Convert a string with "key value" to It's VGL workspace equivalent 
        *
        * @param variable: String address with the variable to be parsed.
        * @return A vector of variables
        *
        * @throws Not a valid format > if a variable is declared and is not following the pattern 'key value'
        */
        WorkspaceBuilder::Structs::Variable ParseVariable(const std::string& variable);

        /**
        * Parses a string represinting a block to It's VGL workspace structure.
        * Block line composition:
        *   Block_tag:Lib:Function:Hostmachine:Block_Id:X_position:Y_position args 
        *
        * @param line: String address with the represented glyph to be parsed. 
        * @param verbose: If true prints in the console what the program is parsing. Default = false
        * @return A VGL block structure
        *
        * @throws Not a valid format > if a variable is declared and is not following the pattern 'key value' 
        */
        WorkspaceBuilder::Structs::Block ParseBlockLine(const std::string& line, bool verbose = false);

        /**
        * Parses the workflow and identify the block lines and parses them
        *
        * @param workflowLines: A vector of string with each string representing a line on a workspace file.
        * @param verbose: If true prints in the console what the program is parsing. Default = false
        * @return A list of blocks in a workspace file
        *
        * @throws Not a valid format > if a variable is declared and is not following the pattern 'key value'
        */
        std::vector<WorkspaceBuilder::Structs::Block> ParseWorkflowBlocks(const std::vector<std::string>& workflowLines, bool verbose = false);

        /**
        * Represents a Output variable of a block
        *
        * @param name: The output variable name.
        * @param variableType: The string representing the type of a variable
        * @return A representation of that output in a VGL structure
        * 
        */
        WorkspaceBuilder::Structs::Output ParseOutput(const std::string& name, std::string variableType);

        /**
        * Represents a Input variable of a block
        *
        * @param name: The input variable name.
        * @param variableType: The string representing the type of a variable
        * @return A representation of that input in a VGL structure
        *
        */
        WorkspaceBuilder::Structs::Input ParseInput(const std::string& name, std::string variableType);

        /**
        * Parses a string represinting a connection between 2 blocks.
        *
        * @param line: String address with the represented connection to be parsed.
        * @param verbose: If true prints in the console what the program is parsing. Default = false
        * @return A VGL connection structure
        */
        WorkspaceBuilder::Structs::Connection ParseConnectionLine(const std::string& line, int id,const std::vector<WorkspaceBuilder::Structs::Block>& blocks, bool verbose = false);

        /**
        * Parses the workflow and identify the connection lines and parses them
        *
        * @param workflowLines: A vector of string with each string representing a line on a workspace file.
        * @param blocks: A list of blocks in a workspace file
        * @param verbose: If true prints in the console what the program is parsing. Default = false
        * @return A VGL connection structure
        */
        std::vector<WorkspaceBuilder::Structs::Connection> ParseWorkflowConnections(const std::vector<std::string>& workflowLines, const std::vector<WorkspaceBuilder::Structs::Block>& blocks, bool verbose = false);

        /**
        * Convert a vector<string> in a workflow structure
        *
        * @param workflowLines: A vector of string with each string representing a line on a workspace file.
        * @param verbose: If true prints in the console what the program is parsing. Default = false
        * @return A VGL Workflow structure
        */
        WorkspaceBuilder::Structs::Workflow ParseWorkflow(const std::vector<std::string>& workflowLines, bool verbose = false);

        std::vector<std::string> ConvertWorkflowToVectorString(const WorkspaceBuilder::Structs::Workflow& workflow, bool verbose = false);
    }
}