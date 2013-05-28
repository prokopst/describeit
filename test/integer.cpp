#include "describeit.h"

describe(IntegerMath) {
    it("passes primitive test") {
        int a = 1;
        int b = 1;
        int c = a + b;
        
        expect(c == 2);
    }
    
    it("passes multiplication") {
        int a = 2;
        int b = 2;
        int c = a * b;
        
        expect(c == 4);
    }
};
