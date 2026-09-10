// MemorySystemUpdater.hpp - Handles working memory and episodic memory updates
#pragma once

#include "BrainSystemUpdater.hpp"

namespace nlm {

class MemorySystemUpdater : public BrainSystemUpdater {
public:
    virtual void update(Brain& brain, SimulationStep currentStep,
                       Timestamp currentTime, TimestepDuration timestep) override;
    virtual std::string getName() const override { return "MemorySystemUpdater"; }
};

} // namespace nlm
