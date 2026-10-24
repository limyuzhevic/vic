// Simple test to verify enhanced test framework
namespace test_types {

void testBasicTypes() {
    // Test basic type operations
    assert(sizeof(int) == 4);
    assert(sizeof(double) == 8);
    assert(sizeof(bool) == 1);
    
    int testInt = 42;
    double testDouble = 3.14;
    bool testBool = true;
    
    assert(testInt == 42);
    assert(testDouble > 3.13 && testDouble < 3.15);
    assert(testBool == true);
    
    std::cout << "    testBasicTypes passed" << std::endl;
}

void testConstants() {
    // Test constant definitions
    const double PI = 3.14159265359;
    const double E = 2.71828182846;
    
    assert(PI > 3.14 && PI < 3.15);
    assert(E > 2.71 && E < 2.72);
    
    std::cout << "    testConstants passed" << std::endl;
}

void testMathOperations() {
    int a = 10;
    int b = 3;
    
    assert(a + b == 13);
    assert(a - b == 7);
    assert(a * b == 30);
    assert(a / b == 3); // Integer division
    assert(a % b == 1);
    
    double x = 5.5;
    double y = 2.2;
    
    assert((x + y) > 7.5);
    assert((x - y) > 3.0);
    assert((x * y) > 12.0);
    assert((x / y) > 2.4 && (x / y) < 2.6);
    
    std::cout << "    testMathOperations passed" << std::endl;
}

void runAll() {
    testBasicTypes();
    testConstants();
    testMathOperations();
}

} // namespace test_types