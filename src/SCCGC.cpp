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

int lineLength = 0;

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
std::string getSequenceFromFile(const char* file) {
    std::ifstream input(file);
    if(!input.good()){
        std::cerr << "Error opening file" << std::endl;
        return nullptr;
    }

    bool firstPass = true;
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
            if(firstPass) {
                lineLength = line.length();
                firstPass = false;
            }
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
 * Function extracts input FASTA file comments
 * 
 * @author Martin Bakac
 * 
 * @param file Input FASTA file
 * @return string of metadata
 */  
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
}

/**
 * Function creates/opens a file and writes to it.
 * 
 * @author Marta Bonacin 
 * 
 * @param filename Name of the file to write in.
 * @param text String which will be written in the file.
*/
void writeToFile(std::string filename, std::string text, bool newLine=true, bool debug=false) {
    std::ofstream file;

    if (debug)
        std::cout << "Writing: '" << text << "' to file: " << filename << "." << std::endl;
    file.open(filename, std::ofstream::out|std::ofstream::app);

    if(!file) {
        std::cout << "Error in file creation!" << std::endl;
        return;
    } else {
        if (debug)
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

    file.close();
}

/**
 * Exectues system 7zip command with appropriate parameters
 * 
 * @author Martin Bakac
 * 
 * @param filename file to compress
 */  
void zipFile(std::string filename, std::string mode="PPMD") {
    std::string output = filename + ".7z ";
    system(("7za a " + output + " " + filename + " -m0=" + mode).c_str());
}

/**
 * Function finds lowercase sequences and returns information about their position.
 * 
 * @author Marta Bonacin
 * 
 * @param sequence String in which to find the position of lowercase characters.
 * @return List of start and end lowercase sequence positions.
*/
std::vector<size_t> getLowercasePosition(const std::string &sequence) {
    bool successive = false;
    std::vector<size_t> lowercasePositionList;

    // Loop through each letter
    for (size_t i=0; i < sequence.size(); i++) {
        if (islower(sequence[i])) {
            // If first lowercase letter
            if (!successive) {
                lowercasePositionList.push_back(i);
                successive = true;
            }
        } else {
            // If end of successive lowercase letter sequence
            if (successive) {
                /**
                 * Pushes the index of first letter that is not lowercase.
                 * Push i - 1 if index of last lowercase letter desired.
                */
                lowercasePositionList.push_back(i);
                successive = false;
            }
        }
    }

    return lowercasePositionList;
}


/**
 * Function generates hash table for given segment/sequence
 * 
 * @author Martin Bakac
 * 
 * @param input segment/sequence
 * @param k_merLength length k_mer
 * 
 * @return hash table with hashes of k_mers as keys and lists of positions in reference segment
 */  
std::map<std::size_t, std::vector<int>> generateHashTable(std::string segment, int k_merLength) {
    std::map<std::size_t, std::vector<int>> hashTable;

    // special case k_mer where all characters are 'N'
    /*
    std::string nString = "";
    for (int i = 0; i < k_merLength; i++) {
        nString += "N";
    }
    
    std::size_t nStringHash = std::hash<std::string>{}(nString);
    */
    for (int i = 0; i < segment.length() - k_merLength + 1; i++) {

        // TODO: upitno
        if (i + k_merLength > segment.length())
            return hashTable;

        std::string k_mer = segment.substr(i, k_merLength);

        std::size_t hash = std::hash<std::string>{}(k_mer);
        
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


/**
 * Represents an output file entry whichs is either a pair of position and length or raw sequence of characters
 * 
 * The string only constructor takes a sequence of characters and sets the type to '1' to represent it's not a match,
 * but a raw string
 * 
 * The p, l, pr constructor takes the p as the position of the matches sequence in the target string and, l as the length
 * of the matched sequence and pr as the positon of the matches sequence in the reference sequence 
 * 
 * Methods are relevats setters/getters and type represents if it's a match or raw characters
 * 
 * @author Martin Bakac
 */  
class Entry {
    private:
    int position, length, positionInReference;
    std::string sequence;

    int type; // 0 for (p, l) pair, 1 for sequence string
  
  public:
    Entry(std::string s) {
        sequence = s;
        type = 1;
    }

    Entry(int p, int l, int pr = 0) {
        position = p;
        length = l;
        type = 0;
        positionInReference = pr;
    }

    inline int getType() const noexcept {return type;}
    
    inline int getPosition() const noexcept {
        if (type == 0)
            return position;
        else
            return 0;
    }

    inline int getPositionInReference() const noexcept {
        if (type == 0)
            return positionInReference;
        else
            return 0;
    }

    inline int getLength() const noexcept {
        if (type == 0)
            return length;
        else
            return 0; 
    }

    inline std::string getSequence() const noexcept {
        if (type == 1)
            return sequence;
        else
            return "";
    }
};

/**
 * Function extracts input FASTA file comments
 * 
 * @author Martin Bakac
 * 
 * @param segment target segment
 * @param referenceSegment reference segment
 * @param hashTable hash table for reference segment
 * @param k_merLength length of k_mer
 * @param referenceStart last index in reference file
 * 
 * @return list of Entry objects representing matches and "loose" characters
 */ 
std::vector<Entry> localMatching(
    std::string segment,
    std::string referenceSegment, 
    std::map<std::size_t, std::vector<int>> hashTable, 
    int k_merLength,
    int referenceStart
) {
    // positions are marked as paris of integers p for position and l for length
    std::vector<Entry> out;

    std::cout << "segment: " << segment.substr(0, k_merLength) << std::endl;

    for (int i = 0; i < segment.length() - k_merLength + 1; i++) {
        std::string k_mer = segment.substr(i, k_merLength);

        std::size_t hash = std::hash<std::string>{}(k_mer);
        Entry lastEntry(0, 0); // potential bug

        if (!hashTable.contains(hash)) {
            Entry e(segment.substr(i, 1));
            std::cout << "doesnt contain k-mer, adding: " << segment.substr(i, 1) << std::endl; 
            out.push_back(e);

            if (i == segment.length() - k_merLength) {
                Entry e(segment.substr(i + 1, segment.length()));
                out.push_back(e);
            }
        } else {
            std::vector<int> list = hashTable[hash];

            int maxLength = 0;
            int maxLengthIndex = 0;

            if (list.size() > 1) {
                std::cout << std::endl << list.size() << std::endl;
            }

            for (int j = 0; j < list.size(); j++) {
                int length = 1;

                while (segment.substr(i + k_merLength, length) == referenceSegment.substr(list[j] + k_merLength, length) and 
                    list[j] + k_merLength + length <= referenceSegment.length() and
                    i + k_merLength + length <= referenceSegment.length()
                ) {
                    length++;
                }

                if (length > maxLength) {
                    maxLength = length;
                    maxLengthIndex = j;
                }
            }
            
            // position in t, l, position in reference
            Entry position(i, maxLength + k_merLength - 2, referenceStart + list[maxLengthIndex]);
            out.push_back(position);
                        
            i += maxLength + k_merLength - 2;
            lastEntry = position;
        }

        if (i + k_merLength > segment.length()) {
            break;
        }
    }

    return out;
}

std::vector<Entry> globalMatching(
    std::string sequence,
    std::string referenceSequence, 
    std::map<std::size_t, std::vector<int>> hashTable, 
    int k_merLength
) {
    // positions are marked as paris of integers p for position and l for length
    std::vector<Entry> out;

    int limit = 100;

    for (int i = 0; i < sequence.length() - k_merLength + 1; i++) {
        std::string kmer = sequence.substr(i, k_merLength);
        std::size_t hash = std::hash<std::string>{}(kmer);
        
        if (!hashTable.contains(hash)) {
            Entry e(sequence.substr(i, 1));
            out.push_back(e);

            // transcribe "uncaught" part of segment (if last k-length segment is not matched)
            if (i == sequence.length() - k_merLength) {
                Entry e(sequence.substr(i + 1, sequence.length()));
                out.push_back(e);
            }
        } else {
            std::vector<int> list = hashTable[hash];

            int maxLength = 0;
            int maxLengthIndex = 0;

           
            for (int j = 0; j < list.size(); j++) {
                int length = 0;

                while (sequence.substr(i + k_merLength, length) == referenceSequence.substr(list[j] + k_merLength, length)) {
                    length++;
                }

                if (length > maxLength) {
                    maxLength = length;
                    maxLengthIndex = j;
                }
                //TODO: situation when there are two same maxLengths
            }
 
            Entry positions(i, maxLength + k_merLength - 2, list[maxLengthIndex]);
            out.push_back(positions);
            i += maxLength + k_merLength - 1;

            Entry e(sequence.substr(i, 1));
            out.push_back(e);
        }
    }

    return out;
}

void constructFile(std::string fileName, std::vector<Entry> entries) {
    bool addNewLine = false;
    for (int i = 0; i < entries.size(); i++) {
        Entry entry = entries[i];
        int delta = 0;

        std::cout << "p: " << entry.getPosition() << " l: " << entry.getLength() << " pir: " << entry.getPositionInReference() << std::endl;

        if (i >= 1) {
            for (int j = i - 1; j > 0; j--) {
                if (entries[j].getType() == 0) {
                    delta = entries[j].getLength() + entries[j].getPositionInReference();
                    break;
                }
            }
        }

        std::cout << "printing" << std::endl;
        if (entry.getType() == 0) {
            //std::cout << std::endl << entry.getPosition() << " " << delta << std::endl;
            
            int positionInReference = entry.getPositionInReference();
            int length = entry.getLength();

            int j = 1;
            bool exit = false;

            while (i + j <= entries.size() and !exit) {
                std::cout << entries[i + j].getType() << std::endl;
                if (entries[i + j].getType() == 0) {
                    length += entries[i + j].getLength() + 1;
                    j++;
                } else {
                    std::cout << "broke" << std::endl;
                    exit = true;
                }
            }

            i += j - 1;
            std::cout << std::to_string((positionInReference - delta)) + "," + std::to_string(length) << std::endl;

            writeToFile(
                fileName,
                std::to_string((positionInReference - delta)) + "," + std::to_string(length),
                true
            );

        } else if (entry.getType() == 1) {
            if (i+1 == entries.size() || entries[i+1].getType() == 0)
                addNewLine = true;
            writeToFile(fileName, entry.getSequence(), addNewLine);
            addNewLine = false;
            std::cout << entry.getSequence() << std::endl;
        }
    }
}

#define t1 (0.5f)
#define t2 (4)
#define segmentSize (1000)
#define k_merLength (21)

int main(int argc, char **argv){
    if (argc <= 3) {
        std::cerr << "Missing argument." << std::endl;
        return -1;
    }

    std::string targetSequence = getSequenceFromFile(argv[1]);
    std::string referenceSequence = getSequenceFromFile(argv[2]);

    const std::string intermFile = std::string(argv[3]) + "/intermediate.txt";

    // pre-processing
    const std::vector<size_t> targetLowercase = getLowercasePosition(targetSequence);
    
    //build lowercase letter position information to store in intermediate file
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

    //clear file before writing
    clearFile(intermFile);

    // write metadata to file
    writeToFile(intermFile, getMetadataFromFile(argv[1]));
    // TODO: chars in line
    writeToFile(intermFile, std::to_string(lineLength));
    //write lowercase letter positions
    writeToFile(intermFile, lowercasePostion);

    // Changes lowercase letters to uppercase ones
    for (char &c : targetSequence) {
        if (c <= 'z' && c >= 'a') {
            c += 'A' - 'a';
        }
    }
    for (char &c : referenceSequence) {
        if (c <= 'z' && c >= 'a') {
            c += 'A' - 'a';
        }
    }

    bool global = false;
    if (!global) writeToFile(intermFile, "");

    // TarSeq and RefSeq are short for target and reference sequence, resepctively
    std::vector<std::string> segmentedTarSeq, segmentedRefSeq;

    /*
    for (int i = 0; i < targetSequence.length(); i += segmentSize) {
        if (i + segmentSize > targetSequence.length()) {
            segmentedTarSeq.push_back(targetSequence.substr(i, targetSequence.length()));
        } else {
            segmentedTarSeq.push_back(targetSequence.substr(i, segmentSize));
        }
    }
    */

    /*
    for (int i = 0; i < referenceSequence.length(); i += segmentSize) {
        if (i + segmentSize > referenceSequence.length()) {
            segmentedRefSeq.push_back(referenceSequence.substr(i, referenceSequence.length()));
        } else {
            segmentedRefSeq.push_back(referenceSequence.substr(i, segmentSize));
        }
    }
    */


    int consecutiveMissmatches = 0;

    std::vector<Entry> foundMatches;

    int numberOfSegments = 0;
    int targetStart = 0;
    int referenceStart = 0;

    // iterate over segments
    bool segments = true;
    //segments = false;

    while (segments) {
        numberOfSegments++;
        std::cout << numberOfSegments + 1 << "-th new segment" << std::endl;
        std::cout << targetSequence.substr(0, 21) << std::endl;
        
        std::string tarSegment, refSegment;

        if (targetStart + segmentSize >= targetSequence.length()) {
            tarSegment = targetSequence.substr(targetStart, targetSequence.length() - targetStart);
            segments = false;
        } else {
            tarSegment = targetSequence.substr(targetStart, segmentSize);
        }

        if (referenceStart + segmentSize >= referenceSequence.length()) {
            refSegment = referenceSequence.substr(referenceStart, referenceSequence.length() - referenceStart);
            segments = false;
        } else {
            refSegment = referenceSequence.substr(referenceStart, segmentSize);
        }
    
        std::cout << "ref seq: " << refSegment.substr(0, 21) << std::endl;
        std::cout << "ref seq length: " << refSegment.length() << std::endl;

        const std::map<std::size_t, std::vector<int>> localHashTable = generateHashTable(refSegment, k_merLength);
        std::vector<Entry> matches = localMatching(tarSegment, refSegment, localHashTable, k_merLength, referenceStart);

        // ovo nebude nikad, treba promenit kaj broji samo type 1 matcheve
        if (matches.size() == 0) {
            int retryK = 11;
            std::map<std::size_t, std::vector<int>> localHashTableRetry = generateHashTable(refSegment, retryK);
            std::vector<Entry> matches = localMatching(tarSegment, refSegment, localHashTableRetry, retryK, referenceStart);
        }

        int characters = -1;
        for (auto match : matches) {
            if (match.getType() == 1) {
                characters++;
            }
        }

        if ((float) characters / (float) segmentSize >= t1) {
            consecutiveMissmatches++;
        } else {
            consecutiveMissmatches = 0;
        }

        if (consecutiveMissmatches > t2) {
            global = true;
            break;
        }

        for (auto match : matches)
            foundMatches.push_back(match);

        Entry lastMatch(1000,1000,0);
        for (int j = 0; j < matches.size(); j++) {
            Entry match = matches[j];

            if (match.getType() == 0) {
                if (match.getPosition() + match.getLength() >= referenceStart) {
                    //std::cout << match.getPositionInReference() << " " << match.getLength() << std::endl;
                    // referenceStart = match.getPositionInReference() + match.getLength() + 1;
                }

                referenceStart = match.getPositionInReference() + match.getLength() + 1;

                lastMatch = match;
                //std::cout << std::endl << match.getPositionInReference() << "," << match.getPositionInReference() + match.getLength() << std::endl;
            } else if (match.getType() == 1) {
                std::cout << match.getSequence();
                //referenceStart++;
            }
        }

        targetStart += lastMatch.getPosition() + lastMatch.getLength() + 1;

        //referenceStart = 0;
        std::cout << "tar start: " << targetStart << "ref start: " << referenceStart << std::endl;

        // print like reference implementation
        /*
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
        */
        
    }
    //global = true;
    if (global) {
        // deleting N characters
        std::string targetNposition = "";
        std::string finalTargetSequence = "";
        for (int i=0; i < targetSequence.size(); i++) {
            if (targetSequence[i] != 'N') {
                finalTargetSequence += targetSequence[i];
            } else {
                targetNposition += std::to_string(i) + ",";
            }        
        }

        std::cout << "target N postions:" << targetNposition << std::endl;
        writeToFile(intermFile, targetNposition);

        std::string finalReferenceSequence = "";
        for (const char &c : referenceSequence) {
            if (c != 'N') {
                finalReferenceSequence += c;
            }
        }

        std::cout << finalReferenceSequence.length() << std::endl;

        const std::map<std::size_t, std::vector<int>> gHash = generateHashTable(finalReferenceSequence, 21);
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

        for(auto match : matches)
            foundMatches.push_back(match);
    }

    constructFile(intermFile, foundMatches);
    zipFile(intermFile);

    return 0;
}