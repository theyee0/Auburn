#include "raylib.h"
#include "movement.h"
#include "fluids.h"
#include <stdlib.h>
#include <string.h>

void get_fluid_model_input(struct fluid_model *fluid, float virtual_ratio);

int main(void)
{
        const int screen_width = 1024;
        const int screen_height = 1024;
        const int virtual_screen_width = 256;
        const int virtual_screen_height = 256;

        const float virtual_ratio = (float)screen_width / (float)virtual_screen_width;

        InitWindow(screen_width, screen_height, "Auburn");
        SetTargetFPS(24);

        RenderTexture2D target = LoadRenderTexture(virtual_screen_width, virtual_screen_height);

        Rectangle sourceRec = {
                0.0f,
                0.0f,
                (float)target.texture.width,
                -(float)target.texture.height
        };

        Rectangle destRec = {
                -virtual_ratio,
                -virtual_ratio,
                screen_width + (virtual_ratio*2),
                screen_height + (virtual_ratio*2)
        };

        Camera2D world_camera = {0};
        Camera2D screen_camera = {0};

        Vector2 origin = {0.0f, 0.0f};
        
        world_camera.zoom = 1.0f;
        screen_camera.zoom = 1.0f;

        struct fluid_model *fluid = NULL;
        struct fluid_model *delta = NULL;

        init_model(&fluid, virtual_screen_width, virtual_screen_height, 0.0f, 0.0f);
        init_model(&delta, virtual_screen_width, virtual_screen_height, 0.0f, 0.0f);

        zero_buffer(fluid, fluid->density);
        zero_buffer(fluid, fluid->velocity_x);
        zero_buffer(fluid, fluid->velocity_y);

        for (int i = 1; i <= fluid->height; i++) {
                for (int j = 1; j <= fluid->width / 2; j++) {
                        fluid->density[i][j] = 1;
                }
        }

        while (!WindowShouldClose()) {
                get_fluid_model_input(delta, virtual_ratio);

                step_velocity(&fluid, &delta, GetFrameTime());
                step_density(&fluid, &delta, GetFrameTime());

                BeginTextureMode(target);
                ClearBackground(RAYWHITE);
                BeginMode2D(world_camera);

                for (int i = 1; i <= virtual_screen_width; i++) {
                        for (int j = 1; j <= virtual_screen_width; j++) {
                                if (fluid->density[i][j] > 1) {
                                        DrawPixel(i, j, BLACK);
                                } else if (fluid->density[i][j] > 0.75) {
                                        DrawPixel(i, j, DARKGRAY);
                                } else if (fluid->density[i][j] > 0.5) {
                                        DrawPixel(i, j, GRAY);
                                } else if (fluid->density[i][j] > 0.25) {
                                        DrawPixel(i, j, LIGHTGRAY);
                                }
                        }
                }

                EndMode2D();
                EndTextureMode();

                BeginDrawing();
                ClearBackground(RAYWHITE);
                BeginMode2D(screen_camera);
                DrawTexturePro(target.texture, sourceRec, destRec, origin, 0.0f, WHITE);
                EndMode2D();
                EndDrawing();
        }

        UnloadRenderTexture(target);
        CloseWindow();

        return 0;
}

void get_fluid_model_input(struct fluid_model *fluid, float virtual_ratio) {
        Vector2 d;
        Vector2 p = GetMousePosition();

        p.x /= virtual_ratio;
        p.y /= virtual_ratio;

        zero_buffer(fluid, fluid->density);
        zero_buffer(fluid, fluid->velocity_x);
        zero_buffer(fluid, fluid->velocity_y);

        d = GetMouseDelta();
        fluid->velocity_x[(int)p.x][(int)p.y] = d.x / GetFrameTime();
        fluid->velocity_y[(int)p.x][(int)p.y] = d.y / GetFrameTime();

        fluid->density[(int)p.x][(int)p.y] = 0;
}
