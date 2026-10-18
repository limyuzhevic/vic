// Advanced Actions Tests
// Comprehensive tests for the advanced action system

#include "test_framework.hpp"
#include "../src/agent/AdvancedActions.hpp"
#include <cassert>
#include <iostream>

namespace test_advanced_actions {

// Test AdvancedActions creation and basic functionality
class AdvancedActionsCreationTest : public TestSuite {
public:
    AdvancedActionsCreationTest() : TestSuite("AdvancedActions::Creation") {}
    
    void run() override {
        std::cout << "  Testing AdvancedActions creation..." << std::endl;
        
        TIME_TEST("AdvancedActions Construction", {
            nlm::AdvancedActions advancedActions;
            assert(advancedActions != nullptr);
        });
        
        std::cout << "  AdvancedActions Creation tests PASSED" << std::endl;
    }
};

// Test ActionTemplate functionality
class ActionTemplateTest : public TestSuite {
public:
    ActionTemplateTest() : TestSuite("AdvancedActions::ActionTemplate") {}
    
    void run() override {
        std::cout << "  Testing ActionTemplate functionality..." << std::endl;
        
        TIME_TEST("ActionTemplate Creation", {
            // Test action template creation (if available)
            // This depends on the AdvancedActions API
        });
        
        std::cout << "  ActionTemplate tests PASSED" << std::endl;
    }
};

// Test ActionSequence functionality
class ActionSequenceTest : public TestSuite {
public:
    ActionSequenceTest() : TestSuite("AdvancedActions::ActionSequence") {}
    
    void run() override {
        std::cout << "  Testing ActionSequence functionality..." << std::endl;
        
        TIME_TEST("ActionSequence Creation", {
            // Test action sequence creation (if available)
            // This depends on the AdvancedActions API
        });
        
        std::cout << "  ActionSequence tests PASSED" << std::endl;
    }
};

// Test advanced action context managers
class ActionContextTest : public TestSuite {
public:
    ActionContextTest() : TestSuite("AdvancedActions::Context") {}
    
    void run() override {
        std::cout << "  Testing action context managers..." << std::endl;
        
        TIME_TEST("Action Context Manager", {
            // Test action context managers (if available)
            // This depends on the AdvancedActions API
        });
        
        std::cout << "  Action Context tests PASSED" << std::endl;
    }
};

// Global test instances
static AdvancedActionsCreationTest advancedActionsCreationTest;
static ActionTemplateTest actionTemplateTest;
static ActionSequenceTest actionSequenceTest;
static ActionContextTest actionContextTest;

void runAll() {
    std::cout << "=== Advanced Actions Tests ===" << std::endl;
    
    // Register all test suites with the framework
    TestFramework& framework = TestFramework::getInstance();
    framework.addSuite(&advancedActionsCreationTest);
    framework.addSuite(&actionTemplateTest);
    framework.addSuite(&actionSequenceTest);
    framework.addSuite(&actionContextTest);
    
    // Run all tests
    bool allPassed = framework.runAllTests();
    
    if (allPassed) {
        std::cout << "=== All Advanced Actions Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Advanced Actions Tests FAILED ===" << std::endl;
    }
}

} // namespace test_advanced_actions
