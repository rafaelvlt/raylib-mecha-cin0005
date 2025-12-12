#include <raylib.h>
#include "systems.h"
#include "screens/screen_death.h"
#include "state_manager.h"
#include <stdio.h>

// Constantes de Tela
#define FADE_SPEED 2.0f
#define GAME_OVER_TEXT_SIZE 80
#define RESTART_TEXT_SIZE 40
#define BUTTON_WIDTH 300
#define BUTTON_HEIGHT 70

// --------------------------------------------------------------------------------------------------
// Inicialização
// --------------------------------------------------------------------------------------------------
void DeathInit(DeathData *data) {
    // Inicializa os textos e o tamanho da fonte
    data->gameOverText = "GAME OVER";
    data->restartText = "REINICIAR";
    data->fontSize = GAME_OVER_TEXT_SIZE;
    
    // Inicializa o estado de hover
    data->isButtonHovered = false;
    
    // Inicializa o alfa para o efeito de fade-in
    data->alpha = 0.0f; 

    // Calcula a posição do botão no centro da tela
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    
    data->restartButtonBounds = (Rectangle){
        (float)screenW / 2 - BUTTON_WIDTH / 2, // X centralizado
        (float)screenH / 2 + 50,              // Y abaixo do texto "GAME OVER"
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    // Configuração inicial
    TraceLog(LOG_INFO, "SCREEN_DEATH: Inicializada.");
}

// --------------------------------------------------------------------------------------------------
// Atualização
// --------------------------------------------------------------------------------------------------
void DeathUpdate(DeathData *data, struct Systems *systems) {
    // Efeito de Fade-in
    if (data->alpha < 1.0f) {
        data->alpha += GetFrameTime() * FADE_SPEED;
        if (data->alpha > 1.0f) data->alpha = 1.0f;
    }

    // Lógica do Botão
    if (data->alpha >= 1.0f) {
        // Verifica se o mouse está sobre o botão
        data->isButtonHovered = CheckCollisionPointRec(GetMousePosition(), data->restartButtonBounds);

        if (data->isButtonHovered) {
            // Verifica o clique do mouse
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Se clicado, retorna a ação de reiniciar
                TraceLog(LOG_INFO, "SCREEN_DEATH: Botão REINICIAR pressionado.");
                RequestScreenChange(systems, SCREEN_SECOND_LEVEL);
            }
        }
        
        // Permitir sair da tela de morte com ESC
        if (IsKeyPressed(KEY_ESCAPE)) {
            TraceLog(LOG_INFO, "SCREEN_DEATH: ESC pressionado. Sair/Voltar ao Menu Principal.");
            RequestScreenChange(systems, SCREEN_MAIN_MENU);
        }
    }
    
}

// --------------------------------------------------------------------------------------------------
// Desenho
// --------------------------------------------------------------------------------------------------
void DrawDeathScreen(const DeathData *data) {
    // Desenha o fundo da tela de morte
    ClearBackground(BLACK);

    Color primaryColor = Fade(RED, data->alpha);
    Color secondaryColor = Fade(YELLOW, data->alpha);
    Color buttonColor = Fade(data->isButtonHovered ? GREEN : DARKGREEN, data->alpha);
    
    int screenW = GetScreenWidth();
    
    // Desenha o texto "GAME OVER"
    int textWidth = MeasureText(data->gameOverText, GAME_OVER_TEXT_SIZE);
    DrawText(data->gameOverText, 
             screenW / 2 - textWidth / 2, 
             GetScreenHeight() / 2 - GAME_OVER_TEXT_SIZE, 
             GAME_OVER_TEXT_SIZE, 
             primaryColor);
             
    // Desenha o Botão de Reiniciar
    Rectangle button = data->restartButtonBounds;
    
    // Desenha o fundo do botão
    DrawRectangleRounded(button, 0.5f, 8, buttonColor);
    DrawRectangleRoundedLines(button, 0.5f, 8, Fade(LIME, data->alpha));

    // Desenha o texto do botão
    int restartTextWidth = MeasureText(data->restartText, RESTART_TEXT_SIZE);
    DrawText(data->restartText, 
             (int)(button.x + button.width / 2 - restartTextWidth / 2), 
             (int)(button.y + button.height / 2 - RESTART_TEXT_SIZE / 2), 
             RESTART_TEXT_SIZE, 
             Fade(WHITE, data->alpha));

    // Desenha uma instrução secundária
    const char *hintText = "Pressione ESC para sair.";
    int hintWidth = MeasureText(hintText, 20);
    DrawText(hintText, 
             screenW / 2 - hintWidth / 2, 
             GetScreenHeight() - 50, 
             20, 
             secondaryColor);
}

// --------------------------------------------------------------------------------------------------
// Descarregamento (Unload)
// --------------------------------------------------------------------------------------------------
void DeathUnload(DeathData *data) {
    // Aqui seria onde você liberaria texturas, sons ou memória alocada dinamicamente.
    // Como estamos usando apenas dados estáticos, não há muito a fazer.
    TraceLog(LOG_INFO, "SCREEN_DEATH: Descarregada.");
}