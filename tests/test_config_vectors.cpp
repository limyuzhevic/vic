#include "Config.hpp"
#include <cassert>
#include <iostream>

namespace test_config {

void testVectorInt() {
    nlm::Config config;
    
    // Test setting and getting vector<int>
    std::vector<int> vec1 = {1, 2, 3};
    config.set("vec_int", vec1);
    
    auto result = config.get<std::vector<int>>("vec_int");
    assert(result.has_value());
    assert(result.value() == vec1);
    
    std::cout << "    testVectorInt passed" << std::endl;
}

void testVectorDouble() {
    nlm::Config config;
    
    // Test setting and getting vector<double>
    std::vector<double> vec2 = {1.5, 2.5, 3.5};
    config.set("vec_double", vec2);
    
    auto result = config.get<std::vector<double>>("vec_double");
    assert(result.has_value());
    assert(result.value() == vec2);
    
    std::cout << "    testVectorDouble passed" << std::endl;
}

void testVectorString() {
    nlm::Config config;
    
    // Test setting and getting vector<std::string>
    std::vector<std::string> vec3 = {"a", "b", "c"};
    config.set("vec_string", vec3);
    
    auto result = config.get<std::vector<std::string>>("vec_string");
    assert(result.has_value());
    assert(result.value() == vec3);
    
    std::cout << "    testVectorString passed" << std::endl;
}

void testVectorGetOr() {
    nlm::Config config;
    
    // Test getOr with vector types
    std::vector<int> default_vec = {0, 0, 0};
    std::vector<int> result = config.getOr<std::vector<int>>("missing", default_vec);
    assert(result == default_vec);
    
    std::vector<int> vec = {1, 2, 3};
    config.set("existing", vec);
    result = config.getOr<std::vector<int>>("existing", default_vec);
    assert(result == vec);
    
    std::cout << "    testVectorGetOr passed" << std::endl;
}

void testVectorLoadSave() {
    nlm::Config config;
    
    // Test vector loading from file simulation
    config.set("vec_from_file", std::vector<int>({10, 20, 30}));
    config.set("vec_str_from_file", std::vector<std::string>({"x", "y", "z"}));
    
    // Test that summary includes vector types
    std::string summary = config.summary();
    assert(summary.find("vec_from_file = [10, 20, 30]") != std::string::npos);
    assert(summary.find("vec_str_from_file = [\"x\", \"y\", \"z\"]") != std::string::npos);
    
    std::cout << "    testVectorLoadSave passed" << std::endl;
}

void runAll() {
    testVectorInt();
    testVectorDouble();
    testVectorString();
    testVectorGetOr();
    testVectorLoadSave();
}

} // namespace test_config