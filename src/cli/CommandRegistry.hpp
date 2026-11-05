class CommandRegistry {
private:
    std::map<std::string, std::unique_ptr<Command>> commands;
    
public:
    void registerCommand(std::unique_ptr<Command> command);
    Command* getCommand(const std::string& name);
    std::vector<std::string> getCommandNames() const;
    std::string getHelp(const std::string& commandName) const;
};