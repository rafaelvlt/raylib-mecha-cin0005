#include <raylib.h>
#include <raymath.h>
#include "ecs/entitymanager.h"
#include "ecs/systems.h"
#include "ecs/types.h"
#include "event_manager.h"
#include "resource_manager.h"
#include "systems.h"
#include <stdio.h>

// Definições visuais para o HUD
#define BAR_WIDTH 250.0f
#define BAR_HEIGHT 20.0f
#define MARGIN 25.0f

// Protótipo local para a função de desenho da barra
static void DrawBar(float x, float y, float current, float max, Color fillColor, Color outlineColor, const char* label, const char* unit);

/**
 * @brief Sistema de Desenho do HUD para Barras de Status. 
 * Itera sobre as entidades e desenha as informações de HP e Calor para o jogador.
 * @param systems Ponteiro para a estrutura Systems global.
 */
void DrawHUDSystem(struct Systems* systems) {
    EntityManager* em = &systems->entityManager;
    uint32_t neededMask = COMPONENT_PLAYER_CONTROL | COMPONENT_COCKPIT_HUD | COMPONENT_HEALTH;

    // Apenas precisamos de um jogador (Entidade com PlayerControl)
    for (Entity e = 0; e < em->numEntities; e++) {
        if ((em->componentMasks[e] & neededMask) == neededMask) {
            
            CockpitHUDComponent* hud = &em->cockpitHUDComponents[e];
            HealthComponent* hp = &em->healthComponents[e];

            // Posições baseadas no canto superior esquerdo (HP) e superior direito (Heat)
            float screenW = (float)GetScreenWidth();
            float x_left = MARGIN;
            float y_top = MARGIN;

            // 1. Desenhar a Barra de Vida (Health) - Topo Esquerdo
            DrawBar(x_left, y_top, hp->currentHealth, hp->maxHealth, LIME, DARKGRAY, "HP", "%%");

            // 2. Desenhar a Barra de Calor (Heat) - Topo Direito
            float x_right = screenW - BAR_WIDTH - MARGIN;
            DrawBar(x_right, y_top, hud->currentHeat, hud->maxHeat, ORANGE, DARKGRAY, "CALOR", "%%");
            
            break; // Apenas um jogador é esperado
        }
    }
}

/**
 * @brief Desenha uma barra de progresso genérica.
 * @param x Coordenada X da barra.
 * @param y Coordenada Y da barra.
 * @param current Valor atual.
 * @param max Valor máximo.
 * @param fillColor Cor de preenchimento.
 * @param outlineColor Cor da borda.
 * @param label Rótulo do valor (e.g., "HP").
 * @param unit Unidade para o valor percentual (e.g., "%%").
 */
static void DrawBar(float x, float y, float current, float max, Color fillColor, Color outlineColor, const char* label, const char* unit) {
    if (max <= 0.0f) max = 1.0f; // Evita divisão por zero
    float percentage = current / max;
    float fillWidth = BAR_WIDTH * percentage;

    // 1. Fundo da barra (Barra completa)
    DrawRectangle(x, y, BAR_WIDTH, BAR_HEIGHT, outlineColor);

    // 2. Preenchimento da barra
    DrawRectangle(x, y, fillWidth, BAR_HEIGHT, fillColor);

    // 3. Borda (para destacar)
    DrawRectangleLinesEx((Rectangle){ x, y, BAR_WIDTH, BAR_HEIGHT }, 2, outlineColor);

    // 4. Texto do rótulo e valor
    char text[64];
    float value = percentage * 100.0f;
    // Formatando o texto: "HP 75%"
    snprintf(text, 64, "%s %.0f%s", label, value, unit);

    // Centraliza o texto na barra
    int fontSize = 16;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, x + (BAR_WIDTH / 2) - (textWidth / 2), y + (BAR_HEIGHT / 2) - (fontSize / 2), fontSize, RAYWHITE);
}

// -------------------------------------------------------------
// IMPLEMENTAÇÃO DO MINIMAPA
// -------------------------------------------------------------

/**
 * @brief Sistema de Desenho do Minimapa.
 * Desenha o radar/minimapa no canto superior direito, abaixo da barra de CALOR.
 * Esta função deve ser chamada pela DrawFirstLevelScreen.
 * @param systems Ponteiro para a estrutura Systems global.
 * @param data Ponteiro para os dados da fase (necessário para a câmera e minimapa).
 */
void DrawMinimapSystem(struct Systems* systems, FirstLevelData* data)
{
    EntityManager* em = &systems->entityManager;
    Entity player = MAX_ENTITIES;

    // 1. Encontrar a Entidade do Jogador
    for (Entity i = 0; i < em->numEntities; i++) {
        if ((em->componentMasks[i] & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL) {
            player = i;
            break;
        }
    }

    if (player == MAX_ENTITIES) return;

    // ---------------------------------------------------------
    // 2. Desenhar Minimapa (Canto Superior Direito, Abaixo da Barra de Heat)
    // ---------------------------------------------------------
    const int mapSize = 150;
    const int mapPadding = MARGIN;
    
    // O minimapa cobre uma área de 50x50 no mundo (-25 a +25)
    // Ajustado para 50.0f para um bom campo de visão
    const float mapWorldSize = 50.0f; 
    
    // Posição X da barra de CALOR para centralizar o minimapa
    int screenW = GetScreenWidth();
    int barX = (int)(screenW - BAR_WIDTH - MARGIN);

    // Posição Y, abaixo da barra de CALOR + MARGIN
    int mapX = barX + (BAR_WIDTH / 2) - (mapSize / 2); // Centralizado sob a barra
    int mapY = (int)(MARGIN + BAR_HEIGHT + MARGIN); 
    int mapCenterX = mapX + mapSize / 2;
    int mapCenterY = mapY + mapSize / 2;

    // Fundo do Minimapa
    DrawRectangle(mapX, mapY, mapSize, mapSize, Fade(DARKGRAY, 0.7f));
    DrawRectangleLines(mapX, mapY, mapSize, mapSize, RAYWHITE);
    
    // Rótulo
    DrawText("RADAR", mapX + mapSize/2 - MeasureText("RADAR", 14)/2, mapY + 5, 14, RAYWHITE);

    // Fator de escala: (tamanho do mapa / tamanho do mundo)
    float scaleFactor = (float)mapSize / mapWorldSize;

    // Iterar sobre as Entidades para desenhar no Minimapa
    uint32_t mapMask = COMPONENT_TRANSFORM;
    
    for (Entity e = 0; e < em->numEntities; e++) {
        // Apenas desenha entidades que têm TransformComponent
        if ((em->componentMasks[e] & mapMask) == mapMask) {
            TransformComponent* trans = &em->transformComponents[e];
            Vector3 worldPos = trans->position;
            
            // Projeta a posição 3D (X, Z) para a posição 2D do minimapa
            // X (mundo) -> X (minimapa), Z (mundo) -> Y (minimapa)
            int relativeX = (int)(worldPos.x * scaleFactor);
            int relativeY = (int)(worldPos.z * scaleFactor);

            int drawX = mapCenterX + relativeX;
            int drawY = mapCenterY + relativeY;

            // Ponto para desenhar
            int dotSize = 4;
            Color dotColor = GRAY;

            // Desenha o Player (Transformado em Círculo Verde)
            if (e == player) {
                dotColor = GREEN;
                dotSize = 6;
                
                // Desenha o círculo se estiver dentro do mapa
                if (drawX >= mapX && drawX <= mapX + mapSize && 
                    drawY >= mapY && drawY <= mapY + mapSize) 
                {
                    DrawCircle(drawX, drawY, dotSize, dotColor);
                    
                    // Linha de Visão Simples (indicando a frente)
                    // Pega a direção da câmera no plano XZ
                    Vector3 forward = Vector3Normalize(Vector3Subtract(data->camera.target, data->camera.position));
                    
                    Vector2 center = (Vector2){ (float)drawX, (float)drawY };
                    // Projeta a direção no minimapa (X e Z) e estende a linha
                    Vector2 lineEnd = (Vector2){ drawX + forward.x * (float)dotSize * 3, drawY + forward.z * (float)dotSize * 3 };
                    
                    DrawLineV(center, lineEnd, YELLOW);
                }

            } 
            // Desenha o Inimigo (Quadrado Vermelho)
            else if (em->componentMasks[e] & COMPONENT_AI_CONTROL) {
                dotColor = RED;
                dotSize = 5;
                
                // Certifique-se de que o ponto esteja dentro dos limites do minimapa
                if (drawX >= mapX && drawX <= mapX + mapSize && 
                    drawY >= mapY && drawY <= mapY + mapSize) 
                {
                    DrawRectangle(drawX - dotSize/2, drawY - dotSize/2, dotSize, dotSize, dotColor);
                }
            }
            // Desenha outros objetos (Círculo Cinza)
            else {
                 if (drawX >= mapX && drawX <= mapX + mapSize && 
                    drawY >= mapY && drawY <= mapY + mapSize) 
                {
                    DrawCircle(drawX, drawY, dotSize, dotColor);
                }
            }
        }
    }
    
    // Ocluir partes que saem da borda
    DrawRectangleLines(mapX, mapY, mapSize, mapSize, RAYWHITE);
}