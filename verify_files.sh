#!/bin/bash

echo "=== Neuromodulation Files Status ==="
echo ""

neuromod_files=(
    "Neuromodulator.hpp:Neuromodulator base class"
    "Dopamine.hpp:Separate Dopamine header"
    "Curiosity.hpp:Curiosity class"
    "PredictionError.hpp:PredictionError class"
    "Novelty.hpp:Novelty class"
)

impl_files=(
    "Neuromodulator.cpp:All implementations"
    "Curiosity.cpp:Curiosity implementation"
    "PredictionError.cpp:PredictionError implementation"
    "Novelty.cpp:Novelty implementation"
    "Reward.cpp:Reward implementation (existing)"
)

echo "Checking header files:"
for file_info in "${neuromod_files[@]}"; do
    file="${file_info%%:*}"
    desc="${file_info#*:}"
    if [ -f "src/neuromodulation/$file" ]; then
        echo "✓ $file: $desc"
    else
        echo "✗ $file: $desc (MISSING)"
    fi
done

echo ""
echo "Checking implementation files:"
for file_info in "${impl_files[@]}"; do
    file="${file_info%%:*}"
    desc="${file_info#*:}"
    if [ -f "src/neuromodulation/$file" ]; then
        echo "✓ $file: $desc"
    else
        echo "✗ $file: $desc (MISSING)"
    fi
done

echo ""
echo "=== File Counts ==="
echo "Header files: $(ls -1 src/neuromodulation/*.hpp 2>/dev/null | wc -l)"
echo "Implementation files: $(ls -1 src/neuromodulation/*.cpp 2>/dev/null | wc -l)"

echo ""
echo "=== Checking for Dopamine.hpp ==="
if [ -f "src/neuromodulation/Dopamine.hpp" ]; then
    echo "✓ Dopamine.hpp exists - separate Dopamine class declaration"
    echo "  Contents: $(grep -c "class Dopamine" src/neuromodulation/Dopamine.hpp) class definitions"
else
    echo "✗ Dopamine.hpp missing"
fi

echo ""
echo "=== Checking Neuromodulator.hpp structure ==="
if grep -q "class Neuromodulator {" src/neuromodulation/Neuromodulator.hpp; then
    echo "✓ Neuromodulator base class defined"
fi
if grep -q "class Dopamine : public Neuromodulator" src/neuromodulator.hpp; then
    echo "✓ Dopamine inherits from Neuromodulator"
fi
if grep -q "class Curiosity : public Neuromodulator" src/neuromodulation/Neuromodulator.hpp; then
    echo "✓ Curiosity inherits from Neuromodulator"
fi
if grep -q "class PredictionError : public Neuromodulator" src/neuromodulation/Neuromodulator.hpp; then
    echo "✓ PredictionError inherits from Neuromodulator"
fi
if grep -q "class Novelty : public Neuromodulator" src/neuromodulation/Neuromodulator.hpp; then
    echo "✓ Novelty inherits from Neuromodulator"
fi

