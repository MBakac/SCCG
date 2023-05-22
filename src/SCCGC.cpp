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
void writeToFile(std::string filename, std::string text, bool newLine=true) {
    std::ofstream file;

    //std::cout << "Writing: '" << text << "' to file: " << filename << "." << std::endl;
    file.open(filename, std::ofstream::out|std::ofstream::app);

    if(!file) {
        std::cout << "Error in file creation!" << std::endl;
        return;
    } else {
        //std::cout << "File created/opened successfully!" << std::endl;
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

        // TODO: upitno
        if (i + k > segment.length())
            return hashTable;

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

    /*
    for (const auto& [key, value] : hashTable) {
        std::vector<int> list = value;
        std::string listString = "";
        for (auto s : list) {
            listString += std::to_string(s);
            listString += ", ";
        }
        std::string seq = ref.substr(list[0], k);
        std::cout << "hash: " << key << " list:" << listString << " seq: " << seq << std::endl;            
    }
    */

    return hashTable;
}


// file entry
class Entry {
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

    int getType() {return type;}
    
    int getPosition() {
        if (type == 0)
            return position;
        else
            return 0;
    }

    int getPositionInReference() {
        if (type == 0)
            return positionInReference;
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
    int k,
    int nSegment, 
    int segmentLength
) {
    // positions are marked as paris of integers p for position and l for length
    std::vector<Entry> out;

    std::cout << "segment: " << segment.substr(0, k) << std::endl;

    for (int i = 0; i < segment.length() - k + 1; i++) {
        std::string kmer = segment.substr(i, k);

        std::size_t hash = std::hash<std::string>{}(kmer);
        Entry lastEntry(0, 0); // potential bug

        if (!hashTable.contains(hash)) {
            // break if the rest of the tagreg segment is shorter than k-mer length, i.e. no matches can be made anymore 
            /*
            if (segment.length() - i < k) {
                break;
            }
            */

            Entry e(segment.substr(i, 1));
            std::cout << "doesnt contain k-mer, adding: " << segment.substr(i, 1) << std::endl; 
            out.push_back(e);

            if (i == segment.length() - k) {
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

                while (segment.substr(i + k, length) == referenceSegment.substr(list[j] + k, length) and 
                    list[j] + k + length <= referenceSegment.length() and
                    i + k + length <= referenceSegment.length()
                ) {
                    length++;
                }

                if (length > maxLength) {
                    maxLength = length;
                    maxLengthIndex = j;
                }
            }
            
            // position in t, l, position in reference
            Entry position(i, maxLength + k - 2, nSegment + list[maxLengthIndex]);
            out.push_back(position);
            
            std::cout << "adding p,l for : " << kmer << " p: " << i << " l: " << maxLength + k - 2 << " pir: " << list[maxLengthIndex] << std::endl; 
            
            i += maxLength + k - 2;

            lastEntry = position;

            //Entry e(segment.substr(i, 1));
            //out.push_back(e);
        }

        if (i + k > segment.length()) {
            /*
            std::cout << "here test" << std::endl;
            int charsLeft = segment.length() - lastEntry.getPosition() - lastEntry.getLength() - 1;
            if (charsLeft) {
                std::cout << "i: " << i << std::endl;
                std::cout << lastEntry.getPosition() << " " << lastEntry.getLength() << std::endl;
                std::cout << lastEntry.getPosition() + lastEntry.getLength() << std::endl;
                std::cout << "chars left: " << segment.length() - lastEntry.getPosition() - lastEntry.getLength() - 1 << std::endl;
                Entry e(
                    segment.substr(
                        lastEntry.getPosition() + lastEntry.getLength(),
                        segment.length() - lastEntry.getPosition() - lastEntry.getLength() + 1
                    )
                );
                out.push_back(e);
                std::cout << "left with some characters, adding: " << e.getSequence() << std::endl; 
            }
            */
            break;
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
            Entry e(sequence.substr(i, 1));
            out.push_back(e);

            // transcribe "uncaught" part of segment (if last k-length segment is not matched)
            if (i == sequence.length() - k) {
                Entry e(sequence.substr(i + 1, sequence.length()));
                out.push_back(e);
            }
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
 
            Entry positions(i, maxLength + k - 2, list[maxLengthIndex]);
            out.push_back(positions);
            i += maxLength + k - 1;

            Entry e(sequence.substr(i, 1));
            out.push_back(e);
        }
    }

    return out;
}

void constructFile(std::string fileName, std::vector<Entry> entries) {
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
                //"\n" + std::to_string((entry.getPositionInReference())) + "," + std::to_string(entry.getPositionInReference() + entry.getLength()) + "\n",
                  "\n" + std::to_string((positionInReference - delta)) + "," + std::to_string(length) + "\n",
                false
            );

        } else if (entry.getType() == 1) {
            writeToFile(fileName, entry.getSequence(), false);
            std::cout << entry.getSequence() << std::endl;
        }
    }
}

int main(int argc, char **argv){
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

    // write metadata to file
    clearFile(intermFile);
    writeToFile(intermFile, getMetadataFromFile(argv[1]));
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

    /*
    for (int i = 0; i < tUpperSequence.length(); i += segmentSize) {
        if (i + segmentSize > tUpperSequence.length()) {
            segmentedTarSeq.push_back(tUpperSequence.substr(i, tUpperSequence.length()));
        } else {
            segmentedTarSeq.push_back(tUpperSequence.substr(i, segmentSize));
        }
    }
    */

    /*
    for (int i = 0; i < rUpperSequence.length(); i += segmentSize) {
        if (i + segmentSize > rUpperSequence.length()) {
            segmentedRefSeq.push_back(rUpperSequence.substr(i, rUpperSequence.length()));
        } else {
            segmentedRefSeq.push_back(rUpperSequence.substr(i, segmentSize));
        }
    }
    */

    float t1 = 0.5;
    float t2 = 4.0;

    int consecutiveMissmatches = 0;

    std::vector<Entry> foundMatches;

    int numerOfSegments = 0;
    int targetStart = 0;
    int referenceStart = 0;

    // iterate over segments
    bool segments = true;
    segments = false;

    while (segments) {
        numerOfSegments++;
        std::cout << numerOfSegments + 1 << "-th new segment" << std::endl;
        std::cout << tUpperSequence.substr(0, 21) << std::endl;
        
        std::string tarSegment, refSegment;

        if (targetStart + segmentSize >= tUpperSequence.length()) {
            tarSegment = tUpperSequence.substr(targetStart, tUpperSequence.length() - targetStart);
            segments = false;
        } else {
            tarSegment = tUpperSequence.substr(targetStart, segmentSize);
        }

        if (referenceStart + segmentSize >= rUpperSequence.length()) {
            refSegment = rUpperSequence.substr(referenceStart, rUpperSequence.length() - referenceStart);
            segments = false;
        } else {
            refSegment = rUpperSequence.substr(referenceStart, segmentSize);
        }
    
        std::cout << "ref seq: " << refSegment.substr(0, 21) << std::endl;
        std::cout << "ref seq length: " << refSegment.length() << std::endl;
        // k_mer size
        int k = 21;

        std::map<std::size_t, std::vector<int>> localHashTable = generateHashTable(refSegment, k);
        std::vector<Entry> matches = localMatching(tarSegment, refSegment, localHashTable, k, referenceStart, segmentSize);

        // ovo nebude nikad, treba promenit kaj broji samo type 1 matcheve
        if (matches.size() == 0) {
            int retryK = 11;
            std::map<std::size_t, std::vector<int>> localHashTableRetry = generateHashTable(refSegment, retryK);
            std::vector<Entry> matches = localMatching(tarSegment, refSegment, localHashTableRetry, retryK, referenceStart, segmentSize);
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
    
    global = true;
    if (global) {
        //clearFile(intermFile);
        //writeToFile(intermFile, lowercasePostion);

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
        targetN += ",";

        writeToFile(intermFile, targetN);

        std::string finalReferenceSequence = "";
        for (int i=0; i<strlen(rUpperSequence.c_str()); i++) {
            if (rUpperSequence[i] != 'N')
                finalReferenceSequence += rUpperSequence[i];
        }

        std::cout << finalReferenceSequence.length() << std::endl;

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

        for(auto match : matches)
            foundMatches.push_back(match);
    }

    writeToFile(intermFile, "");
    writeToFile(intermFile, "");

    constructFile(intermFile, foundMatches);
    zipFile(intermFile);

    return 0;
}