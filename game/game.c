
internal void
game_construct(void) {
    game.chunkLoadingRadius = 4;
    game.chunkLoadingMaxDistance = game.chunkLoadingRadius/2.0f;
    game.chunkLoadingCenter = (v3){0,0,0};
    load_chunks(game.chunkLoadingCenter, game.chunkLoadingRadius);
}

internal void
game_update(void) {
    player_movement_input();
    
    v2 loadingCenterV2 = {game.chunkLoadingCenter.x, game.chunkLoadingCenter.z};
    v2 cameraPV2 = {game.playerCamera.p.x, game.playerCamera.p.z};
    
    f32 dist = (v2_dist(loadingCenterV2, cameraPV2) / CHUNK_DIM);
    if (dist > game.chunkLoadingMaxDistance) {
        
        v2 dir = v2_sub(cameraPV2, loadingCenterV2);
        v2 newCenter = v2_div(dir, CHUNK_DIM*game.chunkLoadingMaxDistance);
        
        game.chunkLoadingCenter = v3_add(game.chunkLoadingCenter, (v3){newCenter.x, newCenter.y, 0});
        
        unload_farthest_chunks(game.chunkLoadingCenter, game.chunkLoadingRadius);
        load_chunks(game.chunkLoadingCenter, game.chunkLoadingRadius);
        
        // TODO: many small y chunks... or a single tall chunk?
    }
}

internal void
player_movement_input(void) {
    if (keyboard.w.down) {
        camera_first_person_move_north(&game.playerCamera);
    }
    
    if (keyboard.a.down) {
        camera_first_person_move_east(&game.playerCamera);
    }
    
    if (keyboard.s.down) {
        camera_first_person_move_south(&game.playerCamera);
    }
    
    if (keyboard.d.down) {
        camera_first_person_move_west(&game.playerCamera);
    }
    
    if (keyboard.space.down) {
        camera_first_person_move_up(&game.playerCamera);
    }
    
    if (keyboard.lshift.down) {
        camera_first_person_move_down(&game.playerCamera);
    }
    
    v2 deltaP = v2_sub(mouse.p, game.lastMouseP);
    game.lastMouseP = mouse.p;
    
    game.playerCamera.yaw -= 0.2f * deltaP.x;
    game.playerCamera.pitch -= 0.1f * deltaP.y;
}