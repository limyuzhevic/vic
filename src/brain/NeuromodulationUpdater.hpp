// NeuromodulationUpdater.hpp - Handles neuromodulation effects on neural excitability
#pragma once

#include "BrainSystemUpdater.hpp"

namespace nlm {

class NeuromodulationUpdater : public BrainSystemUpdater {
public:
    virtual void update(Brain& brain, SimulationStep currentStep,
                       Timestamp currentTime, TimestepDuration timestep) override;
    virtual std::string getName() const override { return "NeuromodulationUpdater"; }
};

} // namespace nlm
