#include "raylib.h"
#include "raymath.h"
#include "ecs_entitymanager.h" 
#include "ecs_systems.h"       


int main(void) {
    // --- 1. INICIALIZAÇÃO GERAL ---
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "Teste do ECS - Primeira Pessoa");

    Camera camera = { 0 };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    // --- 2. INICIALIZAÇÃO DO ECS ---
    EntityManager entityManager;
    InitEntityManager(&entityManager);

    // --- 3. CRIAÇÃO DOS RECURSOS (ASSETS) ---
    Mesh playerMesh = GenMeshCube(2.0f, 4.0f, 2.0f); // Malha do jogador
    Model playerModel = LoadModelFromMesh(playerMesh);

    Mesh enemyMesh = GenMeshSphere(1.5f, 16, 16);    // Malha do inimigo
    Model enemyModel = LoadModelFromMesh(enemyMesh);

    // --- 4. CRIAÇÃO DAS ENTIDADES ---
    
    // Cria a entidade do Jogador
    Entity player = CreateEntity(&entityManager);
    AddTransformComponent(&entityManager, player, (Vector3){ 0.0f, 2.0f, 0.0f });
    AddPhysicsComponent(&entityManager, player, (Vector3){ 0.0f, 0.0f, 0.0f });
    AddRenderComponent(&entityManager, player, playerModel, BLUE);
    AddPlayerControlComponent(&entityManager, player, &camera, 0.003f); // Sensibilidade do rato ajustada
    
    // Esconde o modelo do jogador para a visão em primeira pessoa.
    entityManager.renderComponents[player].isVisible = false;
    
    // Cria a entidade do Inimigo
    Entity enemy = CreateEntity(&entityManager);
    AddTransformComponent(&entityManager, enemy, (Vector3){ 15.0f, 1.0f, 10.0f });
    AddPhysicsComponent(&entityManager, enemy, (Vector3){ -2.0f, 0.0f, 0.0f }); // Dá uma velocidade inicial
    AddRenderComponent(&entityManager, enemy, enemyModel, RED);

    DisableCursor();
    SetTargetFPS(60);

    // --- 5. O LOOP PRINCIPAL DO JOGO ---
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // --- FASE DE UPDATE ---
        
        // 1. O InputSystem lê o teclado/mouse e atualiza os ângulos da câmara e a aceleração do jogador.
        InputSystem_Update(&entityManager);
        
        // 2. O MovementSystem aplica a física a tidas as entidades com Transform e Physics.
        MovementSystem_Update(&entityManager, dt);
        
        // 3. Atualiza a câmara para seguir a entidade do jogador
        PlayerControlComponent* playerControl = &entityManager.playerControlComponents[player];
        camera.position = entityManager.transformComponents[player].position;

        // Ajusta a altura da câmara para uma posição de "olhos"
        camera.position.y += 1.8f; 
        
        // Calcula o alvo da câmara usando os ângulos de yaw e pitch
        Matrix rotation = MatrixMultiply(MatrixRotateX(playerControl->cameraPitch), MatrixRotateY(playerControl->cameraYaw));
        Vector3 forward = Vector3Transform((Vector3){ 0, 0, 1 }, rotation);
        camera.target = Vector3Add(camera.position, forward);


        // --- FASE DE DESENHO ---
        BeginDrawing();
            ClearBackground(DARKGRAY);
            BeginMode3D(camera);
                // 4. O RenderSystem desenha as entidades visíveis.
                RenderSystem_Draw(&entityManager);
                DrawGrid(40, 1.0f);
            EndMode3D();
            
            // --- UI de Debug ---
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

    // --- 6. LIMPEZA ---
    UnloadModel(playerModel);
    UnloadModel(enemyModel);
    
    CloseWindow();

    return 0;
}

