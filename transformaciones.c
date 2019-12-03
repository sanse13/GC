#include <GL/glut.h>
#include "definitions.h"
#include "camera.h"
#include "load_obj.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define PI 3.14159265358979323846

#define MODO_OBJ 0
#define MODO_CAMARA 1

#define TRASLACION 0
#define ROTACION 1
#define ESCALADO 2

#define COORD_GLOBAL 0
#define COORD_LOCAL 1 

extern object3d *_first_object;
extern object3d *_selected_object;
extern lista_camera *_camera_list_first;
extern lista_camera *_selected_camera;

extern int modo_activo;
extern int transformacion_activa;
extern int coordenada_activa;
extern int camera_object_mode;


transf_values *obj_up_transf_values;
transf_values *obj_down_transf_values;
transf_values *obj_right_transf_values;
transf_values *obj_left_transf_values;
transf_values *obj_avpag_transf_values;
transf_values *obj_repag_transf_values;
transf_values *obj_plus_transf_values;
transf_values *obj_minus_transf_values;

void init_transf_values(){

    obj_up_transf_values = (transf_values *)malloc(sizeof(transf_values));
    obj_down_transf_values = (transf_values *)malloc(sizeof(transf_values));
    obj_right_transf_values = (transf_values *)malloc(sizeof(transf_values));
    obj_left_transf_values = (transf_values *)malloc(sizeof(transf_values));
    obj_avpag_transf_values = (transf_values *)malloc(sizeof(transf_values));
    obj_repag_transf_values = (transf_values *)malloc(sizeof(transf_values));
    obj_plus_transf_values = (transf_values *)malloc(sizeof(transf_values));
    obj_minus_transf_values = (transf_values *)malloc(sizeof(transf_values));

    obj_up_transf_values->scale_v = (vector3){.x = 1.0f, .y = 2.0f, .z = 1.0f};
    obj_up_transf_values->translate_v = (vector3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
    obj_up_transf_values->rotation_v = (vector3){.x = 1.0f, .y = 0.0f, .z = 0.0f};

    obj_down_transf_values->scale_v = (vector3){.x = 1.0f, .y = 0.5f, .z = 1.0f};
    obj_down_transf_values->translate_v = (vector3){.x = 0.0f, .y = -1.0f, .z = 0.0f};
    obj_down_transf_values->rotation_v = (vector3){.x = -1.0f, .y = 0.0f, .z = 0.0f};

    obj_right_transf_values->scale_v = (vector3){.x = 2.0f, .y = 1.0f, .z = 1.0f};
    obj_right_transf_values->translate_v = (vector3){.x = 1.0f, .y = 0.0f, .z = 0.0f};
    obj_right_transf_values->rotation_v = (vector3){.x = 0.0f, .y = 1.0f, .z = 0.0f};

    obj_left_transf_values->scale_v = (vector3){.x = 0.5f, .y = 1.0f, .z = 1.0f};
    obj_left_transf_values->translate_v = (vector3){.x = -1.0f, .y = 0.0f, .z = 0.0f};
    obj_left_transf_values->rotation_v = (vector3){.x = 0.0f, .y = -1.0f, .z = 0.0f};

    obj_avpag_transf_values->scale_v = (vector3){.x = 1.0f, .y = 1.0f, .z = 1.5f};
    obj_avpag_transf_values->translate_v = (vector3){.x = 0.0f, .y = 0.0f, .z = 1.0f};
    obj_avpag_transf_values->rotation_v = (vector3){.x = 0.0f, .y = 0.0f, .z = 1.0f};

    obj_repag_transf_values->scale_v = (vector3){.x = 1.0f, .y = 1.0f, .z = 0.5f};
    obj_repag_transf_values->translate_v = (vector3){.x = 0.0f, .y = 0.0f, .z = -1.0f};
    obj_repag_transf_values->rotation_v = (vector3){.x = 0.0f, .y = 0.0f, .z = -1.0f};

    obj_plus_transf_values->scale_v = (vector3){.x = 1.5f, .y = 1.5f, .z = 1.5f};

    obj_minus_transf_values->scale_v = (vector3){.x = 0.5f, .y = 0.5f, .z = 0.5f};
}


void transf_matrix_init(){

    glMatrixMode(GL_MODELVIEW);
    if (modo_activo == MODO_OBJ){
        if (coordenada_activa == COORD_LOCAL)
            glLoadMatrixf(_selected_object->list_matrix->m);
        else
            glLoadIdentity();
    } else {
        if (coordenada_activa == COORD_LOCAL)
            glLoadMatrixf(_selected_camera->actual_camera->m_invert);
        else {
            glLoadIdentity();
            glTranslatef(-_selected_object->list_matrix->m[12], 
            -_selected_object->list_matrix->m[13], 
            -_selected_object->list_matrix->m[14]);

        }
    }
}

void transf_matrix_set(){

    GLfloat m_aux[16];
    if (modo_activo == MODO_OBJ){
        list_matrix *n_ptr = (list_matrix*)malloc(sizeof(list_matrix));
        if (coordenada_activa == COORD_LOCAL)
            glGetFloatv(GL_MODELVIEW_MATRIX, n_ptr->m);
        else {
            glMultMatrixf(_selected_object->list_matrix->m);
            glGetFloatv(GL_MODELVIEW_MATRIX, n_ptr->m);
        }

        n_ptr->nextptr = _selected_object->list_matrix;
        _selected_object->list_matrix = n_ptr;

        if (camera_object_mode == 1)
            add_camera_mode_obj(_selected_object);
        } else {
            if (coordenada_activa == COORD_LOCAL){
                glGetFloatv(GL_MODELVIEW_MATRIX, _selected_camera->actual_camera->m_invert);
                set_inv_m(_selected_camera);
            } else {
                glTranslatef(_selected_object->list_matrix->m[12],
                _selected_object->list_matrix->m[13],
                _selected_object->list_matrix->m[14]);
                glMultMatrixf(_selected_camera->actual_camera->m_invert);
                glGetFloatv(GL_MODELVIEW_MATRIX, _selected_camera->actual_camera->m_invert);
                set_inv_m(_selected_camera);
            }
        }
}

void transform(transf_values *values){

    transf_matrix_init();

    switch (transformacion_activa)
    {
    case TRASLACION:
        glTranslatef(values->translate_v.x, values->translate_v.y, values->translate_v.z);
        break;
    
    case ROTACION:
        glRotatef(18.0f, values->rotation_v.x, values->rotation_v.y, values->rotation_v.z);
        break;

    case ESCALADO:
        if (modo_activo == MODO_OBJ){
            glScalef(values->scale_v.x, values->scale_v.y, values->scale_v.z);
        }
        break; 
    }

    transf_matrix_set();   
}

GLfloat distance_camera_to_obj(){
    GLfloat px, py, pz;

    px = _selected_object->list_matrix->m[12] - _selected_camera->actual_camera->m_invert[12];
    py = _selected_object->list_matrix->m[13] - _selected_camera->actual_camera->m_invert[13];
    pz = _selected_object->list_matrix->m[14] - _selected_camera->actual_camera->m_invert[14];

    return sqrt(pow(px, 2) + pow(py, 2) + pow(pz, 2));
}
