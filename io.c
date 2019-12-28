#include "definitions.h"
#include "transformaciones.h"
#include "load_obj.h"
#include <GL/glut.h>
#include <stdio.h>
#include <stdbool.h>
#include "camera.h"
#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846
#define ANGULO 18.0

#define MODO_OBJ 0
#define MODO_CAMARA 1
#define MODO_LUZ 2

#define TRASLACION 0
#define ROTACION 1
#define ESCALADO 2

#define COORD_GLOBAL 0
#define COORD_LOCAL 1

#define LUZ_DESACTIVADA 0
#define LUZ_ACTIVADA 1

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

objetos_luz global_lights[8];
extern GLint flat_smooth;

int camera_object_mode = 0;
int modo_activo = MODO_OBJ;
int transformacion_activa = TRASLACION;
int coordenada = COORD_GLOBAL;
int modo_luz = LUZ_ACTIVADA;
int luz_activada = 1;
GLint _selected_light = 0;


material_light *ruby, *obsidian;


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
    printf("<A/a>\t\t Activar modo luz.\n");
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

void matriz_identidad(GLfloat *m){
    int i, j;
    for (i = 0; i < 4; i++){
        for (j = 0; j < 4; j++){
            m[i*4+j] = 0;
            if (i == j)
                m[i*4+j] = 1;
        }
    }
}

void free_ptr(object3d *object){
    int i;
    for (i = 0; i < object->num_faces; i++)
                free(object->face_table[i].vertex_table);
                

            free(object->face_table);
            free(object->vertex_table);
            free(object);
}

void modo_analisis(int x, int y){
    GLfloat px, py, pz, distance;
    GLfloat m_minus[16], m_plus[16], m_rot[16];

    px = _selected_object->list_matrix->m[12] - _selected_camera->current_camera->m_invert[12];
    py = _selected_object->list_matrix->m[13] - _selected_camera->current_camera->m_invert[13];
    pz = _selected_object->list_matrix->m[14] - _selected_camera->current_camera->m_invert[14];

    distance = sqrt(pow(px, 2) + pow(py, 2) + pow(pz, 2));

    matriz_identidad(m_minus); matriz_identidad(m_plus); matriz_identidad(m_rot);

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
    glMultMatrixf(_selected_camera->current_camera->m_invert);
    glMultMatrixf(m_rot);
    glGetFloatv(GL_MODELVIEW_MATRIX, _selected_camera->current_camera->m_invert);

    matriz_inversa(_selected_camera);
}

void put_light(GLint i){
    switch (i){
    case 0:
        glLightfv(GL_LIGHT0, GL_POSITION, global_lights[i].position);
        glLightfv(GL_LIGHT0, GL_AMBIENT, global_lights[i].ambient); 
        glLightfv(GL_LIGHT0, GL_DIFFUSE, global_lights[i].diffuse); 
        glLightfv(GL_LIGHT0, GL_SPECULAR, global_lights[i].specular); 
        break;
    
    case 1:
        glLightfv(GL_LIGHT1, GL_AMBIENT, global_lights[i].ambient); 
        glLightfv(GL_LIGHT1, GL_DIFFUSE, global_lights[i].diffuse); 
        glLightfv(GL_LIGHT1, GL_SPECULAR, global_lights[i].specular); 
        glLightfv(GL_LIGHT1, GL_POSITION, global_lights[i].position); 
        break;

    case 2:
        glLightfv(GL_LIGHT2, GL_AMBIENT, global_lights[i].ambient); 
        glLightfv(GL_LIGHT2, GL_DIFFUSE, global_lights[i].diffuse); 
        glLightfv(GL_LIGHT2, GL_SPECULAR, global_lights[i].specular);
        glLightfv(GL_LIGHT2, GL_POSITION, global_lights[i].position); 
        glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, global_lights[i].spot_direction);
        glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, global_lights[i].cut_off);
        break;

    case 3:
        glLightfv(GL_LIGHT3, GL_AMBIENT, global_lights[i].ambient); 
        glLightfv(GL_LIGHT3, GL_DIFFUSE, global_lights[i].diffuse); 
        glLightfv(GL_LIGHT3, GL_SPECULAR, global_lights[i].specular);
        glLightfv(GL_LIGHT3, GL_POSITION, global_lights[i].position); 

        if (global_lights[i].type == FOCO){
            glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, global_lights[i].spot_direction);
            glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, global_lights[i].cut_off);
        }
        break;

    case 4:
        glLightfv(GL_LIGHT4, GL_AMBIENT, global_lights[i].ambient); 
        glLightfv(GL_LIGHT4, GL_DIFFUSE, global_lights[i].diffuse); 
        glLightfv(GL_LIGHT4, GL_SPECULAR, global_lights[i].specular);
        glLightfv(GL_LIGHT4, GL_POSITION, global_lights[i].position); 

        if (global_lights[i].type == FOCO){
             glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, global_lights[i].spot_direction);
             glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, global_lights[i].cut_off);
        }
        break;

    case 5:
        glLightfv(GL_LIGHT5, GL_AMBIENT, global_lights[i].ambient); 
        glLightfv(GL_LIGHT5, GL_DIFFUSE, global_lights[i].diffuse); 
        glLightfv(GL_LIGHT5, GL_SPECULAR, global_lights[i].specular);
        glLightfv(GL_LIGHT5, GL_POSITION, global_lights[i].position); 

        if (global_lights[i].type == FOCO){
            glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, global_lights[i].spot_direction);
            glLightf(GL_LIGHT5, GL_SPOT_CUTOFF, global_lights[i].cut_off);
        }
        break;

    case 6:
        glLightfv(GL_LIGHT6, GL_AMBIENT, global_lights[i].ambient); 
        glLightfv(GL_LIGHT6, GL_DIFFUSE, global_lights[i].diffuse); 
        glLightfv(GL_LIGHT6, GL_SPECULAR, global_lights[i].specular);
        glLightfv(GL_LIGHT6, GL_POSITION, global_lights[i].position); 
 
        if (global_lights[i].type == FOCO){
            glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, global_lights[i].spot_direction);
            glLightf(GL_LIGHT6, GL_SPOT_CUTOFF, global_lights[i].cut_off);
        }
        break;

    case 7:
        glLightfv(GL_LIGHT7, GL_AMBIENT, global_lights[i].ambient); 
        glLightfv(GL_LIGHT7, GL_DIFFUSE, global_lights[i].diffuse); 
        glLightfv(GL_LIGHT7, GL_SPECULAR, global_lights[i].specular);
        glLightfv(GL_LIGHT7, GL_POSITION, global_lights[i].position); 

        if (global_lights[i].type == FOCO){
            glLightfv(GL_LIGHT7, GL_SPOT_DIRECTION, global_lights[i].spot_direction);
            glLightf(GL_LIGHT7, GL_SPOT_CUTOFF, global_lights[i].cut_off);
        }
        break;

    default:
        break;
    }
}

void inicializar_luces(){
    //objetos_luz bombilla, sol, foco;

    global_lights[0].position[0] = 0.0f;
    global_lights[0].position[1] = 1.0f;
    global_lights[0].position[2] = 0.0f;
    global_lights[0].position[3] = 0.0f;
    global_lights[0].ambient[0] = 1.2f;
    global_lights[0].ambient[1] = 1.2f;
    global_lights[0].ambient[2] = 1.2f;
    global_lights[0].ambient[3] = 1.0f;
    global_lights[0].diffuse[0] = 1.0f;
    global_lights[0].diffuse[1] = 1.0f;
    global_lights[0].diffuse[2] = 1.0f;
    global_lights[0].diffuse[3] = 1.0f;
    global_lights[0].specular[0] = 1.0f;
    global_lights[0].specular[1] = 1.0f;
    global_lights[0].specular[2] = 1.0f;
    global_lights[0].specular[3] = 1.0f;
    //global_lights[0].is_on = 0;

    //operaciones
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    put_light(0);
    glGetFloatv(GL_MODELVIEW_MATRIX, global_lights[0].m_obj);
    global_lights[0].type = SOL;
    global_lights[0].is_on = 1;


    global_lights[1].position[0] = 1.0f;
    global_lights[1].position[1] = 1.0f;
    global_lights[1].position[2] = 0.0f;
    global_lights[1].position[3] = 1.0f;
    global_lights[1].ambient[0] = 1.2f;
    global_lights[1].ambient[1] = 1.2f;
    global_lights[1].ambient[2] = 1.2f;
    global_lights[1].ambient[3] = 1.0f;
    global_lights[1].diffuse[0] = 1.0f;
    global_lights[1].diffuse[1] = 1.0f;
    global_lights[1].diffuse[2] = 1.0f;
    global_lights[1].diffuse[3] = 1.0f;
    global_lights[1].specular[0] = 1.0f;
    global_lights[1].specular[1] = 1.0f;
    global_lights[1].specular[2] = 1.0f;
    global_lights[1].specular[3] = 1.0f;
    //global_lights[1].is_on = 0;

    //operaciones
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    put_light(1);
    glGetFloatv(GL_MODELVIEW_MATRIX, global_lights[1].m_obj);
    global_lights[1].type = BOMBILLA;
    global_lights[1].is_on = 0;
}


void m_foco(){
    int i;
    for (i = 0; i < 16; i++){
        global_lights[2].m_obj[i] = _selected_object->list_matrix->m[i];
    }
}

void foco_obj(){
    global_lights[2].position[0] = _selected_object->max.x / 2;
    global_lights[2].position[1] = _selected_object->max.y / 2;
    global_lights[2].position[2] = _selected_object->max.z;
    global_lights[2].position[3] = 1;

    global_lights[2].ambient[0] = 1.5f;
    global_lights[2].ambient[1] = 1.5f;
    global_lights[2].ambient[2] = 1.5f;
    global_lights[2].ambient[3] = 1.0f;

    global_lights[2].diffuse[0] = 1.5f;
    global_lights[2].diffuse[1] = 1.5f;
    global_lights[2].diffuse[2] = 1.5f;
    global_lights[2].diffuse[3] = 1.0f;

    global_lights[2].specular[0] = 1.0f;
    global_lights[2].specular[1] = 1.0f;
    global_lights[2].specular[2] = 1.0f;
    global_lights[2].specular[3] = 1.0f;

    global_lights[2].cut_off = 45.0f;

    global_lights[2].spot_direction[0] = 0.0f;
    global_lights[2].spot_direction[1] = 0.0f;
    global_lights[2].spot_direction[2] = 1.0f;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    put_light(2);
    m_foco();
    global_lights[2].type = FOCO_OBJETO;
    global_lights[2].is_on = 0;
}

void anadir_luz(){
    GLint luz, pos, values;
    objetos_luz new;
    printf("Elige el tipo de luz: 1 SOL, 2 BOMBILLA, 3 FOCO.\n");
    scanf("%d", &luz);

    while (luz < 1 || luz > 3){
        printf("Error, elija entre 1 y 3");
        scanf("%d", &luz);
    }
    printf("A continuacion introduce la posicion de 4-8\n");
    scanf("%d", &pos);

    while (pos < 4 || pos > 8){
        printf("Error, elija una posicion entre 4 y 8\n");
        scanf("%d", &pos);
    }


    printf("Ahora si desea propiedades por defecto pulse 0, si desea insertarlas pulse 1.\n");
    scanf("%d", &values);

    if (values == 0){
        new.ambient[0] = 1.2f;
        new.ambient[1] = 1.2f;
        new.ambient[2] = 1.2f;
        new.ambient[3] = 1.0f;

        new.diffuse[0] = 1.0f;
        new.diffuse[1] = 1.0f;
        new.diffuse[2] = 1.0f;
        new.diffuse[3] = 1.0f;

        new.specular[0] = 1.0f;
        new.specular[1] = 1.0f;
        new.specular[2] = 1.0f;
        new.specular[3] = 1.0f;
    } else {
        printf("Inserta la luz ambiental de la siguiente manera: r g b a\n");
        scanf("%f %f %f %f", &new.ambient[0], &new.ambient[1], &new.ambient[2], &new.ambient[3]);

        printf("Inserta la luz difusa de la misma manera que antes.\n");
        scanf("%f %f %f %f", &new.diffuse[0], &new.diffuse[1], &new.diffuse[2], &new.diffuse[3]);

        printf("Inserta la luz especular de la misma manera que antes.\n");
        scanf("%f %f %f %f", &new.specular[0], &new.specular[1], &new.specular[2], &new.specular[3]);
    }

    if (luz != 1){
        printf("Inserte la posicion de la siguiente manera: x y z\n");
        scanf("%f %f %f", &new.position[0], &new.position[1], &new.position[2]);
    }

    switch (luz){
    case 1:
        new.type = SOL;
        new.position[0] = 0.0f;
        new.position[1] = 1.0f;
        new.position[2] = 0.0f;
        new.position[3] = 0.0f;
        break;

    case 2:
        new.type = BOMBILLA;
        new.position[3] = 1.0f;
        break;

    case 3:
        new.type = FOCO;
        new.position[3] = 1.0f;
        printf("Para el foco necesitamos un punto de direccion y un angulo\n");
        printf("De nuevo, si desea por defecto pulse 0, si desea introducirlo pulse 1.\n");
        scanf("%d", &values);

        if (values == 0){
            new.cut_off == 45.0f;
            new.spot_direction[0] = 0.0f;
            new.spot_direction[1] = 0.0f;
            new.spot_direction[2] = 1.0f;
        } else {
            printf("Inserte el punto de la siguiente manera: x y z\n");
            scanf("%f %f %f", &new.spot_direction[0], &new.spot_direction[1], &new.spot_direction[2]);

            printf("Inserte el angulo\n");
            scanf("%f", &new.cut_off);
        }
        break;
    
    default:
        printf("Numero de tipo de luz incorrecto!\n");
        break;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    put_light(pos-1);
    glGetFloatv(GL_MODELVIEW_MATRIX, new.m_obj);
    new.is_on = 0;
    global_lights[pos-1] = new;

    printf("Una vez creada, pulse F%d para activarla.\n", pos);

}

void inicializar_materiales(){
    ruby = (material_light*)malloc(sizeof(material_light));
    obsidian = (material_light*)malloc(sizeof(material_light));

    ruby->m_ambient[0] = 0.1745f;
    ruby->m_ambient[1] = 0.01175f;
    ruby->m_ambient[2] = 0.01175f;
    ruby->m_ambient[3] = 0.55f;
    ruby->m_diffuse[0] = 0.61424f;
    ruby->m_diffuse[1] = 0.04136f;
    ruby->m_diffuse[2] = 0.04136f;
    ruby->m_diffuse[3] = 0.55f;
    ruby->m_specular[0] = 0.727811f;
    ruby->m_specular[1] = 0.626959f;
    ruby->m_specular[2] = 0.626959f;
    ruby->m_specular[3] = 0.55f;
    ruby->no_shininess[0] = 76.8f;

    obsidian->m_ambient[0] = 0.05375f;
    obsidian->m_ambient[1] = 0.05f;
    obsidian->m_ambient[2] = 0.06625f;
    obsidian->m_ambient[3] = 0.82f;
    obsidian->m_diffuse[0] = 0.18275f;
    obsidian->m_diffuse[1] = 0.17f;
    obsidian->m_diffuse[2] = 0.22525f;
    obsidian->m_diffuse[3] = 0.82f;
    obsidian->m_specular[0] = 0.332741f;
    obsidian->m_specular[1] = 0.328634f;
    obsidian->m_specular[2] = 0.346435f;
    obsidian->m_specular[3] = 0.82f;
    obsidian->no_shininess[0] = 38.4f;
}

void anadir_material(object3d *_selected_object){
    _selected_object->material_light = ruby;
}

void cambiar_material(object3d *_selected_object){
    if (_selected_object->material_light == ruby){
        _selected_object->material_light = obsidian;
    } else {
        _selected_object->material_light = ruby;
    }
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
    //glEnable(GL_LIGHTING);
    
    object3d *auxiliar_object = 0;
    GLdouble wd,he,midx,midy;

    list_matrix *aux_list;
    camera *aux_camera = 0;
    lista_camera *aux_camera_obj = 0;

    int i;


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

                normal_vectors();
                anadir_material(_selected_object);
                _selected_object->flat_smooth = FLAT;
                foco_obj();
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

        if (_selected_object != 0) foco_obj();

        if (modo_activo == MODO_CAMARA && coordenada == COORD_GLOBAL)
            centre_camera_to_obj(_selected_object);
        if (camera_object_mode == 1) add_camera_mode_obj(_selected_object);

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
            if (_selected_object != 0) foco_obj();
        }
    break;

    case '-':
        if (modo_activo == MODO_OBJ)
            global_scalation(0.5f, 0.5f, 0.5f);
        else if (global_lights[_selected_light].type == FOCO || 
            global_lights[_selected_light].type == FOCO_OBJETO) global_lights[_selected_light].cut_off -= 3;
        else { //modo activo == modo camara
            wd = (_selected_camera->current_camera->proj->right - _selected_camera->current_camera->proj->left) / KG_STEP_ZOOM;
            he = (_selected_camera->current_camera->proj->bottom - _selected_camera->current_camera->proj->top) / KG_STEP_ZOOM;

            midx = (_selected_camera->current_camera->proj->right + _selected_camera->current_camera->proj->left) / 2;
            midy = (_selected_camera->current_camera->proj->bottom + _selected_camera->current_camera->proj->top) / 2;

            _selected_camera->current_camera->proj->right = (midx + wd) / 2;
            _selected_camera->current_camera->proj->left = (midx - wd) / 2;
            _selected_camera->current_camera->proj->bottom = (midy + he) / 2;
            _selected_camera->current_camera->proj->top = (midy - he) / 2;

        }
    break;

    case '+':
        if (modo_activo == MODO_OBJ)
            global_scalation(2.0f, 2.0f, 2.0f);
        else if (global_lights[_selected_light].type == FOCO || 
            global_lights[_selected_light].type == FOCO_OBJETO) global_lights[_selected_light].cut_off += 3;
        else { //modo activo == modo camara
            wd = (_selected_camera->current_camera->proj->right - _selected_camera->current_camera->proj->left) * KG_STEP_ZOOM;
            he = (_selected_camera->current_camera->proj->left - _selected_camera->current_camera->proj->top) * KG_STEP_ZOOM;

            midx = (_selected_camera->current_camera->proj->right + _selected_camera->current_camera->proj->left) / 2;
            midy = (_selected_camera->current_camera->proj->bottom + _selected_camera->current_camera->proj->top) / 2;

            _selected_camera->current_camera->proj->right = (midx + wd) / 2;
            _selected_camera->current_camera->proj->left = (midx - wd) / 2;
            _selected_camera->current_camera->proj->bottom = (midy + he) / 2;
            _selected_camera->current_camera->proj->top = (midy - he) / 2;
        }

        
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
        if (coordenada != COORD_GLOBAL){
            if (modo_activo == MODO_CAMARA) centre_camera_to_obj(_selected_object);
            printf("Activadas transformaciones en mundo.\n");
            coordenada = COORD_GLOBAL;
        }
    break;

    case 'l':
    case 'L': 
        if (coordenada != COORD_LOCAL){
            coordenada = COORD_LOCAL;    
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
        if (camera_object_mode == 0 && modo_activo != MODO_CAMARA){
            if (_selected_object != 0 && coordenada == COORD_GLOBAL)
                centre_camera_to_obj(_selected_object);
            printf("Modo camara activado.\n");
            modo_activo = MODO_CAMARA;
        }
        
    break;

    case 'p':
    case 'P': //cambiando el tipo de proyeccion (perspectiva / paralela)
        if (modo_activo == MODO_CAMARA){
            if (_selected_camera->current_camera->proj->type == PROJECTION_PERSP){
                printf("Cambiado a ortografica.\n");
                _selected_camera->current_camera->proj = global_ortho;
            } else {
                printf("Cambiado a perspectiva.\n");
                _selected_camera->current_camera->proj = global_perspective;
            }
        }
    break;

    case 'a':
    case 'A':
        if (modo_activo != MODO_LUZ){
            printf("Modo luz activado.\n");
            modo_activo = MODO_LUZ;
        }

    break;

    case '0':
        //insertar luz
        anadir_luz();
        break;

    case '1':
        _selected_light = 0;
        printf("SOL seleccionado.\n");
        break;

    case '2':
        _selected_light = 1;    
        printf("BOMBILLA seleccionada.\n");
        break;

    case '3':
        _selected_light = 2;
        printf("FOCO seleccionado.\n");
        break;

    case '4':
        _selected_light = 3;
        printf("Seleccionada LUZ 4.\n");
        break;

    case '5':
        _selected_light = 4;
        printf("Seleccionada LUZ 5.\n");
        break;

    case '6':
        _selected_light = 5;
        printf("Seleccionada LUZ 6.\n");
        break;

    case '7':
        _selected_light = 6;
        printf("Seleccionada LUZ 7.\n");
        break;

    case '8':
        _selected_light = 7;
        printf("Seleccionada LUZ 8.\n");
        break;

    case 'w':
    case 'W':
        cambiar_material(_selected_object);
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

transformaciones *camara;
glMatrixMode(GL_MODELVIEW);
switch(key){

case GLUT_KEY_UP:

    if (modo_activo == MODO_OBJ){
        aplicar_transformaciones(up_values);
    } else if (modo_activo == MODO_CAMARA){
        switch(transformacion_activa){
            case TRASLACION:
                if (coordenada == COORD_LOCAL)
                    aplicar_transformaciones(up_values);
                break;
            case ROTACION:
                if (coordenada == COORD_GLOBAL){
                    modo_analisis(-1, 0);
                } else {
                    aplicar_transformaciones(up_values);
                }
                break;
            case ESCALADO:
                _selected_camera->current_camera->proj->top -= 0.01;
                _selected_camera->current_camera->proj->bottom += 0.01;
                break;
            default:    
                break; 
        }
    } else {
        //printf("estoy en luz\n");
        switch (transformacion_activa){
        
        case TRASLACION:
            if (global_lights[_selected_light].type == BOMBILLA ||
                global_lights[_selected_light].type == FOCO)
                    aplicar_transformaciones(up_values);
                    //printf("transformada bombilla traslacion arriba\n");
        break;
        
        case ROTACION:
                if (global_lights[_selected_light].type == SOL || 
                    global_lights[_selected_light].type == FOCO)
                        aplicar_transformaciones(up_values);
            break;
        }
    }
break;

case GLUT_KEY_DOWN:

    if (modo_activo == MODO_OBJ){
        aplicar_transformaciones(down_values);
    } else if (modo_activo == MODO_CAMARA){
        switch (transformacion_activa){
        case TRASLACION:
            if (coordenada == COORD_LOCAL)
                aplicar_transformaciones(down_values);
            break;
        case ROTACION:
            if (coordenada == COORD_GLOBAL)
                modo_analisis(1, 0);
            else
                aplicar_transformaciones(down_values);
            break;
        case ESCALADO:
            _selected_camera->current_camera->proj->top += 0.01;
            _selected_camera->current_camera->proj->bottom -= 0.01;
            break;
        default:
            break;
        }
    } else {
        switch (transformacion_activa){
        case TRASLACION:
            if (global_lights[_selected_light].type == BOMBILLA ||
                global_lights[_selected_light].type == FOCO){
                    aplicar_transformaciones(down_values);
                }
            break;

        case ROTACION:
            if (global_lights[_selected_light].type == SOL ||
                global_lights[_selected_light].type == FOCO){
                    aplicar_transformaciones(down_values);
                }
            break;
        
        default:
            break;
        }
    }
    
break;

case GLUT_KEY_LEFT:

    if (modo_activo == MODO_OBJ){
        aplicar_transformaciones(left_values);
    } else if (modo_activo == MODO_CAMARA){
        switch (transformacion_activa){
        case TRASLACION:
            if (coordenada == COORD_LOCAL)
                aplicar_transformaciones(left_values);
            break;

        case ROTACION:
            if (coordenada == COORD_GLOBAL)
                modo_analisis(0, -1);
            else
                aplicar_transformaciones(left_values);
            break;

        case ESCALADO:
            _selected_camera->current_camera->proj->left += 0.01;
            _selected_camera->current_camera->proj->right -= 0.01;
            break;
        default:
            break;
        }
    } else {
        switch (transformacion_activa){
        case TRASLACION:
            if (global_lights[_selected_light].type == BOMBILLA || 
                global_lights[_selected_light].type == FOCO){
                    //printf("TRANSFORMANDO IZQUIERDA BOMBILLA\n");
                    aplicar_transformaciones(left_values);
                }
            break;

        case ROTACION:
            if (global_lights[_selected_light].type == SOL || 
                global_lights[_selected_light].type == FOCO){
                aplicar_transformaciones(left_values);
            }
            break;
        
        default:
            break;
        }
    }

break;

case GLUT_KEY_RIGHT:

    if (modo_activo == MODO_OBJ){
        aplicar_transformaciones(right_values);
    } else if (modo_activo == MODO_CAMARA){
        switch (transformacion_activa){

        case TRASLACION:
            if (coordenada == COORD_LOCAL)
                aplicar_transformaciones(right_values);
            break;
        
        case ROTACION:
            if (coordenada == COORD_GLOBAL)
                modo_analisis(0, 1);
            else
                aplicar_transformaciones(right_values);
            break;

        case ESCALADO:
            _selected_camera->current_camera->proj->left -= 0.01;
            _selected_camera->current_camera->proj->right += 0.01;
            break;
        default:
            break;
        }
    } else {
        switch (transformacion_activa){
        case TRASLACION:
            if (global_lights[_selected_light].type == BOMBILLA ||
                global_lights[_selected_light].type == FOCO){
                    aplicar_transformaciones(right_values);
                }
            break;

        case ROTACION:
            if (global_lights[_selected_light].type == SOL ||
                 global_lights[_selected_light].type == FOCO){
                aplicar_transformaciones(right_values);
            }
            break;
        
        default:
            break;
        }
    }

break;

case GLUT_KEY_PAGE_UP: //tecla Re Pág

    if (modo_activo == MODO_OBJ){
        aplicar_transformaciones(repag_values);
    } else if (modo_activo == MODO_CAMARA){
        switch (transformacion_activa){
        case TRASLACION:
            if (coordenada == COORD_GLOBAL){
                if (distancia_camara_objeto() > 2.5){
                    camara = (transformaciones*)malloc(sizeof(transformaciones));
                    camara->translate = (vector3){
                        .x = -_selected_camera->current_camera->m_invert[8],
                        .y = -_selected_camera->current_camera->m_invert[9],
                        .z = -_selected_camera->current_camera->m_invert[10]
                    };
                    aplicar_transformaciones(camara);
                }
            } else {
                aplicar_transformaciones(repag_values);
            }
            break;
        
        case ROTACION:
            if (coordenada == COORD_LOCAL)
                aplicar_transformaciones(repag_values);
            break;

        case ESCALADO:
            _selected_camera->current_camera->proj->near -= 0.01;
            _selected_camera->current_camera->proj->far += 0.01;
            break;
        default:
            break;
        }
    } else {
        switch (transformacion_activa){
        case TRASLACION:
            if (global_lights[_selected_light].type == BOMBILLA ||
                global_lights[_selected_light].type == FOCO){
                    aplicar_transformaciones(repag_values);
                }
            break;
        
        default:
            break;
        }
    }
    
break;

case GLUT_KEY_PAGE_DOWN: //tecla Av Pág no corregido
    
    if (modo_activo == MODO_OBJ){
        aplicar_transformaciones(avpag_values);
    } else if (modo_activo == MODO_CAMARA){
        switch (transformacion_activa){
        case TRASLACION:
            if (coordenada == COORD_GLOBAL){
                if (distancia_camara_objeto() < 100){
                    camara = (transformaciones*)malloc(sizeof(transformaciones));
                    camara->translate = (vector3){
                        .x = _selected_camera->current_camera->m_invert[8],
                        .y = _selected_camera->current_camera->m_invert[9],
                        .z = _selected_camera->current_camera->m_invert[10]
                    };
                    aplicar_transformaciones(camara);
                }
            } else {
                aplicar_transformaciones(avpag_values);
            }
            break;
        
        case ROTACION:
            if (coordenada == COORD_LOCAL){
                aplicar_transformaciones(avpag_values);
            }
            break;

        case ESCALADO:
            _selected_camera->current_camera->proj->near += 0.01;
            _selected_camera->current_camera->proj->far += 0.01;
            break;
        default:
            break;
        }
    } else {
        switch (transformacion_activa){
        case TRASLACION:
            if (global_lights[_selected_light].type == BOMBILLA ||
                global_lights[_selected_light].type == FOCO){
                    aplicar_transformaciones(avpag_values);
                }
            break;
        
        default:
            break;
        }
    }
break;

case GLUT_KEY_F9:

    if (modo_luz == LUZ_DESACTIVADA){
        printf("Luz activada.\n");
        modo_luz = LUZ_ACTIVADA;
        glEnable(GL_LIGHTING);
    } else {
        printf("Luz desactivada.\n");
        modo_luz = LUZ_DESACTIVADA;
        glDisable(GL_LIGHTING);
    }

break;

case GLUT_KEY_F1:

    switch(global_lights[0].is_on){
        case 0:
            global_lights[0].is_on = 1;
            glEnable(GL_LIGHT0);
            //printf("enciendo\n");
            break;
        case 1:
            global_lights[0].is_on = 0;
            glDisable(GL_LIGHT0);
            //printf("apago\n");
            break;
        printf("sol\n");
    }
    
break;

case GLUT_KEY_F2:

    if (global_lights[1].is_on == 0){
        global_lights[1].is_on = 1;
        glEnable(GL_LIGHT1);
    } else {
        global_lights[1].is_on = 0;
        glDisable(GL_LIGHT1);
    }
    printf("bombilla\n");

break;

case GLUT_KEY_F3:

    if (global_lights[2].is_on == 0){
        global_lights[2].is_on = 1;
        glEnable(GL_LIGHT2);
    } else {
        global_lights[2].is_on = 0;
        glDisable(GL_LIGHT2);
    }
    printf("foco\n");

break;

case GLUT_KEY_F4:

    if (global_lights[3].is_on == 0){
        global_lights[3].is_on = 1;
        glEnable(GL_LIGHT3);
    } else {
        global_lights[3].is_on = 0;
        glDisable(GL_LIGHT3);
    }

break;

case GLUT_KEY_F5:

    if (global_lights[4].is_on == 0){
        global_lights[4].is_on = 1;
        glEnable(GL_LIGHT4);
    } else {
        global_lights[4].is_on = 0;
        glDisable(GL_LIGHT4);
    }

break;

case GLUT_KEY_F6:

    if (global_lights[5].is_on == 0){
        global_lights[5].is_on = 1;
        glEnable(GL_LIGHT5);
    } else {
        global_lights[5].is_on = 0;
        glDisable(GL_LIGHT5);
    }

break;

case GLUT_KEY_F7:

    if (global_lights[6].is_on == 0){
        global_lights[6].is_on = 1;
        glEnable(GL_LIGHT6);
    } else {
        global_lights[6].is_on = 0;
        glDisable(GL_LIGHT6);
    }

break;

case GLUT_KEY_F8:

    if (global_lights[7].is_on == 0){
        global_lights[7].is_on = 1;
        glEnable(GL_LIGHT7);
    } else {
        global_lights[7].is_on = 0;
        glDisable(GL_LIGHT7);
    }

break;

case GLUT_KEY_F12:

    if (_selected_object->flat_smooth == 0) _selected_object->flat_smooth = 1;
    else  _selected_object->flat_smooth = 0;

break;

}


glutPostRedisplay();
}




