#include <UnitTest++/UnitTest++.h>
#include "../../include/Config.h"
#include <cstdint>

namespace {

TEST(Protocol_Format) {
    CHECK_EQUAL("ERR", Config::ERR_MSG);
    CHECK_EQUAL("OK", Config::OK_MSG);
}

TEST(Protocol_Sequence) {
    CHECK_EQUAL(3, Config::ERR_MSG.length());
    CHECK_EQUAL(2, Config::OK_MSG.length());
}

TEST(Protocol_BinaryFormatConcept) {
    CHECK_EQUAL(4, sizeof(uint32_t));
    CHECK_EQUAL(8, sizeof(double));
}

} // unnamed namespace
