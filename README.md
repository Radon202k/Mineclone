# Mineclone
A 3D voxel game written in C.

## Goals

### Core Game Engine
- Rendering
- Input handling
- Terrain generation
- Collision detection

### Single-Player Functionality
- Basic player movement
- Block placement and removal
- Crafting
- Inventory management

### Networking Integration
- Server-client communication
- Game state synchronization
- Interpolation
- Prediction

### Multiplayer Gameplay
- Player interactions
- Chat
- Shared world events

### Optimization and Polish
- Addressing performance issues
- Addressing security concerns
- Refining the user interface

### Testing and Debugging
- Identifying remaining issues
- Ensuring a smooth multiplayer experience

# Currently doing

## Terrain generation goals

- Biome generation:
  - Use noise functions, temperature, and precipitation values to determine the biome for each location.
  - Different biomes have different types of terrain, vegetation, and structures.

- Terrain shaping:
  - Shape the terrain using various noise functions after generating biomes.
  - Noise functions control the overall shape of the terrain, including elevation, mountains, caves, and other features.
  - Each biome has its own set of noise functions and parameters to create distinctive terrain features.
  - Use noise functions to create smooth transitions between biomes.

- Structure generation:
  - Generate trees, plants, rocks, and buildings based on the biome.
  - Each biome has its own set of structures and rules for generating them.
  - Use noise functions to control the placement and density of these elements.

- Decoration and post-processing:
  - Decorate the terrain with additional details such as grass, flowers, ores, and water or lava sources.
  - Apply post-processing to smooth out any rough edges or fix any inconsistencies.
