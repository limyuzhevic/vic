# Error handling and safety improvements

// Add null checks and better error handling for critical operations

// Example improvements in Brain.cpp:
// Add null pointer checks before using pointers:

// In Brain.cpp, around line 196-217:
if (sensoryPop) {
    for (auto* neuron : sensoryPop->getNeurons()) {
        // Added null check
        if (neuron) pImpl->sensoryNeurons.push_back(neuron);
    }
}

// Similar improvements needed in:
// - STDP.cpp: Check for null synapse before using it
// - PlasticityRule.cpp: Check for null synapse
// - MotorSystem.cpp: Check for null components

// Add return error codes instead of silent failures:
brain->injectCurrent(neuron, current) {
    if (!neuron) {
        NLM_LOG_ERROR("Cannot inject current: neuron is null");
        return false;
    }
    // ... rest of function
    return true;
}

// Add validation in configuration loading:
bool Config::loadFromFile(const std::string& filepath) {
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config file path is empty");
        return false;
    }
    
    // Check file exists and is readable
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    // ... rest of function
    return true;
}