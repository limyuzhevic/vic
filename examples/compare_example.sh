#!/bin/bash
# examples/compare_example.sh

# NLM compare example

echo "NLM Compare Example"
echo "=================="
echo ""
echo "This example demonstrates parameter comparison across different configurations."
echo ""

# Parameter comparison
echo "1. Parameter comparison test:"
./nlm --mode compare

echo ""
echo "2. Parameter comparison with output to file:"
./nlm --mode compare --output parameter_comparison.txt

echo ""
echo "3. Parameter sensitivity analysis:"
./nlm --mode compare --pattern sensitivity --output sensitivity_results.json

echo ""
echo "4. Compare with validation:"
./nlm --mode compare --validate

echo ""
echo "5. Batch parameter comparison:"
for params in "baseline" "high_plasticity" "low_plasticity" "balanced"; do
    echo "   Testing parameters: $params"
    ./nlm --mode compare --pattern "$params" --output "$params"_comparison.txt
done

echo ""
echo "Compare example completed successfully!"
