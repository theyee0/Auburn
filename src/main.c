#include "raylib.h"
#include "movement.h"
#include "fluids.h"
#include <stdlib.h>
#include <string.h>

void get_fluid_model_input(struct fluid_model *fluid, float virtual_ratio);
void add_random_variations(struct fluid_model *fluid);

int main(void)
{
        const int screen_width = 1024;
        const int screen_height = 1024;
        const int virtual_screen_width = 256;
        const int virtual_screen_height = 256;

        const float virtual_ratio = (float)screen_width / (float)virtual_screen_width;

        InitWindow(screen_width, screen_height, "Auburn");
        SetTargetFPS(60);

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

        struct fluid_model *fluid = NULL;
        struct fluid_model *delta = NULL;

        world_camera.zoom = 1.0f;
        screen_camera.zoom = 1.0f;

        init_model(&fluid, virtual_screen_width, virtual_screen_height, 0.0f, 0.0f);
        init_model(&delta, virtual_screen_width, virtual_screen_height, 0.0f, 0.0f);

        zero_buffer(fluid, fluid->density);
        zero_buffer(fluid, fluid->velocity_x);
        zero_buffer(fluid, fluid->velocity_y);

        struct leaf leaves[128];
        int nleaves = 0;

        init_leaf(&leaves[0], 120, 120);

        init_leaves(leaves, &nleaves, 128, virtual_screen_width, virtual_screen_height);

        while (!WindowShouldClose()) {
                get_fluid_model_input(delta, virtual_ratio);
                add_random_variations(delta);

                if (IsKeyPressed(KEY_R)) {
                        zero_buffer(fluid, fluid->density);
                        checker_buffer(fluid, fluid->density);
                }

                step_velocity(&fluid, &delta, 1 / 24.0);
                step_density(&fluid, &delta, 1 / 24.0);

                maintain_leaves(leaves, &nleaves, virtual_screen_width, virtual_screen_height);

                simulate_leaves(leaves, nleaves, fluid, 1.0 / 24.0);

                BeginTextureMode(target);
                        ClearBackground(SKYBLUE);
                        BeginMode2D(world_camera);
                        if (!IsKeyUp(KEY_D)) {
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
                        } else {
                                draw_leaves(leaves, nleaves);
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
        Vector2 d = GetMouseDelta();
        Vector2 p = GetMousePosition();
        int samples = 10;
        int x, y;

        zero_buffer(fluid, fluid->density);
        zero_buffer(fluid, fluid->velocity_x);
        zero_buffer(fluid, fluid->velocity_y);

        for (int i = 0; i < samples; i++) {
                x = (p.x - i * d.x / samples) / virtual_ratio + 1;
                y = (p.y - i * d.y / samples) / virtual_ratio + 1;

                if (x < 1) {
                        x = 1;
                } else if (x > fluid->width) {
                        x = fluid->width;
                }

                if (y < 1) {
                        y = 1;
                } else if (y > fluid->height) {
                        y = fluid->height;
                }

                fluid->velocity_x[x][y] = d.x / virtual_ratio * 2;
                fluid->velocity_y[x][y] = d.y / virtual_ratio * 2;
        }
}

void assign_radius(struct fluid_model *fluid, float **arr, int x, int y, int r, float v) {
        for (int i = -r; i <= r; i++) {
                for (int j = -r; j <= r; j++) {
                        if (1 <= (x + i) && (x + i) <= fluid->width &&
                            1 <= (y + i) && (y + i) <= fluid->height) {
                                arr[x + i][y + i] = v;
                        }
                }
        }
}

void add_random_variations(struct fluid_model *fluid) {
        int num = 30;
        int radius = 50;
        int x, y;
        int a = 10;

        for (int i = 0; i < num; i++) {
                x = rand() % fluid->width + 1;
                y = rand() % fluid->height + 1;

                assign_radius(fluid, fluid->velocity_x, x, y, radius, rand() % (2 * a + 1) - a);

                x = rand() % fluid->width + 1;
                y = rand() % fluid->height + 1;

                assign_radius(fluid, fluid->velocity_y, x, y, radius, rand() % (2 * a + 1) - a);
        }
}
