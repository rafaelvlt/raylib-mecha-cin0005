#include "raylib.h"
#include "raymath.h"
#include "ecs_entitymanager.h" 
#include "ecs_systems.h"       
#include "states.h"

int main(void) {
    // --- INICIALIZAÇÃO GERAL ---
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "Teste do ECS - Primeira Pessoa");

    Camera camera = { 0 };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // --- MENU -------
    // Definição da máquina de estado do jogo
    GameScreen gameScreen = MENU;

    // Variáveis do MENU
    int framesCounter = 0;
    
    // --- INICIALIZAÇÃO DO ECS ---
    EntityManager entityManager;
    InitEntityManager(&entityManager);

    // ---  CRIAÇÃO DOS RECURSOS (ASSETS) ---
    Mesh playerMesh = GenMeshCube(2.0f, 4.0f, 2.0f); // Malha do jogador
    Model playerModel = LoadModelFromMesh(playerMesh);

    Mesh enemyMesh = GenMeshSphere(1.5f, 16, 16);    // Malha do inimigo
    Model enemyModel = LoadModelFromMesh(enemyMesh);

    // --- CRIAÇÃO DAS ENTIDADES ---
    Entity player = CreateEntity(&entityManager);
    AddTransformComponent(&entityManager, player, (Vector3){ 0.0f, 2.0f, 0.0f });
    AddRenderComponent(&entityManager, player, playerModel, BLUE);
    Entity enemy = CreateEntity(&entityManager);
    AddTransformComponent(&entityManager, enemy, (Vector3){ 15.0f, 1.0f, 10.0f });
    AddRenderComponent(&entityManager, enemy, enemyModel, RED);

    DisableCursor();
    SetTargetFPS(60);

    // --- O LOOP PRINCIPAL DO JOGO ---
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();


        // --- FASE DE DESENHO ---
        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode3D(camera);
                // O RenderSystem desenha as entidades visíveis.
                RenderSystem_Draw(&entityManager);
                DrawGrid(40, 1.0f);
            EndMode3D();
            
            //Debug
            DrawFPS(GetScreenWidth() - 100, 10);

        EndDrawing();
    }

    // ---  LIMPEZA ---
    UnloadModel(playerModel);
    UnloadModel(enemyModel);
    
    CloseWindow();
    return 0;
}

