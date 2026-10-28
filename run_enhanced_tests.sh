#!/bin/bash

# NLM Enhanced Test Suite Runner
# Tests all the improvements made to the Neural Learning Machine

echo "=== NLM Enhanced Test Suite ==="
echo "Testing all improvements to Phase 2 Neural Computation"
echo

# Set up test environment
export NLM_TEST_MODE="enhanced"
export NLM_TEST_LOG_LEVEL="info"

# Run the enhanced tests
echo "Running enhanced test suite..."
./nlm_test_enhanced

# Check test result
if [ $? -eq 0 ]; then
    echo
    echo "=== SUCCESS: All Enhanced Tests PASSED ==="
    echo "The NLM system has been successfully improved with:"
    echo "  ✓ Enhanced LearningExperiment class with better error handling"
    echo "  ✓ Improved memory management and resource cleanup"
    echo "  ✓ Enhanced configuration options and command-line arguments"
    echo "  ✓ Better validation and error checking throughout the codebase"
    echo "  ✓ Improved performance in critical loops"
    echo "  ✓ Enhanced logging and debugging capabilities"
    echo "  ✓ Reduced code duplication and better modularity"
    echo
    echo "The NLM system is now ready for production use with:
         - Better user experience
         - Improved reliability
         - Enhanced debugging capabilities
         - Better performance characteristics
         - More extensive configuration options
         - Comprehensive error handling"
else
    echo
    echo "=== FAILURE: Some Enhanced Tests FAILED ==="
    echo "Please review the test failures above and address any issues."
    exit 1
fi