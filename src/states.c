#include "states.h"
#include "raylib.h"

// Definição da variável global de estado (declarada em game_states.h)
GameScreen currentScreen = TITLE;

// Variáveis do MENU
static int framesCounter = 0;
static int finishScreen = 0; // 0-Nenhuma, 1-Pronto, 2-Sair do jogo


// Funções do MENU
void InitTitleScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
}

void UpdateTitleScreen(void)
{
    framesCounter++;

    // Lógica para transição de estados no menu
    if (IsKeyPressed(KEY_ENTER))
    {
        currentScreen = GAMEPLAY;
        finishScreen = 1; 
    }
    if (IsKeyPressed(KEY_E))
    {
        currentScreen = ENDING;
    }
    if (IsKeyPressed(KEY_ESCAPE))
    {
        currentScreen = ENDING;
        finishScreen = 2;
    }
}

void DrawTitleScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), LIGHTGRAY);
    DrawText("MENU PRINCIPAL", GetScreenWidth()/2 - MeasureText("MENU PRINCIPAL", 40)/2, GetScreenHeight()/4, 40, DARKBLUE);
    DrawText("Pressione ENTER para JOGAR", GetScreenWidth()/2 - MeasureText("Pressione ENTER para JOGAR", 20)/2, GetScreenHeight()/2, 20, BLACK);
    DrawText("Pressione E para os CRÉDITOS", GetScreenWidth()/2 - MeasureText("Pressione E para os CRÉDITOS", 20)/2, GetScreenHeight()/2 + 30, 20, BLACK);
    DrawText("Pressione ESC para SAIR", GetScreenWidth()/2 - MeasureText("Pressione ESC para SAIR", 20)/2, GetScreenHeight()/2 + 60, 20, BLACK);
    DrawText(TextFormat("Contador: %04i", framesCounter/60), GetScreenWidth()/2 - 100, GetScreenHeight() - 30, 20, MAROON);
}

void UnloadTitleScreen(void)
{
    
}


// Funções da tela de GAMEPLAY
void InitGameplayScreen(void)
{

}

void UpdateGameplayScreen(void)
{
    
}

void DrawGameplayScreen(void)
{

}

void UnloadGameplayScreen(void)
{

}

// Funções de Tela Final
void InitEndingScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
}

void UpdateEndingScreen(void)
{
    framesCounter++;
    if (IsKeyPressed(KEY_ENTER))
    {
        currentScreen = TITLE; // Voltar para o menu
        finishScreen = 1;
    }
    if (IsKeyPressed(KEY_ESCAPE))
    {
        finishScreen = 2; // Sair do jogo
    }
}

void DrawEndingScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    DrawText("TELA FINAL", GetScreenWidth()/2 - MeasureText("TELA FINAL", 40)/2, GetScreenHeight()/4, 40, WHITE);
    DrawText("Pressione ENTER para voltar ao MENU", GetScreenWidth()/2 - MeasureText("Pressione ENTER para voltar ao MENU", 20)/2, GetScreenHeight()/2, 20, GRAY);
    DrawText("Pressione ESC para SAIR", GetScreenWidth()/2 - MeasureText("Pressione ESC para SAIR", 20)/2, GetScreenHeight()/2 + 30, 20, GRAY);
}

void UnloadEndingScreen(void)
{
    // Descarregar recursos específicos da tela final
}
