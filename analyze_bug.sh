#!/bin/bash
# Check the bug - workingMemory returning nullptr

# First check if workingMemory is initialized by looking at the Brain constructor

echo "=== Checking Brain.cpp for workingMemory initialization ==="
echo "1. Looking at Impl constructor (lines 81-154):"
sed -n '81,154p' /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_d65d5ad7-dbfe-4c3a-b223-f306083b7acd/src/brain/Brain.cpp | grep -A2 -B2 "workingMemory"

echo ""
echo "2. Checking Brain::initialize() method:"
echo "Lines around workingMemory usage:"
sed -n '232,238p' /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_d65d5ad7-dbfe-4c3a-b223-f306083b7acd/src/brain/Brain.cpp

echo ""
echo "3. Checking getWorkingMemory() method:"
echo "Line 1008-1010:"
sed -n '1008,1010p' /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_d65d5ad7-dbfe-4c3a-b223-f306083b7acd/src/brain/Brain.cpp

echo ""
echo "4. Check NeuralWorkingMemory.hpp for Brain* reference:"
echo "Class NeuralWorkingMemory members:"
sed -n '20,107p' /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_d65d5ad7-dbfe-4c3a-b223-f306083b7acd/src/memory/NeuralWorkingMemory.hpp

echo ""
echo "5. Check NeuralWorkingMemory.cpp implementation:"
echo "initialize() method:"
sed -n '30,35p' /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_d65d5ad7-dbfe-4c3a-b223-f306083b7acd/src/memory/NeuralWorkingMemory.cpp

# The bug analysis:
echo ""
echo "=== BUG ANALYSIS ==="
echo "1. workingMemory is initialized in Impl constructor at line 112: workingMemory = std::make_unique<NeuralWorkingMemory>();"
echo "2. workingMemory should NOT be nullptr after initialization"
echo "3. getWorkingMemory() returns pImpl->workingMemory.get()"
echo "4. The issue might be that workingMemory initialization is conditional or incomplete"
echo "5. Check if there's an early return or exception in Brain::initialize() before workingMemory is used"
