#include "describeit.h"
#include <sstream>
#include <iostream>
#include <cassert>

const std::string EXPECTED_ALL =
"Integer\n"
"- should handle addition\n"
"- should handle subtraction (FAILED)\n"
"- passes multiplication\n"
"Test\n"
"- should raise custom exception (ERROR)\n"
"- should raise standard exception (ERROR)\n"
"- should fail in expect (FAILED)\n"
"- should catch custom exception in expect_exception\n"
"- should not catch custom exception in expect_exception with std::exception (ERROR)\n";

template <typename T1, typename T2>
size_t check(T1 expected, T2 actual) {
    bool result = (expected == actual);
    if (result == false) {
        std::cout << ">> runAll FAILURE" << std::endl;
        std::cout << ">> expected output ----------" << std::endl;
        std::cout << expected << std::endl;
        std::cout << ">> actual output ------------" << std::endl;
        std::cout << actual << std::endl;
        std::cout << ">> --------------------------" << std::endl << std::endl;
        return 1;
    }
    else {
        std::cout << ">> runAll SUCCESS" << std::endl << std::endl;
        return 0;
    }
}

int main(int, char**) {
    describeit::DescribeIt* describeIt = describeit::DescribeIt::getInstance();
    
    std::ostringstream output;
    describeit::BehavePrinter behavePrinter(output);
    
    size_t descriptionsCount = describeIt->descriptionsCount();
    assert(descriptionsCount == 2);
    
    describeIt->runAll(behavePrinter);
    
    std::string str = output.str();
    
    size_t failureCount = 0;
    failureCount += check(EXPECTED_ALL, str);
    
    return failureCount;
}
