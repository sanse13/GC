#include "definitions.h"

void default_cameras();
void add_camera_input();
void set_camera_projection();
void cambiar_camara();
void add_camera_obj(object3d *object);
void add_camera_list(lista_camera *l_camera);
void centre_camera_to_obj(object3d *obj);
void add_camera_mode_obj(object3d *obj);
void matriz_inversa(lista_camera *c);