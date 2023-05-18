#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
#include <list>
#include <string>
#include <vector>
#include <map>
#include <utility>

#define ll long long 

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

    std::cout << "Write to file" << std::endl;
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
    std::cout << "Clearing file" << std::endl;
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

    // special case k_mer where all characters are 'N'
    /*
    std::string nString = "";
    for (int i = 0; i < k; i++) {
        nString += "N";
    }
    
    std::size_t nStringHash = std::hash<std::string>{}(nString);
    */
    for (int i = 0; i < segment.length() - k + 1; i++) {
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
}


// file entry
class Entry {
    int position, length;
    std::string sequence;

    int type; // 0 for (p, l) pair, 1 for sequence string
  
  public:
    Entry(std::string s) {
        sequence = s;
        type = 1;
    }

    Entry(int p, int l) {
        position = p;
        length = l;
        type = 0;
    }

    int getType() {return type;}
    
    int getPosition() {
        if (type == 0)
            return position;
        else
            return 0;
    }

    int getLength() {
        if (type == 0)
            return length;
        else
            return 0; 
    }

    std::string getSequence() {
        if (type == 1)
            return sequence;
        else
            return "";
    }
};

std::vector<Entry> localMatching(
    std::string segment,
    std::string referenceSegment, 
    std::map<std::size_t, std::vector<int>> hashTable, 
    int k
) {
    // positions are marked as paris of integers p for position and l for length
    std::vector<Entry> out;

    for (int i = 0; i < segment.length() - k + 1; i++) {
        std::string kmer = segment.substr(i, k);

        std::size_t hash = std::hash<std::string>{}(kmer);
        
        if (!hashTable.contains(hash)) {
            // break if the rest of the tagreg segment is shorter than k-mer length, i.e. no matches can be made anymore 
            if (segment.length() - i < k) {
                break;
            }

            Entry e(segment.substr(i, 1));
            out.push_back(e);
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
 
            Entry positions(list[maxLengthIndex], maxLength + k - 2);
            out.push_back(positions);
            i += maxLength + k - 1;

            Entry e(segment.substr(i, 1));
            out.push_back(e);
        }
    }

    return out;
}

std::vector<Entry> globalMatching(
    std::string sequence,
    std::string referenceSequence, 
    std::map<std::size_t, std::vector<int>> hashTable, 
    int k
) {
    // positions are marked as paris of integers p for position and l for length
    std::vector<Entry> out;

    int limit = 100;

    for (int i = 0; i < sequence.length() - k + 1; i++) {
        std::string kmer = sequence.substr(i, k);

        std::size_t hash = std::hash<std::string>{}(kmer);
        
        if (!hashTable.contains(hash)) {
            // break if the rest of the tagreg segment is shorter than k-mer length, i.e. no matches can be made anymore 
            if (sequence.length() - i < k) {
                break;
            }

            Entry e(sequence.substr(i, 1));
            out.push_back(e);
        } else {
            std::vector<int> list = hashTable[hash];

            int maxLength = 0;
            int maxLengthIndex = 0;

            bool match = false;
            for (int j = 0; j < list.size(); j++) {
                int length = 0;

              // TODO
              //if (list[j] -  > limit || list[j] -  < -limit) {
              //    continue;
              //}

                match = true;
                while (sequence.substr(i + k, length) == referenceSequence.substr(list[j] + k, length)) {
                    length++;
                }

                if (length > maxLength) {
                    maxLength = length;
                    maxLengthIndex = j;
                }
                //opcija kada su duljine jednake
            }
            // search matches in whole sequence without application of limit
            if (!match) {
                 for (int j = 0; j < list.size(); j++) {
                    int length = 0;

                    match = true;
                    while (sequence.substr(i + k, length) == referenceSequence.substr(list[j] + k, length)) {
                        length++;
                    }

                    if (length > maxLength) {
                        maxLength = length;
                        maxLengthIndex = j;
                    }
                    //opcija kada su duljine jednake
            }
            }
 
            Entry positions(list[maxLengthIndex], maxLength + k - 2);
            out.push_back(positions);
            i += maxLength + k - 1;

            Entry e(sequence.substr(i, 1));
            out.push_back(e);
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

    // pre-processing

    std::list<int> targetLowercase = getLowercasePosition(targetSequence);
    
    std::string lowercasePostion = ""; 
    int index = 1;
    for (auto x = targetLowercase.begin(); x != targetLowercase.end(); ++x) {
        lowercasePostion += std::to_string(*x);
        //separate start & end pairs by ; in file
        if(index % 2 == 0) {
            lowercasePostion += ";";
        } else {
            lowercasePostion += " ";
        }
        index += 1;
    }

    writeToFile(intermFile, lowercasePostion);

    std::string tUpperSequence;
    for (int i=0; i<strlen(targetSequence.c_str()); i++) {        
        tUpperSequence += toupper(targetSequence[i]);
    }

    std::string rUpperSequence;
    for (int i=0; i<strlen(referenceSequence.c_str()); i++) {
        rUpperSequence += toupper(referenceSequence[i]);
    }

    bool global = false;

    // segment sequences
    int segmentSize = 1000;

    // TarSeq and RefSeq are short for target and reference sequence, resepctively
    std::vector<std::string> segmentedTarSeq, segmentedRefSeq;

    for (int i = 0; i < tUpperSequence.length(); i += segmentSize) {
        if (i + segmentSize > tUpperSequence.length()) {
            segmentedTarSeq.push_back(tUpperSequence.substr(i, tUpperSequence.length()));
        } else {
            segmentedTarSeq.push_back(tUpperSequence.substr(i, segmentSize));
        }
    }

    for (int i = 0; i < rUpperSequence.length(); i += segmentSize) {
        if (i + segmentSize > rUpperSequence.length()) {
            segmentedRefSeq.push_back(rUpperSequence.substr(i, rUpperSequence.length()));
        } else {
            segmentedRefSeq.push_back(rUpperSequence.substr(i, segmentSize));
        }
    }

    // iterate over segments
    for (int i = 0; i < segmentedTarSeq.size(); i++) {
        std::string tarSegment = segmentedTarSeq[i];
        std::string refSegment = segmentedRefSeq[i];
    
        // k_mer size
        int k = 21;

        std::map<std::size_t, std::vector<int>> localHashTable = generateHashTable(refSegment, k);
        std::vector<Entry> matches = localMatching(tarSegment, refSegment, localHashTable, k);

        if (matches.size() == 0) {
            int retryK = 11;
            std::map<std::size_t, std::vector<int>> localHashTableRetry = generateHashTable(refSegment, retryK);
            std::vector<Entry> matches = localMatching(tarSegment, refSegment, localHashTableRetry, retryK);
        }

        if (matches.size() == 0) {
            global = true;
            break;
        }

        for (int i = 0; i < matches.size(); i++) {
            Entry match = matches[i];
            int delta = 0;

            if (i >= 1) {
                for (int j = i - 1; j > 0; j--) {
                    if (matches[j].getType() == 0) {
                        delta = matches[j].getLength() + matches[j].getPosition();
                        break;
                    }
                }
            }

            if (match.getType() == 0) {
                std::cout << std::endl << match.getPosition() - delta << "," << match.getLength() << std::endl;
            } else if (match.getType() == 1) {
                std::cout << match.getSequence();
            }
            
            //writeToFile()
        }    
    }

    /*
    global = true;

    if(global) {
        clearFile(intermFile);
        writeToFile(intermFile, lowercasePostion);

        // deleting N characters
        std::string targetN = "";
        std::string finalTargetSequence = "";
        bool first = true;
        for (int i=0; i<strlen(tUpperSequence.c_str()); i++) {
            if (tUpperSequence[i] != 'N') {
                finalTargetSequence += tUpperSequence[i];
            } else {
                if (!first) {
                    targetN += ",";
                }
                targetN += std::to_string(i);
                first = false;
            }        
        }

        writeToFile(intermFile, targetN);

        std::string finalReferenceSequence = "";
        for (int i=0; i<strlen(rUpperSequence.c_str()); i++) {
            if (rUpperSequence[i] != 'N')
                finalReferenceSequence += rUpperSequence[i];
        }


        std::map<std::size_t, std::vector<int>> gHash = generateHashTable(finalReferenceSequence, 21);
        std::vector<Entry> matches = globalMatching(finalTargetSequence, finalReferenceSequence, gHash, 21);

        for (int i = 0; i < matches.size(); i++) {
            Entry match = matches[i];
            int delta = 0;

            if (i >= 1) {
                for (int j = i - 1; j > 0; j--) {
                    if (matches[j].getType() == 0) {
                        delta = matches[j].getLength() + matches[j].getPosition();
                        break;
                    }
                }
            }

            if (match.getType() == 0) {
                std::cout << std::endl << match.getPosition() - delta << "," << match.getLength() << std::endl;
            } else if (match.getType() == 1) {
                std::cout << match.getSequence();
            }
        }
    }
    */
    

    return 0;
}