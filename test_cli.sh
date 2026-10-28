#!/bin/sh
# Quick test script for NLM CLI

echo "NLM CLI Test"
echo "============"

echo "1. Testing help command..."
echo "./nlm help" | head -20
echo ""

echo "2. Testing version command..."
echo "./nlm --version"
echo ""

echo "3. Testing status command..."
echo "./nlm status" | head -15

echo "Test completed!"