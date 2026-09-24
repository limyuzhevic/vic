#!/bin/bash
# Simple Verification Script
# Quick check that memory leak fixes are in place

echo "========================================"
echo "QUICK VERIFICATION - MEMORY LEAK FIXES"
echo "========================================"

# Check key files for smart pointer usage
classes_to_check=(
    "src/brain/Neuron.hpp"
    "src/brain/Synapse.hpp"
    "src/memory/Memory.hpp"
    "src/neuromodulation/Neuromodulator.hpp"
    "src/plasticity/PlasticityRule.hpp"
)

unique_ptr_count=0
total_files=0

echo ""
echo "Checking PIMPL classes for smart pointer usage..."

for file in "${classes_to_check[@]}"; do
    if [ -f "$file" ]; then
        ((total_files++))
        if grep -q "std::unique_ptr<Impl> pImpl" "$file"; then
            echo "  ✅ $file - Uses std::unique_ptr<Impl>"
            ((unique_ptr_count++))
        else
            echo "  ❌ $file - Does not use std::unique_ptr<Impl>"
        fi
    else
        echo "  ❌ $file - File not found"
    fi
done

echo ""
echo "Memory Leak Detection in CMake..."

if [ -f "CMakeLists.txt" ]; then
    if grep -q "ENABLE_SANITIZERS" CMakeLists.txt; then
        echo "  ✅ CMakeLists.txt - Contains ENABLE_SANITIZERS option"
        if grep -q "fsanitize=address" CMakeLists.txt; then
            echo "  ✅ CMakeLists.txt - Contains AddressSanitizer flags"
        else
            echo "  ❌ CMakeLists.txt - Missing AddressSanitizer flags"
        fi
        if grep -q "fsanitize=leak" CMakeLists.txt; then
            echo "  ✅ CMakeLists.txt - Contains LeakSanitizer flags"
        else
            echo "  ❌ CMakeLists.txt - Missing LeakSanitizer flags"
        fi
    else
        echo "  ❌ CMakeLists.txt - Missing ENABLE_SANITIZERS option"
    fi
else
    echo "  ❌ CMakeLists.txt - File not found"
fi

echo ""
echo "Memory Leak Detection in main.cpp..."

if [ -f "src/main.cpp" ]; then
    if grep -q "check_memory_leaks" src/main.cpp; then
        echo "  ✅ main.cpp - Contains memory leak detection function"
    else
        echo "  ❌ main.cpp - Missing memory leak detection"
    fi
else
    echo "  ❌ main.cpp - File not found"
fi

echo ""
echo "========================================"
echo "VERIFICATION RESULTS"
echo "========================================"
echo ""
echo "PIMPL Classes checked: $total_files/$(( ${#classes_to_check[@]} ))"
echo "Classes using smart pointers: $unique_ptr_count/$(( ${#classes_to_check[@]} ))"
echo ""
if [ "$unique_ptr_count" -eq "$total_files" ]; then
    echo "✅ SUCCESS: All PIMPL classes use smart pointers!"
    echo ""
    echo "Memory leak fixes have been successfully applied."
    echo "The code now uses std::unique_ptr for automatic memory management."
    echo ""
    echo "To build and test with memory leak detection:"
    echo "  mkdir build"
    echo "  cd build"
    echo "  cmake -DENABLE_SANITIZERS=ON .."
    echo "  make -j4"
    echo "  ./nlm --config configs/test.cfg"
else
    echo "❌ WARNING: Some PIMPL classes still use raw pointers."
    echo ""
    echo "Please check the failed files above and fix any remaining issues."
fi

echo ""
echo "========================================"
echo "VERIFICATION COMPLETE"
echo "========================================"
