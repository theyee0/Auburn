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

void init_leaves(struct leaf *leaves, int *n, int m, int width, int height) {
        int x, y;
        while (*n < m) {
                x = rand() % width;
                y = rand() % height;
                init_leaf(&leaves[*n], x, y);
                (*n)++;
        }
}

void maintain_leaves(struct leaf *leaves, int *n, int width, int height) {
        int x, y;
        for (int i = 0; i < *n; i++) {
                x = (int)(leaves[i].position.x * PIXELS_PER_METER);
                y = (int)(leaves[i].position.y * PIXELS_PER_METER);

                if (x < 0 || width <= x || y < 0 || height <= y) {
                        x = rand() % width;
                        y = rand() % height;
                        init_leaf(&leaves[i], x, y);
                }
        }
}

Vector2 calc_gravity(struct leaf leaf) {
        Vector2 force;

        force.x = 0;
        force.y = leaf.mass * 9.80;

        return force;
}

Vector2 avgvel(struct fluid_model *fluid, int x, int y, int radius) {
        Vector2 sum;
        int num = 0;

        sum.x = 0;
        sum.y = 0;

        for (int i = -radius; i <= radius; i++) {
                for (int j = -radius; j <= radius; j++) {
                        if (0 <= (x + i) && (x + i) < fluid->width && 0 <= (y + j) && (y + j) < fluid->height) {
                                sum.x += fluid->velocity_x[x + i + 1][y + j + 1];
                                sum.y += fluid->velocity_y[x + i + 1][y + j + 1];
                                num++;
                        }
                }
        }

        sum.x /= num;
        sum.y /= num;

        return sum;
}

Vector2 calc_wind(struct leaf leaf, struct fluid_model *fluid) {
        Vector2 force;
        int x, y;
        float dvx, dvy;
        Vector2 avgwind;

        x = leaf.position.x * PIXELS_PER_METER;
        y = leaf.position.y * PIXELS_PER_METER;

        avgwind = avgvel(fluid, x, y, 3);

        force.x = 0;
        force.y = 0;

        dvx = avgwind.x - leaf.velocity.x;
        dvy = avgwind.y - leaf.velocity.y;

        force.x = dvx * dvx * 0.015f * 1.5f * (dvx < 0 ? -1 : 1);
        force.y = dvy * dvy * 0.015f * 1.5f * (dvy < 0 ? -1 : 1);

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

void simulate_leaves(struct leaf *leaves, int n, struct fluid_model *fluid, float dt) {
        for (int i = 0; i < n; i++) {
                leaves[i].acceleration.x = 0;
                leaves[i].acceleration.y = 0;

                apply_force(&leaves[i], calc_gravity(leaves[i]));
                apply_force(&leaves[i], calc_wind(leaves[i], fluid));

                calc_position(&leaves[i], dt);
        }
}

void draw_leaves(struct leaf *leaves, int n) {
        const Color colors[4] = {RED, ORANGE, GOLD, YELLOW};

        for (int i = 0; i < n; i++) {
                if (rand() % 10 == 0) {
                        leaves[i].color = colors[rand() % 4];
                }

                DrawCircle(leaves[i].position.x * PIXELS_PER_METER,
                           leaves[i].position.y * PIXELS_PER_METER,
                           leaves[i].size,
                           leaves[i].color);
        }
}
