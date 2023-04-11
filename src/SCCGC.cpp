#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
#include <list>
#include <string>


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
 * Function creates/opens a file and writes to it.
 * 
 * @author Marta Bonacin 
 * 
 * @param filename Name of the file to write in.
 * @param text String which will be written in the file.
*/
void writeToFile(std::string filename, std::string text) {
    std::ofstream file;

    file.open(filename, std::ofstream::out|std::ofstream::app);

    if(!file) {
        std::cout << "Error in file creation!" << std::endl;
        return;
    } else {
        std::cout << "File created/opened successfully!" << std::endl;
    }

    file << text;
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

    file.open(filename, std::ofstream::out|std::ofstream::trunc);

    file.close();
}

/**
 * Function finds lowercase sequences and returns information about their position.
 * 
 * @author Marta Bonacin
 * 
 * @param sequence String in which to find the position of lowercase characters.
 * @return List of start and end lowercase sequence positions.
*/
std::list<int> getLowercasePosition(std::string sequence) {
    bool successive = false;
    int start = 0;
    int end = 0;
    std::list<int> lowercasePositionList;

    for (int i=0; i<strlen(sequence.c_str()); i++) {
        if (islower(sequence[i])) {
            if (successive) {
                end += 1;
            } else {
                start = i;
                end += 1;
                successive = true;
            }
        } else {
            if (successive) {
                lowercasePositionList.push_back(start);
                lowercasePositionList.push_back(end);
            } 
            successive = false;
            start = 0;
            end = i + 1;
        }
    }

    if (successive) {
        lowercasePositionList.push_back(start);
        lowercasePositionList.push_back(end);
    }

    return lowercasePositionList;
}


int main( int argc, char **argv){
    if (argc <= 3) {
        std::cerr << "Missing argument." << std::endl;
        return -1;
    }

    std::string targetSequence = getSequenceFromFile(argv[1]);
    std::string referenceSequence = getSequenceFromFile(argv[2]);
    std::string finalFolder = argv[3];

    std::string intermFile = finalFolder + "/intermediate.txt";

    //pre-processing

    std::list<int> targetLowercase = getLowercasePosition(targetSequence);
    
    std::string text = ""; 
    int index = 1;
    for (auto x = targetLowercase.begin(); x != targetLowercase.end(); ++x) {
        text += std::to_string(*x);
        //separate start & end pairs by ; in file
        if(index % 2 == 0) {
            text += ";";
        } else {
            text += " ";
        }
        index += 1;
    }

    writeToFile(intermFile, text);
    
    std::string tSequence;
    for (int i=0; i<strlen(targetSequence.c_str()); i++) {        
        tSequence += toupper(targetSequence[i]);
    }

    std::string rSequence;
    for (int i=0; i<strlen(referenceSequence.c_str()); i++) {
        rSequence += toupper(referenceSequence[i]);
    }

    // deleting N characters
    std::string targetN = "";
    bool first = true;
    for (int i=0; i<strlen(tSequence.c_str()); i++) {
        if (tSequence[i] != 'N') {
            targetSequence += tSequence[i];
        } else {
            if (!first) {
                targetN += ",";
            }
            targetN += std::to_string(i);
            first = false;         
        }        
    }

    writeToFile(intermFile, targetN);

    for (int i=0; i<strlen(rSequence.c_str()); i++) {
        if (rSequence[i] != 'N')
            referenceSequence += rSequence[i];
    }

    return 0;
}