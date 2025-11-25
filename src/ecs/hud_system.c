#include <raylib.h>
#include <raymath.h>
#include <rlgl.h> 
#include "event_manager.h"
#include "resource_manager.h"
#include "systems.h"
#include <stdio.h>

// Definições visuais para o HUD
#define BAR_WIDTH 400.0f
#define BAR_HEIGHT 25.0f
#define MARGIN_TOP 15.0f

// Protótipo local
static void DrawBar(float x, float y, float current, float max, Color fillColor, Color outlineColor, const char* label, const char* unit);

void DrawHUDSystem(struct Systems* systems) {
    EntityManager* em = &systems->entityManager;
    uint32_t neededMask = COMPONENT_PLAYER_CONTROL | COMPONENT_HEALTH;

    for (Entity e = 0; e < em->numEntities; e++) {
        if ((em->componentMasks[e] & neededMask) == neededMask) {
            HealthComponent* hp = &em->healthComponents[e];
            float screenW = (float)GetScreenWidth();
            float x_center = (screenW - BAR_WIDTH) / 2.0f;
            float y_pos = MARGIN_TOP;

            Color outlineColor = (Color){ 0, 255, 255, 255 };
            Color fillColor = (Color){ 0, 100, 200, 200 };
            
            if (hp->currentHealth < hp->maxHealth * 0.3f) {
                fillColor = (Color){ 200, 0, 0, 200 };
                outlineColor = RED;
            }

            DrawBar(x_center, y_pos, hp->currentHealth, hp->maxHealth, fillColor, outlineColor, "HP", "%");
            break; 
        }
    }
}




static void DrawBar(float x, float y, float current, float max, Color fillColor, Color outlineColor, const char* label, const char* unit) {
    if (max <= 0.0f) max = 1.0f;
    float percentage = current / max;
    if (percentage < 0.0f) percentage = 0.0f;
    if (percentage > 1.0f) percentage = 1.0f;

    float fillWidth = BAR_WIDTH * percentage;
    const float cornerSize = 12.0f;
    const float thickness = 2.0f;
    
    DrawRectangle(x, y, BAR_WIDTH, BAR_HEIGHT, Fade(BLACK, 0.6f));
    DrawRectangle(x, y, fillWidth, BAR_HEIGHT, fillColor);

    DrawLineEx((Vector2){x + cornerSize, y}, (Vector2){x + BAR_WIDTH - cornerSize, y}, thickness, outlineColor);
    DrawLineEx((Vector2){x + cornerSize, y + BAR_HEIGHT}, (Vector2){x + BAR_WIDTH - cornerSize, y + BAR_HEIGHT}, thickness, outlineColor);
    
    DrawLineEx((Vector2){x, y + cornerSize}, (Vector2){x + cornerSize, y}, thickness, outlineColor);
    DrawLineEx((Vector2){x, y + BAR_HEIGHT - cornerSize}, (Vector2){x + cornerSize, y + BAR_HEIGHT}, thickness, outlineColor);
    DrawLineEx((Vector2){x, y + cornerSize}, (Vector2){x, y + BAR_HEIGHT - cornerSize}, thickness, outlineColor);

    DrawLineEx((Vector2){x + BAR_WIDTH - cornerSize, y}, (Vector2){x + BAR_WIDTH, y + cornerSize}, thickness, outlineColor);
    DrawLineEx((Vector2){x + BAR_WIDTH - cornerSize, y + BAR_HEIGHT}, (Vector2){x + BAR_WIDTH, y + BAR_HEIGHT - cornerSize}, thickness, outlineColor);
    DrawLineEx((Vector2){x + BAR_WIDTH, y + cornerSize}, (Vector2){x + BAR_WIDTH, y + BAR_HEIGHT - cornerSize}, thickness, outlineColor);

    char text[64];
    float value = percentage * 100.0f;
    snprintf(text, 64, "%s %.0f%s", label, value, unit);

    int fontSize = 20;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, x + (BAR_WIDTH / 2) - (textWidth / 2) + 1, y + (BAR_HEIGHT / 2) - (fontSize / 2) + 1, fontSize, BLACK);
    DrawText(text, x + (BAR_WIDTH / 2) - (textWidth / 2), y + (BAR_HEIGHT / 2) - (fontSize / 2), fontSize, WHITE);
}




void DrawCrosshair(struct Systems* systems){
  ConfigManager* cfg = &systems->configManager;
  Texture crosshair = *GetTexture(&systems->resourceManager, TEXTURE_ID_CROSSHAIR_SPRITE);

  int screenW = cfg->screenResolution.x;
  int screenH = cfg->screenResolution.y;

  DrawTexture(crosshair, screenW/2 - crosshair.width/2, screenH/2 - crosshair.height/2, WHITE);
}




// -------------------------------------------------------------
// IMPLEMENTAÇÃO DO MINIMAPA (CENTRALIZADO NO JOGADOR)
// -------------------------------------------------------------

void DrawMinimapSystem(struct Systems* systems, FirstLevelData* data)
{
    EntityManager* em = &systems->entityManager;
    Entity player = MAX_ENTITIES;
    Vector3 playerPos = {0};
    float playerYaw = 0.0f; // Para rotacionar o mapa

    // Encontrar a Entidade do Jogador e sua Posição
    for (Entity i = 0; i < em->numEntities; i++) {
        if ((em->componentMasks[i] & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL) {
            player = i;
            if (em->componentMasks[i] & COMPONENT_TRANSFORM) {
                playerPos = em->transformComponents[i].position;
                
                // Pega o ângulo da câmera para rotacionar o mapa
                Vector3 forward = Vector3Subtract(data->camera.target, data->camera.position);
                playerYaw = -atan2f(forward.z, forward.x) - PI/2.0f; // Ajuste de 90 graus
            }
            break;
        }
    }

    if (player == MAX_ENTITIES) return;

    // Configuração do Minimapa
    const int mapSize = 180; // Tamanho visual em pixels do minimapa
    const int mapPadding = 20;
    
    // RAIO DE ABRANGÊNCIA
    // Quanto maior este valor, mais "longe" o radar vê (zoom out).
    // Area abrangida do mundo pelo minimapa
    const float mapWorldSize = 1100.0f; 
    
    int screenW = GetScreenWidth();
    int mapX = screenW - mapSize - mapPadding; 
    int mapY = mapPadding + 10;
    
    int mapCenterX = mapX + mapSize / 2;
    int mapCenterY = mapY + mapSize / 2;

    // Fundo do radar
    DrawCircle(mapCenterX, mapCenterY, mapSize/2, Fade(DARKBLUE, 0.6f));
    DrawCircleLines(mapCenterX, mapCenterY, mapSize/2, Fade(SKYBLUE, 0.8f));
    
    // Linhas de grade (opcional, giram com o jogador ou ficam fixas)
    DrawLine(mapCenterX - mapSize/2, mapCenterY, mapCenterX + mapSize/2, mapCenterY, Fade(SKYBLUE, 0.2f));
    DrawLine(mapCenterX, mapCenterY - mapSize/2, mapCenterX, mapCenterY + mapSize/2, Fade(SKYBLUE, 0.2f));

    DrawText("RADAR", mapX + mapSize/2 - MeasureText("RADAR", 10)/2, mapY + mapSize + 5, 10, SKYBLUE);

    float scaleFactor = (float)mapSize / mapWorldSize;
    uint32_t mapMask = COMPONENT_TRANSFORM;
    
    // Pré-calcula seno e cosseno para a rotação do mapa
    float cosYaw = cosf(playerYaw);
    float sinYaw = sinf(playerYaw);

    for (Entity e = 0; e < em->numEntities; e++) {
        if ((em->componentMasks[e] & mapMask) == mapMask) {
            // Não desenha o próprio jogador no loop (desenhamos fixo no centro depois)
            if (e == player) continue;

            TransformComponent* trans = &em->transformComponents[e];
            Vector3 worldPos = trans->position;
            
            // *** CÁLCULO RELATIVO AO JOGADOR ***
            float dx = worldPos.x - playerPos.x;
            float dz = worldPos.z - playerPos.z;

            // Aplica a rotação (para que "frente" seja sempre cima no minimapa)
            // Se quiser Norte Fixo, remova estas duas linhas e use dx/dz diretamente.
            float rotatedX = dx * cosYaw - dz * sinYaw;
            float rotatedY = dx * sinYaw + dz * cosYaw;

            // Converte para coordenadas de tela do minimapa
            int relativeX = (int)(rotatedX * scaleFactor);
            int relativeY = (int)(rotatedY * scaleFactor); // Inverte Y pois tela é Y-down

            int drawX = mapCenterX + relativeX;
            int drawY = mapCenterY + relativeY;

            // Verifica se está dentro do círculo do radar
            float distFromCenter = sqrtf(powf(drawX - mapCenterX, 2) + powf(drawY - mapCenterY, 2));
            
            if (distFromCenter <= mapSize / 2) {
                if (em->componentMasks[e] & COMPONENT_AI_CONTROL) {
                    // Inimigo (Ponto Vermelho)
                    DrawCircle(drawX, drawY, 4.0f, RED);
                }

            if (worldPos.y == -15.0f && worldPos.z == -500.0f)
            {
                DrawCircle(drawX, drawY, 8.0f, ORANGE);
            }  
            }
        }
    }

    // Desenha o Jogador (Sempre no Centro)
    // Triângulo apontando para cima (já que o mapa gira ao redor dele)
    Vector2 playerScreenPos = { (float)mapCenterX, (float)mapCenterY };
    DrawPoly(playerScreenPos, 3, 6.0f, -90.0f, YELLOW);
}



void Hud3DSystem(struct Systems* systems){
  uint32_t mask = (COMPONENT_PLAYER_CONTROL | COMPONENT_WEAPON_CONTROL);
  EntityManager* em = &systems->entityManager;

  for (int i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & mask) == mask) {

      WeaponControlComponent* wc = &em->weaponControlComponents[i];
      Entity target = wc->lockedTarget;

      // Checks for locked target
      if (target < MAX_ENTITIES && (em->componentMasks[target] & (COMPONENT_TRANSFORM | COMPONENT_COLLISION))) {

        TransformComponent* tTarget = &em->transformComponents[target];
        CollisionComponent* cTarget = &em->collisionComponents[target];

        BoundingBox worldBox;
        worldBox.min = Vector3Add(cTarget->hitbox.min, tTarget->position);
        worldBox.max = Vector3Add(cTarget->hitbox.max, tTarget->position);

        // Calculation for center of mass
        Vector3 center = Vector3Scale(Vector3Add(worldBox.min, worldBox.max), 0.5f);
        Vector3 size = Vector3Subtract(worldBox.max, worldBox.min);

        // Desabilita o teste de profundidade.
        // Tudo desenhado agora vai aparecer "na frente" das paredes.
        rlDisableDepthTest();

        // Draws a marker above the locked target
        Color lockColor = (Color){ 255, 0, 0, 200 }; // Verde transparente
        Vector3 markerPos = (Vector3){ center.x, worldBox.max.y + 2.0f, center.z };
        DrawCylinderWires(markerPos, 1.0f, 0.0f, 2.0f, 4, lockColor);


        rlEnableDepthTest();
      }
    }
  }
}


