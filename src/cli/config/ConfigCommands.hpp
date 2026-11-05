class ConfigCommands {
public:
    // Base class for configuration commands
    class ConfigCommand : public Command {
    public:
        virtual ~ConfigCommand() = default;
        
        // Configure command prefix
        static std::string getCommandPrefix() { return "config"; }
    };
};