#ifndef IO_H
#define IO_H

void keyboard(unsigned char key, int x, int y);
void print_help();
void SpecialInput(int key, int x, int y);
void inicializar_luces();
void inicializar_materiales();
void put_light(GLint i);
void set_m_spotlight();

#endif // IO_H
