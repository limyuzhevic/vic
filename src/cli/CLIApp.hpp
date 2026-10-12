#include "cli/CLIParser.hpp"
#include "cli/ConfigManager.hpp"
#include "cli/CommandHandler.hpp"
#include "cli/HelpSystem.hpp"
#include "cli/LoggerConfig.hpp"
#include "cli/ProgressMonitor.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "errors/ErrorTypes.hpp"
#include "errors/ErrorContext.hpp"
#include "error_reporting/ErrorReporter.hpp"
#include "monitoring/SystemMonitor.hpp"
#include <memory>

namespace nlm {
namespace cli {

// Main CLI Application
class CLIApp {
public:
    CLIApp();
    ~CLIApp() = default;
    
    // Run the CLI application
    int run(int argc, char** argv);
    
    // Main application loop
    int execute();

private:
    CLIParser parser;
    ConfigManager configManager;
    CommandHandler commandHandler;
    HelpSystem helpSystem;
    LoggerConfig loggerConfig;
    ProgressMonitor progressMonitor;
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimulationClock> clock;
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_APP_HPP
