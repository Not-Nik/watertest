// watertest (c) Nikolas Wipper 2023

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

RenderTexture2D LoadRenderTextureWithDepthTexture(int width, int height) {
    RenderTexture2D target = {0};

    target.id = rlLoadFramebuffer(width, height);   // Load an empty framebuffer

    if (target.id > 0) {
        rlEnableFramebuffer(target.id);

        // Create color texture (default to RGBA)
        target.texture.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
        target.texture.width = width;
        target.texture.height = height;
        target.texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        target.texture.mipmaps = 1;

        // Create depth texture
        target.depth.id = rlLoadTextureDepth(width, height, false);
        target.depth.width = width;
        target.depth.height = height;
        target.depth.format = 19;       //DEPTH_COMPONENT_24BIT?
        target.depth.mipmaps = 1;

        // Attach color texture and depth texture to FBO
        rlFramebufferAttach(target.id, target.texture.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
        rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

        // Check if fbo is complete with attachments (valid)
        if (rlFramebufferComplete(target.id))
            TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);

        rlDisableFramebuffer();
    } else
        TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");

    return target;
}

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);

    InitWindow(1080, 720, "water test");

    Model waterplane = LoadModelFromMesh(GenMeshPlane(50, 150, 100, 100));

    float vertices[] = {
        -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,
    };

    Mesh mesh = Mesh{0};
    mesh.vertices = vertices;
    mesh.vertexCount = sizeof(vertices) / sizeof(float) / 3;
    mesh.triangleCount = mesh.vertexCount / 3;

    UploadMesh(&mesh, false);

    Model skyboxbox = LoadModelFromMesh(mesh);

    Shader watershader = LoadShader("water.vert.glsl", "water.frag.glsl");
    Shader skyboxshader = LoadShader("skybox.vert.glsl", "skybox.frag.glsl");
    Shader postshader = LoadShader(nullptr, "post.frag.glsl");

    Texture cubemap = LoadTextureCubemap(LoadImage("skybox.png"), CUBEMAP_LAYOUT_AUTO_DETECT);
    //Texture cubemap = LoadTextureCubemap(GenImageColor(1, 6, SKYBLUE), CUBEMAP_LAYOUT_LINE_VERTICAL);

    waterplane.materials[0].shader = watershader;

    skyboxshader.locs[SHADER_LOC_MAP_CUBEMAP] = GetShaderLocation(skyboxshader, "skybox");

    skyboxbox.materials[0].shader = skyboxshader;
    skyboxbox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = cubemap;

    auto cam = Camera{
        .position = Vector3{-28, 5, 0}, .target = Vector3{-25, 5, 0}, .up = Vector3{
            0, 1, 0
        }, .fovy = 72.f, .projection = CAMERA_PERSPECTIVE
    };

    int time_loc = GetShaderLocation(watershader, "time");
    int water_light_loc = GetShaderLocation(watershader, "lightDir");
    int water_cam_loc = GetShaderLocation(watershader, "camPos");

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
    unsigned int wave_count = 32;
    float wave_peak = 2;
    float wave_peak_offset = 2.5;
    float initial_frequency = 1;
    float initial_amplitude = 1;
    float initial_speed = 2;
    float frequency_mult = 1.18;
    float amplitude_mult = 0.82;
    float speed_mult = 1.07;

    SetShaderValue(watershader, water_light_loc, &light_direction, SHADER_UNIFORM_VEC3);
    SetShaderValue(watershader, wave_count_loc, &wave_count, SHADER_UNIFORM_INT);
    SetShaderValue(watershader, wave_peak_loc, &wave_peak, SHADER_UNIFORM_FLOAT);
    SetShaderValue(watershader, wave_offs_loc, &wave_peak_offset, SHADER_UNIFORM_FLOAT);
    SetShaderValue(watershader, initial_freq_loc, &initial_frequency, SHADER_UNIFORM_FLOAT);
    SetShaderValue(watershader, initial_amp_loc, &initial_amplitude, SHADER_UNIFORM_FLOAT);
    SetShaderValue(watershader, initial_speed_loc, &initial_speed, SHADER_UNIFORM_FLOAT);
    SetShaderValue(watershader, freq_mult_loc, &frequency_mult, SHADER_UNIFORM_FLOAT);
    SetShaderValue(watershader, amp_mult_loc, &amplitude_mult, SHADER_UNIFORM_FLOAT);
    SetShaderValue(watershader, speed_mult_loc, &speed_mult, SHADER_UNIFORM_FLOAT);

    RenderTexture target = LoadRenderTextureWithDepthTexture(1080, 720);
    Model screenQuad = LoadModelFromMesh(GenMeshPlane(1, 1, 1, 1));

    screenQuad.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = target.texture;
    screenQuad.materials[0].maps[MATERIAL_MAP_HEIGHT].texture = target.depth;
    screenQuad.materials[0].shader = postshader;

    postshader.locs[SHADER_LOC_MAP_ALBEDO] = GetShaderLocation(postshader, "color");
    postshader.locs[SHADER_LOC_MAP_HEIGHT] = GetShaderLocation(postshader, "depth");
    int post_cam_loc = GetShaderLocation(postshader, "camPos");
    int post_light_loc = GetShaderLocation(postshader, "lightDir");
    int inv_view_loc = GetShaderLocation(postshader, "invView");
    int inv_proj_loc = GetShaderLocation(postshader, "invProj");

    SetShaderValue(postshader, post_light_loc, &light_direction, SHADER_UNIFORM_VEC3);

    while (!WindowShouldClose()) {
        auto time = (float) GetTime();
        SetShaderValue(watershader, time_loc, &time, SHADER_UNIFORM_FLOAT);

        SetShaderValue(watershader, water_cam_loc, &cam.position, SHADER_UNIFORM_VEC3);
        SetShaderValue(postshader, post_cam_loc, &cam.position, SHADER_UNIFORM_VEC3);

        if (IsWindowResized()) {
            UnloadRenderTexture(target);
            target = LoadRenderTextureWithDepthTexture(GetScreenWidth(), GetScreenHeight());
            screenQuad.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = target.texture;
            screenQuad.materials[0].maps[MATERIAL_MAP_HEIGHT].texture = target.depth;
        }

        BeginDrawing();

        {
            BeginTextureMode(target);
            {

                ClearBackground(BLACK);

                BeginMode3D(cam);
                {

                    rlDisableDepthMask();

                    DrawModel(skyboxbox, Vector3{-53, 6.75, 0}, 1, WHITE);

                    rlEnableDepthMask();

                    // Draw water
                    DrawModel(waterplane, Vector3{0, 0, 0}, 1, BLUE);
                }
                EndMode3D();
            }
            EndTextureMode();

            ClearBackground(BLACK);

            rlClearScreenBuffers();

            // Inlined BeginMode3D
            rlDrawRenderBatchActive();

            rlMatrixMode(RL_PROJECTION);
            rlPushMatrix();
            rlLoadIdentity();

            rlOrtho(-0.5, 0.5, -0.5, 0.5, RL_CULL_DISTANCE_NEAR, RL_CULL_DISTANCE_FAR);

            rlMatrixMode(RL_MODELVIEW);
            rlLoadIdentity();

            Matrix matView = MatrixLookAt((Vector3) {0, 1, 0}, (Vector3) {0}, (Vector3) {0, 0, 1});
            rlMultMatrixf(MatrixToFloat(matView));

            double top = 0.01 * tan(cam.fovy * 0.5 * DEG2RAD);
            double right = top * ((float) GetScreenWidth() / (float) GetScreenHeight());
            Matrix projection = MatrixFrustum(-right, right, -top, top, 0.01, 1000.0);

            SetShaderValueMatrix(postshader, inv_view_loc, MatrixInvert(GetCameraMatrix(cam)));
            SetShaderValueMatrix(postshader, inv_proj_loc, MatrixInvert(projection));

            DrawModel(screenQuad, (Vector3) {0, 0, 0}, 1, WHITE);

            EndMode3D();

            DrawText(TextFormat("%lf", GetFrameTime()), 10, 10, 35, WHITE);
        }
        EndDrawing();
    }
    return 0;
}
