#include "dragonshell.cc"
#include "utils.cc"
#include "commands.cc"
#include <gtest/gtest.h>

TEST(CheckIfFileExists, positive)
{
    const char* fileName = "/bin/ls";
    ASSERT_EQ(true, checkIfFileExists(fileName));
}