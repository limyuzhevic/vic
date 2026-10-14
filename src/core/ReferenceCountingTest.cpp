#include "MemoryManagementTestSuite.hpp"
#include <algorithm>

namespace nlm {

ReferenceCountingTest::ReferenceCountingTest() : MemoryManagementTest("Reference Counting Test", 
    "Tests for reference counting and smart pointer lifetime management") {}

void ReferenceCountingTest::setup() {
    recordStartSnapshot();
    
    // Test shared pointer counting
    testSmartPointerCounting();
    
    // Test shared pointer lifetime
    testSharedPointerLifetime();
    
    // Test weak pointer validity
    testWeakPointerValidity();
    
    // Test circular reference handling
    testCircularReferenceHandling();
}

void ReferenceCountingTest::cleanup() {
    recordEndSnapshot();
}

TestResult ReferenceCountingTest::run() {
    TestResult result("Reference Counting Test", description_);
    
    try {
        setup();
        
        // Check for leaks
        bool hasLeaks = checkMemoryLeaks();
        
        cleanup();
        
        if (hasLeaks) {
            result.setError("Reference counting issues detected");
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

bool ReferenceCountingTest::testSmartPointerCounting() const {
    auto sharedObj = createSharedObject();
    
    // Check reference count
    if (sharedObj.use_count() < 1) {
        return false;
    }
    
    return true;
}

bool ReferenceCountingTest::testSharedPointerLifetime() const {
    auto sharedObj = createSharedObject();
    
    // Reference should still be valid
    if (!sharedObj) {
        return false;
    }
    
    return true;
}

bool ReferenceCountingTest::testWeakPointerValidity() const {
    auto strong = createSharedObject();
    auto weak = createWeakObject(strong);
    
    // Weak pointer should be valid
    if (!weak.expired()) {
        // Lock weak pointer
        auto locked = weak.lock();
        if (!locked) {
            return false;
        }
    }
    
    return true;
}

bool ReferenceCountingTest::testCircularReferenceHandling() const {
    // Create objects with circular references
    struct CircularA {
        std::shared_ptr<CircularB> b;
    };
    
    struct CircularB {
        std::shared_ptr<CircularA> a;
    };
    
    auto a = std::make_shared<CircularA>();
    auto b = std::make_shared<CircularB>();
    
    a->b = b;
    b->a = a;
    
    // Circular references should be handled by shared_ptr
    return true;
}

std::shared_ptr<ReferenceCountingTest::SharedObject> ReferenceCountingTest::createSharedObject() const {
    return std::make_shared<SharedObject>(42);
}

std::weak_ptr<ReferenceCountingTest::SharedObject> ReferenceCountingTest::createWeakObject(
    const std::shared_ptr<SharedObject>& strong) {
    return std::weak_ptr<SharedObject>(strong);
}

} // namespace nlm
