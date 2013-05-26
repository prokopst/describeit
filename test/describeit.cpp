#include "describeit.h"

describe("describeit") {
    it("passes primitive test") {
        int a = 1;
        int b = 1;
        int c = a + b;
        expect(c == 2)
    }
};

describeit_main