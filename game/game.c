internal char *
string_new(char *string) {
    u32 length = (u32)strlen(string);
    assert(length > 0);
    char *result = (char *)malloc(length+1);
    memcpy(result, string, length);
    result[length] = '\0';
    return result;
}

internal void
game_construct(void) {
    game.lines = lines3D_construct(MAX_LINES);
    
    game.chunkLoadingDiameter = 5;
    
    // Set spawn location
    s32 spawnX = 0;
    s32 spawnZ = 0;
    f32 spawnY = 10;
    
    // Position the player
    game.playerCamera.p = (v3){(f32)spawnX, spawnY + 1.0f, (f32)spawnZ};
    
    
    game.centerChunkX = 1000;
    game.centerChunkY = 1000;
    game.centerChunkZ = 1000;
    
    game.textureFilenames[BLOCK_TEXTURE_DIRT] = string_new("textures/dirt.png");
    game.textureFilenames[BLOCK_TEXTURE_STONE] = string_new("textures/stone.png");
    game.textureFilenames[BLOCK_TEXTURE_GRASS_BLOCK_TOP] = string_new("textures/grass_block_top.png");
}

internal void
game_update(void) {
    // Update elapsed time
    game.elapsedTime += 1 * renderer.dt;
    
    // Update the sun direction
    f32 sunRotationSpeed = 0.1f; // Adjust this value to control the speed of sun movement
    f32 angle = game.elapsedTime * sunRotationSpeed;
    
    f32 cosAngle = cosf(angle);
    f32 sinAngle = sinf(angle);
    
    renderer.sunDirection.x = -0.3f; // X
    renderer.sunDirection.y = -1.0f; // Y
    renderer.sunDirection.z = -0.5f; // Z
    v3 initialSunDirection = renderer.sunDirection;
    
    renderer.sunDirection.x = initialSunDirection.x * cosAngle - initialSunDirection.z * sinAngle;
    renderer.sunDirection.y = initialSunDirection.y;
    renderer.sunDirection.z = initialSunDirection.x * sinAngle + initialSunDirection.z * cosAngle;
    
    player_movement_input();
    
    load_chunks_update();
}

internal void
player_movement_input(void) {
    if (keyboard.w.down) {
        camera_first_person_move_north(&game.playerCamera, 10);
    }
    
    if (keyboard.a.down) {
        camera_first_person_move_east(&game.playerCamera, 10);
    }
    
    if (keyboard.s.down) {
        camera_first_person_move_south(&game.playerCamera, 10);
    }
    
    if (keyboard.d.down) {
        camera_first_person_move_west(&game.playerCamera, 10);
    }
    
    if (keyboard.space.down) {
        camera_first_person_move_up(&game.playerCamera, 10);
    }
    
    if (keyboard.lshift.down) {
        camera_first_person_move_down(&game.playerCamera, 10);
    }
    
    game.playerCamera.yaw -= 0.2f * renderer.mouseDelta.x;
    game.playerCamera.pitch -= 0.1f * renderer.mouseDelta.y;
}