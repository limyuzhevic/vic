#pragma once

#include "PluginBase.hpp"
#include "../environment/Observation.hpp"
#include "../motor/Action.hpp"

namespace nlm {

class InputOutputPlugin : public NLMPlugin {
public:
    // I/O extension
    virtual std::unique_ptr<Observation> getSensoryInput() = 0;
    virtual void sendMotorOutput(const Action& action) = 0;
    virtual void updateIO(double deltaTime) = 0;
    
    // I/O parameters
    virtual std::string getInputType() const = 0;
    virtual std::string getOutputType() const = 0;
    virtual double getSamplingRate() const = 0;
    virtual double getLatency() const = 0;
};

} // namespace nlm
