#include "describeit.h"

class Complex {
protected:
    float real;
    float imag;
public:
    Complex() : real(0.0f), imag(0.0f) {}
    Complex(float real, float imag) : real(real), imag(imag) {}
    
};

describe(FloatMath) {
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
