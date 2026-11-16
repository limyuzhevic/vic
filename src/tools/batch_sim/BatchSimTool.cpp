// BatchSimTool.cpp

#include "BatchSimTool.hpp"
#include "../Tools.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <future>
#include <algorithm>

namespace nlm {

BatchSimToolImpl::BatchSimToolImpl() {
    NLM_LOG_INFO("BatchSimToolImpl initialized");
}

int BatchSimTool::run(int argc, char** argv) {
    if (argc < 2) {
        printHelp();
        return static_cast<int>(ToolExitCode::UsageError);
    }
    
    std::string command = argv[1];
    
    if (command == "load") {
        return loadCommand(argc, argv);
    } else if (command == "run") {
        return runCommand(argc, argv);
    } else if (command == "status") {
        return statusCommand(argc, argv);
    } else if (command == "resume") {
        return resumeCommand(argc, argv);
    } else if (command == "cancel") {
        return cancelCommand(argc, argv);
    } else if (command == "help") {
        printHelp();
        return 0;
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        printHelp();
        return static_cast<int>(ToolExitCode::UsageError);
    }
}

int BatchSimTool::loadCommand(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Error: Missing scenario file path" << std::endl;
        return 1;
    }
    
    std::string filepath = argv[2];
    
    if (!ToolUtils::validateFile(filepath)) {
        std Cerr << "Error: Invalid scenario file: " << filepath << std::endl;
        return 1;
    }
    
    if (toolImpl_->loadScenarios(filepath)) {
        std::cout << "Scenarios loaded from: " << filepath << std::endl;
        return 0;
    }
    
    return 1;
}

int BatchSimTool::runCommand(int argc, char** argv) {
    std::string outputDir = "./batch_results";
    size_t parallelThreads = 4;
    bool dryRun = false;
    bool verbose = false;
    
    for (int i = 2; i < argc; ++i) {
        if (argv[i] == std::string("--output") && i + 1 < argc) {
            outputDir = argv[++i];
        } else if (argv[i] == std::string("--parallel") && i + 1 < argc) {
            parallelThreads = static_cast<size_t>(std::stoull(argv[++i]));
        } else if (argv[i] == std::string("--dry-run")) {
            dryRun = true;
        } else if (argv[i] == std::string("--verbose")) {
            verbose = true;
        }
    }
    
    if (!ToolUtils::ensureDirectory(outputDir)) {
        std::cerr << "Error: Cannot create output directory: " << outputDir << std::endl;
        return 1;
    }
    
    std::cout << "Running batch simulation..." << std::endl;
    std::cout << "Output directory: " << outputDir << std::endl;
    
    if (toolImpl_->runBatch(outputDir, parallelThreads, dryRun, verbose)) {
        std::cout << "Batch simulation completed successfully" << std::endl;
        return 0;
    }
    
    return 1;
}

int BatchSimTool::statusCommand(int argc, char** argv) {
    toolImpl_->showStatus();
    return 0;
}

int BatchSimTool::resumeCommand(int argc, char** argv) {
    std::cout << "Resuming batch simulation..." << std::endl;
    
    if (toolImpl_->resume()) {
        std::cout << "Batch simulation resumed successfully" << std::endl;
        return 0;
    }
    
    return 1;
}

int BatchSimTool::cancelCommand(int argc, char** argv) {
    std::string scenarioName = "all";
    
    if (argc >= 3) {
        scenarioName = argv[2];
    }
    
    std::cout << "Cancelling batch simulation: " << scenarioName << std::endl;
    
    if (toolImpl_->cancel(scenarioName)) {
        std::cout << "Batch simulation cancelled successfully" << std::endl;
        return 0;
    }
    
    return 1;
}

void BatchSimTool::printHelp() {
    std::cout << "Batch Simulation Tool" << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "Usage: nlm-batch <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  load <file>                    Load scenarios from file" << std::endl;
    std::cout << "  run [--output dir] [--parallel N] [--dry-run] [--verbose] Run batch simulations" << std::endl;
    std::cout << "  status                          Show current batch status" << std::endl;
    std::cout << "  resume                          Resume interrupted batch" << std::endl;
    std::cout << "  cancel [name]                  Cancel batch or specific scenario" << std::endl;
    std::cout << "  help                            Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm-batch load scenarios.txt" << std::endl;
    std::cout << "  nlm-batch run --output results --parallel 4 --dry-run" << std::endl;
    std::cout << "  nlm-batch status" << std::endl;
}

} // namespace nlm