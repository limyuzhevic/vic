// Simple test demonstrating the new test framework
#include "core/Types/Types.hpp"
#include "nlm/test/TestFramework.hpp"
using namespace nlm::test;

TEST_CASE("Framework", "test_framework_basic") {
    // Test basic types
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    
    TEST_ASSERT_NE(id1, id2);
    TEST_ASSERT_LT(id1, id2);
    TEST_ASSERT_EQ(id1.index(), 1);
    TEST_ASSERT_EQ(id2.index(), 2);
}

TEST_CASE("Framework", "test_assertion_macros") {
    // Test various assertion macros
    TEST_ASSERT_EQ(1, 1);
    TEST_ASSERT_NE(1, 2);
    TEST_ASSERT_GT(2, 1);
    TEST_ASSERT_GE(2, 2);
    TEST_ASSERT_LT(1, 2);
    TEST_ASSERT_LE(1, 1);
}

TEST_CASE("Framework", "test_custom_test_case") {
    class SimpleTest : public TestCase {
    public:
        std::string getName() const override { return "SimpleTest"; }
        std::string getSuite() const override { return "Framework"; }
        
        void setup() override {
            testValue_ = 42;
        }
        
        void run() override {
            TEST_ASSERT_EQ(testValue_, 42);
            testValue_ = testValue_ * 2;
            TEST_ASSERT_EQ(testValue_, 84);
        }
        
    private:
        int testValue_ = 0;
    };
    
    SimpleTest test;
    test.run();
}

TEST_CASE("Framework", "test_framework_exceptions") {
    // Test that exceptions work properly
    TEST_ASSERT_THROW(
        throw std::invalid_argument("Test exception"),
        std::invalid_argument
    );
    
    TEST_ASSERT_NO_THROW(
        int x = 42;
    );
}

TEST_CASE("Framework", "test_framework_setup_teardown") {
    // Test that setup and teardown are called
    class SetupTeardownTest : public TestCase {
    public:
        std::string getName() const override { return "SetupTeardownTest"; }
        std::string getSuite() const override { return "Framework"; }
        
        void setup() override {
            setupCalled_ = true;
        }
        
        void teardown() override {
            teardownCalled_ = true;
        }
        
        void run() override {
            TEST_ASSERT_TRUE(setupCalled_);
            TEST_ASSERT_FALSE(teardownCalled_); // Should not be called yet
            TEST_ASSERT_EQ(testValue_, 0);
        }
        
    private:
        bool setupCalled_ = false;
        bool teardownCalled_ = false;
        int testValue_ = 0;
    };
    
    SetupTeardownTest test;
    test.setup();
    test.run();
    test.teardown();
    TEST_ASSERT_TRUE(teardownCalled_);
}
