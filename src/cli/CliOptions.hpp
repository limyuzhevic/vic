// NLM Command-Line Interface Options

#pragma once

#include <string>
#include <vector>
#include <map>

namespace nlm {

enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Command-line options structure
struct CliOptions {
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    std::string logFile;
    LogLevel logLevel = LogLevel::Info;
    bool verbose = false;
    bool batchMode = false;
    std::string batchFile;
    std::string experimentName;
    size_t maxSteps = 10000;
    size_t neuronCount = 1000;
    bool help = false;
    bool version = false;
    std::string demoType = "main"; // main, phase3, phase4, phase5, phase6, phase6_text, benchmark
    bool listExperiments = false;
    bool listBenchmarks = false;
};