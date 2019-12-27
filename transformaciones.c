#include <GL/glut.h>
#include "definitions.h"
#include "camera.h"
#include "load_obj.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

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
extern int coordenada;
extern int camera_object_mode;

extern objetos_luz global_lights[];
extern GLint _selected_light;


transformaciones *up_values;
transformaciones *down_values;
transformaciones *right_values;
transformaciones *left_values;
transformaciones *avpag_values;
transformaciones *repag_values;
transformaciones *plus_values;
transformaciones *minus_values;

void set_transformation_values(){

    up_values = (transformaciones *)malloc(sizeof(transformaciones));
    down_values = (transformaciones *)malloc(sizeof(transformaciones));
    right_values = (transformaciones *)malloc(sizeof(transformaciones));
    left_values = (transformaciones *)malloc(sizeof(transformaciones));
    avpag_values = (transformaciones *)malloc(sizeof(transformaciones));
    repag_values = (transformaciones *)malloc(sizeof(transformaciones));
    plus_values = (transformaciones *)malloc(sizeof(transformaciones));
    minus_values = (transformaciones *)malloc(sizeof(transformaciones));

    up_values->translate = (vector3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
    up_values->rotate = (vector3){.x = 1.0f, .y = 0.0f, .z = 0.0f};
    up_values->scale = (vector3){.x = 1.0f, .y = 2.0f, .z = 1.0f};

    down_values->translate = (vector3){.x = 0.0f, .y = -1.0f, .z = 0.0f};
    down_values->rotate = (vector3){.x = -1.0f, .y = 0.0f, .z = 0.0f};
    down_values->scale = (vector3){.x = 1.0f, .y = 0.5f, .z = 1.0f};

    right_values->translate = (vector3){.x = 1.0f, .y = 0.0f, .z = 0.0f};
    right_values->rotate = (vector3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
    right_values->scale = (vector3){.x = 2.0f, .y = 1.0f, .z = 1.0f};

    left_values->translate = (vector3){.x = -1.0f, .y = 0.0f, .z = 0.0f};
    left_values->rotate = (vector3){.x = 0.0f, .y = -1.0f, .z = 0.0f};
    left_values->scale = (vector3){.x = 0.5f, .y = 1.0f, .z = 1.0f};

    avpag_values->translate = (vector3){.x = 0.0f, .y = 0.0f, .z = 1.0f};
    avpag_values->rotate = (vector3){.x = 0.0f, .y = 0.0f, .z = 1.0f};
    avpag_values->scale = (vector3){.x = 1.0f, .y = 1.0f, .z = 1.5f};

    repag_values->translate = (vector3){.x = 0.0f, .y = 0.0f, .z = -1.0f};
    repag_values->rotate = (vector3){.x = 0.0f, .y = 0.0f, .z = -1.0f};
    repag_values->scale = (vector3){.x = 1.0f, .y = 1.0f, .z = 0.5f};

    plus_values->scale = (vector3){.x = 1.5f, .y = 1.5f, .z = 1.5f};

    minus_values->scale = (vector3){.x = 0.5f, .y = 0.5f, .z = 0.5f};
}


void transformation_matrix(){

    glMatrixMode(GL_MODELVIEW);
    if (modo_activo == MODO_OBJ){
        if (coordenada == COORD_LOCAL)
            glLoadMatrixf(_selected_object->list_matrix->m);
        else
            glLoadIdentity();
    } else if (modo_activo == MODO_CAMARA) {
        if (coordenada == COORD_LOCAL)
            glLoadMatrixf(_selected_camera->current_camera->m_invert);
        else {
            glLoadIdentity();
            glTranslatef(-_selected_object->list_matrix->m[12], 
            -_selected_object->list_matrix->m[13], 
            -_selected_object->list_matrix->m[14]);

        }
    } else 
        glLoadMatrixf(global_lights[_selected_light].m_obj);
}

void set_transformation_matrix(){

    GLfloat m_aux[16];
    if (modo_activo == MODO_OBJ){
        list_matrix *n_ptr = (list_matrix*)malloc(sizeof(list_matrix));
        if (coordenada == COORD_LOCAL)
            glGetFloatv(GL_MODELVIEW_MATRIX, n_ptr->m);
        else {
            glMultMatrixf(_selected_object->list_matrix->m);
            glGetFloatv(GL_MODELVIEW_MATRIX, n_ptr->m);
        }

        n_ptr->nextptr = _selected_object->list_matrix;
        _selected_object->list_matrix = n_ptr;

        if (camera_object_mode == 1)
            add_camera_mode_obj(_selected_object);
        m_foco();
        } else if (modo_activo == MODO_CAMARA) {
            if (coordenada == COORD_LOCAL){
                glGetFloatv(GL_MODELVIEW_MATRIX, _selected_camera->current_camera->m_invert);
                matriz_inversa(_selected_camera);
            } else {
                glTranslatef(_selected_object->list_matrix->m[12],
                _selected_object->list_matrix->m[13],
                _selected_object->list_matrix->m[14]);
                glMultMatrixf(_selected_camera->current_camera->m_invert);
                glGetFloatv(GL_MODELVIEW_MATRIX, _selected_camera->current_camera->m_invert);
                matriz_inversa(_selected_camera);
            }
        } else 
            glGetFloatv(GL_MODELVIEW_MATRIX, global_lights[_selected_light].m_obj);
}

void aplicar_transformaciones(transformaciones *values){

    transformation_matrix();

    switch (transformacion_activa){
    case TRASLACION:
        glTranslatef(values->translate.x, values->translate.y, values->translate.z);
        break;
    
    case ROTACION:
        glRotatef(18.0f, values->rotate.x, values->rotate.y, values->rotate.z);
        break;

    case ESCALADO:
        if (modo_activo == MODO_OBJ){
            glScalef(values->scale.x, values->scale.y, values->scale.z);
        }
        break; 
    }

    set_transformation_matrix();  
}

GLfloat distancia_camara_objeto(){
    GLfloat px, py, pz;

    px = _selected_object->list_matrix->m[12] - _selected_camera->current_camera->m_invert[12];
    py = _selected_object->list_matrix->m[13] - _selected_camera->current_camera->m_invert[13];
    pz = _selected_object->list_matrix->m[14] - _selected_camera->current_camera->m_invert[14];

    return sqrt(pow(px, 2) + pow(py, 2) + pow(pz, 2));
}
