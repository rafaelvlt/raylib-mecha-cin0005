#include <raylib.h>
#include <raymath.h> 
#include "systems.h"
#include "ecs/ecs_systems.h"

// --- Configurações do Mecha ---
#define MECH_HEIGHT     6.5f   // Altura da cabine
#define BOB_FREQUENCY   2.0f   // Frequência do balanço
#define BOB_AMPLITUDE   0.2f   // Amplitude do balanço
#define SWAY_SPEED      4.0f   // Suavidade da inclinação
#define MAX_PITCH_RAD   1.3f   // Limite vertical (olhar cima/baixo)
#define MAX_YAW_RAD     1.8f   // Limite horizontal (pescoço)

void PlayerControlSystem(struct Systems* systems) {
  EntityManager* em = &systems->entityManager;
  InputSystem* keys = &systems->configManager.KeyMap;

  for (Entity i = 0; i < em->numEntities; i++) {
    // Filtro de Componentes
    if ((em->componentMasks[i] & (COMPONENT_PLAYER_CONTROL | COMPONENT_PHYSICS | COMPONENT_TRANSFORM)) == 
      (COMPONENT_PLAYER_CONTROL | COMPONENT_PHYSICS | COMPONENT_TRANSFORM)) {

      PlayerControlComponent* p = &em->playerControlComponents[i];
      PhysicsComponent* phys    = &em->physicsComponents[i];
      TransformComponent* trans = &em->transformComponents[i];
      float dt = systems->delta_time;

      // ==================================================================================
      // 1. INPUT: CONTROLE DE MOUSE (CÂMERA/CABEÇA APENAS)
      // ==================================================================================
      
      // Trava/Destrava o mouse com TAB
      if (IsKeyPressed(KEY_TAB)) {
          if (IsCursorHidden()) EnableCursor();
          else DisableCursor();
      }
      if (!IsCursorHidden() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsWindowFocused()) {
          DisableCursor();
      }

      if (IsCursorHidden()) {
          Vector2 mouseDelta = GetMouseDelta();
          // O Mouse afeta apenas as variáveis 'torso', não toca na física do corpo
          p->torsoYaw   -= mouseDelta.x * p->mouseSensitivity;
          p->torsoPitch -= mouseDelta.y * p->mouseSensitivity;

          // Limites do pescoço (Clamp)
          p->torsoPitch = Clamp(p->torsoPitch, -MAX_PITCH_RAD, MAX_PITCH_RAD);
          p->torsoYaw   = Clamp(p->torsoYaw, -MAX_YAW_RAD, MAX_YAW_RAD);
      }

      // ==================================================================================
      // 2. INPUT: CONTROLE DE TECLADO (CORPO APENAS)
      // ==================================================================================
      float targetThrottle = 0.0f;
      float targetTurn = 0.0f;
      
      // W/S movem o robô para onde as PERNAS estão apontando
      if (IsKeyDown(keys->KeyMoveForward))  targetThrottle = 1.0f;
      if (IsKeyDown(keys->KeyMoveBackward)) targetThrottle = -1.0f;
      
      // A/D giram o CHASSI do robô (não a câmera diretamente)
      if (IsKeyDown(keys->KeyTurnLeft))     targetTurn = 1.0f;
      if (IsKeyDown(keys->KeyTurnRight))    targetTurn = -1.0f;

      // Interpolação (Simula peso/inércia do mecha)
      p->throttle = Lerp(p->throttle, targetThrottle, 5.0f * dt);
      p->turnState = Lerp(p->turnState, targetTurn, 5.0f * dt);

      // ==================================================================================
      // 3. FÍSICA DE MOVIMENTO (Aplicada ao Transform Global)
      // ==================================================================================

      // A. Rotação do Corpo (Giro do Chassis)
      if (fabs(p->turnState) > 0.01f) {
        // Rotaciona o transform global no eixo Y (Up)
        Quaternion deltaRot = QuaternionFromAxisAngle((Vector3){0,1,0}, p->turnState * p->turnSpeed * dt);
        trans->orientation = QuaternionMultiply(trans->orientation, deltaRot);
        trans->orientation = QuaternionNormalize(trans->orientation);
      }
      
      // B. Movimento (Velocity)
      // Calcula "Frente" baseada na orientação das PERNAS (trans->orientation)
      // Raylib usa (0,0,-1) como frente padrão
      Vector3 legsForward = Vector3RotateByQuaternion((Vector3){0, 0, -1}, trans->orientation);

      // Aplica velocidade na direção das pernas
      Vector3 desiredVelocity = Vector3Scale(legsForward, p->throttle * p->maxSpeed);
      phys->velocity = Vector3Lerp(phys->velocity, desiredVelocity, 4.0f * dt); 

      // ==================================================================================
      // 4. CÂMERA (Hierarquia: Posição do Corpo -> Rotação Corpo -> Rotação Cabeça)
      // ==================================================================================

      // -- Head Bobbing e Sway (Efeitos de Imersão) --
      if (fabs(p->throttle) > 0.1f) {
        p->headTimer += dt * BOB_FREQUENCY;
        p->walkLerp = Lerp(p->walkLerp, 1.0f, 5.0f * dt);
      } else {
        p->walkLerp = Lerp(p->walkLerp, 0.0f, 5.0f * dt);
        if (p->walkLerp < 0.01f) p->headTimer = 0.0f;
      }

      // Sway: Inclina a câmera levemente baseada na inércia do movimento
      p->lean.x = Lerp(p->lean.x, p->turnState * -0.15f, SWAY_SPEED * dt); 
      p->lean.y = Lerp(p->lean.y, p->throttle * 0.05f, SWAY_SPEED * dt);   

      // -- Cálculo Final da Câmera --
      
      // 1. Posição: Centro do Robô + Altura Fixa + Balanço Vertical
      float bobY = sinf(p->headTimer * PI * 2.0f) * BOB_AMPLITUDE * p->walkLerp;
      p->camera->position = Vector3Add(trans->position, (Vector3){0, MECH_HEIGHT + bobY, 0});

      // 2. Orientação (Matrizes)
      // A ordem de multiplicação é crucial aqui para separar Cabeça de Corpo.
      
      // Matriz do Corpo (Global - Controlada por WASD)
      Matrix matBody = QuaternionToMatrix(trans->orientation);
      
      // Matriz da Cabeça (Local - Controlada por Mouse)
      // Pitch (X) e Yaw (Y) combinados com o Sway (Lean)
      // Criamos duas matrizes separadas para evitar problemas de rotação (Gimbal Lock)
      Matrix matPitch = MatrixRotateX(p->torsoPitch - p->lean.y);
      Matrix matYaw   = MatrixRotateY(p->torsoYaw);
      
      // Cabeça = Yaw * Pitch (Ordem: Gira pescoço, depois inclina cabeça)
      Matrix matHead = MatrixMultiply(matPitch, matYaw);
      
      // Matriz Final = Cabeça * Corpo
      // Significado: A cabeça é "filha" do corpo. Se o corpo gira, a cabeça vai junto.
      Matrix matFinal = MatrixMultiply(matHead, matBody);

      // 3. Alvo (Target)
      // Transforma o vetor "frente" local pela matriz final combinada
      Vector3 viewForward = Vector3Transform((Vector3){0, 0, -1}, matFinal);
      p->camera->target = Vector3Add(p->camera->position, viewForward);

      // 4. Vetor UP (Roll)
      // Adiciona inclinação lateral (Roll) causada pelo balanço
      float bobRoll = cosf(p->headTimer * PI) * 0.02f * p->walkLerp;
      float finalRoll = p->lean.x + bobRoll;
      
      // Usa a matriz do corpo para garantir que o UP da câmera respeite a inclinação do robô
      // Mas adiciona o Roll localmente
      Vector3 upLocal = (Vector3){0, 1, 0};
      upLocal = Vector3Transform(upLocal, MatrixRotateZ(finalRoll)); // Aplica Roll
      
      // Se quiser que a câmera incline se o robô subir rampa, descomente a linha abaixo:
      // upLocal = Vector3Transform(upLocal, matBody); 
      
      p->camera->up = upLocal;
    }
  }
}