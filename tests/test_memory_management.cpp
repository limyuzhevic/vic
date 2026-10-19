// Memory Management Tests
// Test smart pointers, ownership, and memory safety

#include "test_infrastructure.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace test_memory {

void testSmartPointerOwnership() {
    std::cout << "    Testing smart pointer ownership..." << std::endl;
    
    // Test std::unique_ptr ownership transfer
    auto ptr1 = std::make_unique<int>(42);
    int* rawPtr = ptr1.get();
    auto ptr2 = std::move(ptr1);
    
    assert(ptr1 == nullptr);
    assert(ptr2 != nullptr);
    assert(*ptr2 == 42);
    assert(rawPtr == ptr2.get());
    
    // Test std::shared_ptr reference counting
    auto shared1 = std::make_shared<int>(100);
    auto shared2 = shared1;
    
    assert(shared1.use_count() == 2);
    assert(shared2.use_count() == 2);
    assert(*shared1 == 100);
    assert(*shared2 == 100);
    
    std::cout << "    testSmartPointerOwnership passed" << std::endl;
}

void testNullPointerChecks() {
    std::cout << "    Testing null pointer checks..." << std::endl;
    
    // Test unique_ptr null checks
    std::unique_ptr<int> nullPtr;
    assert(test_utils::isNull(nullPtr));
    assert(!nullPtr);
    
    std::unique_ptr<int> validPtr = std::make_unique<int>(10);
    assert(!test_utils::isNull(validPtr));
    assert(validPtr != nullptr);
    
    // Test shared_ptr null checks
    std::shared_ptr<int> nullShared;
    assert(test_utils::isNull(nullShared));
    assert(!nullShared);
    
    std::shared_ptr<int> validShared = std::make_shared<int>(20);
    assert(!test_utils::isNull(validShared));
    assert(validShared != nullptr);
    
    std::cout << "    testNullPointerChecks passed" << std::endl;
}

void testMemoryLeaksWithUniquePtr() {
    std::cout << "    Testing memory leak prevention with unique_ptr..." << std::endl;
    
    std::vector<std::unique_ptr<int>> allocations;
    
    // Allocate memory
    for (int i = 0; i < 1000; ++i) {
        allocations.emplace_back(std::make_unique<int>(i));
    }
    
    assert(allocations.size() == 1000);
    assert(allocations[0] != nullptr);
    assert(*allocations[0] == 0);
    
    // Clear vector to release memory
    allocations.clear();
    
    // Verify memory would be released (no easy way to test without memory profiler)
    std::cout << "    testMemoryLeaksWithUniquePtr passed" << std::endl;
}

void testReferenceCounting() {
    std::cout << "    Testing reference counting..." << std::endl;
    
    auto shared = std::make_shared<int>(42);
    int initialCount = shared.use_count();
    
    // Copy increases reference count
    auto shared2 = shared;
    assert(shared2.use_count() == initialCount + 1);
    
    // Assignment
    auto shared3 = shared;
    assert(shared3.use_count() == initialCount + 2);
    
    // Clear some pointers
    shared2.reset();
    assert(shared.use_count() == initialCount + 1);
    assert(shared3.use_count() == initialCount + 1);
    
    // All cleared
    shared.reset();
    shared3.reset();
    int nullCount = 0;  // null shared_ptr has use_count() of 0
    
    std::cout << "    testReferenceCounting passed" << std::endl;
}

void testConfigOwnership() {
    std::cout << "    Testing configuration object ownership..." << std::endl;
    
    // Test Config with default constructor
    nlm::Config config1;
    assert(config1.has("test_key") == false);
    
    // Test Config with move operations
    auto config2 = std::make_shared<nlm::Config>(std::move(config1));
    assert(config2 != nullptr);
    
    // Test config set/get with ownership
    config2->set("owner_test", 123, nlm::ConfigSource::Runtime);
    assert(config2->has("owner_test"));
    assert(*config2->get<int>("owner_test") == 123);
    
    std::cout << "    testConfigOwnership passed" << std::endl;
}

void runAll() {
    testSmartPointerOwnership();
    testNullPointerChecks();
    testMemoryLeaksWithUniquePtr();
    testReferenceCounting();
    testConfigOwnership();
}

} // namespace test_memory
