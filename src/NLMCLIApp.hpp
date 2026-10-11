// CommandHistory - Handles command history for interactive mode
class CommandHistory {
public:
    CommandHistory(size_t maxHistorySize = 100) : maxHistorySize_(maxHistorySize) {}

    void addCommand(const std::string& command) {
        history_.push_back(command);
        if (history_.size() > maxHistorySize_) {
            history_.erase(history_.begin());
        }
        // Store in current session
        currentSessionHistory_ = history_;
    }

    std::string getCommand(size_t index) const {
        if (index < history_.size()) {
            return history_[index];
        }
        return "";
    }

    bool canNavigateUp(size_t index) const {
        return index > 0;
    }

    bool canNavigateDown(size_t index, size_t currentSessionSize) const {
        return index < currentSessionSize - 1;
    }

    size_t getCurrentSessionSize() const {
        return currentSession_.size();
    }

    void saveSession(const std::string& sessionName) {
        sessionHistory_[sessionName] = history_;
        currentSessionHistory_ = history_;
        currentSession_ = sessionName;
    }

    void restoreSession(const std::string& sessionName) {
        auto it = sessionHistory_.find(sessionName);
        if (it != sessionHistory_.end()) {
            history_ = it->second;
            currentSessionHistory_ = history_;
            currentSession_ = sessionName;
        }
    }

    void clearSession() {
        history_.clear();
        currentSessionHistory_.clear();
        currentSession_.clear();
    }

    std::vector<std::string> getHistory() const {
        return history_;
    }

private:
    std::vector<std::string> history_;
    std::vector<std::string> currentSessionHistory_;
    std::string currentSession_;
    std::map<std::string, std::vector<std::string>> sessionHistory_;
    size_t maxHistorySize_;
};

// NLM CLI Application - Main application class
class NLMCLIApp {
public:
    NLMCLIApp() : commandHistory_(100), shouldExit_(false), isInteractive_(false),
                  lineNumber_(1), autoSaveEnabled_(true) {
        logger = &Logger::getGlobal();
        
        // Initialize command parser
        parser = new CommandParser();
        
        // Initialize simulation controller
        controller = new SimulationController();
        
        // Setup command handlers
        setupCommandHandlers();
        
        // Load configuration
        config = std::make_shared<Config>();
        loadConfiguration();
    }

    ~NLMCLIApp() {
        delete parser;
        delete controller;
    }

    bool initialize() {
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);

        // Initialize simulation controller with configuration
        if (!controller->initialize(config)) {
            logger->error("Failed to initialize simulation controller");
            return false;
        }

        logger->info("NLM CLI Application initialized successfully");
        printWelcomeBanner();
        
        return true;
    }

    void run() {
        std::cout << "Type 'help' for available commands or 'help <command>' for command details." << std::endl;
        
        if (isInteractive_) {
            runInteractiveMode();
        } else {
            runBatchMode();
        }
    }

    void runInteractiveMode() {
        std::string line;
        
        while (!shouldExit_) {
            // Display prompt
            std::cout << ">>> ";
            std::cout.flush();
            
            // Read input line
            if (!std::getline(std::cin, line)) {
                // EOF or error
                break;
            }
            
            // Trim leading/trailing whitespace
            line = trim(line);
            
            if (line.empty()) {
                continue;
            }
            
            // Add to history
            commandHistory_.addCommand(line);
            
            // Parse and execute command
            executeCommand(line);
            
            // Display prompt again
            std::cout << std::endl;
        }
    }

    void runBatchMode() {
        // In batch mode, read from file or stdin
        std::string line;
        
        while (!shouldExit_ && std::getline(std::cin, line)) {
            line = trim(line);
            
            if (line.empty()) {
                continue;
            }
            
            // Parse and execute command
            executeCommand(line);
        }
    }

    bool executeCommand(const std::string& input) {
        std::string commandName;
        std::vector<std::string> args;
        
        if (!parser->parseCommand(input, commandName, args)) {
            std::cout << "Error: Invalid command format" << std::endl;
            return false;
        }
        
        // Check if command exists
        const CommandParser::Command* cmd = parser->getCommand(commandName);
        if (!cmd) {
            std::cout << "Error: Unknown command: '" << commandName << "'" << std::endl;
            return false;
        }
        
        // Check if simulation is required
        if (cmd->requiresSimulation && !controller->isSimulationRunning()) {
            std::cout << "Error: Command requires a running simulation" << std::endl;
            return false;
        }
        
        // Execute command handler
        try {
            bool result = cmd->handler(args, controller);
            if (!result) {
                std::cout << "Error: Command execution failed" << std::endl;
            }
            return result;
        } catch (const std::exception& e) {
            std::cout << "Error: Command execution exception: " << e.what() << std::endl;
            return false;
        }
    }

    void displayHelp(const std::string& commandName = "") {
        parser->displayHelp(commandName);
    }

    void saveSession(const std::string& sessionName) {
        commandHistory_.saveSession(sessionName);
        logger->info("Session saved: " + sessionName);
    }

    void restoreSession(const std::string& sessionName) {
        commandHistory_.restoreSession(sessionName);
        logger->info("Session restored: " + sessionName);
    }

    void clearHistory() {
        commandHistory_.clearSession();
        logger->info("Command history cleared");
    }

    void showHistory(size_t count = 10) {
        std::vector<std::string> history = commandHistory_.getHistory();
        size_t start = (count > history.size()) ? 0 : history.size() - count;
        
        std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                                Command History                                ║" << std::endl;
        std::cout << "╠═══════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        for (size_t i = start; i < history.size(); ++i) {
            std::cout << "║ " << std::setw(2) << (i + 1) << ": " << history[i] << "..." << std::endl;
        }
        
        std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
    }

    void setAutoSave(bool enabled) {
        autoSaveEnabled_ = enabled;
        if (enabled) {
            logger->info("Auto-save enabled");
        } else {
            logger->info("Auto-save disabled");
        }
    }

private:
    void printWelcomeBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     A comprehensive CLI for controlling and monitoring         ║
    ║     the NLM brain simulation.                                  ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════════════════════╝
        )" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Welcome to NLM Phase 6 - Final Integration CLI" << std::endl;
        std::cout << "Type 'help' for available commands" << std::endl;
        std::cout << std::endl;
    }

    void loadConfiguration() {
        // Try to load from default config file
        std::string configFile = "configs/nlm_cli.cfg";
        if (config->loadFromFile(configFile)) {
            logger->info("Loaded CLI configuration from: " + configFile);
        } else {
            logger->info("Using default CLI configuration");
        }
    }

    std::string trim(const std::string& str) {
        const std::string whitespace = " \t\n\r";
        size_t start = str.find_first_not_of(whitespace);
        if (start == std::string::npos) return "";
        
        size_t end = str.find_last_not_of(whitespace);
        return str.substr(start, end - start + 1);
    }

    void setupCommandHandlers() {
        // Get all commands from parser
        std::vector<std::string> commandNames = parser->getAllCommandNames();
        
        // For each command, find the appropriate handler in controller
        // This is a simplified approach - in real implementation, you'd use
        // a more sophisticated registration mechanism
    }

    // Core components
    std::shared_ptr<Config> config;
    Logger* logger;
    CommandParser* parser;
    SimulationController* controller;
    
    // Application state
    CommandHistory commandHistory_;
    bool shouldExit_;
    bool isInteractive_;
    size_t lineNumber_;
    bool autoSaveEnabled_;
};