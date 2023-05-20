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

	return 0;
}