#pragma once

#include <string>
#include <map>
#include <functional>
#include <memory>
#include <chrono>
#include "brain/Brain.hpp"

namespace nlm {

class Brain;
class CommandInterface;

// Command line argument parser
class CommandLineParser {
public:
    struct Options {
        std::string configFile = "configs/default.cfg";
        std::string mode = "interactive";  // interactive, run, train, test, benchmark, visualize
        int steps = 1000;
        double timestep = 0.001;
        bool help = false;
        bool version = false;
        bool verbose = false;
        std::string outputFile = "";
        bool saveState = false;
        std::string checkpointFile = "";
        int loadCheckpoint = -1;
        std::map<std::string, std::string> customArgs;
    };

    static Options parse(int argc, char** argv);
    static void printHelp();
    static void printVersion();
    static void printBanner();
};

// Command interface for different modes
class CommandInterface {
public:
    virtual ~CommandInterface() = default;
    virtual void execute(std::shared_ptr<Brain> brain, const CommandLineParser::Options& options) = 0;
    virtual std::string getName() const = 0;
};

// Command factory
class CommandFactory {
public:
    static std::unique_ptr<CommandInterface> createCommand(const std::string& mode);
};

} // namespace nlm

