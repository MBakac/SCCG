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
#include <algorithm>

#define ll long long

/**
 * Represents location information for certain character types
 * 
 * The empty constructor generates an instance of the class and initializes its variables to zero.
 * 
 * The s, c constructor takes value for the start position of such characters and the number of such characters. 
 * 
 * Methods are getters and setters, addConsecutive to add another character in a row and an output function.
 * 
 * @author Marta Bonacin
 */  
class Location {
    int start, numberOfConsecutive;
  
  public:
    Location() {
        start = 0;
        numberOfConsecutive = 0;
    }
    Location(int s, int c) {
       start = s;
       numberOfConsecutive = c; 
    }
    void setStart(int s) {
        start = s;
        numberOfConsecutive = 1;
    }

    void addConsecutive() {
        numberOfConsecutive += 1;
    }

    int getStart() {return start;}
    int getNumberOfConsecutive() {return numberOfConsecutive;}

    void getOutput() {
        std::cout << "start: " << start << ", number of consecutive: " << numberOfConsecutive << std::endl;
    }
};

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

    int previousEnd = 0;
    bool first = true;
    for(Location position : Nposition) {
        //position.getOutput();
        if(first) {
            /*
            for(int i = 0; i < position.getNumberOfConsecutive(); i++) {
                finalTarget.insert(position.getStart() + i, 1, 'N');
            }
            */
            finalTarget.insert(position.getStart(), position.getNumberOfConsecutive(), 'N');
            first = false;
        } else {
            /*
            for(int i = 0; i < position.getNumberOfConsecutive(); i++) {
                finalTarget.insert(position.getStart() + previousEnd + i, 1, 'N');
            }
            */
            if (position.getStart() + previousEnd > finalTarget.length()) {
                std::string ns(position.getNumberOfConsecutive(), 'N'); 
                finalTarget += ns;
            } else {
                finalTarget.insert(position.getStart() + previousEnd, position.getNumberOfConsecutive(), 'N');
            }
        }
        
        previousEnd += position.getStart() + position.getNumberOfConsecutive() - 1;
    }

    std::cout << "N posi reseni!" << std::endl;


    first = true;
    previousEnd = 0;
    int c = 0;

    for (Location item : lowercasePosition) {
        item.getOutput();
        int consecutive = item.getNumberOfConsecutive();
        int start = item.getStart() - 1;    

        if (first) {
            //std::cout << finalTarget.substr(start, consecutive) << std::endl;
            for (int i = start + 1; i < consecutive + start + 1; i++) {
                finalTarget[i] = tolower(finalTarget[i]);
            }
            //std::cout << finalTarget.substr(start, consecutive) << std::endl;
            
            first = false;
        } else {
            //std::cout << finalTarget.substr(start + previousEnd, consecutive) << std::endl;
            for (int i = start + previousEnd + 1; i < consecutive + start + previousEnd + 1; i++) {
                finalTarget[i] = tolower(finalTarget[i]);
            }
            //std::cout << finalTarget.substr(start + previousEnd, consecutive) << std::endl;
        }

        previousEnd += start + consecutive;
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
    int lineLength;

    std::vector<Location> lowercasePositions;
    std::vector<Location> Npositions;
    for(int i = 0; i < lines.size(); i++) {
        std::string line = lines[i];
        std::cout << "processing line: " << i << "/" << lines.size()<< std::endl;

        // skip lines 2, 3 and 4
        if (line[0] == '>') {
            std::cout << line << std::endl;
            writeToFile(outputFile, line.substr(0, line.length()));
        } else if (line.empty()) {
            std::cout << "line is empty" << std::endl;
            //writeToFile(outputFile, "");

        } else if(i == 1) {
            lineLength = std::stoi(line);
        } else if (i == 2 && !line.empty()) { //lowercase info
            int relativeStartL, consecutiveL;

            while(line.find(";") && line.length() > 1) {
                relativeStartL = std::stoi(line.substr(0, line.find(" ")));
                consecutiveL = std::stoi(line.substr(line.find(" "), line.find(";")));
                
                Location locLower(relativeStartL, consecutiveL);
                lowercasePositions.push_back(locLower);
                //locLower.getOutput();
                
                line = line.substr(line.find(";") + 1, line.length() - line.find(";"));
            }
            
        } else if (i == 3 && !line.empty()) { // N info
            int relativeStartN, consecutiveN;
            while(line.find(";") && line.length() > 1) {
                relativeStartN = std::stoi(line.substr(0, line.find(" ")));
                consecutiveN = std::stoi(line.substr(line.find(" "), line.find(";")));
                
                Location Npos(relativeStartN, consecutiveN);
                Npositions.push_back(Npos);
                //Npos.getOutput();

                line = line.substr(line.find(";") + 1, line.length() - line.find(";"));
            }
        } else {
            // contains ','
            if (line.find(",") != std::string::npos) {
                begin = std::stoi(line.substr(0, line.find(",")));
                length = std::stoi(line.substr(line.find(",") + 1, line.length() - line.find(",") - 1));
                std::cout << "posovi" << std::endl;
                std::cout << "Begin: " << begin << ", Length: " << length << std::endl;
                std::cout << "Begin + end: " << begin + end << ", Length: " << length + 1 << std::endl;
                target += referenceFile.substr(begin + end, length + 1);

                //std::cout << "adding: " << referenceFile.substr(begin + end, length + 1) << std::endl;
                end += begin + length;
                
            } else {
                target += line.substr(0, line.length());
            }
        }
    }

    std::string finalTarget = modifyCharacters(target, lowercasePositions, Npositions);
    std::cout << "chars modified?" << std::endl;

    std::string finalFinalTarget = "";

    for (int i = 0; i < finalTarget.length(); i++) {
        if (i % lineLength == 0) {
            finalFinalTarget += "\n";
        }

        finalFinalTarget += finalTarget[i];
    }
    std::cout << "made finalfinal" << std::endl;

    finalFinalTarget.erase(0,1);

    std::cout << "Target BP:\n" << target.length() << std::endl;
    writeToFile(outputFile, finalFinalTarget);
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

    
    for (char &c : referenceSequence) {
        if (c <= 'z' && c >= 'a') {
            c += 'A' - 'a';
        }
    }

    std::string finalReferenceSequence = "";
    for (const char &c : referenceSequence) {
        if (c != 'N') {
            finalReferenceSequence += c;
        }
    }

    
    clearFile(output);
    reconstruct(output, compressedFile, finalReferenceSequence);

	return 0;
}