#include "cli/CLIMonitor.hpp"

CLIMonitor::CLIMonitor() : performanceMonitor_(nullptr), monitoringActive_(false) {
}

bool CLIMonitor::parseCommandLine(int argc, char** argv) {
    if (argc < 3) {
        printHelp();
        return true;
    }

    std::string command = argv[2];

    if (command == "start") {
        return handleStartCommand(argc - 2, &argv[2]);
    } else if (command == "stop") {
        return handleStopCommand();
    } else if (command == "report") {
        return handleReportCommand();
    } else if (command == "help") {
        printHelp();
        return true;
    } else {
        std::cerr << "Unknown monitor command: " << command << std::endl;
        return false;
    }
}

bool CLIMonitor::handleStartCommand(int argc, char** argv) {
    double interval = 0.1;
    double duration = 0.0;  // 0 means infinite
    std::string outputFile = "";

    // Parse command line arguments
    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--interval" && i + 1 < argc) {
            interval = std::stod(argv[++i]);
        } else if (arg == "--duration" && i + 1 < argc) {
            duration = std::stod(argv[++i]);
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--help") {
            printStartHelp();
            return true;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return false;
        }
    }

    std::cout << "Starting performance monitoring..." << std::endl;
    std::cout << "Interval: " << interval << "s" << std::endl;
    std::cout << "Duration: " << (duration > 0 ? std::to_string(duration) + "s" : "infinite") << std::endl;

    // Create and start performance monitor
    performanceMonitor_ = std::make_unique<RealTimePerformanceMonitor>();
    performanceMonitor_->start_real_time_monitoring(nullptr, nullptr, nullptr, interval);
    monitoringActive_ = true;

    std::cout << "Monitoring started (press Ctrl+C to stop)" << std::endl;

    // Wait for duration or run indefinitely
    if (duration > 0) {
        std::this_thread::sleep_for(std::chrono::duration<double>(duration));
        std::cout << "Monitoring stopped (duration reached)" << std::endl;
        return handleStopCommand();
    } else {
        std::cout << "Press Ctrl+C to stop monitoring..." << std::endl;
        std::cin.get();  // Wait for user input
        return handleStopCommand();
    }
}

bool CLIMonitor::handleStopCommand() {
    if (monitoringActive_ && performanceMonitor_) {
        performanceMonitor_->stop_monitoring();
        monitoringActive_ = false;
        std::cout << "Performance monitoring stopped." << std::endl;
    }

    if (performanceMonitor_) {
        auto currentMetrics = performanceMonitor_->get_live_metrics();
        if (currentMetrics) {
            std::cout << "\nFinal Performance Metrics:" << std::endl;
            std::cout << "  Current step time: " << currentMetrics->current_step_time << "ms" << std::endl;
            std::cout << "  Current firing rate: " << currentMetrics->current_firing_rate << " Hz" << std::endl;
            std::cout << "  Current CPU usage: " << currentMetrics->current_cpu_usage << "%" << std::endl;
            std::cout << "  Current memory usage: " << currentMetrics->current_memory_usage << "MB" << std::endl;
            std::cout << "  Samples collected: " << currentMetrics->samples_count << std::endl;
        }
    }

    performanceMonitor_.reset();
    return true;
}

bool CLIMonitor::handleReportCommand() {
    if (!performanceMonitor_) {
        std::cout << "No monitoring data available. Start monitoring first." << std::endl;
        return false;
    }

    std::cout << "Performance Monitoring Report" << std::endl;
    std::cout << "===========================" << std::endl;

    auto currentMetrics = performanceMonitor_->get_live_metrics();
    if (currentMetrics) {
        std::cout << "\nCurrent Performance (last sample):" << std::endl;
        std::cout << "  Step time: " << currentMetrics->current_step_time << "ms" << std::endl;
        std::cout << "  Firing rate: " << currentMetrics->current_firing_rate << " Hz" << std::endl;
        std::cout << "  CPU usage: " << currentMetrics->current_cpu_usage << "%" << std::endl;
        std::cout << "  Memory usage: " << currentMetrics->current_memory_usage << "MB" << std::endl;
        std::cout << "  Samples: " << currentMetrics->samples_count << std::endl;
    }

    std::cout << "\nReal-time Metrics:" << std::endl;
    std::cout << "  Average step time: " << std::fixed << std::setprecision(2)
              << performanceMonitor_->getLiveMetrics()['avg_step_time'] << "ms" << std::endl;
    std::cout << "  Average firing rate: " << std::fixed << std::setprecision(2)
              << performanceMonitor_->getLiveMetrics()['avg_firing_rate'] << " Hz" << std::endl;

    return true;
}

void CLIMonitor::printHelp() {
    std::cout << "NLM Monitor Command" << std::endl;
    std::cout << "===================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Monitor NLM simulation performance in real-time." << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: nlm monitor <command> [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  start             Start performance monitoring" << std::endl;
    std::cout << "  stop              Stop performance monitoring" << std::endl;
    std::cout << "  report            Show current performance report" << std::endl;
    std::cout << "  help              Show this help message" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm monitor start --interval 0.1 --duration 60" << std::endl;
    std::cout << "  nlm monitor start" << std::endl;
    std::cout << "  nlm monitor stop" << std::endl;
    std::cout << "  nlm monitor report" << std::endl;
}

void CLIMonitor::printStartHelp() {
    std::cout << "NLM Monitor Start Command" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Start real-time performance monitoring." << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: nlm monitor start [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --interval SECONDS   Sampling interval in seconds (default: 0.1)" << std::endl;
    std::cout << "  --duration SECONDS   Monitoring duration in seconds (default: infinite)" << std::endl;
    std::cout << "  --output FILE        Save monitoring data to file" << std::endl;
    std::cout << "  --help               Show this help message" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm monitor start --interval 0.1" << std::endl;
    std::cout << "  nlm monitor start --interval 0.5 --duration 30" << std::endl;
}