#ifndef __MOVEMENT_H
#define __MOVEMENT_H

#include "raylib.h"
#include "fluids.h"
#include "stdlib.h"

#define PIXELS_PER_METER 10

struct leaf {
        int size;
        float mass;
        Vector2 position;
        Vector2 velocity;
        Vector2 acceleration;
        Color color;
};

void init_leaf(struct leaf *leaf, float x, float y);

void init_leaves(struct leaf *leaves, int *n, int m, int width, int height);
void maintain_leaves(struct leaf *leaves, int *n, int width, int height);

Vector2 calc_gravity(struct leaf leaf);
Vector2 calc_wind(struct leaf leaf, struct fluid_model *fluid);

Vector2 apply_force(struct leaf *leaf, Vector2 force);
Vector2 simulate_forces(struct leaf *leaf, Vector2 *forces, int n);

Vector2 calc_position(struct leaf *leaf, float interval);

void simulate_leaves(struct leaf *leaves, int n, struct fluid_model *fluid, float dt);
void draw_leaves(struct leaf *leaves, int n);
#endif
