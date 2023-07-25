// watertest (c) Nikolas Wipper 2023

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT);

    InitWindow(1080, 720, "water test");
    SetTargetFPS(30);

    Model waterplane = LoadModelFromMesh(GenMeshPlane(10, 10, 100, 100));
    Shader watershader = LoadShader("vert.glsl", "frag.glsl");

    waterplane.materials[0].shader = watershader;

    Camera cam = Camera{
        .position = Vector3{7, 5, -9}, .target = Vector3{0, 0, 0}, .up = Vector3{
            0, 1, 0
        }, .fovy = 72.f, .projection = CAMERA_PERSPECTIVE
    };

    int time_loc = GetShaderLocation(watershader, "time");
    int light_loc = GetShaderLocation(watershader, "lightDir");
    int cam_loc = GetShaderLocation(watershader, "camPos");

    Vector3 light_direction = Vector3Normalize(Vector3{2, 1, 0});

    SetShaderValue(watershader, light_loc, &light_direction, SHADER_UNIFORM_VEC3);

    while (!WindowShouldClose()) {
        UpdateCamera(&cam, CAMERA_ORBITAL);

        float time = GetTime();
        SetShaderValue(watershader, time_loc, &time, SHADER_UNIFORM_FLOAT);

        SetShaderValue(watershader, cam_loc, &cam.position, SHADER_UNIFORM_VEC3);

        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode3D(cam);

        DrawModel(waterplane, Vector3{0, 0, 0}, 1, BLUE);

        DrawLine3D(Vector3 {0, 1, 0}, Vector3 { 1, 1, 0}, RED);
        DrawLine3D(Vector3 {0, 1, 0}, Vector3 { 0, 1, 1}, GREEN);
        DrawLine3D(Vector3 {0, 1, 0}, Vector3 { 0, 2, 0}, BLUE);

        EndMode3D();

        EndDrawing();
    }
    return 0;
}
