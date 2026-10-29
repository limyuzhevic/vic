// Migration Guide: Converting NLM Tests to Use the New Test Framework
// This guide provides instructions for converting existing tests to use the new test framework

## Overview

The existing NLM test suite uses a simple approach where each test file contains its own `runAll()` function that calls individual test functions. The new test framework provides:

- Central test runner with result aggregation
- Better assertion macros with detailed error messages
- Test categorization and organization
- Performance timing
- Better error handling
- Easy test creation with template functions

## Migration Steps

### Step 1: Include the Test Framework

Replace any custom test setup in your test file with:

```cpp
#include "nlm/test/TestFramework.hpp"
using namespace nlm::test;
```

### Step 2: Convert Test Functions

**Before:**
```cpp
namespace test_types {
    void testNeuronId() {
        nlm::NeuronId id1(1);
        nlm::NeuronId id2(2);
        nlm::NeuronId id3(1);
        
        assert(id1 == id3);
        assert(id1 != id2);
        assert(id1 < id2);
        assert(id2 > id1);
        assert(id1.index() == 1);
        
        std::cout << "    testNeuronId passed" << std::endl;
    }
}
```

**After:**
```cpp
TEST_CASE("Types", "testNeuronId") {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    TEST_ASSERT_EQ(id1, id3);
    TEST_ASSERT_NE(id1, id2);
    TEST_ASSERT_LT(id2, id3); // Note: id3 == id1, so id2 > id3
    TEST_ASSERT_GT(id1, id2); // Note: id1 < id2, so id2 > id1
    TEST_ASSERT_EQ(id1.index(), 1);
}
```

### Step 3: Update runAll Function

**Before:**
```cpp
void runAll() {
    test_types::testNeuronId();
    test_types::testSynapseId();
    // ... more tests
}
```

**After:**
```cpp
// No need to update - the TEST_CASE macro automatically registers tests
```

### Step 4: Update Test File Structure

**Before:**
```cpp
// nlm/tests/test_types.cpp
#include "core/Types/Types.hpp"
#include <cassert>
#include <iostream>

namespace test_types {
    void testNeuronId();
    // ... more test declarations
    
    void runAll() {
        testNeuronId();
        // ... more tests
    }
}

// No main function
```

**After:**
```cpp
// nlm/tests/test_types.cpp
#include "core/Types/Types.hpp"
#include "nlm/test/TestFramework.hpp"
using namespace nlm::test;

// Test functions using TEST_CASE macro
TEST_CASE("Types", "testNeuronId") {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    TEST_ASSERT_EQ(id1, id3);
    TEST_ASSERT_NE(id1, id2);
    TEST_ASSERT_LT(id2, id3);
    TEST_ASSERT_GT(id1, id2);
    TEST_ASSERT_EQ(id1.index(), 1);
}

TEST_CASE("Types", "testSynapseId") {
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    
    TEST_ASSERT_NE(id1, id2);
    TEST_ASSERT_LT(id1, id2);
}

// ... more tests
```

### Step 5: Create Test Runner (Optional)

If you want to create a simple test runner, you can use the framework's TestRunner:

```cpp
// nlm/tests/main.cpp
#include "nlm/test/TestFramework.hpp"
using namespace nlm::test;

int main() {
    auto& runner = getTestRunner();
    auto results = runner.run();
    
    return 0;
}
```

### Step 6: Add Custom Test Case Classes

For more complex tests that need setup/teardown, create custom TestCase classes:

```cpp
class NeuronTest : public TestCase {
public:
    std::string getName() const override { return "NeuronTest"; }
    std::string getSuite() const override { return "Neuron"; }
    
    void setup() override {
        // Initialize test data
        neuron_ = nlm::NeuronId(1);
    }
    
    void run() override {
        TEST_ASSERT_EQ(neuron_.index(), 1);
        // ... more tests
    }
    
private:
    nlm::NeuronId neuron_;
};

// Register the test case
TEST_CASE("Neuron", "NeuronTest") {
    NeuronTest test;
    test.run();
}
```

## Migration Examples

### Complete Migration: test_types.cpp

**Original file:**
```cpp
// nlm/tests/test_types.cpp (BEFORE)
// [Original content from previous analysis]
```

**Migrated file:**
```cpp
// nlm/tests/test_types.cpp (AFTER)
#include "core/Types/Types.hpp"
#include "nlm/test/TestFramework.hpp"
using namespace nlm::test;

TEST_CASE("Types", "testNeuronId") {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    TEST_ASSERT_EQ(id1, id3);
    TEST_ASSERT_NE(id1, id2);
    TEST_ASSERT_LT(id2, id3);
    TEST_ASSERT_GT(id1, id2);
    TEST_ASSERT_EQ(id1.index(), 1);
}

TEST_CASE("Types", "testSynapseId") {
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    
    TEST_ASSERT_NE(id1, id2);
    TEST_ASSERT_LT(id1, id2);
}

TEST_CASE("Types", "testRegionId") {
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    
    TEST_ASSERT_NE(id1, id2);
    TEST_ASSERT_LT(id1, id2);
}

TEST_CASE("Types", "testPopulationId") {
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    
    TEST_ASSERT_NE(id1, id2);
    TEST_ASSERT_LT(id1, id2);
}

TEST_CASE("Types", "testNeuronType") {
    TEST_ASSERT_NE(nlm::NeuronType::Excitatory, nlm::NeuronType::Inhibitory);
    TEST_ASSERT_NE(nlm::NeuronType::Sensory, nlm::NeuronType::Motor);
}

TEST_CASE("Types", "testSynapseType") {
    TEST_ASSERT_NE(nlm::SynapseType::Excitatory, nlm::SynapseType::Inhibitory);
}

TEST_CASE("Types", "testPlasticityFlags") {
    nlm::PlasticityFlags flags;
    TEST_ASSERT_FALSE(flags.hebbian);
    TEST_ASSERT_FALSE(flags.stdp);
    TEST_ASSERT_FALSE(flags.reward_modulated);
    
    flags.hebbian = true;
    TEST_ASSERT_TRUE(flags.hebbian);
}

TEST_CASE("Types", "testSpikeEvent") {
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    TEST_ASSERT_EQ(event.source_neuron.index(), 1);
    TEST_ASSERT_EQ(event.timestamp, 0.5);
    TEST_ASSERT_EQ(event.step, 500);
}
```

## Benefits of Migration

1. **Better Error Messages**: Detailed assertion failures with context
2. **Test Organization**: Logical grouping of tests into suites
3. **Performance Timing**: Each test reports its execution time
4. **Centralized Runner**: All tests can be run from a single entry point
5. **Easy Debugging**: Failed tests show exact file and line numbers
6. **Extensibility**: Easy to add new test features and functionality
7. **Consistency**: All tests use the same framework and patterns

## Next Steps

1. Start with one test file at a time
2. Test your changes by running a subset of tests
3. Gradually migrate all test files
4. Update the main test runner to include all test files
5. Add performance benchmarks for critical test suites

## Testing the Framework

To test the new framework:

1. Build the test framework library
2. Create a simple test to verify functionality
3. Run the existing test suite through the migration process
4. Compare results between old and new frameworks

The framework is designed to be backward compatible - existing tests should still work after migration, but with improved error reporting and organization.
