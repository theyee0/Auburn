#include "movement.h"

void init_leaf(struct leaf *leaf, float x, float y) {
        leaf->size = 2;
        leaf->mass = 0.001;
        leaf->color = ORANGE;
        leaf->position.x = x / PIXELS_PER_METER;
        leaf->position.y = y / PIXELS_PER_METER;
        leaf->velocity.x = 0;
        leaf->velocity.y = 0;
        leaf->acceleration.x = 0;
        leaf->acceleration.y = 0;
}

Vector2 calc_drag(struct leaf leaf) {
        Vector2 force;

        force.x = -leaf.velocity.x * leaf.velocity.x * 0.015f * 1.5f;
        force.y = -leaf.velocity.y * leaf.velocity.y * 0.015f * 1.5f;

        return force;
}

Vector2 calc_gravity(struct leaf leaf) {
        Vector2 force;

        force.x = 0;
        force.y = leaf.mass * 9.80;

        return force;
}


Vector2 apply_force(struct leaf *leaf, Vector2 force) {
        leaf->acceleration.x += force.x / leaf->mass;
        leaf->acceleration.y += force.y / leaf->mass;

        return leaf->acceleration;
}

Vector2 simulate_forces(struct leaf *leaf, Vector2 *forces, int n) {
        int i;

        for (i = 0; i < n; i++) {
                apply_force(leaf, forces[i]);
        }

        return leaf->acceleration;
}

Vector2 calc_position(struct leaf *leaf, float interval) {
        leaf->position.x += (leaf->velocity.x + leaf->acceleration.x * interval / 2.0f) * interval;
        leaf->position.y += (leaf->velocity.y + leaf->acceleration.y * interval / 2.0f) * interval;

        leaf->velocity.x += leaf->acceleration.x * interval;
        leaf->velocity.y += leaf->acceleration.y * interval;

        return leaf->position;
}
