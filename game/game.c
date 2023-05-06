
internal void
game_construct(void) {
    game.chunkLoadingDiameter = 5;
    load_chunk(0,0,0);
    game.playerCamera.p = (v3){0,4,0};
}

internal void
game_update(void) {
    player_movement_input();
    
    load_chunks_update();
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