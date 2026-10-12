#!/bin/bash
echo "Verifying neuromodulation implementation..."
echo ""

echo "1. Checking Neuromodulator.hpp:"
if grep -q "class Neuromodulator {" src/neuromodulation/Neuromodulator.hpp; then
    echo "   ✓ Neuromodulator base class defined"
else
    echo "   ✗ Neuromodulator base class missing"
fi

if grep -q "class Dopamine : public Neuromodulator" src/neuromodulation/Neuromodulator.hpp; then
    echo "   ✓ Dopamine class defined in Neuromodulator.hpp"
else
    echo "   ✗ Dopamine class missing in Neuromodulator.hpp"
fi

if grep -q "class Curiosity : public Neuromodulator" src/neuromodulation/Neuromodulator.hpp; then
    echo "   ✓ Curiosity class defined in Neuromodulator.hpp"
else
    echo "   ✗ Curiosity class missing in Neuromodulator.hpp"
fi

if grep -q "class PredictionError : public Neuromodulator" src/neuromodulation/Neuromodulator.hpp; then
    echo "   ✓ PredictionError class defined in Neuromodulator.hpp"
else
    echo "   ✗ PredictionError class missing in Neuromodulator.hpp"
fi

if grep -q "class Novelty : public Neuromodulator" src/neuromodulation/Neuromodulator.hpp; then
    echo "   ✓ Novelty class defined in Neuromodulator.hpp"
else
    echo "   ✗ Novelty class missing in Neuromodulator.hpp"
fi

echo ""
echo "2. Checking Neuromodulator.cpp:"
if [ -f src/neuromodulation/Neuromodulator.cpp ]; then
    echo "   ✓ Neuromodulator.cpp exists"
    
    # Count implementations
    dopamine_impl=$(grep -c "Dopamine::" src/neuromodulation/Neuromodulator.cpp)
    curiosity_impl=$(grep -c "Curiosity::" src/neuromodulation/Neuromodulator.cpp)
    predictionerror_impl=$(grep -c "PredictionError::" src/neuromodulation/Neuromodulator.cpp)
    novelty_impl=$(grep -c "Novelty::" src/neuromodulation/Neuromodulator.cpp)
    
    if [ $dopamine_impl -gt 100 ]; then
        echo "   ✓ Dopamine implementation present ($dopamine_impl methods)"
    else
        echo "   ✗ Dopamine implementation incomplete ($dopamine_impl methods)"
    fi
    
    if [ $curiosity_impl -gt 50 ]; then
        echo "   ✓ Curiosity implementation present ($curiosity_impl methods)"
    else
        echo "   ✗ Curiosity implementation incomplete ($curiosity_impl methods)"
    fi
    
    if [ $predictionerror_impl -gt 50 ]; then
        echo "   ✓ PredictionError implementation present ($predictionerror_impl methods)"
    else
        echo "   ✗ PredictionError implementation incomplete ($predictionerror_impl methods)"
    fi
    
    if [ $novelty_impl -gt 50 ]; then
        echo "   ✓ Novelty implementation present ($novelty_impl methods)"
    else
        echo "   ✗ Novelty implementation incomplete ($novelty_impl methods)"
    fi
else
    echo "   ✗ Neuromodulator.cpp missing"
fi

echo ""
echo "3. Checking Dopamine.hpp:"
if [ -f src/neuromodulation/Dopamine.hpp ]; then
    echo "   ✓ Dopamine.hpp exists"
    
    if grep -q "class Dopamine : public Neuromodulator" src/neuromodulation/Dopamine.hpp; then
        echo "   ✓ Dopamine class declaration in Dopamine.hpp"
    else
        echo "   ✗ Dopamine class declaration missing in Dopamine.hpp"
    fi
    
    if grep -q "virtual float getPlasticityFactor" src/neuromodulation/Dopamine.hpp; then
        echo "   ✓ Plasticity factor declaration present"
    else
        echo "   ✗ Plasticity factor declaration missing"
    fi
else
    echo "   ✗ Dopamine.hpp missing"
fi

echo ""
echo "4. Checking other neuromodulator headers:"
for neuromod in Curiosity PredictionError Novelty Reward; do
    if [ -f src/neuromodulation/${neurom}.hpp ]; then
        echo "   ✓ $neurom.hpp exists"
    else
        echo "   ✗ $neurom.hpp missing"
    fi
    if [ -f src/neuromodulation/${neurom}.cpp ]; then
        echo "   ✓ $neurom.cpp exists"
    else
        echo "   ✗ $neurom.cpp missing"
    fi
    echo
    
done

echo ""
echo "5. Checking CMake configuration:"
if grep -q "src/neuromodulation/Dopamine.cpp" CMakeLists.txt; then
    echo "   ✓ Dopamine.cpp included in CMakeLists.txt"
else
    echo "   ✗ Dopamine.cpp not in CMakeLists.txt"
fi

echo ""
echo "Implementation verification complete!"
