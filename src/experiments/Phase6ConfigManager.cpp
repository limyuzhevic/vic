#include "Phase6ConfigManager.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>

namespace nlm {

Phase6ConfigManager::Phase6ConfigManager() {
    // Initialize with default values
    maxSteps_ = 10000;
    neuronCount_ = 1000;
    regionCount_ = 1;
    connectionProbability_ = 0.1f;
    enableCheckpointing_ = true;
    enableReplay_ = true;
    enableDevelopment_ = true;
    enableCuriosity_ = true;
    enableNovelty_ = true;
    enablePredictionError_ = true;
    checkpointPath_ = "./checkpoint_test.bin";
}

Phase6ConfigManager::~Phase6ConfigManager() {
    clearValidationErrors();
}

bool Phase6ConfigManager::validateConfiguration() const {
    clearValidationErrors();
    
    // Validate neuron count
    if (neuronCount_ == 0) {
        validationErrors_.push_back("Neuron count cannot be zero");
    } else if (neuronCount_ > 100000) {
        validationErrors_.push_back("Neuron count too high (max 100,000)");
    }
    
    // Validate region count
    if (regionCount_ == 0) {
        validationErrors_.push_back("Region count cannot be zero");
    } else if (regionCount_ > 100) {
        validationErrors_.push_back("Region count too high (max 100)");
    }
    
    // Validate connection probability
    if (connectionProbability_ < 0.0f || connectionProbability_ > 1.0f) {
        validationErrors_.push_back("Connection probability must be between 0.0 and 1.0");
    }
    
    // Validate max steps
    if (maxSteps_ == 0) {
        validationErrors_.push_back("Max steps cannot be zero");
    } else if (maxSteps_ > 1000000000) {
        validationErrors_.push_back("Max steps too high (max 1,000,000,000)");
    }
    
    // Validate checkpoint path
    if (checkpointPath_.empty()) {
        validationErrors_.push_back("Checkpoint path cannot be empty");
    } else if (checkpointPath_.length() > 1024) {
        validationErrors_.push_back("Checkpoint path too long (max 1024 characters)");
    }
    
    return validationErrors_.empty();
}

std::string Phase6ConfigManager::getValidationErrors() const {
    std::stringstream ss;
    for (size_t i = 0; i < validationErrors_.size(); ++i) {
        if (i > 0) ss << "; ";
        ss << validationErrors_[i];
    }
    return ss.str();
}

void Phase6ConfigManager::clearValidationErrors() {
    validationErrors_.clear();
}

Phase6Config Phase6ConfigManager::getDefaultConfig() {
    Phase6Config config;
    Phase6ConfigManager manager;
    
    // Copy default values
    config.maxSteps = manager.maxSteps_;
    config.neuronCount = manager.neuronCount_;
    config.regionCount = manager.regionCount_;
    config.connectionProbability = manager.connectionProbability_;
    config.enableCheckpointing = manager.enableCheckpointing_;
    config.enableReplay = manager.enableReplay_;
    config.enableDevelopment = manager.enableDevelopment_;
    config.checkpointPath = manager.checkpointPath_;
    
    return config;
}

std::shared_ptr<Brain> Phase6ConfigManager::createBrain(const Phase6Config& config) const {
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", config.neuronCount);
    cfg->set("region_count", config.regionCount);
    cfg->set("connection_probability", config.connectionProbability);
    cfg->set("stdp_ltp_weight", 0.01f);
    cfg->set("stdp_ltd_weight", 0.012f);
    cfg->set("synaptogenesis_rate", 0.0001f);
    cfg->set("pruning_rate", 0.00001f);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain from configuration");
    }
    
    return brain;
}

std::shared_ptr<AgentBrain> Phase6ConfigManager::createAgent(const Phase6Config& config, const SimpleWorld& world) const {
    auto brain = createBrain(config);
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(world);
    
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(config.enableDevelopment);
    agent->enableDevelopment(config.enableDevelopment);
    agent->enableCuriosity(config.enableCuriosity);
    
    return agent;
}

std::shared_ptr<SimpleWorld> Phase6ConfigManager::createWorld(const Phase6Config& config) const {
    auto world = std::make_shared<SimpleWorld>();
    world->initialize(16, 16);  // Default world size
    return world;
}

std::shared_ptr<SimpleWorld> Phase6ConfigManager::createWorld(size_t width, size_t height) const {
    auto world = std::make_shared<SimpleWorld>();
    world->initialize(width, height);
    return world;
}

std::string Phase6ConfigManager::getConfigSummary() const {
    std::stringstream ss;
    ss << "Phase 6 Configuration Summary:";
    ss << "\n  Neurons: " << neuronCount_;
    ss << "\n  Regions: " << regionCount_;
    ss << "\n  Connection probability: " << std::fixed << std::setprecision(3) << connectionProbability_;
    ss << "\n  Max steps: " << maxSteps_;
    ss << "\n  Enable checkpointing: " << (enableCheckpointing_ ? "yes" : "no");
    ss << "\n  Enable replay: " << (enableReplay_ ? "yes" : "no");
    ss << "\n  Enable development: " << (enableDevelopment_ ? "yes" : "no");
    ss << "\n  Enable curiosity: " << (enableCuriosity_ ? "yes" : "no");
    ss << "\n  Checkpoint path: " << checkpointPath_;
    return ss.str();
}

bool Phase6ConfigManager::loadFromArgs(int argc, char** argv) {
    clearValidationErrors();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp();
            return true;
        }
        
        if (arg.find("--neurons=") == 0) {
            try {
                neuronCount_ = std::stoull(arg.substr(10));
            } catch (...) {
                validationErrors_.push_back("Invalid neuron count: " + arg.substr(10));
            }
        } else if (arg.find("--regions=") == 0) {
            try {
                regionCount_ = std::stoull(arg.substr(10));
            } catch (...) {
                validationErrors_.push_back("Invalid region count: " + arg.substr(10));
            }
        } else if (arg.find("--connprob=") == 0) {
            try {
                connectionProbability_ = std::stof(arg.substr(11));
            } catch (...) {
                validationErrors_.push_back("Invalid connection probability: " + arg.substr(11));
            }
        } else if (arg.find("--steps=") == 0) {
            try {
                maxSteps_ = std::stoull(arg.substr(7));
            } catch (...) {
                validationErrors_.push_back("Invalid max steps: " + arg.substr(7));
            }
        } else if (arg == "--no-checkpoint") {
            enableCheckpointing_ = false;
        } else if (arg == "--no-replay") {
            enableReplay_ = false;
        } else if (arg == "--no-development") {
            enableDevelopment_ = false;
        } else if (arg == "--no-curiosity") {
            enableCuriosity_ = false;
        } else if (arg.find("--checkpoint=") == 0) {
            checkpointPath_ = arg.substr(13);
        }
    }
    
    return validateConfiguration();
}

bool Phase6ConfigManager::loadFromFile(const std::string& filename) {
    clearValidationErrors();
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        validationErrors_.push_back("Cannot open configuration file: " + filename);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        if (key == "neurons") {
            try { neuronCount_ = std::stoull(value); } catch (...) { validationErrors_.push_back("Invalid neurons value"); }
        } else if (key == "regions") {
            try { regionCount_ = std::stoull(value); } catch (...) { validationErrors_.push_back("Invalid regions value"); }
        } else if (key == "connprob") {
            try { connectionProbability_ = std::stof(value); } catch (...) { validationErrors_.push_back("Invalid connprob value"); }
        } else if (key == "steps") {
            try { maxSteps_ = std::stoull(value); } catch (...) { validationErrors_.push_back("Invalid steps value"); }
        } else if (key == "checkpoint") {
            enableCheckpointing_ = (value == "true" || value == "1" || value == "yes");
        } else if (key == "replay") {
            enableReplay_ = (value == "true" || value == "1" || value == "yes");
        } else if (key == "development") {
            enableDevelopment_ = (value == "true" || value == "1" || value == "yes");
        } else if (key == "curiosity") {
            enableCuriosity_ = (value == "true" || value == "1" || value == "yes");
        } else if (key == "checkpoint_path") {
            checkpointPath_ = value;
        }
    }
    
    file.close();
    
    return validateConfiguration();
}

bool Phase6ConfigManager::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# Phase 6 Configuration File" << std::endl;
    file << "# Generated by Phase6ConfigManager" << std::endl;
    file << std::endl;
    
    file << "neurons = " << neuronCount_ << std::endl;
    file << "regions = " << regionCount_ << std::endl;
    file << "connprob = " << std::fixed << std::setprecision(3) << connectionProbability_ << std::endl;
    file << "steps = " << maxSteps_ << std::endl;
    file << "checkpoint = " << (enableCheckpointing_ ? "true" : "false") << std::endl;
    file << "replay = " << (enableReplay_ ? "true" : "false") << std::endl;
    file << "development = " << (enableDevelopment_ ? "true" : "false") << std::endl;
    file << "curiosity = " << (enableCuriosity_ ? "true" : "false") << std::endl;
    file << "checkpoint_path = " << checkpointPath_ << std::endl;
    
    file.close();
    return true;
}

void Phase6ConfigManager::printHelp() {
    std::cout << "Phase 6 Configuration Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --neurons=<n>             Number of neurons (default: " << 1000 << ")" << std::endl;
    std::cout << "  --regions=<r>             Number of regions (default: " << 1 << ")" << std::endl;
    std::cout << "  --connprob=<p>            Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --steps=<s>               Maximum simulation steps (default: " << 10000 << ")" << std::endl;
    std::cout << "  --no-checkpoint           Disable checkpointing" << std::endl;
    std::cout << "  --no-replay               Disable replay" << std::endl;
    std::cout << "  --no-development          Disable development" << std::endl;
    std::cout << "  --no-curiosity            Disable curiosity" << std::endl;
    std::cout << "  --checkpoint=<path>       Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration file format:" << std::endl;
    std::cout << "  neurons = <n>" << std::endl;
    std::cout << "  regions = <r>" << std::endl;
    std::cout << "  connprob = <p>" << std::endl;
    std::cout << "  steps = <s>" << std::endl;
    std::cout << "  checkpoint = true/false" << std::endl;
    std::cout << "  replay = true/false" << std::endl;
    std::cout << "  development = true/false" << std::endl;
    std::cout << "  curiosity = true/false" << std::endl;
    std::cout << "  checkpoint_path = <path>" << std::endl;
}

} // namespace nlm
