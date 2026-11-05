class CLI {
private:
    CommandRegistry registry;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    
public:
    CLI();
    bool run(int argc, char** argv);
    void printHelp() const;
    bool executeCommand(const std::string& commandName, 
                       const std::vector<std::string>& args);
};