#pragma once

#include "CLIContext.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

namespace nlm {

// Simulation control command implementations
class SimulationCommand {
public:
    static bool executeSimulate(CLIContext& context, const std::map<std::string, std::string>& options);
};

} // namespace nlm
