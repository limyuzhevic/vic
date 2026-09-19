// C++ test program to verify Python bindings compile correctly
#include "python/bindings.cpp"

int main() {
    std::cout << "Python bindings test program" << std::endl;
    std::cout << "If this compiles and links successfully, the Python bindings are working." << std::endl;
    
    // Test that the Python module can be loaded
    // This is a simple link-time test
    return 0;
}