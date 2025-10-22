#ifndef __MOVEMENT_H
#define __MOVEMENT_H

#include "raylib.h"

struct leaf {
        int size;
        float mass;
        Vector2 position;
        Vector2 velocity;
        Vector2 acceleration;
        Color color;
};


void init_leaf(struct leaf *leaf);
Vector2 calc_drag(struct leaf leaf);
Vector2 calc_gravity(struct leaf leaf);
Vector2 apply_force(struct leaf *leaf, Vector2 force);
Vector2 simulate_forces(struct leaf *leaf, Vector2 *forces, int n);
Vector2 calc_position(struct leaf *leaf, float interval);
#endif
