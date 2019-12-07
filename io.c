#include "definitions.h"
#include "transformaciones.h"
#include "load_obj.h"
#include <GL/glut.h>
#include <stdio.h>
#include <stdbool.h>
#include "camera.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846
#define ANGULO 18.0
#define MODO_OBJ 0
#define MODO_CAMARA 1

#define TRASLACION 0
#define ROTACION 1
#define ESCALADO 2

#define COORD_GLOBAL 0
#define COORD_LOCAL 1

//comenzar programa con alguna transformacion activa


extern object3d * _first_object;
extern object3d * _selected_object;

extern lista_camera *_camera_list_first;
extern lista_camera *_selected_camera;

extern projection *global_perspective, *global_ortho;

extern GLdouble _ortho_x_min,_ortho_x_max;
extern GLdouble _ortho_y_min,_ortho_y_max;
extern GLdouble _ortho_z_min,_ortho_z_max;

extern transformaciones *up_values;
extern transformaciones *down_values;
extern transformaciones *right_values;
extern transformaciones *left_values;
extern transformaciones *avpag_values;
extern transformaciones *repag_values;
extern transformaciones *plus_values;
extern transformaciones *minus_values;

int camera_object_mode = 0;
int modo_activo = MODO_OBJ;
int transformacion_activa = TRASLACION;
int coordenada_activa = COORD_GLOBAL;

list_matrix *newptr;
vector3 camera_pos;
vector3 camera_front;
vector3 camera_up;
/**
 * @brief This function just prints information about the use
 * of the keys
 */
void print_help(){
    printf("KbG Irakasgaiaren Praktika. Programa honek 3D objektuak \n");
    printf("aldatzen eta bistaratzen ditu.  \n\n");
    printf("\n\n");
    printf("FUNTZIO NAGUSIAK \n");
    printf("<?>\t\t Laguntza hau bistaratu \n");
    printf("<ESC>\t\t Programatik irten \n");
    printf("<F>\t\t Objektua bat kargatu\n");
    printf("<TAB>\t\t Kargaturiko objektuen artean bat hautatu\n");
    printf("<DEL>\t\t Hautatutako objektua ezabatu\n");
    printf("<CTRL + ->\t Bistaratze-eremua handitu\n");
    printf("<CTRL + +>\t Bistaratze-eremua txikitu\n");
    printf("<M/m>\t\t Activar traslacion\n");
    printf("<B/b>\t\t Activar rotacion\n");
    printf("<T/t>\t\t Activar escalado\n");
    printf("<G/g>\t\t Activar transformaciones en SR mundo\n");
    printf("<L/l>\t\t Activar transformaciones en SR objeto\n");
    printf("<c>\t\t Cambiar de camara.\n");
    printf("<C>\t\t Visualizar lo que ve el objeto seleccionado.\n");
    printf("<K>\t\t Activar modo camara. Transformaciones modifican solo la camara.\n");
    printf("--------------Una vez activada la camara:---------------\n");
    printf("<G/g>\t\t Camara en modo analisis.\n");
    printf("<L/l>\t\t Camara en modo vuelo.\n");
    printf("<T/t>\t\t Cambio de volumen de vision.\n");
    printf("<B/b>\t\t Rotaciones a la camara.\n");
    printf("<M/m>\t\t Traslaciones a la camara.\n");
    printf("<P/p>\t\t Cambio de tipo de proyeccion (perspectiva o paralela).\n");
    printf("¡¡¡TRASLACION ACTIVADA POR DEFECTO!!!\n");
    printf("\n\n");
}

void traslation(GLfloat x, GLfloat y, GLfloat z){
    glLoadMatrixf(_selected_object->list_matrix->m);
    glTranslatef(x, y, z);
    newptr = malloc(sizeof(list_matrix));
    glGetFloatv(GL_MODELVIEW_MATRIX, newptr->m);
    newptr->nextptr = _selected_object->list_matrix;
    _selected_object->list_matrix = newptr;
    glutPostRedisplay();
}

void global_traslation(float x, float y, float z){
    glLoadIdentity();
    glTranslatef(x, y, z);
    glMultMatrixf(_selected_object->list_matrix->m);
    newptr = malloc(sizeof(list_matrix));
    glGetFloatv(GL_MODELVIEW_MATRIX, newptr->m);
    newptr->nextptr = _selected_object->list_matrix;
    _selected_object->list_matrix = newptr;
    glutPostRedisplay();
    
}

void rotation(GLfloat x, GLfloat y, GLfloat z){
    glLoadMatrixf(_selected_object->list_matrix->m);
    glRotatef(ANGULO, x, y, z);
    newptr = malloc(sizeof(list_matrix));
    glGetFloatv(GL_MODELVIEW_MATRIX, newptr->m);
    newptr->nextptr = _selected_object->list_matrix;
    _selected_object->list_matrix = newptr;
    glutPostRedisplay();
}

void global_rotation(GLfloat x, GLfloat y, GLfloat z){
    glLoadIdentity();
    glRotatef(ANGULO, x, y, z);
    glMultMatrixf(_selected_object->list_matrix->m);
    newptr = malloc(sizeof(list_matrix));
    glGetFloatv(GL_MODELVIEW_MATRIX, newptr->m);
    newptr->nextptr = _selected_object->list_matrix;
    _selected_object->list_matrix = newptr;
    glutPostRedisplay();
}

void scalation(float x, float y, float z){
    glLoadMatrixf(_selected_object->list_matrix->m);
    glScalef(x, y, z);
    newptr = malloc(sizeof(list_matrix));
    glGetFloatv(GL_MODELVIEW_MATRIX, newptr->m);
    newptr->nextptr = _selected_object->list_matrix;
    _selected_object->list_matrix = newptr;
    glutPostRedisplay();
}

void global_scalation(float x, float y, float z){
    glLoadIdentity();
    glScalef(x, y, z);
    glMultMatrixf(_selected_object->list_matrix->m);
    newptr = malloc(sizeof(list_matrix));
    glGetFloatv(GL_MODELVIEW_MATRIX, newptr->m);
    newptr->nextptr = _selected_object->list_matrix;
    _selected_object->list_matrix = newptr;
    glutPostRedisplay();
}

void set_identity(GLfloat *m){
    int i, j;
    for (i = 0; i < 4; i++){
        for (j = 0; j < 4; j++){
            m[i*4+j] = 0;
            if (i == j)
                m[i*4+j] = 1;
        }
    }
}

void modo_analisis(int x, int y){
    GLfloat px, py, pz, distance;
    GLfloat m_minus[16], m_plus[16], m_rot[16];

    px = _selected_object->list_matrix->m[12] - _selected_camera->actual_camera->m_invert[12];
    py = _selected_object->list_matrix->m[13] - _selected_camera->actual_camera->m_invert[13];
    pz = _selected_object->list_matrix->m[14] - _selected_camera->actual_camera->m_invert[14];

    distance = sqrt(pow(px, 2) + pow(py, 2) + pow(pz, 2));

    set_identity(m_minus); set_identity(m_plus); set_identity(m_rot);

    m_minus[12] = 0; m_minus[13] = 0; m_minus[14] = -distance;
    m_plus[12] = 0; m_plus[13] = 0; m_plus[14] = distance;

    if (x != 0){
        m_rot[5] = cos(x * KG_STEP_ROTATE * M_PI / 180.0);
        m_rot[6] = sin(x * KG_STEP_ROTATE * M_PI / 180.0);
        m_rot[9] = -1*(sin(x * KG_STEP_ROTATE * M_PI / 180.0));
        m_rot[10] = cos(x * KG_STEP_ROTATE * M_PI / 180.0);
    } else {
        m_rot[0] = cos(y * KG_STEP_ROTATE * M_PI / 180.0);
        m_rot[2] = -1 * (sin(y * KG_STEP_ROTATE * M_PI / 180.0));
        m_rot[8] = sin(y * KG_STEP_ROTATE * M_PI / 180.0);
        m_rot[10] = cos(y * KG_STEP_ROTATE * M_PI / 180.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMultMatrixf(m_minus); glMultMatrixf(m_rot); glMultMatrixf(m_plus);
    glGetFloatv(GL_MODELVIEW_MATRIX, m_rot);

    glLoadIdentity();
    glMultMatrixf(_selected_camera->actual_camera->m_invert);
    glMultMatrixf(m_rot);
    glGetFloatv(GL_MODELVIEW_MATRIX, _selected_camera->actual_camera->m_invert);

    matriz_inversa(_selected_camera);
}



void free_ptr(object3d *object){
    int i;
    for (i = 0; i < object->num_faces; i++)
                free(object->face_table[i].vertex_table);
                

            free(object->face_table);
            free(object->vertex_table);
            free(object);
}


/**
 * @brief Callback function to control the basic keys
 * @param key Key that has been pressed
 * @param x X coordinate of the mouse pointer when the key was pressed
 * @param y Y coordinate of the mouse pointer when the key was pressed
 */
void keyboard(unsigned char key, int x, int y) {

    char* fname = malloc(sizeof (char)*128); /* Note that scanf adds a null character at the end of the vector*/
    int read = 0;
    
    object3d *auxiliar_object = 0;
    GLdouble wd,he,midx,midy;

    list_matrix *aux_list;
    camera *aux_camera = 0;
    lista_camera *aux_camera_obj = 0;


    switch (key) {
    case 'f':
    case 'F':
        if (modo_activo == MODO_OBJ){
            /*Ask for file*/
            printf("%s", KG_MSSG_SELECT_FILE);
            scanf("%s", fname);
            glLoadIdentity();
            /*Allocate memory for the structure and read the file*/
            auxiliar_object = (object3d *) malloc(sizeof (object3d));
            read = read_wavefront(fname, auxiliar_object);
            switch (read) {
            /*Errors in the reading*/
            case 1:
                printf("%s: %s\n", fname, KG_MSSG_FILENOTFOUND);
                break;
            case 2:
                printf("%s: %s\n", fname, KG_MSSG_INVALIDFILE);
                break;
            case 3:
                printf("%s: %s\n", fname, KG_MSSG_EMPTYFILE);
                break;
            /*Read OK*/
            case 0:
                /*Insert the new object in the list*/
                aux_list = (list_matrix *)malloc(sizeof(list_matrix));

                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glGetFloatv(GL_MODELVIEW_MATRIX, aux_list->m);
                aux_list->nextptr = 0;
                auxiliar_object->list_matrix = aux_list;

                auxiliar_object->next = _first_object;
                _first_object = auxiliar_object;
                _selected_object = _first_object;
                printf("%s\n",KG_MSSG_FILEREAD);
                break;
            }
        } else {
            add_camera_input();
        }
        break;

    case 9: /* <TAB> */
        if (_selected_object != 0)
            _selected_object = _selected_object->next;
        /*The selection is circular, thus if we move out of the list we go back to the first element*/
        if (_selected_object == 0) _selected_object = _first_object;

        if (modo_activo == MODO_CAMARA && coordenada_activa == COORD_GLOBAL)
            centre_camera_to_obj(_selected_object);
        break;

    case 127: /* <SUPR> */
        /*Erasing an object depends on whether it is the first one or not*/
        if (_selected_object == 0)
            printf("No existe ningun objeto.\n");
        else {
            if (_selected_object == _first_object)
            {
                /*To remove the first object we just set the first as the current's next*/
                _first_object = _first_object->next;
                /*Once updated the pointer to the first object it is save to free the memory*/
                
                free_ptr(_selected_object);
                /*Finally, set the selected to the new first one*/
                _selected_object = _first_object;
            } else {
                /*In this case we need to get the previous element to the one we want to erase*/
                auxiliar_object = _first_object;
                while (auxiliar_object->next != _selected_object)
                    auxiliar_object = auxiliar_object->next;
                /*Now we bypass the element to erase*/
                auxiliar_object->next = _selected_object->next;
                /*free the memory*/
                    


                free_ptr(_selected_object);
                /*and update the selection*/
                _selected_object = auxiliar_object;
            }
        }
    break;

    case '-':
        global_scalation(0.5f, 0.5f, 0.5f);
    break;

    case '+':
        global_scalation(2.0f, 2.0f, 2.0f);
        
    break;

    case 'm':
    case 'M':
        if (transformacion_activa != TRASLACION){
            transformacion_activa = TRASLACION;
            printf("Activada la traslacion.\n");
        }
    break;

    case 'b':
    case 'B':
        if (transformacion_activa != ROTACION){
            transformacion_activa = ROTACION;
            printf("Activada la rotacion.\n");
        }
    break;

    case 't':
    case 'T':
        if (transformacion_activa != ESCALADO){
            transformacion_activa = ESCALADO;
            printf("Activado el escalado.\n");
        }
    break;

    case 'g':
    case 'G': 
        if (coordenada_activa != COORD_GLOBAL){
            if (modo_activo == MODO_CAMARA) centre_camera_to_obj(_selected_object);
            printf("Activadas transformaciones en mundo.\n");
            coordenada_activa = COORD_GLOBAL;
        }
    break;

    case 'l':
    case 'L': 
        if (coordenada_activa != COORD_LOCAL){
            coordenada_activa = COORD_LOCAL;    
            printf("Activadas transformaciones locales.\n");
        
        }
    break;

    case 'o':
    case 'O':
        if (modo_activo != MODO_OBJ){
            modo_activo = MODO_OBJ;
            printf("Activado modo Objeto\n");
        }
    break;
        
    case 'c': //cambiar de camara
        cambiar_camara();
    break;

    case 'C': //visualizar lo que ve el objeto seleccionado
       if (camera_object_mode == 0){
           camera_object_mode = 1;
           add_camera_mode_obj(_selected_object);
       } else {
           camera_object_mode = 0;
           default_cameras();
       }
    break;

    case 'k':
    case 'K': //activar modo camara, las transformaciones afectan a la camara
        if (modo_activo != MODO_CAMARA){ 
            if (coordenada_activa == COORD_GLOBAL) centre_camera_to_obj(_selected_object);
            printf("Activado modo camara.\n");
            modo_activo = MODO_CAMARA;
        }
        
    break;

    case 'p':
    case 'P': //cambiando el tipo de proyeccion (perspectiva / paralela)
        if (modo_activo == MODO_CAMARA){
            if (_selected_camera->actual_camera->proj->type == PROJECTION_PERSP){
                printf("Cambiado a ortografica.\n");
                _selected_camera->actual_camera->proj = global_ortho;
            } else {
                printf("Cambiado a perspectiva.\n");
                _selected_camera->actual_camera->proj = global_perspective;
            }
        }


    break;

    


    case '?':
        print_help();
        break;

    case 26: //CTRL+Z

        if (_selected_object->list_matrix->nextptr != 0){
            printf("Transformacion deshecha.\n");
            _selected_object->list_matrix = _selected_object->list_matrix->nextptr;
            if (camera_object_mode == 1)
               add_camera_mode_obj (_selected_object);
        }
        break;

    case 27: /* <ESC> */
        exit(0);
        break;

    default:
        /*In the default case we just print the code of the key. This is usefull to define new cases*/
        printf("%d %c\n", key, key);
        break;
    }
    /*In case we have do any modification affecting the displaying of the object, we redraw them*/
    glutPostRedisplay();
}

void SpecialInput(int key, int x, int y){

transformaciones *t_cam;
glMatrixMode(GL_MODELVIEW);
switch(key){

case GLUT_KEY_UP:

    if (modo_activo == MODO_CAMARA){
        switch(transformacion_activa){
            case TRASLACION:
                if (coordenada_activa == COORD_LOCAL)
                    aplicar_transformaciones(up_values);
                break;
            case ROTACION:
                if (coordenada_activa == COORD_GLOBAL){
                    modo_analisis(-1, 0);
                } else {
                    aplicar_transformaciones(up_values);
                }
                break;
            case ESCALADO:
                _selected_camera->actual_camera->proj->top -= 0.01;
                _selected_camera->actual_camera->proj->bottom += 0.01;
                break;
            default:    
                break; 
        }
    } else {
        aplicar_transformaciones(up_values);
    }
break;

case GLUT_KEY_DOWN:
    if (modo_activo == MODO_CAMARA){
        switch (transformacion_activa){
        case TRASLACION:
            if (coordenada_activa == COORD_LOCAL)
                aplicar_transformaciones(down_values);
            break;
        case ROTACION:
            if (coordenada_activa == COORD_GLOBAL)
                modo_analisis(1, 0);
            else
                aplicar_transformaciones(down_values);
            break;
        case ESCALADO:
            _selected_camera->actual_camera->proj->top += 0.01;
            _selected_camera->actual_camera->proj->bottom -= 0.01;
            break;
        }
    } else {
        aplicar_transformaciones(down_values);
    }
    
break;

case GLUT_KEY_LEFT:

    if (modo_activo == MODO_CAMARA){
        switch (transformacion_activa){
        case TRASLACION:
            if (coordenada_activa == COORD_LOCAL)
                aplicar_transformaciones(left_values);
            break;

        case ROTACION:
            if (coordenada_activa == COORD_GLOBAL)
                modo_analisis(0, -1);
            else
                aplicar_transformaciones(left_values);
            break;

        case ESCALADO:
            _selected_camera->actual_camera->proj->left += 0.01;
            _selected_camera->actual_camera->proj->right -= 0.01;
            break;
        }
    } else {
        aplicar_transformaciones(left_values);
    }

break;

case GLUT_KEY_RIGHT:

    if (modo_activo == MODO_CAMARA){
        switch (transformacion_activa){

        case TRASLACION:
            if (coordenada_activa == COORD_LOCAL)
                aplicar_transformaciones(right_values);
            break;
        
        case ROTACION:
            if (coordenada_activa == COORD_GLOBAL)
                modo_analisis(0, 1);
            else
                aplicar_transformaciones(right_values);
            break;

        case ESCALADO:
            _selected_camera->actual_camera->proj->left -= 0.01;
            _selected_camera->actual_camera->proj->right += 0.01;
            break;
        }
    } else {
        aplicar_transformaciones(right_values);
    }

break;

case GLUT_KEY_PAGE_UP: //tecla Re Pág

    if (modo_activo == MODO_CAMARA){
        switch (transformacion_activa){
        case TRASLACION:
            if (coordenada_activa == COORD_GLOBAL){
                if (distancia_camara_objeto() > 2.5){
                    t_cam = (transformaciones*)malloc(sizeof(transformaciones));
                    t_cam->translate = (vector3){
                        .x = -_selected_camera->actual_camera->m_invert[8],
                        .y = -_selected_camera->actual_camera->m_invert[9],
                        .z = -_selected_camera->actual_camera->m_invert[10]
                    };
                    aplicar_transformaciones(t_cam);
                }
            } else {
                aplicar_transformaciones(repag_values);
            }
            break;
        
        case ROTACION:
            if (coordenada_activa == COORD_LOCAL)
                aplicar_transformaciones(repag_values);
            break;

        case ESCALADO:
            _selected_camera->actual_camera->proj->near -= 0.01;
            _selected_camera->actual_camera->proj->far += 0.01;
            break;
        }
    } else {
        aplicar_transformaciones(repag_values);
    }
    
break;

case GLUT_KEY_PAGE_DOWN: //tecla Av Pág no corregido
    
    if (modo_activo == MODO_CAMARA){
        switch (transformacion_activa){
        case TRASLACION:
            if (coordenada_activa == COORD_GLOBAL){
                if (distancia_camara_objeto() < 100){
                    t_cam = (transformaciones*)malloc(sizeof(transformaciones));
                    t_cam->translate = (vector3){
                        .x = _selected_camera->actual_camera->m_invert[8],
                        .y = _selected_camera->actual_camera->m_invert[9],
                        .z = _selected_camera->actual_camera->m_invert[10]
                    };
                    aplicar_transformaciones(t_cam);
                }
            } else {
                aplicar_transformaciones(avpag_values);
            }
            break;
        
        case ROTACION:
            if (coordenada_activa == COORD_LOCAL){
                aplicar_transformaciones(avpag_values);
            }
            break;

        case ESCALADO:
            _selected_camera->actual_camera->proj->near += 0.01;
            _selected_camera->actual_camera->proj->far += 0.01;
            break;
        }
    } else {
        aplicar_transformaciones(avpag_values);
    }
break;
}

glutPostRedisplay();
}




