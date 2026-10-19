#!/bin/bash
# examples/benchmark_example.sh

# NLM benchmark example

echo "NLM Benchmark Example"
echo "======================"
echo ""
echo "This example demonstrates performance benchmarking with different configurations."
echo ""

# Standard benchmark
echo "1. Standard benchmark suite:"
./nlm --mode benchmark

echo ""
echo "2. Custom benchmark with 20 iterations:"
./nlm --mode benchmark --iterations 20

echo ""
echo "3. Custom benchmark with output to file:"
./nlm --mode benchmark --iterations 10 --output benchmark_results.txt

echo ""
echo "4. Performance comparison with different configurations:"
./nlm --mode benchmark --config examples/benchmark.cfg

echo ""
echo "5. Benchmark with validation:"
./nlm --mode benchmark --validate

echo ""
echo "Benchmark example completed successfully!"
