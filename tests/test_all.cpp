#include "tests/test_stdp.h"
#include "tests/test_types.h"
#include "tests/test_clock.h"
#include "tests/test_random.h"
#include "tests/test_config.h"
#include "tests/test_brain.h"
#include "tests/test_neuron.h"
#include "tests/test_synapse.h"
#include "tests/test_main.h"

int main() {
    ::testing::InitGoogleTest();
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    return result;
}
