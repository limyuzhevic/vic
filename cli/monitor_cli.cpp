// NLM CLI Monitor - Real-time Monitoring Commands
// Handles real-time monitoring and visualization of NLM simulations

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <atomic>

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {
namespace cli {

class MonitorCLI {
public:
    MonitorCLI() {
        initializeLogger();
    }
    
    int run(int argc, char** argv) {
        if (argc < 2) {
            printUsage();
            return 1;
        }
        
        std::string command = argv[1];
        
        try {
            if (command == "live") {
                return handleLiveMonitorCommand(argc, argv);
            } else if (command == "replay") {
                return handleReplayMonitorCommand(argc, argv);
            } else if (command == "metrics") {
                return handleMetricsMonitorCommand(argc, argv);
            } else if (command == "log") {
                return handleLogMonitorCommand(argc, argv);
            } else if (command == "export") {
                return handleExportMonitorCommand(argc, argv);
            } else {
                std::cerr << "Unknown monitor command: " << command << std::endl;
                printUsage();
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }
    
private:
    void initializeLogger() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    int handleLiveMonitorCommand(int argc, char** argv) {
        std::string configFile = "configs/default.cfg";
        double interval = 0.1;
        size_t duration = 0;  // 0 = infinite
        std::string outputFile = "monitoring.log";
        bool plotData = false;
        
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            } else if (arg.substr(0, 9) == "--interval") {
                if (arg.find('=') != std::string::npos) {
                    interval = std::stod(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    interval = std::stod(argv[++i]);
                }
            } else if (arg.substr(0, 10) == "--duration") {
                if (arg.find('=') != std::string::npos) {
                    duration = static_cast<size_t>(std::stoull(arg.substr(arg.find('=') + 1)));
                } else if (i + 1 < argc) {
                    duration = static_cast<size_t>(std::stoull(argv[++i]));
                }
            } else if (arg.substr(0, 13) == "--output-file") {
                if (arg.find('=') != std::string::npos) {
                    outputFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    outputFile = argv[++i];
                }
            } else if (arg == "--plot") {
                plotData = true;
            }
        }
        
        NLM_LOG_INFO("Starting live monitoring...");
        NLM_LOG_INFO("Config file: " + configFile);
        NLM_LOG_INFO("Interval: " + std::to_string(interval) + "s");
        if (duration > 0) {
            NLM_LOG_INFO("Duration: " + std::to_string(duration) + "s");
        } else {
            NLM_LOG_INFO("Duration: infinite");
        }
        NLM_LOG_INFO("Output file: " + outputFile);
        NLM_LOG_INFO("Plot data: " + std::string(plotData ? "yes" : "no"));
        
        return runLiveMonitoring(configFile, interval, duration, outputFile, plotData);
    }
    
    int runLiveMonitoring(const std::string& configFile, double interval, 
                         size_t duration, const std::string& outputFile,
                         bool plotData) {
        auto config = std::make_shared<Config>();
        
        if (configFile.empty() || !config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        NLM_LOG_INFO("Monitoring brain with " + std::to_string(brain->getTotalNeuronCount()) + 
                    " neurons, " + std::to_string(brain->getTotalSynapseCount()) + " synapses");
        
        std::ofstream logFile;
        if (!outputFile.empty()) {
            logFile.open(outputFile);
            if (logFile.is_open()) {
                logFile << "Time,Step,Neurons,Spikes,Firing,Active,Throughput,E/I Ratio" << std::endl;
            }
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto lastLogTime = startTime;
        SimulationStep step = 0;
        bool running = true;
        
        std::atomic<bool> stopMonitoring(false);
        
        // Start monitoring thread
        auto monitorThread = std::thread([&]() {
            while (!stopMonitoring && (duration == 0 || 
                                      std::chrono::duration_cast<std::chrono::seconds>(
                                          std::chrono::high_resolution_clock::now() - startTime).count() < duration)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(interval * 1000)));
                
                // Skip if we shouldn't run (for continuous monitoring)
                if (stopMonitoring) break;
                
                // Run one step of simulation
                brain->step(step, step * 0.001);
                step++;
                
                // Log data
                if (logFile.is_open()) {
                    auto currentTime = std::chrono::high_resolution_clock::now();
                    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                        currentTime - startTime).count() / 1000.0;
                    
                    logFile << elapsed << ","
                            << step << ","
                            << brain->getTotalNeuronCount() << ","
                            << brain->getTotalSpikeCount() << ","
                            << brain->getFiringNeuronCount() << ","
                            << brain->getActiveNeuronCount() << ","
                            << brain->getAverageFiringRate() << ","
                            << brain->getExcitationInhibitionRatio()
                            << std::endl;
                }
                
                // Print to console every 10 steps
                if (step % 10 == 0) {
                    NLM_LOG_INFO("Step " + std::to_string(step) + 
                                " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                                " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                                " | Active: " + std::to_string(brain->getActiveNeuronCount()) +
                                " | E/I: " + std::to_string(brain->getExcitationInhibitionRatio()));
                }
            }
        });
        
        // Handle user input (for interactive control)
        std::string input;
        while (running && !stopMonitoring) {
            std::cout << "\nMonitor> ";
            std::getline(std::cin, input);
            
            if (input == "pause") {
                NLM_LOG_INFO("Monitoring paused.");
                // Wait for resume command
                std::string pauseInput;
                while (running) {
                    std::cout << "Pause> ";
                    std::getline(std::cin, pauseInput);
                    if (pauseInput == "resume") {
                        NLM_LOG_INFO("Monitoring resumed.");
                        break;
                    } else if (pauseInput == "stop") {
                        running = false;
                        break;
                    }
                }
            } else if (input == "stop") {
                NLM_LOG_INFO("Stopping monitoring.");
                running = false;
                stopMonitoring = true;
                break;
            } else if (input == "status") {
                brain->logStatus();
            } else if (input == "help") {
                printMonitorHelp();
            }
        }
        
        // Wait for monitoring thread to finish
        if (monitorThread.joinable()) {
            monitorThread.join();
        }
        
        if (logFile.is_open()) {
            logFile.close();
            NLM_LOG_INFO("Monitoring data saved to: " + outputFile);
        }
        
        if (plotData) {
            NLM_LOG_INFO("Plotting data is not yet implemented.");
        }
        
        brain->logStatus();
        NLM_LOG_INFO("Monitoring completed.");
        return 0;
    }
    
    int handleReplayMonitorCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing replay file for replay monitor command" << std::endl;
            return 1;
        }
        
        std::string replayFile = argv[2];
        
        NLM_LOG_INFO("Replaying monitoring data from: " + replayFile);
        
        // Placeholder for replay implementation
        NLM_LOG_WARNING("Replay monitoring not yet implemented.");
        
        return 0;
    }
    
    int handleMetricsMonitorCommand(int argc, char** argv) {
        std::string configFile = "configs/default.cfg";
        size_t steps = 1000;
        std::string outputFile = "metrics.json";
        
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            } else if (arg.substr(0, 8) == "--steps") {
                if (arg.find('=') != std::string::npos) {
                    steps = std::stoull(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    steps = std::stoull(argv[++i]);
                }
            } else if (arg.substr(0, 13) == "--output-file") {
                if (arg.find('=') != std::string::npos) {
                    outputFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    outputFile = argv[++i];
                }
            }
        }
        
        NLM_LOG_INFO("Collecting metrics...");
        NLM_LOG_INFO("Config file: " + configFile);
        NLM_LOG_INFO("Steps: " + std::to_string(steps));
        NLM_LOG_INFO("Output file: " + outputFile);
        
        // Placeholder for metrics collection
        NLM_LOG_WARNING("Metrics collection not yet implemented.");
        
        return 0;
    }
    
    int handleLogMonitorCommand(int argc, char** argv) {
        std::string logFile = "nlm.log";
        LogLevel level = LogLevel::Info;
        bool follow = false;
        
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 10) == "--log-file") {
                if (arg.find('=') != std::string::npos) {
                    logFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    logFile = argv[++i];
                }
            } else if (arg.substr(0, 10) == "--level") {
                if (arg.find('=') != std::string::npos) {
                    std::string levelStr = arg.substr(arg.find('=') + 1);
                    level = stringToLogLevel(levelStr);
                } else if (i + 1 < argc) {
                    level = stringToLogLevel(argv[++i]);
                }
            } else if (arg == "--follow") {
                follow = true;
            }
        }
        
        NLM_LOG_INFO("Displaying log: " + logFile);
        NLM_LOG_INFO("Log level: " + logLevelToString(level));
        NLM_LOG_INFO("Follow mode: " + std::string(follow ? "enabled" : "disabled"));
        
        // Placeholder for log display
        NLM_LOG_WARNING("Log monitoring not yet implemented.");
        
        return 0;
    }
    
    int handleExportMonitorCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing format for monitor export command" << std::endl;
            return 1;
        }
        
        std::string format = argv[2];
        std::string outputFile = "monitor_export.txt";
        
        // Parse additional arguments
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 13) == "--output-file") {
                if (arg.find('=') != std::string::npos) {
                    outputFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    outputFile = argv[++i];
                }
            }
        }
        
        NLM_LOG_INFO("Exporting monitor data to: " + outputFile);
        NLM_LOG_INFO("Format: " + format);
        
        // Placeholder for monitor data export
        NLM_LOG_WARNING("Monitor export not yet implemented.");
        
        return 0;
    }
    
    LogLevel stringToLogLevel(const std::string& levelStr) {
        if (levelStr == "trace") return LogLevel::Trace;
        if (levelStr == "debug") return LogLevel::Debug;
        if (levelStr == "info") return LogLevel::Info;
        if (levelStr == "warning") return LogLevel::Warning;
        if (levelStr == "error") return LogLevel::Error;
        if (levelStr == "fatal") return LogLevel::Fatal;
        return LogLevel::Info;
    }
    
    std::string logLevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Trace: return "trace";
            case LogLevel::Debug: return "debug";
            case LogLevel::Info: return "info";
            case LogLevel::Warning: return "warning";
            case LogLevel::Error: return "error";
            case LogLevel::Fatal: return "fatal";
            default: return "info";
        }
    }
    
    void printUsage() {
        std::cout << "Usage: nlm-monitor <command> [options]" << std::endl;
        std::cout << "\nCommands:" << std::endl;
        std::cout << "  live          Live monitoring of simulation" << std::endl;
        std::cout << "  replay <file> Replay monitoring data" << std::endl;
        std::cout << "  metrics       Collect performance metrics" << std::endl;
        std::cout << "  log           Display log file" << std::endl;
        std::cout << "  export <format>  Export monitoring data" << std::endl;
    }
    
    void printMonitorHelp() {
        std::cout << "Available commands:" << std::endl;
        std::cout << "  help          Show this help" << std::endl;
        std::cout << "  status        Show current brain status" << std::endl;
        std::cout << "  pause         Pause monitoring" << std::endl;
        std::cout << "  resume        Resume monitoring" << std::endl;
        std::cout << "  stop          Stop monitoring" << std::endl;
    }
};

int main(int argc, char** argv) {
    MonitorCLI cli;
    return cli.run(argc, argv);
}

} // namespace cli
} // namespace nlm
