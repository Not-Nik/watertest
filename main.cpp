// watertest (c) Nikolas Wipper 2023

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT);

    InitWindow(1080, 720, "water test");

    Model waterplane = LoadModelFromMesh(GenMeshPlane(10, 10, 100, 100));
    Shader watershader = LoadShader("vert.glsl", "frag.glsl");
    Texture noise = LoadTextureFromImage(GenImageWhiteNoise(128, 1, 1));

    waterplane.materials[0].shader = watershader;
    waterplane.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = noise;

    Camera cam = Camera{
        .position = Vector3{7, 7, -9}, .target = Vector3{0, 0, 0}, .up = Vector3{
            0, 1, 0
        }, .fovy = 72.f, .projection = CAMERA_PERSPECTIVE
    };

    int time_loc = GetShaderLocation(watershader, "time");
    int light_loc = GetShaderLocation(watershader, "lightDir");
    int cam_loc = GetShaderLocation(watershader, "camPos");

    int wave_count_loc = GetShaderLocation(watershader, "waveCount");
    int wave_peak_loc = GetShaderLocation(watershader, "wavePeak");
    int wave_offs_loc = GetShaderLocation(watershader, "wavePeakOffset");
    int initial_freq_loc = GetShaderLocation(watershader, "initialFrequency");
    int initial_amp_loc = GetShaderLocation(watershader, "initialAmplitude");
    int initial_speed_loc = GetShaderLocation(watershader, "initialSpeed");
    int freq_mult_loc = GetShaderLocation(watershader, "frequencyMult");
    int amp_mult_loc = GetShaderLocation(watershader, "amplitudeMult");
    int speed_mult_loc = GetShaderLocation(watershader, "speedMult");

    Vector3 light_direction = Vector3Normalize(Vector3{2, 1, 0});
    unsigned int wave_count = 16;
    float wave_peak = 1;
    float wave_peak_offset = 1;
    float initial_frequency = 1;
    float initial_amplitude = 1;
    float initial_speed = 2;
    float frequency_mult = 1.18;
    float amplitude_mult = 0.82;
    float speed_mult = 1.07;

    SetShaderValue(watershader, light_loc, &light_direction, SHADER_UNIFORM_VEC3);
    SetShaderValue(watershader, wave_count_loc, &wave_count, SHADER_UNIFORM_INT);
    SetShaderValue(watershader, wave_peak_loc, &wave_peak, SHADER_UNIFORM_FLOAT);
    SetShaderValue(watershader, wave_offs_loc, &wave_peak_offset, SHADER_UNIFORM_FLOAT);
    SetShaderValue(watershader, initial_freq_loc, &initial_frequency, SHADER_UNIFORM_FLOAT);
    SetShaderValue(watershader, initial_amp_loc, &initial_amplitude, SHADER_UNIFORM_FLOAT);
    SetShaderValue(watershader, initial_speed_loc, &initial_speed, SHADER_UNIFORM_FLOAT);
    SetShaderValue(watershader, freq_mult_loc, &frequency_mult, SHADER_UNIFORM_FLOAT);
    SetShaderValue(watershader, amp_mult_loc, &amplitude_mult, SHADER_UNIFORM_FLOAT);
    SetShaderValue(watershader, speed_mult_loc, &speed_mult, SHADER_UNIFORM_FLOAT);

    while (!WindowShouldClose()) {
        UpdateCamera(&cam, CAMERA_ORBITAL);

        auto time = (float) GetTime();
        SetShaderValue(watershader, time_loc, &time, SHADER_UNIFORM_FLOAT);

        SetShaderValue(watershader, cam_loc, &cam.position, SHADER_UNIFORM_VEC3);

        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode3D(cam);

        DrawModel(waterplane, Vector3{0, 0, 0}, 1, BLUE);

        DrawLine3D(Vector3 {0, 1, 0}, Vector3 { 1, 1, 0}, RED);
        DrawLine3D(Vector3 {0, 1, 0}, Vector3 { 0, 2, 0}, GREEN);
        DrawLine3D(Vector3 {0, 1, 0}, Vector3 { 0, 1, 1}, BLUE);

        EndMode3D();

        DrawText(TextFormat("%lf", GetFrameTime()), 10, 10, 35, WHITE);

        EndDrawing();
    }
    return 0;
}
