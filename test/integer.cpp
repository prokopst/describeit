#include "describeit.h"
#include <climits>

struct Integer {
public:
    static const int POSITIVE_INFINITY = INT_MAX;
    static const int NEGATIVE_INFINITY = INT_MIN;
    int value;
    
    Integer(int value): value(value) {
    }
    
    Integer& operator*(const Integer& other) {
        value *= other.value;
        return *this;
    }
    
    Integer& operator+(const Integer& other) {
        value += other.value;
        return *this;
    }
    
    Integer& operator-(const Integer& other) {
        value -= other.value;
        return *this;
    }
};

namespace detail {
    template <>
    struct Expectation<Integer> : public ExpectationBase<int> {
        Expectation(const Integer& integer) : ExpectationBase<int>(integer.value) {
            
        }
    };
}

describe(Integer) {    
    it("should handle addition") {
        Integer a = 1000;
        Integer b = 337;
        Integer c = a + b;
        
        expect(c) == 1337;
    }
    
    it("should handle subtraction") {
        Integer a = 1337;
        Integer b = 337;
        Integer c = a - b;
        
        expect(c) == 1000;
    }
    
    it("passes multiplication") {
        Integer a = 3;
        Integer b = 2;
        Integer c = a * b;
        
        expect(c) == 6;
    }
};
