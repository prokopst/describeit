#include "describeit.h"
#include <sstream>
#include <iostream>
#include <cassert>

const std::string EXPECTED_ALL =
"math:\n"
"- passes primitive test\n";

template <typename T1, typename T2>
bool check(T1 expected, T2 actual) {
    bool result = (expected == actual);
    if (result == false) {
        std::cout << ">> runAll FAILURE" << std::endl;
        std::cout << "expected output ----------" << std::endl;
        std::cout << expected << std::endl;
        std::cout << "actual output ------------" << std::endl;
        std::cout << actual << std::endl;
        std::cout << "--------------------------" << std::endl << std::endl;
        return false;
    }
    else {
        std::cout << ">> runAll SUCCESS" << std::endl << std::endl;
        return true;
    }
}

int main(int, char**) {
    DescribeIt* describeIt = DescribeIt::getInstance();
    
    std::ostringstream output;
    BehavePrinter behavePrinter(output);
    
    size_t descriptionsCount = describeIt->descriptionsCount();
    assert(descriptionsCount == 2);
    
    describeIt->runAll(behavePrinter);
    
    std::string str = output.str();
    
    return (int)check(str, EXPECTED_ALL);
}
