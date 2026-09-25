ComponentRegistry.hpp

This file contains the ComponentRegistry implementation for the NLM (Neural Learning Machine)
project's Component Registry system. It provides:

1. Central component registry for managing all NLM components
2. Component registration and discovery
3. Plugin architecture for dynamic component loading
4. Component validation and compatibility checking
5. Component lifecycle management
6. Serialization and export/import functionality

The ComponentRegistry system is part of NLM Phase 6 "FINAL INTEGRATION",
powering advanced artificial brain capabilities through a comprehensive
component management and extension framework.

The registry supports:
- Registration of components with metadata and factories
- Discovery of components by type, category, tags, and full-text search
- Plugin management for dynamic component loading
- Component lifecycle events
- Configuration management
- Export and import of component registries
- Registry event handling

Integration with:
- Brain (central neural coordination)
- AgentBrain (agent interface)
- World (environmental interaction)
- Memory systems (working, episodic, semantic, procedural)
- Prediction systems (forward models)
- Cognitive systems (planning, attention, concept formation)
- Neuromodulation systems (dopamine, serotonin, norepinephrine, acetylcholine)
- Development systems (maturation, learning)
- Performance infrastructure (memory pools, event queues)

See src/registry/README.md for detailed documentation.