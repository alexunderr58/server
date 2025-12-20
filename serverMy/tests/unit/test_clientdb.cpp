#include <UnitTest++/UnitTest++.h>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include "../../include/ClientDB.h"

namespace {

struct ClientDBFixture {
    std::string tempFile;
    ClientDB* db;
    
    ClientDBFixture() {
        tempFile = "/tmp/test_clientdb_" + std::to_string(rand()) + ".conf";
        std::ofstream file(tempFile);
        file << "user:FE78BDB4183B677A55E711B17E9CB6CCC2AE7315\n";
        file << "admin:5BA5E8871A3D6B55F6D2C9B5F6A8C7D9E0F1A2B3\n";
        file.close();
        
        db = new ClientDB(tempFile);
        db->loadFromFile();
    }
    
    ~ClientDBFixture() {
        delete db;
        std::remove(tempFile.c_str());
    }
};

TEST(ClientDB_SaltFormat) {
    std::string salt = ClientDB::generateSalt();
    
    CHECK_EQUAL(16, salt.length());
    
    for (char c : salt) {
        CHECK((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'));
    }
}

TEST(ClientDB_HashFormat) {
    std::string salt = "0123456789ABCDEF";
    std::string password = "testpassword";
    
    std::string hash = ClientDB::generateHash(salt, password);
    
    CHECK_EQUAL(40, hash.length());
    
    for (char c : hash) {
        CHECK((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'));
    }
}

TEST_FIXTURE(ClientDBFixture, ClientDB_ClientExistsValid) {
    CHECK(db->clientExists("user"));
    CHECK(db->clientExists("admin"));
}

TEST_FIXTURE(ClientDBFixture, ClientDB_ClientExistsInvalid) {
    CHECK(!db->clientExists("wronguser"));
    CHECK(!db->clientExists("nonexistent"));
}

TEST_FIXTURE(ClientDBFixture, ClientDB_VerifyPassword) {
    CHECK(db->verifyPassword("user", "FE78BDB4183B677A55E711B17E9CB6CCC2AE7315"));
    CHECK(!db->verifyPassword("user", "WRONGHASH1234567890ABCDEF1234567890ABCDEF"));
}

} // unnamed namespace
