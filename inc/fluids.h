#ifndef __FLUIDS_H
#define __FLUIDS_H

#include <stdlib.h>
#include <string.h>

/* Implementation adapted from the paper
   https://graphics.cs.cmu.edu/nsp/course/15-464/Fall09/papers/StamFluidforGames.pdf */

struct fluid_model {
        int width;
        int height;
        float diffusion_rate;
        float viscosity;
        float **density;
        float **velocity_x;
        float **velocity_y;
};

enum boundary_type {
        NEITHER,
        HORIZONTAL,
        VERTICAL
};

void init_model(struct fluid_model **fluid, int width, int height, float diffusion_rate, float viscosity);
void zero_buffer(struct fluid_model *fluid, float **buffer);

void swap_buffers(struct fluid_model **fluid, struct fluid_model **source);
void simulate_boundary(struct fluid_model *fluid, float **density, enum boundary_type type);
void add_source(struct fluid_model *fluid, float **density,
                struct fluid_model *disturbance, float **disturbance_density,
                float dt);
void diffuse(struct fluid_model *fluid,
             enum boundary_type type,
             float **density,
             struct fluid_model *source, float **source_density,
             float dt);
void advect(struct fluid_model *fluid,
            enum boundary_type type,
            float **density,
            struct fluid_model *source, float **source_density,
            float **velocity_x, float **velocity_y,
            float dt);

void step_density(struct fluid_model **fluid, struct fluid_model **source,
                  float dt);
void step_velocity(struct fluid_model **fluid, struct fluid_model **source, float dt);
void restore_mass(struct fluid_model *fluid, struct fluid_model *source);

#endif
