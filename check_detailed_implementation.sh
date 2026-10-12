#!/bin/bash
echo "Detailed implementation check..."
echo ""

echo "=== Dopamine.hpp ==="
if [ -f src/neuromodulation/Dopamine.hpp ]; then
    echo "File exists: YES"
    echo "Contains class Dopamine: $(grep -c "class Dopamine" src/neuromodulation/Dopamine.hpp)"
    echo "Contains virtual getPlasticityFactor: $(grep -c "getPlasticityFactor" src/neuromodulation/Dopamine.hpp)"
    echo "Contains virtual update: $(grep -c "update" src/neuromodulation/Dopamine.hpp | grep -v "setUpdate\|getUpdate")"
else
    echo "File exists: NO"
fi

echo ""
echo "=== Neuromodulator.hpp ==="
echo "Contains Neuromodulator base class: $(grep -c "class Neuromodulator {" src/neuromodulation/Neuromodulator.hpp)"
echo "Contains Dopamine class: $(grep -c "class Dopamine :" src/neuromodulation/Neuromodulator.hpp)"
echo "Contains Curiosity class: $(grep -c "class Curiosity :" src/neuromodulation/Neuromodulator.hpp)"
echo "Contains PredictionError class: $(grep -c "class PredictionError :" src/neuromodulation/Neuromodulator.hpp)"
echo "Contains Novelty class: $(grep -c "class Novelty :" src/neuromodulation/Neuromodulator.hpp)"

echo ""
echo "=== Neuromodulator.cpp ==="
echo "Contains Dopamine:: implementation: $(grep -c "Dopamine::" src/neuromodulation/Neuromodulator.cpp)"
echo "Contains Curiosity:: implementation: $(grep -c "Curiosity::" src/neuromodulation/Neuromodulator.cpp)"
echo "Contains PredictionError:: implementation: $(grep -c "PredictionError::" src/neuromodulation/Neuromodulator.cpp)"
echo "Contains Novelty:: implementation: $(grep -c "Novelty::" src/neuromodulation/Neuromodulator.cpp)"

echo ""
echo "=== Other neuromodulation files ==="
for neuromod in Curiosity PredictionError Novelty Reward; do
    echo "$neuromod:"
    echo "  .hpp exists: $([ -f src/neuromodulation/$neurom.hpp ] && echo YES || echo NO)"
    echo "  .cpp exists: $([ -f src/neuromodulation/$neurom.cpp ] && echo YES || echo NO)"
    if [ -f src/neuromodulation/$neurom.hpp ]; then
        echo "  Contains class $neurom: $(grep -c "class $neurom :" src/neuromodulation/$neurom.hpp)"
    fi
    echo
    
done

echo "=== Final Assessment ==="
echo "All requested files are present:"
missing_files=""
for neuromod in Neuromodulator.hpp Dopamine.hpp Curiosity.hpp PredictionError.hpp Novelty.hpp; do
    if [ ! -f src/neuromodulation/$neurom ]; then
        missing_files="$missing_files $neurom"
    fi
done

if [ -z "$missing_files" ]; then
    echo "✓ All header files present"
else
    echo "✗ Missing files:$missing_files"
fi

echo ""
echo "Implementation files present:"
missing_impl=""
for neuromod in Neuromodulator.cpp Curiosity.cpp PredictionError.cpp Novelty.cpp Reward.cpp; do
    if [ ! -f src/neuromodulation/$neurom ]; then
        missing_impl="$missing_impl $neurom"
    fi
done

if [ -z "$missing_impl" ]; then
    echo "✓ All implementation files present"
else
    echo "✗ Missing implementation:$missing_impl"
fi

echo ""
echo "Total source files in neuromodulation directory:"
find src/neuromodulation -name "*.cpp" -name "*.hpp" | wc -l
echo "$(find src/neuromodulation -name "*.cpp" -name "*.hpp" | tr '\n' ' ')"

