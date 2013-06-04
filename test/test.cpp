#include "describeit.h"
#include <stdexcept>

struct CustomException {};

struct Test {
    void raiseCustomException() const {
        CustomException exception;
        throw exception;
    }
    
    void raiseStandardException() const {
        std::exception exception;
        throw exception;
    }
};

describe(Test) {
    it("should raise custom exception") {
        Test test;
        test.raiseCustomException();
    }
    
    it("should raise standard exception") {
        Test test;
        test.raiseStandardException();
    }
    
    it("should fail in expect") {
        expect(2) == 3;
    }
};
