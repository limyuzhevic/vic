#!/bin/bash
# Simple test runner for NLM integration tests

set -e

echo "=== NLM Integration Test Runner ==="
echo "Building and running integration tests..."

# Create build directory
mkdir -p build
cd build

# Configure CMake with tests enabled
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON

# Build the project
make -j$(nproc)

# Run integration tests if they exist
if [ -f "nlm" ]; then
    echo "Running main tests..."
    ./nlm
elif [ -f "nlm_phase3_demo" ]; then
    echo "Running Phase 3 demo..."
    ./nlm_phase3_demo
else
    echo "No main executable found, trying to compile tests separately..."
    # Try to compile just the integration test
    g++ -std=c++20 -I../src -O2 ../tests/test_integration.cpp ../tests/test_brain.cpp ../src/core/Config/Config.cpp ../src/core/Logger/Logger.cpp ../src/core/Random/Random.cpp ../src/core/SimulationClock/SimulationClock.cpp ../src/brain/Brain.cpp ../src/brain/Neuron.cpp ../src/brain/Synapse.cpp ../src/brain/NeuralPopulation.cpp ../src/brain/NeuralRegion.cpp ../src/dynamics/SpikeSystem.cpp ../src/plasticity/STDP.cpp ../src/plasticity/Hebbian.cpp ../src/plasticity/StructuralPlasticity.cpp ../src/plasticity/PlasticityRule.cpp ../src/development/DevelopmentSystem.cpp ../src/development/Synaptogenesis.cpp ../src/development/Pruning.cpp ../src/development/Maturation.cpp ../src/neuromodulation/Neuromodulator.cpp ../src/neuromodulation/Reward.cpp ../src/neuromodulation/PredictionError.cpp ../src/neuromodulation/Novelty.cpp ../src/neuromodulation/Curiosity.cpp ../src/memory/WorkingMemory.cpp ../src/memory/EpisodicMemory.cpp ../src/memory/SemanticMemory.cpp ../src/memory/ProceduralMemory.cpp ../src/memory/AssociativeMemory.cpp ../src/memory/NeuralWorkingMemory.cpp ../src/memory/NeuralEpisodicMemory.cpp ../src/cognition/ConceptFormation.cpp ../src/cognition/NeuralPlanner.cpp ../src/sensory/SensoryInput.cpp ../src/sensory/Vision.cpp ../src/sensory/Audio.cpp ../src/sensory/InternalSignals.cpp ../src/prediction/PredictionSystem.cpp ../src/prediction/NeuralPrediction.cpp ../src/motor/MotorSystem.cpp ../src/environment/Environment.cpp ../src/environment/Action.cpp ../src/environment/Observation.cpp ../src/experiments/Experiment.cpp ../src/experiments/ExperimentRunner.cpp ../src/experiments/Metrics.cpp ../src/experiments/Phase3Experiment.cpp -o nlm_integration_test

    # Run the integration test
    if [ -f "nlm_integration_test" ]; then
        echo "Running integration tests..."
        ./nlm_integration_test
    else
        echo "Failed to build integration tests"
        exit 1
    fi
else
    echo "Build failed - no executable found"
    exit 1
fi

echo "=== All Tests Completed ==="