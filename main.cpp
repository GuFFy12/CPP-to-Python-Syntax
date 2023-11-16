#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <regex>

void handleError(const std::string &message) {
    std::cerr << "Error: " << message << std::endl;
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        handleError("Usage: " + std::string(argv[0]) + " <inputFile> <outputFile> <shiftSyntaxCharsBy>");
    }

    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];

    int shiftSyntaxCharsBy;
    try {
        shiftSyntaxCharsBy = std::stoi(argv[3]);
        if (shiftSyntaxCharsBy < 0) {
            handleError("shiftSyntaxCharsBy must be non-negative.");
        }
    } catch (const std::invalid_argument &e) {
        handleError("Invalid argument for value: " + std::string(e.what()));
    } catch (const std::out_of_range &e) {
        handleError("Value out of range: " + std::string(e.what()));
    }

    std::ifstream inputFile(inputFilename);
    if (!inputFile.is_open()) {
        handleError("Unable to open the input file.");
    }

    std::istringstream inputStringStream((std::string(std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>())));
    inputFile.close();

    std::string output;

    std::regex lineRegex(R"(^(\s*)([{};]*)(.*?)([{};]*)(\s*)(\/\/.*|$))");
    for (std::string line; getline(inputStringStream, line);) {
        std::smatch lineMatch;
        if (regex_search(line, lineMatch, lineRegex)) {
            std::ssub_match startWhitespacesMatch = lineMatch[1];
            std::ssub_match startSyntaxMatch = lineMatch[2];
            std::string codeMatch = lineMatch[3].str();
            std::ssub_match endSyntaxMatch = lineMatch[4];
            std::ssub_match startCommentWhitespacesMatch = lineMatch[5];
            std::ssub_match commentMatch = lineMatch[6];

            if (startSyntaxMatch.length()) {
                output.pop_back();
                output += startSyntaxMatch;

                if (codeMatch.length()) {
                    output += '\n';
                    codeMatch.erase(0, 1);
                }
            }

            if (commentMatch.length()) {
                if (endSyntaxMatch.length()) {
                    codeMatch += startCommentWhitespacesMatch.str() + commentMatch.str().replace(0, 2, "/*") + " */";
                } else {
                    output += startWhitespacesMatch.str() + commentMatch.str();
                }
            }

            output += startWhitespacesMatch.str() + codeMatch +
                      std::string(shiftSyntaxCharsBy - (startWhitespacesMatch.length() + codeMatch.length()), ' ') + endSyntaxMatch.str();
        }

        output += '\n';
    }

    std::ofstream outputFile(outputFilename);
    if (!outputFile.is_open()) {
        handleError("Unable to open the output file.");
    }
    outputFile << output;
    outputFile.close();

    return 0;
}
