#pragma once

#include "AgentBrain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void AgentBrain::reset() {
    NLM_LOG_INFO("AgentBrain reset completed");
}

} // namespace nlm