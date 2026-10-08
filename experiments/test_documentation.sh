#!/bin/bash
# NLM Documentation Test Script
# Tests documentation and help systems

echo "=== NLM Documentation Test Script ==="

echo "--- Testing help system ---"
./build/nlm help | grep -E "^(help|status|run|save|load|reset|step|exit)" | head -10

echo ""
echo "--- Testing experiment help ---"
./build/nlm run --help | grep -E "^(connectivity|plasticity|stdp|learning|demo|ablation)" | head -10

echo ""
echo "--- Testing parameter help ---"
./build/nlm set --help | head -10

echo ""
echo "=== Documentation Tests Complete ==="
