#include <UnitTest++/UnitTest++.h>
#include <fstream>
#include <cstdlib>
#include <vector> 
#include <algorithm>
#include <cctype>
#include "../../include/ClientDB.h"
#include "../../include/Logger.h"

namespace {

struct AuthFlowFixture {
    std::string tempDbFile;
    std::string tempLogFile;
    ClientDB* db;
    Logger* logger;
    
    AuthFlowFixture() {
        tempDbFile = "/tmp/test_authflow_" + std::to_string(rand()) + ".conf";
        std::ofstream dbFile(tempDbFile);
        dbFile << "user:FE78BDB4183B677A55E711B17E9CB6CCC2AE7315\n";
        dbFile.close();
        
        tempLogFile = "/tmp/test_authflow_" + std::to_string(rand()) + ".log";
        
        db = new ClientDB(tempDbFile);
        db->loadFromFile();
        
        logger = new Logger(tempLogFile);
        logger->initialize();
    }
    
    ~AuthFlowFixture() {
        delete db;
        delete logger;
        std::remove(tempDbFile.c_str());
        std::remove(tempLogFile.c_str());
    }
};

TEST(AuthFlow_SaltFormatValidation) {
    std::vector<std::string> salts;
    for (int i = 0; i < 5; i++) {
        salts.push_back(ClientDB::generateSalt());
    }
    
    for (const auto& salt : salts) {
        CHECK_EQUAL(16, salt.length());
        for (char c : salt) {
            CHECK((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'));
        }
    }
}

TEST(AuthFlow_HashDeterministic) {
    std::string salt = "0123456789ABCDEF";
    std::string password = "test123";
    
    std::string hash1 = ClientDB::generateHash(salt, password);
    std::string hash2 = ClientDB::generateHash(salt, password);
    std::string hash3 = ClientDB::generateHash(salt, "different");
    
    CHECK_EQUAL(hash1, hash2);
    CHECK(hash1 != hash3);
    CHECK_EQUAL(40, hash1.length());
}

TEST_FIXTURE(AuthFlowFixture, AuthFlow_SuccessfulAuthentication) {
    CHECK(db->clientExists("user"));
    
    std::string salt = ClientDB::generateSalt();
    std::string hash = ClientDB::generateHash(salt, "P@ssW0rd");
    
    CHECK_EQUAL(16, salt.length());
    CHECK_EQUAL(40, hash.length());
    
    logger->log(LogLevel::INFO, "Auth test passed", "user=user");
}

TEST_FIXTURE(AuthFlowFixture, AuthFlow_InvalidLogin) {
    CHECK(!db->clientExists("wronguser"));
    logger->log(LogLevel::WARNING, "Invalid login detected", "login=wronguser");
}

TEST_FIXTURE(AuthFlowFixture, AuthFlow_InvalidPassword) {
    std::string salt = ClientDB::generateSalt();
    std::string hashWrong = ClientDB::generateHash(salt, "wrongpass");
    std::string hashCorrect = ClientDB::generateHash(salt, "P@ssW0rd");
    
    CHECK_EQUAL(40, hashWrong.length());
    CHECK_EQUAL(40, hashCorrect.length());
    CHECK(hashWrong != hashCorrect);
}

} // unnamed namespace
