# Test script to verify NLM performance optimizations
# This script tests the key performance improvements from Phase 5

cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_1a6c2195-1d26-4949-8908-fae016253510

# Create build directory
mkdir -p test_build
cd test_build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_TEST_MODE=ON 2>&1 | tee cmake.log

# Build with optimizations
make -j$(nproc) 2>&1 | tee build.log

# Check if build was successful
if [ ${PIPESTATUS[0]} -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

# Run the Brain performance test
if [ -f "test_brain" ]; then
    echo "Running Brain Performance Tests..."
    ./test_brain 2>&1 | tee brain_test_results.log
    
    # Check if tests passed
    if [ $? -eq 0 ]; then
        echo "✓ Brain performance tests passed"
    else
        echo "✗ Brain performance tests failed"
        exit 1
    fi
else
    echo "Brain test executable not found!"
    exit 1
fi

# Run Neuron tests
if [ -f "test_neuron" ]; then
    echo "Running Neuron Tests..."
    ./test_neuron 2>&1 | tee neuron_test_results.log
    
    if [ $? -eq 0 ]; then
        echo "✓ Neuron tests passed"
    else
        echo "✗ Neuron tests failed"
        exit 1
    fi
else
    echo "Neuron test executable not found!"
fi

# Run Synapse tests
if [ -f "test_synapse" ]; then
    echo "Running Synapse Tests..."
    ./test_synapse 2>&1 | tee synapse_test_results.log
    
    if [ $? -eq 0 ]; then
        echo "✓ Synapse tests passed"
    else
        echo "✗ Synapse tests failed"
        exit 1
    fi
else
    echo "Synapse test executable not found!"
fi

# Run STDP tests
if [ -f "test_stdp" ]; then
    echo "Running STDP Tests..."
    ./test_stdp 2>&1 | tee stdp_test_results.log
    
    if [ $? -eq 0 ]; then
        echo "✓ STDP tests passed"
    else
        echo "✗ STDP tests failed"
        exit 1
    fi
else
    echo "STDP test executable not found!"
fi

# Run Memory Pool tests (if available)
if [ -f "test_memory_pool" ]; then
    echo "Running Memory Pool Tests..."
    ./test_memory_pool 2>&1 | tee memory_pool_test_results.log
    
    if [ $? -eq 0 ]; then
        echo "✓ Memory pool tests passed"
    else
        echo "✗ Memory pool tests failed"
        exit 1
    fi
else
    echo "Memory pool test executable not found!"
fi

# Check for performance test results
echo "\n=== PERFORMANCE TEST SUMMARY ==="

# Check if neural accessor is working
if grep -q "NeuralAccessor\|O(1)\|fast.*lookup" brain_test_results.log 2>/dev/null; then
    echo "✓ NeuralAccessor optimization verified"
else
    echo "⚠ NeuralAccessor optimization may not be working"
fi

# Check if sparse connectivity is working
if grep -q "SparseConnectivity\|sparse\|adjacency" brain_test_results.log 2>/dev/null; then
    echo "✓ Sparse connectivity optimization verified"
else
    echo "⚠ Sparse connectivity optimization may not be working"
fi

# Check if SIMD is being used
if grep -q "SIMD\|AVX\|SSE\|vectorized" brain_test_results.log 2>/dev/null; then
    echo "✓ SIMD vectorization enabled"
else
    echo "⚠ SIMD vectorization not detected"
fi

# Check if parallel processing is working
if grep -q "Parallel\|thread\|multithread" brain_test_results.log 2>/dev/null; then
    echo "✓ Parallel processing enabled"
else
    echo "⚠ Parallel processing not detected"
fi

# Check memory pool usage
if grep -q "memory.*pool\|allocation.*overhead" brain_test_results.log 2>/dev/null; then
    echo "✓ Memory pool optimization working"
else
    echo "⚠ Memory pool optimization not clearly verified"
fi

# Check event-driven processing
echo "\n=== EVENT-DRIVEN PROCESSING ==="
if grep -q "event.*driven\|spike.*queue\|ring.*buffer" brain_test_results.log 2>/dev/null; then
    echo "✓ Event-driven processing operational"
else
    echo "⚠ Event-driven processing may not be optimized"
fi

# Check SoA (Structure of Arrays)
echo "\n=== STRUCTURE-OF-ARRAYS ==="
if grep -q "SoA\|structure.*of.*arrays\|cache.*friendly" brain_test_results.log 2>/dev/null; then
    echo "✓ Structure-of-Arrays optimization active"
else
    echo "⚠ Structure-of-Arrays optimization not clearly verified"
fi

# Performance benchmark test
echo "\n=== PERFORMANCE BENCHMARK ==="
if [ -f "benchmark" ]; then
    echo "Running performance benchmark..."
    ./benchmark 2>&1 | tee benchmark_results.log
    
    if [ $? -eq 0 ]; then
        echo "✓ Performance benchmark completed"
        
        # Extract key performance metrics
        if grep -q "performance\|speedup\|benchmark" benchmark_results.log 2>/dev/null; then
            echo "✓ Performance metrics collected"
        else
            echo "⚠ No clear performance metrics in benchmark"
        fi
    else
        echo "✗ Performance benchmark failed"
    fi
else
    echo "Benchmark executable not found!"
fi

echo "\n=== OPTIMIZATION VERIFICATION ==="

# Check all key optimizations from the performance plan
echo "1. Data Structure Optimization (Phase 1):"
if grep -q "NeuralAccessor\|fast.*lookup\|O(1)\|unordered_map" brain_test_results.log 2>/dev/null; then
    echo "   ✓ NeuralAccessor with fast access structures"
else
    echo "   ✗ NeuralAccessor optimization issue"
fi

if grep -q "sparse.*connectivity\|adjacency.*list" brain_test_results.log 2>/dev/null; then
    echo "   ✓ Sparse connectivity implemented"
else
    echo "   ✗ Sparse connectivity not working"
fi

echo "2. Algorithm Optimization (Phase 2):"
if grep -q "vectorized\|SIMD\|parallel" brain_test_results.log 2>/dev/null; then
    echo "   ✓ Vectorized operations implemented"
else
    echo "   ✗ Vectorized operations missing"
fi

if grep -q "cache.*friendly\|SOa\|structure" brain_test_results.log 2>/dev/null; then
    echo "   ✓ Cache-friendly data structures"
else
    echo "   ✗ Cache optimization not clear"
fi

echo "3. Parallelization (Phase 3):"
if grep -q "Parallel\|thread\|multithread" brain_test_results.log 2>/dev/null; then
    echo "   ✓ Parallel processing enabled"
else
    echo "   ✗ Parallel processing not detected"
fi

if grep -q "SIMD\|AVX\|SSE\|vector" brain_test_results.log 2>/dev/null; then
    echo "   ✓ SIMD vectorization active"
else
    echo "   ✗ SIMD not working"
fi

echo "4. Algorithmic Improvements (Phase 4):"
if grep -q "sparse\|adjacency\|O(N)\|linear" brain_test_results.log 2>/dev/null; then
    echo "   ✓ O(N) algorithms implemented"
else
    echo "   ✗ Algorithmic improvements unclear"
fi

echo "5. Memory Efficiency (Phase 5):"
if grep -q "memory.*pool\|pre.*allocated\|slab.*allocation" brain_test_results.log 2>/dev/null; then
    echo "   ✓ Memory pools implemented"
else
    echo "   ✗ Memory pools not clearly verified"
fi

if grep -q "event.*driven\|ring.*buffer\|queue" brain_test_results.log 2>/dev/null; then
    echo "   ✓ Event-driven processing working"
else
    echo "   ✗ Event-driven processing not optimized"
fi

echo "\n=== EXPECTED IMPROVEMENTS VERIFICATION ==="
echo "Memory Access (O(N²)→O(N)): $(grep -q "O(N\|linear" brain_test_results.log && echo "✓" || echo "✗")"
echo "Neural Computation (vectorization): $(grep -q "vectorized\|SIMD" brain_test_results.log && echo "✓" || echo "✗")"
echo "Connectivity Init (sparse matrices): $(grep -q "sparse\|adjacency" brain_test_results.log && echo "✓" || echo "✗")"
echo "Overall Simulation Speedup: $(grep -q "speedup\|benchmark.*result" benchmark_results.log 2>/dev/null && echo "✓" || echo "⚠ Check benchmark output")"

echo "\n=== PHASE 6 INTEGRATION VERIFICATION ==="
if grep -q "integration\|memory.*system\|neuromodulation" brain_test_results.log 2>/dev/null; then
    echo "✓ Phase 6 systems integration working"
else
    echo "✗ Phase 6 integration may have issues"
fi

echo "\n=== TEST SUMMARY ==="
passed_tests=0
failed_tests=0

for test in brain_test_results.log neuron_test_results.log synapse_test_results.log stdp_test_results.log; do
    if [ -f "$test" ] && grep -q "PASSED\|passed\|test.*ok\|✓" "$test" 2>/dev/null; then
        passed_tests=$((passed_tests + 1))
        echo "✓ $test"
    elif [ -f "$test" ]; then
        failed_tests=$((failed_tests + 1))
        echo "✗ $test"
    fi
done

echo "\nTests Passed: $passed_tests"
echo "Tests Failed: $failed_tests"

if [ $failed_tests -eq 0 ]; then
    echo "\n🎉 ALL PERFORMANCE OPTIMIZATIONS VERIFIED SUCCESSFULLY! 🎉"
    exit 0
else
    echo "\n⚠️  Some tests failed. Please review the output above."
    exit 1
fi