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
    
    it("should catch custom exception in expect_exception") {
        Test test;
        
        expect_exception(test.raiseCustomException(), CustomException);
    }
    
    it("should not catch custom exception in expect_exception with std::exception") {
        Test test;
        
        expect_exception(test.raiseCustomException(), std::exception);
    }
};
