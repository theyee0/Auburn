#include "fluids.h"
#include <assert.h>

void swap_buffers(struct fluid_model **fluid, struct fluid_model **source) {
        void *tmp;

        tmp = *fluid;
        *fluid = *source;
        *source = tmp;
}

void simulate_boundary(struct fluid_model *fluid, float **density, enum boundary_type type) {
        int M = fluid->width;
        int N = fluid->height;

        /* Compute values for fluid simulation edges (excluding corners) */
        for (int i = 1; i <= M; i++) {
                density[i][0]     = (type == VERTICAL)   ? -density[i][1] : density[i][1];
                density[i][N + 1] = (type == VERTICAL)   ? -density[i][N] : density[i][N];
        }

        for (int i = 1; i <= N; i++) {
                density[0][i]     = (type == HORIZONTAL) ? -density[1][i] : density[1][i];
                density[M + 1][i] = (type == HORIZONTAL) ? -density[M][i] : density[M][i];
        }

        /* Interpolate corner values from nearest edge values */
        density[0][0]         = 0.5 * (density[1][0]     + density[0][1]);
        density[M + 1][0]     = 0.5 * (density[M][0]     + density[M + 1][1]);
        density[0][N + 1]     = 0.5 * (density[1][N + 1] + density[0][N]);
        density[M + 1][N + 1] = 0.5 * (density[M][N + 1] + density[M + 1][N]);
}

void add_source(struct fluid_model *fluid, float **density,
                struct fluid_model *disturbance, float **disturbance_density,
                float dt) {
        /* Add all density values cell-wise */
        for (int i = 0; i < fluid->width + 2; i++) {
                for (int j = 0; j < fluid->height + 2; j++) {
                        density[i][j] += disturbance_density[i][j] * dt;
                }
        }
}

void diffuse(struct fluid_model *fluid,
             enum boundary_type type,
             float **density,
             struct fluid_model *source, float **source_density,
             float dt) {
        /* Diffusion rate constant for `dt` timescale */
        int M = source->width;
        int N = source->height;
        float **d = density;
        float **s = source_density;
        float a = dt * source->diffusion_rate * M * N;
        const int iterations = 20;

        /* Approximate solution to system with Gauss-Seidel Relaxation */
        for (int i = 0; i < iterations; i++) {
                for (int j = 1; j <= M; j++) {
                        for (int k = 1; k <= N; k++) {
                                d[j][k] = (s[j][k] + a * (d[j - 1][k] +
                                                          d[j + 1][k] +
                                                          d[j][k - 1] +
                                                          d[j][k + 1])) / (1 + 4 * a);
                        }
                }

                simulate_boundary(fluid, d, type);
        }
}

static float force_boundary(float *x, float min, float max) {
        if (*x < min) {
                *x = min;
        } else if (*x > max) {
                *x = max;
        }

        return *x;
}

void advect(struct fluid_model *fluid,
            enum boundary_type type,
            float **density,
            struct fluid_model *source, float **source_density,
            float **velocity_x, float **velocity_y,
            float dt) {
        float **vx = velocity_x;
        float **vy = velocity_y;
        float **d = density;
        float **s = source_density;

        int M = source->width;
        int N = source->height;

        int origin_x0, origin_y0, origin_x1, origin_y1;
        float origin_x, origin_y, weight_x, weight_y;

        float step = N * dt;

        for (int x = 1; x <= M; x++) {
                for (int y = 1; y <= N; y++) {
                        origin_x = x - vx[x][y] * step;
                        origin_y = y - vy[x][y] * step;

                        force_boundary(&origin_x, 0.5, M + 0.5); /* Force point into window */
                        force_boundary(&origin_y, 0.5, N + 0.5); /* Force point into window */

                        origin_x0 = (int)origin_x; /* Get floor for integer */
                        origin_x1 = origin_x0 + 1; /* Get ceil */

                        origin_y0 = (int)origin_y; /* Get floor for integer */
                        origin_y1 = origin_y0 + 1; /* Get ceil */

                        weight_x = 1 - (origin_x - origin_x0);
                        weight_y = 1 - (origin_y - origin_y0);

                        d[x][y] = weight_x     * (weight_y       * s[origin_x0][origin_y0] +
                                                  (1 - weight_y) * s[origin_x0][origin_y1]) +
                                (1 - weight_x) * (weight_y       * s[origin_x1][origin_y0] +
                                                  (1 - weight_y) * s[origin_x1][origin_y1]);
                }
        }

        simulate_boundary(fluid, d, type);
}

void step_density(struct fluid_model **fluid, struct fluid_model **source,
                  float dt) {
        add_source(*fluid, (*fluid)->density, *source, (*source)->density, dt);
        swap_buffers(fluid, source);

        diffuse(*fluid, NEITHER, (*fluid)->density, *source, (*source)->density, dt);
        swap_buffers(fluid, source);

        advect(*fluid, NEITHER, (*fluid)->density, *source, (*source)->density,
               (*fluid)->velocity_x, (*fluid)->velocity_y, dt);
}

void step_velocity(struct fluid_model **fluid, struct fluid_model **source, float dt) {
        /* Add effect of external forces */
        add_source(*fluid, (*fluid)->velocity_x, *source, (*source)->velocity_x, dt);
        add_source(*fluid, (*fluid)->velocity_y, *source, (*source)->velocity_y, dt);

        swap_buffers(fluid, source);

        /* Bleed velocity into surrounding gas */
        diffuse(*fluid, HORIZONTAL, (*fluid)->velocity_x, *source, (*source)->velocity_x, dt);
        diffuse(*fluid, VERTICAL, (*fluid)->velocity_y, *source, (*source)->velocity_y, dt);

        restore_mass(*fluid, *source);

        swap_buffers(fluid, source);

        /* Perform self-advection */

        advect(*fluid, HORIZONTAL, (*fluid)->velocity_x, *source, (*source)->velocity_x,
               (*source)->velocity_x, (*source)->velocity_y, dt);
        advect(*fluid, VERTICAL, (*fluid)->velocity_y, *source, (*source)->velocity_y,
               (*source)->velocity_x, (*source)->velocity_y, dt);
        

        restore_mass(*fluid, *source);
}

void restore_mass(struct fluid_model *fluid, struct fluid_model *source) {
        float **vx = fluid->velocity_x;
        float **vy = fluid->velocity_y;

        float **svx = source->velocity_x;
        float **svy = source->velocity_y;

        int M = source->width;
        int N = source->height;
        int iterations = 20;

        for (int i = 1; i <= M; i++) {
                for (int j = 1; j <= N; j++) {
                        svy[i][j] = -0.5 * (vx[i + 1][j] - vx[i - 1][j] +
                                            vy[i][j + 1] - vy[i][j - 1]) / M;
                        svx[i][j] = 0;
                }
        }

        simulate_boundary(source, svx, NEITHER);
        simulate_boundary(source, svy, NEITHER);

        for (int i = 0; i < iterations; i++) {
                for (int j = 1; j <= M; j++) {
                        for (int k = 1; k <= N; k++) {
                                svx[j][k] = (svy[j][k] +
                                             svx[j - 1][k] + svx[j + 1][k] +
                                             svx[j][k - 1] + svx[j][k + 1]) / 4;
                        }
                }

                simulate_boundary(source, svx, NEITHER);
        }

        for (int i = 1; i <= M; i++) {
                for (int j = 1; j <= N; j++) {
                        vx[i][j] -= 0.5 * (svx[i + 1][j] - svx[i - 1][j]) * M;
                        vy[i][j] -= 0.5 * (svx[i][j + 1] - svx[i][j - 1]) * M;
                }
        }

        simulate_boundary(fluid, vx, HORIZONTAL);
        simulate_boundary(fluid, vy, VERTICAL);
}
