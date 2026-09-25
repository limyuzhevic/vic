#include "cli/options/ControlOptions.hpp"
#include <algorithm>

using namespace nlm::cli::options;

ControlOptions::ControlOptions()
    : neuronCount(500)
    , simulationTime(10.0)
    , timestep(0.001)
    , checkpointInterval(100)
    , noCheckpoint(false) {
}

void ControlOptions::parse(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                try {
                    if (key == "neuron-count") {
                        neuronCount = std::stoll(value);
                    } else if (key == "simulation-time") {
                        simulationTime = std::stod(value);
                    } else if (key == "timestep") {
                        timestep = std::stod(value);
                    } else if (key == "checkpoint-interval") {
                        checkpointInterval = std::stoll(value);
                    } else if (key == "no-checkpoint") {
                        noCheckpoint = true;
                    }
                } catch (const std::exception&) {
                    // Ignore invalid values
                }
            }
        }
    }
}
