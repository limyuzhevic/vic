#pragma once

#include <string>

namespace nlm {

// Macro to register enhanced sensory processors
// This should be included in the respective .cpp files

// Register VisionProcessor
REGISTER_SENSORY_PROCESSOR(VisionProcessor, "Vision")

// Register AudioProcessor  
REGISTER_SENSORY_PROCESSOR(AudioProcessor, "Audio")

// Register InternalSignalsProcessor
REGISTER_SENSORY_PROCESSOR(InternalSignalsProcessor, "InternalSignals")

// Function to initialize all registered processors
inline void initializeAllSensoryProcessors() {
    // This forces registration of all processors at program startup
    // The macro REGISTER_SENSORY_PROCESSOR creates static objects
    // that call the factory registration function
}

// Function to get list of available processors
inline std::vector<std::string> getAvailableSensoryProcessors() {
    return SensoryProcessorFactory::getInstance().getRegisteredNames();
}

// Convenience function to create processor for input type
inline std::unique_ptr<SensoryProcessor> createProcessorForInput(const SensoryInput& input) {
    return SensoryProcessorFactory::getInstance().createForInput(input);
}

} // namespace nlm
