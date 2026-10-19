#!/bin/bash
# examples/learning_example.sh

# NLM learning experiment example

echo "NLM Learning Experiment Example"
echo "==============================="
echo ""
echo "This example demonstrates learning experiments with different input patterns."
echo ""

# Learning with random pattern
echo "1. Learning with random pattern (500 iterations):"
./nlm --mode learning --pattern random --iterations 500

echo ""
echo "2. Learning with sinusoidal pattern (500 iterations):"
./nlm --mode learning --pattern sinusoidal --iterations 500

echo ""
echo "3. Learning with correlated pattern (500 iterations):"
./nlm --mode learning --pattern correlated --iterations 500

echo ""
echo "4. Batch learning test (100 iterations each):"
for pattern in random sinusoidal correlated pulse; do
    echo "   Testing pattern: $pattern"
    ./nlm --mode learning --pattern $pattern --iterations 100
    echo ""
done

echo ""
echo "5. Learning with output to file:"
./nlm --mode learning --pattern random --iterations 500 --output learning_results.txt

echo ""
echo "Learning example completed successfully!"
