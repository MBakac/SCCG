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

/**
 * Function creates/opens a file and writes to it.
 * 
 * @author Marta Bonacin 
 * 
 * @param filename Name of the file to write in.
 * @param text String which will be written in the file.
*/
void writeToFile(std::string filename, std::string text, bool newLine=true) {
    std::ofstream file;

    //std::cout << "Writing: '" << text << "' to file: " << filename << "." << std::endl;
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

/**
 * Function receives file name and clears the file.
 * 
 * @author Marta Bonacin
 * 
 * @param filename The name of the file to be cleared.
*/
void clearFile(std::string filename) {
    std::ofstream file;
    std::cout << "Clearing file" << std::endl;
    file.open(filename, std::ofstream::out|std::ofstream::trunc);
}

/**
 * 
 * 
 * @author Martin Bakac
 * 
 * @param filePath 
 * @param outputFilePath 
*/
void unzipFile(std::string filePath, std::string outputFilePath) {
	system(("7za e " + filePath + " -o" + outputFilePath + " -aos").c_str());
}

/**
 * Function turns input FASTA file into string
 * 
 * Modified code from https://rosettacode.org/wiki/FASTA_format#C++ 
 * 
 * @author Martin Bakac
 * 
 * @param file Input FASTA file
 * @return string of file content
 */ 
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

class Location {
    int start, end = 0;
  
  public:
    Location(int s) {
        start = s;
    }
    Location(int s, int e) {
        start = s;
        end = e;
    }
    int getStart() {return start;}
    int getEnd() {return end;}
    void getOutput() {
        std::cout << "start: " << start << ", end: " << end << std::endl;
    }
};


/**
 * Function receives a string and modifies specified characters to produce final output string.
 * 
 * @author Marta Bonacin
 * 
 * @param target A string representing the decompressed file.
 * @param lowercasePosition A list of location information for lowercase characters.
 * @param Nposition A list of location information for N characters.
*/
std::string modifyCharacters(std::string target, std::vector<Location> lowercasePosition, std::vector<Location> Nposition) {
    std::string finalTarget = target;

    for(Location item : lowercasePosition) {
        for (int i=item.getStart(); i <= item.getEnd(); i++) {
            finalTarget[i] = tolower(finalTarget[i]);
        }
    }

    for(Location position : Nposition) {
        //position.getOutput();
        finalTarget[position.getStart()] ='N';
    }

    return finalTarget;
}

// u reconstruct koristeno https://www.techiedelight.com/split-a-string-on-newlines-in-cpp/
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

    std::vector<Location> lowercasePositions;
    std::vector<Location> Npositions;
    for(int i = 0; i < lines.size(); i++) {
        std::string line = lines[i];
        std::cout << "processing line: " << line << std::endl;

        // skip lines 2, 3 and 4
        if (line[0] == '>') {
            std::cout << line << std::endl;
            writeToFile(outputFile, line.substr(0, line.length()));
        } else if (line.empty()) {
            //writeToFile(outputFile, "");

        } else if(i == 1) {

        } else if (i == 2) { //lowercase info
            int start, end;

            while(line.find(";") && line.length() > 1) {
                start = std::stoi(line.substr(0, line.find(" ")));
                end = std::stoi(line.substr(line.find(" "), line.find(";")));
                
                Location locLower(start, end);
                lowercasePositions.push_back(locLower);
                
                line = line.substr(line.find(";") + 1, line.length() - line.find(";"));
            }
            
        } else if (i == 3) { // N info
            int start;
            while(line.find(",") && line.length() > 1) {
                start = std::stoi(line.substr(0, line.find(",")));
                
                Location Npos(start);
                Npositions.push_back(Npos);

                line = line.substr(line.find(",") + 1, line.length() - line.find(","));
            }
            //Npositions.push_back();
        } else {
            // contains ','
            if (line.find(",") != std::string::npos) {
                begin = std::stoi(line.substr(0, line.find(",")));
                length = std::stoi(line.substr(line.find(",") + 1, line.length() - line.find(",") - 1));
                //std::cout << "Begin: " << begin << ", Length: " << length << std::endl;
                target += referenceFile.substr(begin + end, length + 1);

                std::cout << "adding: " << referenceFile.substr(begin + end, length + 1) << std::endl;
                end += begin + length;
                
            } else {
                target += line.substr(0, line.length());
            }
        }
    }

    for (int i = 0; i < target.length(); i++) {
        if (i % 71 == 0) {
            target.insert(i, 1, '\n');
        }
    }

    std::string finalTarget = modifyCharacters(target, lowercasePositions, Npositions);

    std::cout << "Target BP:\n" << target.length() << std::endl;
    writeToFile(outputFile, finalTarget);
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

int main( int argc, char **argv){
	if (argc <= 3) {
        std::cerr << "Missing argument." << std::endl;
        return -1;
    }
	
    std::string referenceSequencePath = argv[1];
    std::string compressedFilePath = argv[2];
    std::string finalFolder = argv[3];


    //TODO: changed for testing
    unzipFile(compressedFilePath, "../data/resultsd/decompressed");
    std::string filename("../data/resultsd/decompressed/intermediate.txt");
    //std::string filename("../data/results/intermediate.txt");//delete
    std::string compressedFile = readFileIntoString(filename);
	//std::cout << compressedFile << std::endl;

    //filename(referenceSequencePath);

    
    //std::string referenceFile = readFileIntoString(referenceSequencePath);
    std::string referenceSequence = getSequenceFromFile(argv[1]);
    //referenceFile = referenceFile.substr(referenceFile.find("\n") - 2, referenceFile.length());

    std::string output = "../data/resultsd/result.fa";

    clearFile(output);
    reconstruct(output, compressedFile, referenceSequence);

	return 0;
}