#pragma once

#include "NeuromodulatorBase.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace nlm {

class NeuromodulatorFactory {
public:
    static Neuromodulator* create(NeuromodulatorType type);
    static std::unique_ptr<Neuromodulator> createUnique(NeuromodulatorType type);
    static bool isTypeAvailable(NeuromodulatorType type);
    static std::vector<NeuromodulatorType> getAvailableTypes();
    
private:
    static std::unordered_map<NeuromodulatorType, Neuromodulator*(*)()> creatorRegistry;
};

} // namespace nlm