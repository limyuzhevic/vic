// Main entry point for brainctl tool
int main(int argc, char* argv[]) {
    CommandLineInterface cli;
    
    if (!cli.initialize(argc, argv)) {
        return 1;
    }
    
    bool success = cli.run();
    
    cli.shutdown();
    
    return success ? 0 : 1;
}
