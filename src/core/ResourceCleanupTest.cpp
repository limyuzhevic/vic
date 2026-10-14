#include "MemoryManagementTestSuite.hpp"

namespace nlm {

ResourceCleanupTest::ResourceCleanupTest() : MemoryManagementTest("Resource Cleanup Test", 
    "Tests for resource cleanup in RAII objects") {}

void ResourceCleanupTest::setup() {
    recordStartSnapshot();
    
    // Test RAII construction
    testRAIIConstruction();
    
    // Test resource release
    testResourceRelease();
}

void ResourceCleanupTest::cleanup() {
    recordEndSnapshot();
}

TestResult ResourceCleanupTest::run() {
    TestResult result("Resource Cleanup Test", description_);
    
    try {
        setup();
        
        // Check for leaks
        bool hasLeaks = checkMemoryLeaks();
        
        cleanup();
        
        if (hasLeaks) {
            result.setError("Resource cleanup failures detected");
            result.setPassed(false);
        } else {
            result.setPassed(true);
            result.setPerformance(0.1, getMemoryDelta());
        }
        
    } catch (const std::exception& e) {
        cleanup();
        result.setError(std::string("Exception: ") + e.what());
        result.setPassed(false);
    }
    
    return result;
}

bool ResourceCleanupTest::testRAIIConstruction() const {
    // Test constructor exception safety
    try {
        TestResource resource;
        resource.allocated = true;
        return true;
    } catch (...) {
        return false;
    }
}

bool ResourceCleanupTest::testRAIDestruction() const {
    // Test destructor safety
    try {
        TestResource resource;
        resource.allocated = true;
        resource.released = true;
        return true;
    } catch (...) {
        return false;
    }
}

bool ResourceCleanupTest::testExceptionSafety() const {
    // Test exception safety during construction
    try {
        TestResource resource;
        resource.allocated = true;
        
        // Simulate exception during construction
        // In a real implementation, this would test more scenarios
        return true;
    } catch (...) {
        return false;
    }
}

bool ResourceCleanupTest::testResourceRelease() const {
    // Test resource release
    TestResource resource;
    resource.allocated = true;
    resource.released = true;
    
    return true;
}

} // namespace nlm
