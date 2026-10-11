// NLMSession Implementation

#include "NLMSession.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>

namespace nlm {
namespace cli {

struct NLMSession::Impl {
    std::vector<std::string> commandBuffer;
    std::vector<std::string> outputBuffer;
    bool sessionActive = false;
    std::chrono::high_resolution_clock::time_point sessionStartTime;
    bool profiling = false;
    double totalCpuTime = 0.0;
    size_t commandCount = 0;
};

NLMSession::NLMSession(std::shared_ptr<Config> config,
                      std::shared_ptr<Logger> logger,
                      std::shared_ptr<CommandLineParser> parser)
    : config_(config), logger_(logger), parser_(parser) {
    sessionId_ = "session_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    pImpl.reset(new Impl());
    
    if (logger_) {
        NLM_LOG_INFO("NLMSession created: " + sessionId_);
    }
}

NLMSession::~NLMSession() {
    shutdown();
}

bool NLMSession::initialize(std::shared_ptr<Brain> brain) {
    brain_ = brain;
    pImpl->sessionActive = true;
    pImpl->sessionStartTime = std::chrono::high_resolution_clock::now();
    pImpl->commandCount = 0;
    
    if (logger_) {
        NLM_LOG_INFO("NLMSession initialized: " + sessionId_);
        if (brain_) {
            NLM_LOG_INFO("Brain initialized for session");
        }
    }
    
    return true;
}

void NLMSession::shutdown() {
    if (!pImpl->sessionActive) return;
    
    if (brain_) {
        // Save final state if needed
        brain_->reset();
    }
    
    pImpl->sessionActive = false;
    
    if (logger_) {
        NLM_LOG_INFO("NLMSession shutdown: " + sessionId_);
    }
}

bool NLMSession::runCommand(const std::string& commandLine) {
    if (!pImpl->sessionActive) {
        if (logger_) {
            NLM_LOG_ERROR("Session not active: " + sessionId_);
        }
        return false;
    }
    
    pImpl->commandBuffer.push_back(commandLine);
    pImpl->commandCount++;
    
    // Parse and execute command
    std::istringstream iss(commandLine);
    std::string command;
    iss >> command;
    
    std::vector<std::string> args;
    std::string arg;
    while (iss >> arg) {
        args.push_back(arg);
    }
    
    bool success = parser_->executeCommand(command, args, shared_from_this());
    
    if (success && logger_) {
        NLM_LOG_INFO("Command executed successfully: " + commandLine);
    } else if (!success && logger_) {
        NLM_LOG_ERROR("Command failed: " + commandLine);
    }
    
    return success;
}

bool NLMSession::runBatchFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        if (logger_) {
            NLM_LOG_ERROR("Cannot open batch file: " + filename);
        }
        return false;
    }
    
    std::string line;
    bool allSuccess = true;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        bool success = runCommand(line);
        if (!success) {
            if (logger_) {
                NLM_LOG_ERROR("Batch file error at line " + std::to_string(lineNum) + ": " + line);
            }
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

void NLMSession::saveBrainState(const std::string& filename) const {
    if (!brain_) {
        if (logger_) {
            NLM_LOG_ERROR("No brain to save");
        }
        return;
    }
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        if (logger_) {
            NLM_LOG_ERROR("Cannot save brain state to: " + filename);
        }
        return;
    }
    
    // Serialize brain state (simplified)
    try {
        // Get brain configuration
        auto config = brain_->getConfig();
        json brainState;
        
        // Store key brain properties
        brainState["session_id"] = sessionId_;
        brainState["neuron_count"] = brain_->getTotalNeuronCount();
        brainState["synapse_count"] = brain_->getTotalSynapseCount();
        brainState["total_spikes"] = brain_->getTotalSpikeCount();
        brainState["firing_neuron_count"] = brain_->getFiringNeuronCount();
        brainState["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
        
        // Save to file
        file << brainState.dump(2);
        
        if (logger_) {
            NLM_LOG_INFO("Brain state saved to: " + filename);
        }
    } catch (const std::exception& e) {
        if (logger_) {
            NLM_LOG_ERROR(std::string("Failed to save brain state: ") + e.what());
        }
    }
}

bool NLMSession::loadBrainState(const std::string& filename) {
    if (!brain_) {
        if (logger_) {
            NLM_LOG_ERROR("No brain to load into");
        }
        return false;
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        if (logger_) {
            NLM_LOG_ERROR("Cannot load brain state from: " + filename);
        }
        return false;
    }
    
    try {
        json brainState;
        file >> brainState;
        
        // Reset current brain
        brain_->reset();
        brain_->initialize();
        
        if (logger_) {
            NLM_LOG_INFO("Brain state loaded from: " + filename);
        }
        
        return true;
    } catch (const std::exception& e) {
        if (logger_) {
            NLM_LOG_ERROR(std::string("Failed to load brain state: ") + e.what());
        }
        return false;
    }
}

void NLMSession::resetBrainState() {
    if (brain_) {
        brain_->reset();
        brain_->initialize();
        
        if (logger_) {
            NLM_LOG_INFO("Brain state reset");
        }
    }
}

void NLMSession::runExperiment(const std::string& experimentName, 
                              const std::vector<std::string>& args) {
    if (!brain_) {
        if (logger_) {
            NLM_LOG_ERROR("No brain for experiment");
        }
        return;
    }
    
    // Simplified experiment runner - in real implementation this would be more sophisticated
    if (experimentName == "plasticity") {
        if (logger_) {
            NLM_LOG_INFO("Running plasticity experiment with " + std::to_string(args.size()) + " args");
        }
        
        // Enable plasticity
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run experiment steps
        for (int step = 0; step < 1000; ++step) {
            for (size_t i = 0; i < 20 && i < brain_->getTotalNeuronCount() / 4; ++i) {
                brain_->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain_->step(step, step * 0.001);
        }
        
        if (logger_) {
            NLM_LOG_INFO("Plasticity experiment completed");
        }
    } else {
        if (logger_) {
            NLM_LOG_WARNING("Unknown experiment: " + experimentName);
        }
    }
}

void NLMSession::stopProfiling() {
    if (!pImpl->profiling) return;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    profilingDuration_ = std::chrono::duration<double>(endTime - profilingStartTime_).count();
    pImpl->profiling = false;
    
    if (logger_) {
        NLM_LOG_INFO("Profiling stopped. Duration: " + std::to_string(profilingDuration_) + "s");
    }
}

double NLMSession::getProfilingTime() const {
    if (!pImpl->profiling) {
        return profilingDuration_;
    }
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(currentTime - profilingStartTime_).count();
}

void NLMSession::logSessionStatus() const {
    if (!logger_) return;
    
    NLM_LOG_INFO("=== NLMSession Status ===");
    NLM_LOG_INFO("Session ID: " + sessionId_);
    NLM_LOG_INFO("Active: " + std::string(pImpl->sessionActive ? "yes" : "no"));
    NLM_LOG_INFO("Commands executed: " + std::to_string(pImpl->commandCount));
    
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(now - pImpl->sessionStartTime).count();
    NLM_LOG_INFO("Session duration: " + std::to_string(duration) + "s");
    
    if (brain_) {
        NLM_LOG_INFO("Brain neurons: " + std::to_string(brain_->getTotalNeuronCount()));
        NLM_LOG_INFO("Brain synapses: " + std::to_string(brain_->getTotalSynapseCount()));
        NLM_LOG_INFO("Total spikes: " + std::to_string(brain_->getTotalSpikeCount()));
    }
}

} // namespace cli
} // namespace nlm
