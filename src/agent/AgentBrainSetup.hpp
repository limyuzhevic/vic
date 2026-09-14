#pragma once

#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {
namespace setup {

class AgentBrainSetup {
public:
    static void initializeConstructor(AgentBrain& agentBrain, std::shared_ptr<Brain> brain);
};

} // namespace setup
} // namespace nlm
