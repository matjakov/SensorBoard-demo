/* 
 * File:   regex_test.cpp
 * Author: mk
 *
 * Created on 16.2.2017, 13:08:51
 */

#include <stdlib.h>
#include <iostream>

/*
 * Simple C++ Test Suite
 */

int RegEx::Match(char* src, int flags);

void testMatch() {
    char* src;
    int flags;
    RegEx regEx;
    int result = regEx.Match(src, flags);
    if (true /*check result*/) {
        std::cout << "%TEST_FAILED% time=0 testname=testMatch (regex_test) message=error message sample" << std::endl;
    }
}

std::string RegEx::Sub(unsigned short index);

void testSub() {
    unsigned short index;
    RegEx regEx;
    std::string result = regEx.Sub(index);
    if (true /*check result*/) {
        std::cout << "%TEST_FAILED% time=0 testname=testSub (regex_test) message=error message sample" << std::endl;
    }
}

int main(int argc, char** argv) {
    std::cout << "%SUITE_STARTING% regex_test" << std::endl;
    std::cout << "%SUITE_STARTED%" << std::endl;

    std::cout << "%TEST_STARTED% testMatch (regex_test)" << std::endl;
    testMatch();
    std::cout << "%TEST_FINISHED% time=0 testMatch (regex_test)" << std::endl;

    std::cout << "%TEST_STARTED% testSub (regex_test)" << std::endl;
    testSub();
    std::cout << "%TEST_FINISHED% time=0 testSub (regex_test)" << std::endl;

    std::cout << "%SUITE_FINISHED% time=0" << std::endl;

    return (EXIT_SUCCESS);
}

