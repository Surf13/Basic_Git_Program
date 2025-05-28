
#include <string>
#include <filesystem>

class basicGit {
public:
    
    // create .basicgit/, objects/, HEAD, etc.
    void init();

    // Adds a file to the staging area
    void add(const std::string& fileName);

    //Sha-1 hash Function
    std::string hash(const std::string& fileName);
    std::string binaryConverter(int value);
    std::string _64BitBinary(uint64_t value);

    void updateIndex(std::string hasgedFile, std::string fileName);

    void status();

    // Commits staged changes with a commit message
    void commit(const std::string& commitMessage);
};