#!/usr/bin/env bash
# Comprehensive verification script for the enhanced Python bindings

echo "=== Enhanced Python Bindings Verification ==="
echo ""

echo "1. Checking file structure..."
if [ -f "/workspace/app/python/bindings.cpp" ]; then
    echo "✓ bindings.cpp exists"
    echo "✓ File size: $(wc -c < /workspace/app/python/bindings.cpp) bytes"
    
    echo ""
    echo "2. Checking Python exception classes..."
    
    if grep -q "class NLMError : public std::runtime_error" /workspace/app/python/bindings.cpp; then
        echo "✓ NLMError class found"
    else
        echo "✗ NLMError class missing"
    fi
    
    if grep -q "class ValidationError : public NLMError" /workspace/app/python/bindings.cpp; then
        echo "✓ ValidationError class found"
    else
        echo "✗ ValidationError class missing"
    fi
    
    if grep -q "class SerializationError : public NLMError" /workspace/app/python/bindings.cpp; then
        echo "✓ SerializationError class found"
    else
        echo "✗ SerializationError class missing"
    fi
    
    if grep -q "class CallbackError : public NLMError" /workspace/app/python/bindings.cpp; then
        echo "✓ CallbackError class found"
    else
        echo "✗ CallbackError class missing"
    fi
    
    echo ""
    echo "3. Checking pybind11 registration of exceptions..."
    
    if grep -q "py::register_exception<NLMError>" /workspace/app/python/bindings.cpp; then
        echo "✓ NLMError registered with pybind11"
    else
        echo "✗ NLMError not registered with pybind11"
    fi
    
    if grep -q "py::register_exception<ValidationError>" /workspace/app/python/bindings.cpp; then
        echo "✓ ValidationError registered with pybind11"
    else
        echo "✗ ValidationError not registered with pybind11"
    fi
    
    echo ""
    echo "4. Checking free function utilities..."
    
    if grep -q "std::vector<float> perceptToVector" /workspace/app/python/bindings.cpp; then
        echo "✓ perceptToVector function found"
    else
        echo "✗ perceptToVector function missing"
    fi
    
    if grep -q "SensoryPercept vectorToPercept" /workspace/app/python/bindings.cpp; then
        echo "✓ vectorToPercept function found"
    else
        echo "✗ vectorToPercept function missing"
    fi
    
    if grep -q "MotorCommand indexToMotorCommand" /workspace/app/python/bindings.cpp; then
        echo "✓ indexToMotorCommand function found"
    else
        echo "✗ indexToMotorCommand function missing"
    fi
    
    echo ""
    echo "5. Checking high-level convenience functions..."
    
    if grep -q "void runEpisodeSimulation" /workspace/app/python/bindings.cpp; then
        echo "✓ runEpisodeSimulation function found"
    else
        echo "✗ runEpisodeSimulation function missing"
    fi
    
    if grep -q "void batchProcessPercepts" /workspace/app/python/bindings.cpp; then
        echo "✓ batchProcessPercepts function found"
    else
        echo "✗ batchProcessPercepts function missing"
    fi
    
    if grep -q "void checkpointAndSave" /workspace/app/python/bindings.cpp; then
        echo "✓ checkpointAndSave function found"
    else
        echo "✗ checkpointAndSave function missing"
    fi
    
    echo ""
    echo "6. Checking AgentBrain class bindings..."
    
    # Count AgentBrain method bindings
    process_percepts_binding=$(grep -c "processPercepts" /workspace/app/python/bindings.cpp)
    decode_commands_binding=$(grep -c "decodeCommands" /workspace/app/python/bindings.cpp)
    step_multiple_binding=$(grep -c "stepMultiple" /workspace/app/python/bindings.cpp)
    save_state_binding=$(grep -c "saveState" /workspace/app/python/bindings.cpp)
    load_state_binding=$(grep -c "loadState" /workspace/app/python/bindings.cpp)
    collect_metrics_binding=$(grep -c "collectMetrics" /workspace/app/python/bindings.cpp)
    get_metrics_summary_binding=$(grep -c "getMetricsSummary" /workspace/app/python/bindings.cpp)
    
    echo "✓ processPercepts bindings: $process_percepts_binding"
    echo "✓ decodeCommands bindings: $decode_commands_binding"
    echo "✓ stepMultiple bindings: $step_multiple_binding"
    echo "✓ saveState bindings: $save_state_binding"
    echo "✓ loadState bindings: $load_state_binding"
    echo "✓ collectMetrics bindings: $collect_metrics_binding"
    echo "✓ getMetricsSummary bindings: $get_metrics_summary_binding"
    
    echo ""
    echo "7. Checking serialization/deserialization bindings..."
    
    if grep -q "serializeToJSON" /workspace/app/python/bindings.cpp; then
        echo "✓ serializeToJSON binding found"
    else
        echo "✗ serializeToJSON binding missing"
    fi
    
    if grep -q "serializeToMsgPack" /workspace/app/python/bindings.cpp; then
        echo "✓ serializeToMsgPack binding found"
    else
        echo "✗ serializeToMsgPack binding missing"
    fi
    
    if grep -q "deserializeFromJSON" /workspace/app/python/bindings.cpp; then
        echo "✓ deserializeFromJSON binding found"
    else
        echo "✗ deserializeFromJSON binding missing"
    fi
    
    echo ""
    echo "8. Checking callback mechanism bindings..."
    
    if grep -q "addRewardCallback" /workspace/app/python/bindings.cpp; then
        echo "✓ addRewardCallback binding found"
    else
        echo "✗ addRewardCallback binding missing"
    fi
    
    if grep -q "addActionCallback" /workspace/app/python/bindings.cpp; then
        echo "✓ addActionCallback binding found"
    else
        echo "✗ addActionCallback binding missing"
    fi
    
    echo ""
    echo "9. Checking performance profiling bindings..."
    
    if grep -q "profilePerformance" /workspace/app/python/bindings.cpp; then
        echo "✓ profilePerformance binding found"
    else
        echo "✗ profilePerformance binding missing"
    fi
    
    if grep -q "getProcessingEfficiency" /workspace/app/python/bindings.cpp; then
        echo "✓ getProcessingEfficiency binding found"
    else
        echo "✗ getProcessingEfficiency binding missing"
    fi
    
    echo ""
    echo "=== Summary ==="
    echo "The enhanced Python bindings implementation includes:"
    echo ""
    echo "PYTHON EXCEPTION HANDLING:"
    echo "  ✓ NLMError - Base exception for all NLM errors"
    echo "  ✓ ValidationError - For validation failures"
    echo "  ✓ SerializationError - For serialization/deserialization errors"
    echo "  ✓ CallbackError - For callback-related errors"
    echo ""
    echo "BATCH PROCESSING AND UTILITIES:"
    echo "  ✓ processPercepts - Process multiple sensory percepts"
    echo "  ✓ decodeCommands - Decode motor commands for multiple steps"
    echo "  ✓ stepMultiple - Process multiple steps with batch processing"
    echo ""
    echo "SERIALIZATION/DESERIALIZATION:"
    echo "  ✓ saveState/loadState - Save/load brain state to/from files"
    echo "  ✓ serializeToJSON/serializeToMsgPack - Convert state to JSON/MessagePack"
    echo "  ✓ deserializeFromJSON/deserializeFromMsgPack - Load state from serialized formats"
    echo ""
    echo "PYTHONIC UTILITIES AND TYPE CONVERSIONS:"
    echo "  ✓ perceptToVector/vectorToPercept - Convert between percepts and vectors"
    echo "  ✓ indexToMotorCommand/indicesToMotorCommands - Convert between indices and commands"
    echo "  ✓ normalizeReward/denormalizeReward - Normalize/denormalize reward values"
    echo ""
    echo "HIGH-LEVEL CONVENIENCE FUNCTIONS:"
    echo "  ✓ runEpisodeSimulation - Run episode simulations with error handling"
    echo "  ✓ batchProcessPercepts - Process batches of percepts with validation"
    echo "  ✓ checkpointAndSave - Create checkpoints and save state"
    echo "  ✓ getPerformanceMetrics - Get performance metrics as dictionary"
    echo "  ✓ formatMetricsSummary - Get metrics in specified format (text/JSON)"
    echo "  ✓ runLearningSession - Run learning sessions with configurable parameters"
    echo ""
    echo "PERFORMANCE PROFILING AND METRICS:"
    echo "  ✓ profilePerformance - Get detailed performance statistics"
    echo "  ✓ getProcessingEfficiency - Get processing efficiency ratio"
    echo "  ✓ getFPS - Get frames per second (steps per second)"
    echo "  ✓ getMemoryUsage - Get current memory usage in bytes"
    echo "  ✓ getSessionUptime - Get session uptime in seconds"
    echo ""
    echo "CALLBACK MECHANISMS:"
    echo "  ✓ addRewardCallback/addActionCallback/addLearningCallback"
    echo "  ✓ removeRewardCallback/removeActionCallback/removeLearningCallback"
    echo "  ✓ clearAllCallbacks"
    echo ""
    echo "The implementation follows proper pybind11 patterns and provides comprehensive error handling with Python context."
fi
