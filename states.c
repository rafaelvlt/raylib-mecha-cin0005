#include "states.h"
#include "raylib.h"

// Definição da variável global de estado (declarada em game_states.h)
GameScreen currentScreen = TITLE; // Começa no menu principal

// --- Variáveis Específicas para o Menu Principal (TITLE) ---
static int framesCounter = 0;
static int finishScreen = 0; // 0-Nenhuma, 1-Pronto, 2-Sair do jogo

// --- Variáveis Específicas para o Jogo (GAMEPLAY) ---
// (Por enquanto, não precisamos de variáveis específicas aqui para o exemplo simples)

// --- Variáveis Específicas para a Tela Final (ENDING) ---
// (Por enquanto, não precisamos de variáveis específicas aqui para o exemplo simples)

//----------------------------------------------------------------------------------
// Funções da tela de TÍTULO/MENU
//----------------------------------------------------------------------------------
void InitTitleScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    // Carregar texturas, fontes, sons específicos do menu se houver
}

void UpdateTitleScreen(void)
{
    framesCounter++;

    // Lógica para transição de estados no menu
    if (IsKeyPressed(KEY_ENTER))
    {
        currentScreen = GAMEPLAY; // Mudar para o estado de jogo
        finishScreen = 1; // Indica que a tela do título terminou sua tarefa
    }
    if (IsKeyPressed(KEY_ESCAPE))
    {
        currentScreen = ENDING; // Mudar para uma tela final ou sair, dependendo do design
        finishScreen = 2; // Indica que a tela do título terminou e o usuário quer sair
    }
}

void DrawTitleScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), LIGHTGRAY);
    DrawText("MENU PRINCIPAL", GetScreenWidth()/2 - MeasureText("MENU PRINCIPAL", 40)/2, GetScreenHeight()/4, 40, DARKBLUE);
    DrawText("Pressione ENTER para JOGAR", GetScreenWidth()/2 - MeasureText("Pressione ENTER para JOGAR", 20)/2, GetScreenHeight()/2, 20, BLACK);
    DrawText("Pressione ESC para SAIR", GetScreenWidth()/2 - MeasureText("Pressione ESC para SAIR", 20)/2, GetScreenHeight()/2 + 30, 20, BLACK);
    DrawText(TextFormat("Contador: %04i", framesCounter/60), GetScreenWidth()/2 - 100, GetScreenHeight() - 30, 20, MAROON);
}

void UnloadTitleScreen(void)
{
    // Descarregar texturas, fontes, sons específicos do menu se houver
}

//----------------------------------------------------------------------------------
// Funções da tela de GAMEPLAY (seu jogo 3D atual)
//----------------------------------------------------------------------------------
void InitGameplayScreen(void)
{
    // Se precisar de alguma inicialização específica ao entrar no jogo (fora do main)
    // Por exemplo, resetar a posição do jogador
}

void UpdateGameplayScreen(void)
{
    // A lógica de atualização do seu jogo 3D será chamada a partir daqui
    // (Por enquanto, esta função estará vazia e o main.c cuidará da lógica)

    // Exemplo de transição para a tela final
    if (IsKeyPressed(KEY_E)) // Pressione 'E' para ir para a tela final
    {
        currentScreen = ENDING;
    }
}

void DrawGameplayScreen(void)
{
    // A lógica de desenho do seu jogo 3D será chamada a partir daqui
    // (Por enquanto, esta função estará vazia e o main.c cuidará da lógica)
}

void UnloadGameplayScreen(void)
{
    // Descarregar recursos específicos do jogo se houver (fora do main)
}

//----------------------------------------------------------------------------------
// Funções da tela FINAL (ENDING)
//----------------------------------------------------------------------------------
void InitEndingScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    // Carregar texturas, fontes, sons específicos da tela final
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