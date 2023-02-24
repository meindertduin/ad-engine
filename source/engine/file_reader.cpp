#include "file_reader.h"

#include <sstream>

FileReader::FileReader(const std::string &filename) {
    openFile(filename);
}

FileReader::~FileReader() {
    mFs.close();
}

void FileReader::openFile(const std::string &filename) {
    mFs.open(filename);
    if (!mFs.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
}

bool FileReader::nextLine(char *line, size_t size) {
    if (mFs.eof()) {
        return false;
    }

    mFs.getline(line, size);

    mLastLine = line;
    return true;
}

bool FileReader::nextLine(std::string &line) {
    char char_line[128];
    if (mFs.eof()) {
        return false;
    }

    mFs.getline(char_line, sizeof(char_line));
    line = char_line;
    mLastLine = char_line;

    return true;
}

std::string FileReader::getFileContent() const {
    std::stringstream ss;
    ss << mFs.rdbuf();
    return ss.str();
}

bool FileReader::eof() const {
    return mFs.eof();
}

std::string FileReader::getLastLine() const {
    return mLastLine;
}