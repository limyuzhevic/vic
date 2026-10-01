// Utility functions for NLM
// Common utility functions used across different modules

#include "Utilities.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>

using namespace nlm;

namespace {
    // Helper function to format numbers with thousands separators
    std::string formatNumber(size_t number) {
        std::string str = std::to_string(number);
        size_t pos = str.length();
        while (pos > 3) {
            str.insert(str.begin() + pos - 3, ',');
            pos -= 3;
        }
        return str;
    }
    
    // Helper function to format floating point numbers
    std::string formatFloat(float value, int precision = 3) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        return ss.str();
    }
    
    // Helper function to generate a simple progress indicator
    void printProgress(int current, int total, const std::string& label) {
        int width = 50;
        float progress = static_cast<float>(current) / total;
        int pos = static_cast<int>(progress * width);
        
        std::cout << "\r" << label << " [";
        for (int i = 0; i < width; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(progress * 100.0f) << "%" << std::flush;
        
        if (current == total) {
            std::cout << std::endl;
        }
    }
}

// String formatting utilities
std::string Utilities::formatNumber(size_t number) {
    return ::formatNumber(number);
}

std::string Utilities::formatFloat(float value, int precision) {
    return ::formatFloat(value, precision);
}

void Utilities::printProgress(int current, int total, const std::string& label) {
    ::printProgress(current, total, label);
}

// Time measurement utilities
class PerformanceTimer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    bool running;
    
public:
    PerformanceTimer() : running(false) {}
    
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
        running = true;
    }
    
    void stop() {
        if (running) {
            endTime = std::chrono::high_resolution_clock::now();
            running = false;
        }
    }
    
    double elapsed() const {
        if (running) {
            return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
        } else {
            return std::chrono::duration<double>(endTime - startTime).count();
        }
    }
    
    bool isRunning() const {
        return running;
    }
};

PerformanceTimer Utilities::startTimer() {
    PerformanceTimer timer;
    timer.start();
    return timer;
}

double Utilities::getElapsedSeconds(const PerformanceTimer& timer) {
    return timer.elapsed();
}

// System information utilities
std::string Utilities::getSystemInfo() {
    std::string info = "NLM System Information:";
    info += "\n  Platform: " __platform__;
    info += "\n  Compiler: " __COMPILER_VERSION__;
    info += "\n  Build: " __DATE__ " " __TIME__;
    return info;
}

std::string Utilities::getVersionString() {
    return "NLM v0.1.0 (Phase 6 - Complete Integration)";
}