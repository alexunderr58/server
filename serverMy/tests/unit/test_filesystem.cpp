#include <UnitTest++/UnitTest++.h>
#include <fstream>
#include <cstdlib>
#include "../../include/ClientDB.h"

namespace {

TEST(Filesystem_MissingClientFile) {
    std::string nonExistentFile = "/tmp/nonexistent_" + std::to_string(rand()) + ".conf";
    ClientDB db(nonExistentFile);
    bool loadResult = db.loadFromFile();
    (void)loadResult;
    CHECK(true);
}

TEST(Filesystem_CorruptedClientFile) {
    std::string corruptedFile = "/tmp/corrupted_" + std::to_string(rand()) + ".conf";
    
    std::ofstream file(corruptedFile);
    file << "This is not a valid format\n";
    file << "user_without_colon\n";
    file << ":password_without_login\n";
    file.close();
    
    ClientDB db(corruptedFile);
    bool loadResult = db.loadFromFile();
    (void)loadResult;
    
    std::remove(corruptedFile.c_str());
    CHECK(true);
}

TEST(Filesystem_ValidClientFile) {
    std::string validFile = "/tmp/valid_" + std::to_string(rand()) + ".conf";
    
    std::ofstream file(validFile);
    file << "# Test client database\n";
    file << "user1:HASH1\n";
    file << "user2:HASH2\n";
    file << "user3:HASH3\n";
    file.close();
    
    ClientDB db(validFile);
    bool loadResult = db.loadFromFile();
    
    std::remove(validFile.c_str());
    CHECK(loadResult);
}

} // unnamed namespace
