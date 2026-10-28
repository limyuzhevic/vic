#!/usr/bin/env bash
# Test the NLM CLI

echo "Testing NLM CLI..."
echo "=================="

echo "1. Testing help command:"
echo "./bin/nlm help" | head -5
echo ""

echo "2. Testing version command:"
echo "./bin/nlm --version" | head -1
echo ""

echo "3. Testing status command:"
echo "./bin/nlm status" | head -10
echo ""

echo "4. Testing config init:"
echo "./bin/nlm config init"
echo ""

echo "5. Testing config list:"
echo "./bin/nlm config list"
echo ""

echo "6. Testing config show neuron_count:"
echo "./bin/nlm config show neuron_count"
echo ""

echo "CLI Test Script Completed!"