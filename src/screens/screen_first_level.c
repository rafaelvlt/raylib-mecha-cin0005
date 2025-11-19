#include <raylib.h>
#include <raymath.h>
#include "utility.h"

// TODO -> Put these constants in a better location
#define GRAVITY         32.0f
#define MAX_SPEED       20.0f
#define CROUCH_SPEED     5.0f
#define JUMP_FORCE      12.0f
#define MAX_ACCEL      150.0f
// Grounded drag
#define FRICTION         0.86f
// Increasing air drag, increases strafing speed
#define AIR_DRAG         0.98f
// Responsiveness for turning movement direction to looked direction
#define CONTROL         15.0f
#define CROUCH_HEIGHT    0.0f
#define STAND_HEIGHT     1.0f
#define BOTTOM_HEIGHT    0.5f

#define NORMALIZE_INPUT  0


// Local helper functions
static void UpdateCameraPlayer(struct Systems* systems, FirstLevelData* data, Camera* camera);
static void getPlayerInput(struct Systems* systems, FirstLevelData* data);
static void DrawLevel();


void InitFirstLevelScreen(struct Systems* systems, FirstLevelData* data)

{

  data->camera.position = (Vector3){ 0 };
  data->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
  data->camera.fovy = 60.0f;
  data->camera.projection = CAMERA_PERSPECTIVE;


}

void UpdateFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{
  data->mouseDelta = GetMouseDelta();
  systems->delta = GetFrameTime();
  UpdateCameraPlayer(systems,data, &(data->camera)); 

  
  getPlayerInput(systems, data);
}

void DrawFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{
  ClearBackground(RAYWHITE);

  BeginMode3D(data->camera);
    DrawLevel();
  EndMode3D();


}

void DestroyFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{

}

// Initial camera update, will change alot
static void UpdateCameraPlayer(struct Systems* systems, FirstLevelData* data, Camera* camera){
  
  const Vector3 up = (Vector3){ 0.0f, 1.0f, 0.0f };
  const Vector3 targetOffset = (Vector3){ 0.0f, 0.0f, -1.0f };
  static Vector2 sensitivity = { 0.001f, 0.001f };
  //static Body player = { 0 };
  static float headTimer = 0.0f;
  static float walkLerp = 0.0f;
  static float headLerp = STAND_HEIGHT;
  static Vector2 lean = { 0 };

  headLerp = Lerp(headLerp, STAND_HEIGHT, 20.0f*systems->delta);
  data->camera.position = (Vector3){
    0,
    0 + (BOTTOM_HEIGHT + headLerp),
    0,
  };

  if ((data->input.forward != 0))
  {
    headTimer += systems->delta*3.0f;
    walkLerp = Lerp(walkLerp, 1.0f, 10.0f*systems->delta);
    data->camera.fovy = Lerp(data->camera.fovy, 55.0f, 5.0f*systems->delta);
  }
  else
{
    walkLerp = Lerp(walkLerp, 0.0f, 10.0f*systems->delta);
    data->camera.fovy = Lerp(data->camera.fovy, 60.0f, 5.0f*systems->delta);
  }

  //lean.x = Lerp(lean.x, sideway*0.02f, 10.0f*delta);
  lean.y = Lerp(lean.y, data->input.forward*0.015f, 10.0f*systems->delta);


  lean.y = Lerp(lean.y, data->input.forward*0.015f, 10.0f*systems->delta);

  Vector2 lookRotation = {0};
  lookRotation.x -= data->mouseDelta.x*sensitivity.x;
  lookRotation.y += data->mouseDelta.y*sensitivity.y;



  // Left and right
  Vector3 yaw = Vector3RotateByAxisAngle(targetOffset, up, lookRotation.x);

  // Clamp view up
  float maxAngleUp = Vector3Angle(up, yaw);
  maxAngleUp -= 0.001f; // Avoid numerical errors
  if ( -(lookRotation.y) > maxAngleUp) { lookRotation.y = -maxAngleUp; }

  // Clamp view down
  float maxAngleDown = Vector3Angle(Vector3Negate(up), yaw);
  maxAngleDown *= -1.0f; // Downwards angle is negative
  maxAngleDown += 0.001f; // Avoid numerical errors
  if ( -(lookRotation.y) < maxAngleDown) { lookRotation.y = -maxAngleDown; }

  // Up and down
  Vector3 right = Vector3Normalize(Vector3CrossProduct(yaw, up));

  // Rotate view vector around right axis
  float pitchAngle = -lookRotation.y - lean.y;
  pitchAngle = Clamp(pitchAngle, -PI/2 + 0.0001f, PI/2 - 0.0001f); // Clamp angle so it doesn't go past straight up or straight down
  Vector3 pitch = Vector3RotateByAxisAngle(yaw, right, pitchAngle);

  // Head animation
  // Rotate up direction around forward axis
  float headSin = sinf(headTimer*PI);
  float headCos = cosf(headTimer*PI);
  const float stepRotation = 0.01f;
  camera->up = Vector3RotateByAxisAngle(up, pitch, headSin*stepRotation + lean.x);

  // Camera BOB
  const float bobSide = 0.1f;
  const float bobUp = 0.15f;
  Vector3 bobbing = Vector3Scale(right, headSin*bobSide);
  bobbing.y = fabsf(headCos*bobUp);

  camera->position = Vector3Add(camera->position, Vector3Scale(bobbing, walkLerp));
  camera->target = Vector3Add(camera->position, pitch);
} 

static void getPlayerInput(struct Systems* systems, FirstLevelData* data){
  if (IsKeyPressed(KEY_W)){
    data->input.forward = (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
  }  
  if (IsKeyPressed(KEY_ENTER))
  {
    RequestScreenChange(systems, SCREEN_MAIN_MENU);
  }
}

// TEMP FUNCTION MADE FROM 3D FPS EXAMPLE, just for testing purposes
static void DrawLevel(void)
{
    const int floorExtent = 25;
    const float tileSize = 5.0f;
    const Color tileColor1 = (Color){ 150, 200, 200, 255 };

    // Floor tiles
    for (int y = -floorExtent; y < floorExtent; y++)
    {
        for (int x = -floorExtent; x < floorExtent; x++)
        {
            if ((y & 1) && (x & 1))
            {
                DrawPlane((Vector3){ x*tileSize, 0.0f, y*tileSize}, (Vector2){ tileSize, tileSize }, tileColor1);
            }
            else if (!(y & 1) && !(x & 1))
            {
                DrawPlane((Vector3){ x*tileSize, 0.0f, y*tileSize}, (Vector2){ tileSize, tileSize }, LIGHTGRAY);
            }
        }
    }

    const Vector3 towerSize = (Vector3){ 16.0f, 32.0f, 16.0f };
    const Color towerColor = (Color){ 150, 200, 200, 255 };

    Vector3 towerPos = (Vector3){ 16.0f, 16.0f, 16.0f };
    DrawCubeV(towerPos, towerSize, towerColor);
    DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

    towerPos.x *= -1;
    DrawCubeV(towerPos, towerSize, towerColor);
    DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

    towerPos.z *= -1;
    DrawCubeV(towerPos, towerSize, towerColor);
    DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

    towerPos.x *= -1;
    DrawCubeV(towerPos, towerSize, towerColor);
    DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

    // Red sun
    DrawSphere((Vector3){ 300.0f, 300.0f, 0.0f }, 100.0f, (Color){ 255, 0, 0, 255 });
}

