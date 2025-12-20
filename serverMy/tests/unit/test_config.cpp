#include <UnitTest++/UnitTest++.h>
#include "../../include/Config.h"

namespace {

TEST(Config_Constants) {
    CHECK_EQUAL(33333, Config::DEFAULT_PORT);
    CHECK_EQUAL("/etc/vcalc.conf", Config::DEFAULT_CLIENT_DB);
    CHECK_EQUAL("/var/log/vcalc.log", Config::DEFAULT_LOG_FILE);
    CHECK_EQUAL(16, Config::SALT_HEX_LENGTH);
    CHECK_EQUAL(64, Config::SALT_BITS);
    CHECK_EQUAL("ERR", Config::ERR_MSG);
    CHECK_EQUAL("OK", Config::OK_MSG);
}

TEST(Config_DefaultParametersValid) {
    CHECK(Config::DEFAULT_PORT >= 1 && Config::DEFAULT_PORT <= 65535);
    CHECK(!Config::DEFAULT_CLIENT_DB.empty());
    CHECK(!Config::DEFAULT_LOG_FILE.empty());
    CHECK(Config::SALT_BITS > 0);
    CHECK(Config::SALT_HEX_LENGTH > 0);
}

TEST(Config_CustomParametersLogic) {
    int validPorts[] = {1, 1000, 33333, 44444, 65535};
    for (int port : validPorts) {
        CHECK(port >= 1 && port <= 65535);
    }
    
    CHECK_EQUAL(3, Config::ERR_MSG.length());
    CHECK_EQUAL(2, Config::OK_MSG.length());
    CHECK(Config::ERR_MSG == "ERR");
    CHECK(Config::OK_MSG == "OK");
}

} // unnamed namespace
