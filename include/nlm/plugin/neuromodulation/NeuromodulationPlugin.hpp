#pragma once

#include "NLMPlugin.hpp"
#include "Neuromodulator.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

/**
 * @class NeuromodulationPlugin
 * @brief Plugin interface for custom neuromodulators
 */
class NeuromodulationPlugin : public virtual NLMPlugin {
public:
    virtual ~NeuromodulationPlugin() = default;
    
    // Type check
    bool isNeuromodulationPlugin() const override { return true; }
    
    // Neuromodulation factory methods
    virtual void* getNeuromodulationFactory() override {
        return static_cast<void*>(this);
    }
    
    // Abstract plugin interface for neuromodulation
    virtual Neuromodulator* createNeuromodulator() = 0;
    virtual Neuromodulator* createNeuromodulator(const std::string& neuromodulatorType) = 0;
    virtual bool canCreateNeuromodulator(const std::string& neuromodulatorType) const = 0;
    virtual std::vector<std::string> getSupportedNeuromodulatorTypes() const = 0;
    
    // Neuromodulation configuration
    virtual bool configureNeuromodulator(Neuromodulator& neuromodulator, const std::string& config) = 0;
    virtual std::string getNeuromodulatorConfig(const Neuromodulator& neuromodulator) const = 0;
};

} // namespace nlm