#include "definitions.h"
#ifndef DISPLAY_H
#define DISPLAY_H

void init(void);
void display(void);
void reshape(int width, int height);
vector3 calculate_surface_normal(int index1, int index2, int index3, vertex *vertex_table);
void normal_vectors();

#endif // DISPLAY_H
