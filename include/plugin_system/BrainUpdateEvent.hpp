#pragma once

#include "../core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <memory>
#include <any>
#include <map>

namespace nlm {

class BrainUpdateEvent {
public:
    enum class Type {
        NeuronSpike,
        SynapseUpdate,
        MemoryUpdate,
        NeuromodulationUpdate,
        PredictionUpdate,
        Custom
    };
    
    BrainUpdateEvent() : timestamp(0.0) {}
    
    Type type;
    std::string source;
    std::map<std::string, std::any> data;
    Timestamp timestamp;
};

} // namespace nlm
