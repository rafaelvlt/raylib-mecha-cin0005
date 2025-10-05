#include "raylib.h"
#include "raymath.h"
#include "ecs_entitymanager.h" 
#include "ecs_systems.h"       

int main(void) {
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "ECS TEST");

    Camera camera = { 0 };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    EntityManager entityManager;
    InitEntityManager(&entityManager);

    Mesh playerMesh = GenMeshCube(2.0f, 4.0f, 2.0f); 
    Model playerModel = LoadModelFromMesh(playerMesh);

    Mesh enemyMesh = GenMeshSphere(1.5f, 16, 16);    
    Model enemyModel = LoadModelFromMesh(enemyMesh);

    Entity player = CreateEntity(&entityManager);
    AddTransformComponent(&entityManager, player, (Vector3){ 0.0f, 2.0f, 0.0f });
    AddPhysicsComponent(&entityManager, player, (Vector3){ 0.0f, 0.0f, 0.0f });
    AddRenderComponent(&entityManager, player, playerModel, BLUE);
    AddPlayerControlComponent(&entityManager, player, &camera, 0.003f);
    
    entityManager.renderComponents[player].isVisible = false;
    
    Entity enemy = CreateEntity(&entityManager);
    AddTransformComponent(&entityManager, enemy, (Vector3){ 15.0f, 1.0f, 10.0f });
    AddPhysicsComponent(&entityManager, enemy, (Vector3){ -2.0f, 0.0f, 0.0f }); 
    AddRenderComponent(&entityManager, enemy, enemyModel, RED);

    DisableCursor();
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        InputSystem_Update(&entityManager);
        
        MovementSystem_Update(&entityManager, dt);

        PlayerControlComponent* playerControl = &entityManager.playerControlComponents[player];
        
        camera.position = entityManager.transformComponents[player].position;

        camera.position.y += 1.8f; 
        
        Matrix rotation = MatrixMultiply(MatrixRotateX(playerControl->cameraPitch), MatrixRotateY(playerControl->cameraYaw));
        Vector3 forward = Vector3Transform((Vector3){ 0, 0, 1 }, rotation);
        camera.target = Vector3Add(camera.position, forward);


        BeginDrawing();
            ClearBackground(DARKGRAY);
            BeginMode3D(camera);
                RenderSystem_Draw(&entityManager);
                DrawGrid(40, 1.0f);
            EndMode3D();


            int yPos = 10;
            DrawRectangle(5, 5, 450, 120, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(5, 5, 450, 120, BLUE);

            DrawText("DEBUG INFO", 15, yPos += 5, 20, BLACK);
            DrawText(TextFormat("Player Pos: (%.2f, %.2f, %.2f)", 
                entityManager.transformComponents[player].position.x, 
                entityManager.transformComponents[player].position.y, 
                entityManager.transformComponents[player].position.z), 15, yPos += 25, 10, BLACK);
            DrawText(TextFormat("Camera Pos: (%.2f, %.2f, %.2f)", camera.position.x, camera.position.y, camera.position.z), 15, yPos += 15, 10, BLACK);
            DrawText(TextFormat("Camera Target: (%.2f, %.2f, %.2f)", camera.target.x, camera.target.y, camera.target.z), 15, yPos += 15, 10, BLACK);
            DrawText(TextFormat("Camera Yaw (H): %.2f rad", playerControl->cameraYaw), 15, yPos += 15, 10, BLACK);
            DrawText(TextFormat("Camera Pitch (V): %.2f rad", playerControl->cameraPitch), 15, yPos += 15, 10, BLACK);
            DrawFPS(GetScreenWidth() - 100, 10);

        EndDrawing();
    }

    UnloadModel(playerModel);
    UnloadModel(enemyModel);
    
    CloseWindow();

    return 0;
}

