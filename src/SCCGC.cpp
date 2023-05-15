#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
#include <list>
#include <string>
#include <vector>
#include <map>


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

std::map<std::size_t, std::vector<int>> generateHashTable(std::string segment, int k) {
    std::map<std::size_t, std::vector<int>> hashTable;

    for (int i = 0; i < strlen(segment.c_str()) - k + 1; i++) {
        std::string kmer = segment.substr(i, k);

        std::size_t hash = std::hash<std::string>{}(kmer);
        
        std::vector<int> list;

        if (hashTable.contains(hash)) {
            list = hashTable[hash];
            list.push_back(i);
        } else {
            list = {i};
        }

        hashTable[hash] = list;
    }

    return hashTable;
    /*
    for (const auto& [key, value] : hashTable) {
        std::vector<int> list = value;
        std::string listString = "";
        for (auto s : list) {
            listString += std::to_string(s);
            listString += ", "
        }
        std::cout << "hash: " << key << " list:" << listString << std::endl;            
    }
    */
}

std::vector<std::string> localMatching(
    std::string segment,
    std::string referenceSegment, 
    std::map<std::size_t, std::vector<int>> hashTable, 
    int k
) {
    std::vector<std::string> out;

    for (int i = 0; i < segment.length() - k + 1; i++) {
        std::string kmer = segment.substr(i, k);

        std::size_t hash = std::hash<std::string>{}(kmer);
        
        if (!hashTable.contains(hash)) {
            // break if the rest of the tagreg segment is shorter than k-mer length, i.e. no matches can be made anymore 
            if (segment.length() - i < k) {
                break;
            }
            out.push_back(segment.substr(i, 1));
        } else {
            std::vector<int> list = hashTable[hash];

            int maxLength = 0;
            int maxLengthIndex = 0;

            for (int j = 0; j < list.size(); j++) {
                int length = 0;

                while (segment.substr(i + k, length) == referenceSegment.substr(list[j] + k, length)) {
                    length++;
                }

                if (length > maxLength) {
                    maxLength = length;
                    maxLengthIndex = j;
                }
            }

            //std::cout << "ml: " << maxLength << " i: " << i << "res: " << maxLength + k - i << std::endl;
            out.push_back(std::to_string(list[maxLengthIndex]) + "," + std::to_string(maxLength + k - 2));
            i += maxLength + k - 1;
            out.push_back(segment.substr(i, 1));
        }
    }

    return out;
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

    std::map<std::size_t, std::vector<int>> testMap = generateHashTable(referenceSequence.substr(0, 30000), 21);
    std::vector<std::string> s = localMatching(targetSequence.substr(0, 30000), referenceSequence.substr(0, 30000), testMap, 21);

    for (auto a : s) {
        if (isdigit(a[0]))
            std::cout << std::endl;    
        std::cout << a;
        if (isdigit(a[0]))
            std::cout << std::endl;    
        
    }
    return 0;
}