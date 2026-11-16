#ifndef NLM_CLI_SUBCOMMANDS_HPP
#define NLM_CLI_SUBCOMMANDS_HPP

#include "core/CLI/CLIFramework.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase5Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase4Demo.hpp"
#include "experiments/Phase5Demo.hpp"
#include "experiments/Phase6Demo.hpp"
#include "experiments/Phase6TextDemo.hpp"
#include "benchmark/Benchmark.hpp"
#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>

namespace nlm {

// Phase 3 CLI commands
int phase3Command(int argc, char** argv);
int phase3DemoCommand(int argc, char** argv);

// Phase 4 CLI commands
int phase4Command(int argc, char** argv);
int phase4DemoCommand(int argc, char** argv);

// Phase 5 CLI commands
int phase5Command(int argc, char** argv);
int phase5DemoCommand(int argc, char** argv);

// Phase 6 CLI commands
int phase6Command(int argc, char** argv);
int phase6DemoCommand(int argc, char** argv);
int phase6TextDemoCommand(int argc, char** argv);

// Configuration management commands
int configCommand(int argc, char** argv);
int configLoadCommand(int argc, char** argv);
int configSaveCommand(int argc, char** argv);
int configValidateCommand(int argc, char** argv);
int configShowCommand(int argc, char** argv);

// Benchmark commands
int benchmarkCommand(int argc, char** argv);

// Utility commands
int helpCommand(int argc, char** argv);
int versionCommand(int argc, char** argv);

// Common utilities
void printBanner();
void printPhaseInfo(int phase);
void setupLogging(const std::string& logLevel = "info");
std::shared_ptr<Config> loadConfigFromFile(const std::string& filename);
void printConfigSummary(const Config& config);
void printBenchmarkResults(const std::vector<BenchmarkResult>& results);

// Argument parsing helpers
template<typename T>
T getArgValue(int argc, char** argv, int index, const T& defaultValue);

} // namespace nlm

#endif // NLM_CLI_SUBCOMMANDS_HPP
