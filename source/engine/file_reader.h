#pragma once

#include <string>
#include <fstream>

class FileReader {
public:
    FileReader() = default;
    explicit FileReader(const std::string &filename);
    ~FileReader();

    FileReader(const FileReader &other) = delete;
    FileReader& operator=(const FileReader &other) = delete;

    void openFile(const std::string &filename);
    bool nextLine(char *line, size_t size);
    bool nextLine(std::string &line);

    bool eof() const;
    std::string getLastLine() const;

    std::string getFileContent() const;
private:
    std::string mLastLine;

    std::ifstream mFs;
    int mCurrentLineNumber;
};