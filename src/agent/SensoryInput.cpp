#include "SensoryInput.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Default constructors/destructors defined in headers

// Helper function to create sensory input from raw data
std::unique_ptr<SensoryInput> createSensoryInput(const std::string& modality, 
                                               const std::vector<float>& data) {
    if (modality == "vision") {
        auto input = std::make_unique<Vision>();
        input->configure(0, 0, data.size());
        input->setData(data);
        return input;
    } else if (modality == "audio") {
        auto input = std::make_unique<Audio>();
        input->configure(data.size() > 0 ? 1 : 0, 44100);
        input->setData(data);
        return input;
    } else if (modality == "internal") {
        auto input = std::make_unique<InternalSignals>();
        input->configure(data.size() > 0 ? 5 : 0);
        input->setData(data);
        return input;
    }
    
    NLM_LOG_WARNING("Unknown modality: " + modality);
    return nullptr;
}

// Helper function to convert sensory input to observation
std::unique_ptr<class Observation> sensoryToObservation(std::unique_ptr<SensoryInput> input) {
    if (!input) {
        return nullptr;
    }
    
    return std::make_unique<Observation>(std::move(input));
}

} // namespace nlm