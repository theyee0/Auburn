#include "raylib.h"
#include "movement.h"
#include <stdlib.h>



int main(void)
{
        int i;

        const int screen_width = 1024;
        const int screen_height = 768;
        const int virtual_screen_width = 320;
        const int virtual_screen_height = 240;

        double time;

        const float virtual_ratio = (float)screen_width / (float)screen_height;

        InitWindow(screen_width, screen_height, "Auburn");

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

        struct leaf *leaves;
        int num_leaves = 1;

        leaves = malloc(1024 * sizeof(*leaves));

        init_leaf(&leaves[0], 80, 0);

        while (!WindowShouldClose()) {
                for (i = 0; i < num_leaves; i++) {
                        leaves[i].acceleration.x = 0;
                        leaves[i].acceleration.y = 0;
                        apply_force(&leaves[i], calc_gravity(leaves[i]));
                        apply_force(&leaves[i], calc_drag(leaves[i]));
                }

                BeginTextureMode(target);
                ClearBackground(RAYWHITE);
                BeginMode2D(world_camera);
                for (i = 0; i < num_leaves; i++) {
                        calc_position(&leaves[i], GetFrameTime());
                        DrawCircle((int)(leaves[i].position.x * PIXELS_PER_METER),
                                   (int)(leaves[i].position.y * PIXELS_PER_METER),
                               leaves[i].size, leaves[i].color);
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
