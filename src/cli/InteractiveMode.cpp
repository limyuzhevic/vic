// src/cli/InteractiveMode.cpp
#include "InteractiveMode.hpp"
#include "AdvancedCLI.hpp"
#include "SimulationController.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

namespace nlm {

struct InteractiveMode::Impl {
    bool active;
    int updateFrequency;
    std::chrono::milliseconds updateInterval;
    AdvancedCLI* cli;
    SimulationController* simulation;
    
    std::vector<std::string> commandHistory;
    size_t historyIndex;
    
    Impl() : active(false)
           , updateFrequency(10)
           , updateInterval(100) // 100ms = 10 updates per second
           , cli(nullptr)
           , simulation(nullptr)
           , historyIndex(0) {
    }
};

InteractiveMode::InteractiveMode() : pImpl(std::make_unique<Impl>()) {
}

InteractiveMode::~InteractiveMode() = default;

bool InteractiveMode::start(AdvancedCLI& cli, SimulationController& simulation) {
    if (active) {
        NLM_LOG_WARNING("Interactive mode is already active");
        return false;
    }
    
    pImpl->cli = &cli;
    pImpl->simulation = &simulation;
    pImpl->active = true;
    pImpl->historyIndex = 0;
    
    NLM_LOG_INFO("Interactive mode started");
    
    // Start display refresh thread
    std::thread refreshThread([this]() {
        while (active) {
            refreshDisplay();
            std::this_thread::sleep_for(pImpl->updateInterval);
        }
    });
    
    refreshThread.detach();
    
    // Start input handling
    std::string input;
    
    std::cout << "=== NLM Interactive Mode ===" << std::endl;
    std::cout << "Type 'help' for available commands or 'quit' to exit." << std::endl;
    std::cout << std::endl;
    
    while (active) {
        std::cout << ">>> ";
        std::getline(std::cin, input);
        
        if (input.empty()) {
            continue;
        }
        
        handleInput(input);
        
        // Add to command history
        pImpl->commandHistory.push_back(input);
        if (pImpl->commandHistory.size() > 100) {
            pImpl->commandHistory.erase(pImpl->commandHistory.begin());
        }
        pImpl->historyIndex = pImpl->commandHistory.size();
    }
    
    return true;
}

bool InteractiveMode::stop() {
    if (!active) {
        return false;
    }
    
    active = false;
    NLM_LOG_INFO("Interactive mode stopped");
    return true;
}

bool InteractiveMode::isActive() const {
    return pImpl->active;
}

void InteractiveMode::displayHelp() const {
    std::cout << "=== NLM Interactive Mode Help ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Simulation Control:" << std::endl;
    std::cout << "  play/p   - Resume simulation" << std::endl;
    std::cout << "  pause    - Pause simulation" << std::endl;
    std::cout << "  step     - Execute one simulation step" << std::endl;
    std::cout << "  reset    - Reset simulation to initial state" << std::endl;
    std::cout << std::endl;
    std::cout << "Speed Control:" << std::endl;
    std::cout << "  speed N   - Set simulation speed to N times normal speed" << std::endl;
    std::cout << "  speed     - Show current speed" << std::endl;
    std::cout << std::endl;
    std::cout << "Recording:" << std::endl;
    std::cout << "  record file:format - Start recording to file" << std::endl;
    std::cout << "  stop      - Stop recording" << std::endl;
    std::cout << "  data      - Show recorded data" << std::endl;
    std::cout << std::endl;
    std::cout << "Brain Control:" << std::endl;
    std::cout << "  brain help - Show brain commands" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  config load path - Load configuration from file" << std::endl;
    std::cout << "  config save path - Save current configuration to file" << std::endl;
    std::cout << std::endl;
    std::cout << "Information:" << std::endl;
    std::cout << "  status   - Show simulation status" << std::endl;
    std::cout << "  brain status - Show brain statistics" << std::endl;
    std::cout << std::endl;
    std::cout << "Interaction:" << std::endl;
    std::cout << "  help     - Show this help" << std::endl;
    std::cout << "  quit/exit - Exit interactive mode" << std::endl;
    std::cout << "  history  - Show command history" << std::endl;
    std::cout << "  clear    - Clear command history" << std::endl;
}

void InteractiveMode::setUpdateFrequency(int frequency) {
    if (frequency <= 0) {
        NLM_LOG_ERROR("Update frequency must be positive");
        return;
    }
    
    pImpl->updateFrequency = frequency;
    pImpl->updateInterval = std::chrono::milliseconds(1000 / frequency);
    NLM_LOG_INFO(std::string("Update frequency set to: ") + std::to_string(frequency) + 
                std::string(" Hz"));
}

int InteractiveMode::getUpdateFrequency() const {
    return pImpl->updateFrequency;
}

void InteractiveMode::refreshDisplay() {
    if (!active || !simulation) {
        return;
    }
    
    system("clear"); // Clear screen (Linux/macOS)
    // For Windows: system("cls");
    
    std::cout << "=== NLM Interactive Mode ===" << std::endl;
    std::cout << std::endl;
    
    // Display simulation status
    std::cout << "Simulation: " << (simulation->isSimulationRunning() ? "RUNNING" : "STOPPED") << std::endl;
    std::cout << "Paused: " << (simulation->isSimulationPaused() ? "YES" : "NO") << std::endl;
    std::cout << "Speed: " << simulation->getSimulationSpeed() << "x" << std::endl;
    std::cout << "Step: " << simulation->getCurrentStep() << std::endl;
    std::cout << "Time: " << simulation->getCurrentTime() << " seconds" << std::endl;
    std::cout << std::endl;
    
    // Display brain statistics if available
    if (simulation->isSimulationRunning() && pImpl->simulation->getCurrentStep() > 0) {
        std::cout << "Brain Statistics:" << std::endl;
        if (pImpl->simulation->getCurrentStep() > 0) {
            // We don't have direct access to brain from SimulationController in this simplified version
            std::cout << "  (Brain statistics available in full implementation)" << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Display recording status
    std::cout << "Recording: " << (simulation->getRecordedData().size() > 0 ? "ACTIVE" : "INACTIVE") << std::endl;
    if (simulation->getRecordedData().size() > 0) {
        std::cout << "Recorded Data Points: " << simulation->getRecordedData().size() << std::endl;
    }
    std::cout << std::endl;
    
    // Display last few command outputs
    std::cout << "Command History (last 5):" << std::endl;
    size_t startIndex = (pImpl->commandHistory.size() > 5) ? 
                        pImpl->commandHistory.size() - 5 : 0;
    
    for (size_t i = startIndex; i < pImpl->commandHistory.size(); ++i) {
        std::cout << "  " << i + 1 << ": " << pImpl->commandHistory[i] << std::endl;
    }
    std::cout << std::endl;
}

bool InteractiveMode::handleInput(const std::string& input) {
    std::istringstream iss(input);
    std::string command;
    iss >> command;
    
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    if (command == "help" || command == "h") {
        displayHelp();
    } else if (command == "quit" || command == "exit" || command == "q") {
        stop();
    } else if (command == "play" || command == "p") {
        if (simulation) {
            simulation->resumeSimulation();
        }
    } else if (command == "pause") {
        if (simulation) {
            simulation->pauseSimulation();
        }
    } else if (command == "step") {
        if (simulation) {
            simulation->stepSimulation();
        }
    } else if (command == "reset") {
        if (simulation) {
            simulation->resetSimulation();
        }
    } else if (command == "status") {
        if (simulation) {
            std::cout << simulation->getSimulationStatistics() << std::endl;
        }
    } else if (command == "record") {
        std::string rest;
        std::getline(iss, rest);
        if (!rest.empty()) {
            rest = rest.substr(1); // Remove leading space
            size_t colonPos = rest.find(':');
            if (colonPos != std::string::npos) {
                std::string outputPath = rest.substr(0, colonPos);
                std::string format = rest.substr(colonPos + 1);
                if (simulation) {
                    simulation->startRecording(outputPath, format);
                }
            } else {
                std::cout << "Usage: record <output_path>:<format>" << std::endl;
                std::cout << "Formats: text, csv, json" << std::endl;
            }
        } else {
            std::cout << "Usage: record <output_path>:<format>" << std::endl;
            std::cout << "Formats: text, csv, json" << std::endl;
        }
    } else if (command == "stop") {
        if (simulation) {
            simulation->stopRecording();
        }
    } else if (command == "data") {
        if (simulation) {
            auto data = simulation->getRecordedData();
            if (data.empty()) {
                std::cout << "No recorded data available" << std::endl;
            } else {
                std::cout << "Last " << std::min(size_t(10), data.size()) << " data points:" << std::endl;
                size_t startIndex = (data.size() > 10) ? data.size() - 10 : 0;
                for (size_t i = startIndex; i < data.size(); ++i) {
                    std::cout << "  " << i + 1 << ": " << data[i] << std::endl;
                }
            }
        }
    } else if (command == "speed") {
        std::string rest;
        std::getline(iss, rest);
        if (rest.empty()) {
            // Show current speed
            if (simulation) {
                std::cout << "Current speed: " << simulation->getSimulationSpeed() << "x" << std::endl;
            }
        } else {
            try {
                double speed = std::stod(rest);
                if (simulation) {
                    simulation->setSimulationSpeed(speed);
                }
            } catch (const std::exception& e) {
                std::cout << "Invalid speed value: " << rest << std::endl;
            }
        }
    } else if (command == "history") {
        std::cout << "Command History:" << std::endl;
        for (size_t i = 0; i < pImpl->commandHistory.size(); ++i) {
            std::cout << "  " << i + 1 << ": " << pImpl->commandHistory[i] << std::endl;
        }
    } else if (command == "clear") {
        pImpl->commandHistory.clear();
        std::cout << "Command history cleared" << std::endl;
    } else if (command == "brain") {
        std::string rest;
        std::getline(iss, rest);
        if (rest.empty()) {
            std::cout << "Brain commands:" << std::endl;
            std::cout << "  brain help - Show brain commands" << std::endl;
            // Add more brain commands as needed
        } else if (rest == "help") {
            std::cout << "Brain commands:" << std::endl;
            std::cout << "  (Brain commands will be implemented in full version)" << std::endl;
        } else {
            std::cout << "Unknown brain command: " << rest << std::endl;
        }
    } else {
        std::cout << "Unknown command: " << command << std::endl;
        std::cout << "Type 'help' for available commands" << std::endl;
    }
    
    return true;
}

} // namespace nlm