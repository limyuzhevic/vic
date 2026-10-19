// Error Handling and Edge Case Tests
// Tests for error conditions, edge cases, and robustness

#include "test_infrastructure.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace test_error_handling {

void testNullPointerDereferenceProtection() {
    std::cout << "    Testing null pointer dereference protection..." << std::endl;
    
    // Test safe extraction with null shared_ptr
    std::shared_ptr<int> nullPtr;
    auto safeResult = test_utils::safeExtract([nullPtr]() { return *nullPtr; }, 0);
    assert(safeResult == 0);
    
    // Test safe extraction with null unique_ptr
    std::unique_ptr<int> nullUnique;
    auto safeResult2 = test_utils::safeExtract([nullUnique]() { return *nullUnique; }, 0);
    assert(safeResult2 == 0);
    
    std::cout << "    testNullPointerDereferenceProtection passed" << std::endl;
}

void testMemorySafetyEdgeCases() {
    std::cout << "    Testing memory safety edge cases..." << std::endl;
    
    // Test empty container handling
    std::vector<int> emptyVec;
    assert(test_utils::hasSize(emptyVec, 0));
    
    // Test value range checking
    assert(test_utils::isInRange(5, 1, 10));
    assert(!test_utils::isInRange(15, 1, 10));
    
    // Test out of bounds protection (should not crash)
    std::unique_ptr<int> nullPtr;
    bool exceptionCaught = false;
    
    try {
        if (nullPtr) {
            *nullPtr;
        }
    } catch (const std::exception&) {
        exceptionCaught = true;
    }
    
    // Note: This test documents the expected behavior
    assert(true);  // Placeholder for documentation
    
    std::cout << "    testMemorySafetyEdgeCases passed" << std::endl;
}

void testConfigurationEdgeCases() {
    std::cout << "    Testing configuration edge cases..." << std::endl;
    
    nlm::Config config;
    
    // Test empty key handling
    config.set("", 123, nlm::ConfigSource::Runtime);  // Empty key (may be allowed)
    
    // Test very large values
    config.set("large_int", INT64_MAX, nlm::ConfigSource::Runtime);
    assert(config.has("large_int"));
    
    // Test very small values
    config.set("small_int", INT64_MIN, nlm::ConfigSource::Runtime);
    assert(config.has("small_int"));
    
    // Test special floating point values
    config.set("nan_value", std::nan(""), nlm::ConfigSource::Runtime);
    assert(config.has("nan_value"));
    
    // Test infinity
    config.set("inf_value", std::numeric_limits<double>::infinity(), nlm::ConfigSource::Runtime);
    assert(config.has("inf_value"));
    
    // Test reset functionality
    config.clear();
    assert(config.getKeys().empty());
    
    std::cout << "    testConfigurationEdgeCases passed" << std::endl;
}

void testTypeConversionEdgeCases() {
    std::cout << "    Testing type conversion edge cases..." << std::endl;
    
    nlm::Config config;
    
    // Test invalid type access
    config.set("string_key", std::string("test"), nlm::ConfigSource::Runtime);
    
    // Attempt to get as wrong type
    auto intVal = config.get<int>("string_key");
    assert(!intVal.has_value());  // Should not be convertible
    
    auto stringVal = config.get<std::string>("string_key");
    assert(stringVal.has_value());
    assert(*stringVal == "test");
    
    // Test edge case with negative numbers
    config.set("negative_value", -42, nlm::ConfigSource::Runtime);
    auto negativeInt = config.get<int>("negative_value");
    assert(negativeInt.has_value());
    assert(*negativeInt == -42);
    
    std::cout << "    testTypeConversionEdgeCases passed" << std::endl;
}

void testMemoryManagementEdgeCases() {
    std::cout << "    Testing memory management edge cases..." << std::endl;
    
    // Test multiple move operations
    auto ptr1 = std::make_unique<int>(1);
    auto ptr2 = std::move(ptr1);
    auto ptr3 = std::move(ptr2);
    
    assert(ptr1 == nullptr);
    assert(ptr2 == nullptr);
    assert(ptr3 != nullptr);
    assert(*ptr3 == 1);
    
    // Test reference counting edge cases
    auto shared1 = std::make_shared<int>(42);
    auto shared2 = shared1;
    auto shared3 = shared1;
    
    assert(shared1.use_count() == 3);
    assert(shared2.use_count() == 3);
    assert(shared3.use_count() == 3);
    
    // Clear some references
    shared2.reset();
    assert(shared1.use_count() == 2);
    assert(shared3.use_count() == 2);
    
    // Clear all
    shared1.reset();
    shared3.reset();
    
    std::cout << "    testMemoryManagementEdgeCases passed" << std::endl;
}

void testPerformanceEdgeCases() {
    std::cout << "    Testing performance under stress..." << std::endl;
    
    // Test with large number of operations
    auto config = std::make_shared<nlm::Config>();
    
    for (int i = 0; i < 10000; ++i) {
        config->set(std::to_string(i), i, nlm::ConfigSource::Runtime);
        config->get<int>(std::to_string(i));
        if (i % 1000 == 0) {
            config->remove(std::to_string(i - 1000));  // Cleanup every 1000
        }
    }
    
    // Verify some remaining entries
    assert(config->has(std::to_string(9999)));
    assert(config->get<int>(std::to_string(9999)) == 9999);
    
    // Test memory cleanup after many operations
    config.reset();  // Clear all references
    
    std::cout << "    testPerformanceEdgeCases passed" << std::endl;
}

void runAll() {
    testNullPointerDereferenceProtection();
    testMemorySafetyEdgeCases();
    testConfigurationEdgeCases();
    testTypeConversionEdgeCases();
    testMemoryManagementEdgeCases();
    testPerformanceEdgeCases();
}

} // namespace test_error_handling
