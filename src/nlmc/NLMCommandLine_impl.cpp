#include "NLMCommandLine.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>

namespace nlmc {

void NLMCommandLine::processCommandLine(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: nlmc <command> [args]" << std::endl;
        std::cout << "Use 'nlmc help' for available commands." << std::endl;
        return;
    }
    
    std::string command = argv[1];
    std::vector<std::string> args;
    
    for (int i = 2; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    auto it = commands.find(command);
    if (it == commands.end()) {
        std::cout << "Unknown command: " << command << std::endl;
        std::cout << "Use 'nlmc help' for available commands." << std::endl;
        return;
    }
    
    CommandResult result = it->second(args);
    
    if (!result.success) {
        std::cerr << "Error: " << result.error << std::endl;
    }
    
    if (!result.output.empty()) {
        std::cout << result.output << std::endl;
    }
}

} // namespace nlmc
