#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
#include <list>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <regex>

#define ll long long

void unzipFile(std::string filePath, std::string outputFilePath) {
	system(("7za e " + filePath + " -o" + outputFilePath + " -aos").c_str());
}
/*
std::string getMetadataFromFile(char* file) {
    std::ifstream input(file);
    if(!input.good()){
        std::cerr << "Error opening file" << std::endl;
        return nullptr;
    }

    std::string line, content;
    while(std::getline(input, line).good()) {
        if(line.empty() || line[0] == '>') {
            content += line;
        }
    }

    return content;
}*/

std::string getSequenceFromFile(char* file) {
    std::ifstream input(file);
    if(!input.good()){
        std::cerr << "Error opening file" << std::endl;
        return nullptr;
    }

    std::string line, content;
    while(std::getline(input, line).good()) {
        // skip comment line and return content if empty or comment line is read
        if(line.empty() || line[0] == '>') {
            if (!content.empty()) {
                std::cout << "Loaded file. [" << content.length() << " BP]" << std::endl;
                return content;
            } else {
                std::cout << "Loading: " << line.substr(1, 50) << "..." << std::endl;
            }
        } else {
            if (!line.empty())
                content += line;
        }
    }

    if (!content.empty()) {
        std::cout << "Loaded file. [" << content.length() << " BP]" << std::endl;
        return content;
    } else {
        return nullptr;
    }    
}

void writeToFile(std::string filename, std::string text, bool newLine=true) {
    std::ofstream file;

    std::cout << "Writing: '" << text << "' to file: " << filename << "." << std::endl;
    file.open(filename, std::ofstream::out|std::ofstream::app);

    if(!file) {
        std::cout << "Error in file creation!" << std::endl;
        return;
    } else {
        std::cout << "File created/opened successfully!" << std::endl;
    }

    file << text;

    if (newLine)
        file << "\n";

    file.close();
}

void reconstruct(std::string outputFile, const std::string& intermFile, std::string referenceFile) {
    std::vector<std::string> lines;

    std::stringstream ss(intermFile);
    std::string line;
    while(std::getline(ss, line, '\n')) {
        lines.push_back(line);
    }

    std::string target = "";
    int begin, length;
    int end = 0;

    std::cout << "Reference: \n" << referenceFile << std::endl;

    for(auto line : lines) {
        if (line[0] == '>') {
            writeToFile(outputFile, line);
        } else if (line.empty()) {
            writeToFile(outputFile, "");
        } else {
            //contains ,
            if (line.find(",") != std::string::npos) {
                begin = std::stoi(line.substr(0, line.find(",")));
                length = std::stoi(line.substr(line.find(",") + 1, line.length() - line.find(",") - 1));
                //std::cout << "Begin: " << begin << ", Length: " << length << std::endl;
                target += referenceFile.substr(begin + end, length);
                end += begin + length;
                
            } else {
                target += line.substr(0, line.length() - 1);
            }
        }
    }

    std::cout << "Target:\n" << target << std::endl;
}

/* COPY PASTE s https://www.delftstack.com/howto/cpp/read-file-into-string-cpp/ */
std::string readFileIntoString(const std::string& path) {
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        std::cerr << "Could not open the file - '"
             << path << "'" << std::endl;
        exit(EXIT_FAILURE);
    }
    return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

// u reconstruct koristeno https://www.techiedelight.com/split-a-string-on-newlines-in-cpp/

int main( int argc, char **argv){
	if (argc <= 3) {
        std::cerr << "Missing argument." << std::endl;
        return -1;
    }
	
    std::string referenceSequencePath = argv[1];
    std::string compressedFilePath = argv[2];
    std::string finalFolder = argv[3];

    unzipFile(compressedFilePath, "../data/resultsd/decompressed");
    std::string filename("../data/resultsd/decompressed/test.txt");

    std::string compressedFile = readFileIntoString(filename);
	std::cout << compressedFile << std::endl;

    //filename(referenceSequencePath);

    
    //std::string referenceFile = readFileIntoString(referenceSequencePath);
    std::string referenceSequence = getSequenceFromFile(argv[1]);
    //referenceFile = referenceFile.substr(referenceFile.find("\n") - 2, referenceFile.length());

    std::string output = "../data/resultsd/result.fa";

    reconstruct(output, compressedFile, referenceSequence);

	return 0;
}