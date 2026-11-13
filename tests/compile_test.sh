#!/usr/bin/env bash

# Simple test to verify the test framework compilation

echo "Testing NLM Test Framework compilation..."

# Create a simple test program to verify the framework
test_code="
#include \"TestFramework/TestFramework.hpp\"
#include <iostream>

namespace nlm {
namespace test {

class SimpleTestCategory : public BasicTestCategory {
public:
    SimpleTestCategory() : BasicTestCategory(\"Simple Tests\") {
        addTest(\"testBasic\", [this]() { testBasic(); }, \"Basic\");
    }
    
    void testBasic() {
        // Simple test
        std::cout << \"  Running basic test\" << std::endl;
    }
};

} // namespace test
} // namespace nlm

int main() {
    nlm::test::TestFramework framework;
    framework.addCategory(std::make_shared<SimpleTestCategory>());
    return framework.runAllTests();
}
"

echo "$test_code" > test_framework.cpp

# Try to compile
g++ -std=c++20 -I./tests/TestFramework -I./src -I./src/core -c test_framework.cpp -o test_framework 2>&1

if [ $? -eq 0 ]; then
    echo "Test framework compilation successful!"
    ./test_framework
else
    echo "Test framework compilation failed!"
fi

# Clean up
rm -f test_framework.cpp test_framework
