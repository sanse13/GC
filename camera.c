#include "definitions.h"
#include "GL/gl.h"
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

projection *global_perspective, *global_ortho;

extern lista_camera *_first_camera;
extern lista_camera *_selected_camera;

void set_camera_projection(){
    global_perspective = (projection*)malloc(sizeof(projection));
    global_ortho = (projection*)malloc(sizeof(projection));

    global_perspective->type = PROJECTION_PERSP;
    global_ortho->type = PROJECTION_ORT;

    global_perspective->left = -0.1;
    global_perspective->right = 0.1;
    global_perspective->top = 0.1;
    global_perspective->bottom = -0.1;
    global_perspective->near = 0.1;
    global_perspective->far = 100.0;
    
    global_ortho->left = -3.0;
    global_ortho->right = 3.0;
    global_ortho->top = 3.0;
    global_ortho->bottom = -3.0;
    global_ortho->near = 0.0;
    global_ortho->far = 100.0;
}

lista_camera* create_camera(vector3 camera_pos, vector3 camera_front, vector3 camera_up){
    camera *cam = (camera*)malloc(sizeof(camera));
    GLfloat *inv = (GLfloat*)malloc(sizeof(GLfloat)*16);
    lista_camera *lista_aux = (lista_camera*)malloc(sizeof(lista_camera));

    lista_aux->next = 0;
    lista_aux->current_camera = cam;
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera_pos.x, camera_pos.y, camera_pos.z, 
    camera_front.x, camera_front.y, camera_front.z,
    camera_up.x, camera_up.y, camera_up.z);
    
    glGetFloatv(GL_MODELVIEW_MATRIX, lista_aux->current_camera->m);
    
    lista_aux->current_camera->m_invert[0] = lista_aux->current_camera->m[0];
    lista_aux->current_camera->m_invert[1] = lista_aux->current_camera->m[4];
    lista_aux->current_camera->m_invert[2] = lista_aux->current_camera->m[8];
    lista_aux->current_camera->m_invert[3] = 0;
    lista_aux->current_camera->m_invert[4] = lista_aux->current_camera->m[1];
    lista_aux->current_camera->m_invert[5] = lista_aux->current_camera->m[5];
    lista_aux->current_camera->m_invert[6] = lista_aux->current_camera->m[9];
    lista_aux->current_camera->m_invert[7] = 0;
    lista_aux->current_camera->m_invert[8] = lista_aux->current_camera->m[2];
    lista_aux->current_camera->m_invert[9] = lista_aux->current_camera->m[6];
    lista_aux->current_camera->m_invert[10] = lista_aux->current_camera->m[10];
    lista_aux->current_camera->m_invert[11] = 0;
    lista_aux->current_camera->m_invert[12] = camera_pos.x;
    lista_aux->current_camera->m_invert[13] = camera_pos.y;
    lista_aux->current_camera->m_invert[14] = camera_pos.z;
    lista_aux->current_camera->m_invert[15] = 1;

    return lista_aux;

}   

void add_camera_list(lista_camera *l_camera){
    _selected_camera->next = l_camera;
    _selected_camera = l_camera;
}

void default_cameras(){

    set_camera_projection();
    lista_camera *lista_aux = (lista_camera*)malloc(sizeof(lista_camera));

    vector3 cam_pos; cam_pos.x = 5.0f; cam_pos.y = 5.0f; cam_pos.z = -3.0f;
    vector3 cam_front; cam_front.x = 0.0f; cam_front.y = 0.0f; cam_front.z = 0.0f;
    vector3 cam_up; cam_up.x = 0.0f; cam_up.y = 1.0f; cam_up.z = 0.0f;

    lista_aux = create_camera(cam_pos, cam_front, cam_up);
    
    lista_aux->current_camera->proj = global_perspective;

    _first_camera = (lista_camera*)malloc(sizeof(lista_camera));
    _first_camera = lista_aux;
    
    _first_camera->next = 0;

    _selected_camera = _first_camera;
}

void cambiar_camara(){
    if (_selected_camera != 0)
        _selected_camera = _selected_camera->next;
    if (_selected_camera == 0)
        _selected_camera = _first_camera;
}

void add_camera_input(){
    vector3 pos, front, up;
    lista_camera *lista_aux = (lista_camera*)malloc(sizeof(lista_camera));

    printf("Inserte las coordenadas x y z, en ese formato y orden.\n");
    printf("Inserte la posicion:\n");
    scanf("%lf %lf %lf", &pos.x, &pos.y, &pos.z);
    printf("Inserte front:\n");
    scanf("%lf %lf %lf", &front.x, &front.y, &front.z);
    up.x = 0; up.y = 1; up.z = 0;

    lista_aux = create_camera(pos, front, up);

    add_camera_list(lista_aux);

    lista_aux->current_camera->proj = global_perspective;


}


void matriz_inversa(lista_camera *c){
    c->current_camera->m[0] = c->current_camera->m_invert[0];
    c->current_camera->m[4] = c->current_camera->m_invert[1];
    c->current_camera->m[8] = c->current_camera->m_invert[2];
    c->current_camera->m[12] = -(c->current_camera->m_invert[12] * c->current_camera->m_invert[0] +
    c->current_camera->m_invert[13] * c->current_camera->m_invert[1] +
    c->current_camera->m_invert[14] * c->current_camera->m_invert[2]);

    c->current_camera->m[1] = c->current_camera->m_invert[4];
    c->current_camera->m[5] = c->current_camera->m_invert[5];
    c->current_camera->m[9] = c->current_camera->m_invert[6];
    c->current_camera->m[13] = -(c->current_camera->m_invert[12] * c->current_camera->m_invert[4] +
    c->current_camera->m_invert[13] * c->current_camera->m_invert[5] +
    c->current_camera->m_invert[14] * c->current_camera->m_invert[6]);

    c->current_camera->m[2] = c->current_camera->m_invert[8];
    c->current_camera->m[6] = c->current_camera->m_invert[9];
    c->current_camera->m[10] = c->current_camera->m_invert[10];
    c->current_camera->m[14] = -(c->current_camera->m_invert[12] * c->current_camera->m_invert[8] +
    c->current_camera->m_invert[13] * c->current_camera->m_invert[9] +
    c->current_camera->m_invert[14] * c->current_camera->m_invert[10]);

    c->current_camera->m[3] = 0;
    c->current_camera->m[7] = 0;
    c->current_camera->m[11] = 0;
    c->current_camera->m[15] = 1;
}

void add_camera_mode_obj(object3d *obj)
{
    _selected_camera->current_camera->m_invert[0] = obj->list_matrix->m[0];
    _selected_camera->current_camera->m_invert[1] = obj->list_matrix->m[4];
    _selected_camera->current_camera->m_invert[2] = -obj->list_matrix->m[8];
    _selected_camera->current_camera->m_invert[3] = 0;
    _selected_camera->current_camera->m_invert[4] = obj->list_matrix->m[1];
    _selected_camera->current_camera->m_invert[5] = obj->list_matrix->m[5];
    _selected_camera->current_camera->m_invert[6] = -obj->list_matrix->m[9];
    _selected_camera->current_camera->m_invert[7] = 0;
    _selected_camera->current_camera->m_invert[8] = obj->list_matrix->m[2];
    _selected_camera->current_camera->m_invert[9] = obj->list_matrix->m[6];
    _selected_camera->current_camera->m_invert[10] = -obj->list_matrix->m[10];
    _selected_camera->current_camera->m_invert[11] = 0;
    _selected_camera->current_camera->m_invert[12] = obj->list_matrix->m[12];
    _selected_camera->current_camera->m_invert[13] = obj->list_matrix->m[13];
    _selected_camera->current_camera->m_invert[14] = obj->list_matrix->m[14];
    _selected_camera->current_camera->m_invert[15] = 1;

    matriz_inversa(_selected_camera);
}



void centre_camera_to_obj(object3d *obj){
    lista_camera *aux_list = (lista_camera*)malloc(sizeof(lista_camera));

    aux_list = create_camera(
        (vector3) { .x = _selected_camera->current_camera->m_invert[12], .y = _selected_camera->current_camera->m_invert[13], .z = _selected_camera->current_camera->m_invert[14] }, 
        (vector3) { .x = obj->list_matrix->m[12], .y = obj->list_matrix->m[13], .z = obj->list_matrix->m[14] },
        (vector3) { .x = 0, .y = 1, .z = 0 }
    );

    aux_list->next = _selected_camera->next;
    aux_list->current_camera->proj = _selected_camera->current_camera->proj;
    _selected_camera = aux_list;
}



