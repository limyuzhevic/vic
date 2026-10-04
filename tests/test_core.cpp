// Unit test for core functionality
#include <iostream>
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"

int test_unit_core() {
    std::cout << "Unit Core Test: Testing core types..." << std::endl;
    
    // Test basic types
    nlm::Vector3 test_vector(1.0f, 2.0f, 3.0f);
    nlm::Matrix4x4 test_matrix;
    
    std::cout << "Vector: (" << test_vector.x << ", " << test_vector.y << ", " << test_vector.z << ")" << std::endl;
    std::cout << "Matrix initialized successfully" << std::endl;
    
    return 0;
}

int main() {
    return test_unit_core();
}
