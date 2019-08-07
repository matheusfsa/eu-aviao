// para compilar: g++ -o cenario Sphere.cpp Bmp.cpp cenario.cpp -lGL -lGLU -lglut
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
#include <string>
#include "Sphere.h"
#include "Bmp.h"

#include <vector>
using namespace std;
typedef struct Rotacao
{
  float angle;
  float x;
  float y;
  float z;
  
} Rotacao;
typedef struct Point
{
  float x;
  float y;
  float z;
  
} Point;

vector<float> lasers;
vector<float> lasers_color;
vector<Rotacao> rotacoes;
Sphere planeta, sol;
GLuint texPlanetaId, texSolId;
GLuint texture[2];
float posCameraX,posCameraY,posCameraZ,sol_raio, solX,solY, solZ, upX, upY, upZ, raio, angulo, spinX,spinY,spinZ, h,dh, h_max, h_min, v;
float eixo_frenteX,eixo_frenteY,eixo_frenteZ,eixo_ladoX,eixo_ladoY,eixo_ladoZ;
float stack_pos_atual, sector_pos_atual,stack_ref_atual, sector_ref_atual;
float laser_size, refX, refY, refZ;
void  draw_tex_sphere(Sphere sphere, GLuint tex);

GLfloat luz_pontual[] = { 0.0, 1.0, 50.0, 1.0 };

Rotacao create_rotate(float angle, float x, float y, float z){
   Rotacao rot;
   rot.angle = angle;
   rot.x = x;
   rot.y = y;
   rot.z = z;
   return rot;
}
int get_polar_coordinates(float raio, float stack, float sector, int stackCount, int sectorCount, float* x, float* y, float* z){
    const float PI = 3.1415926f;
    float sectorStep = 2 * PI / sectorCount; 
    float stackStep = PI / stackCount;
    float stackAngle = PI / 2 - stack * stackStep; //starting from pi/2 to -pi/2
    float sectorAngle = sector * sectorStep; // starting from 0 to 2pi
    float xy; 
    xy = raio * cosf(stackAngle);
    *x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
    *y = raio * sinf(stackAngle);             // r * cos(u) * sin(v)
    *z = -xy * sinf(sectorAngle); 
    if(*x < 0.00001 && *x > -0.00001)
       *x = 0;
    if(*z < 0.00001 && *z > -0.00001)
       *z = 0;
   if(*y < 0.00001 && *y > -0.00001)
       *y = 0;
    
}
void get_sector_stack(float raio, float x, float y, float z, int stackCount, int sectorCount, float* stack, float* sector){
    const float PI = 3.1415926f;
    float sectorStep = 2 * PI / sectorCount; 
    float stackStep = PI / stackCount;
    float stackAngle = asin(y/raio);
    float xy; 
    xy = raio * cosf(stackAngle);
    float xxy = x/xy;
    float sectorAngle;
    cout <<"xxy:"<< xxy  << endl;
    if(abs(1-xxy) > 0.0001){
       sectorAngle = acos(xxy);
    }else{
        //cout <<"xxy:"<< xxy << endl;
        sectorAngle = 0;
    }
    //cout <<"x:"<< x << endl;
    //cout <<"xy:"<< xy << endl;
    
    cout << "sectorAngle:" << sectorAngle << endl;
    *stack = (PI/2 - stackAngle)/stackStep;
    *sector = sectorAngle/sectorStep;
    
}
GLuint loadTexture(const char* fileName, bool wrap, GLuint texture)
{
    Image::Bmp bmp;
    if(!bmp.read(fileName))
        return 0;     // exit if failed load image

    // get bmp info
    int width = bmp.getWidth();
    int height = bmp.getHeight();
    const unsigned char* data = bmp.getDataRGB();
    GLenum type = GL_UNSIGNED_BYTE;    // only allow BMP with 8-bit per channel

    // We assume the image is 8-bit, 24-bit or 32-bit BMP
    GLenum format;
    int bpp = bmp.getBitCount();
    if(bpp == 8)
        format = GL_LUMINANCE;
    else if(bpp == 24)
        format = GL_RGB;
    else if(bpp == 32)
        format = GL_RGBA;
    else
        return 0;               // NOT supported, exit

   
    glBindTexture(GL_TEXTURE_2D, texture);

    // select modulate to mix texture with color for shading
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // copy texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);
    //glGenerateMipmap(GL_TEXTURE_2D);

    // build our texture mipmaps
    switch(bpp)
    {
    case 8:
        gluBuild2DMipmaps(GL_TEXTURE_2D, 1, width, height, GL_LUMINANCE, type, data);
        break;
    case 24:
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, type, data);
        break;
    case 32:
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, type, data);
        break;
    }

    return texture;
}

void iluminar(){
   //LUZ
   // no mínimo 8 fontes podem ser utilizadas 
   //(iniciadas com cor preta)
   // número de fontes de luz afeta performance
    
   //LUZ 0
   
   //define características para a fonte de luz 0	
   //fonte de luz direcional (por que w==0?)
   GLfloat light0_position[] = { 0.0, 1.0, 0.0, 0.0 };
   GLfloat light0_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
   //atribui características para a fonte de luz 0
   //cor padrão: branco
   //glLightfv(GL_LIGHT2, GL_POSITION, light0_position);
   //glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
   glLightfv(GL_LIGHT2, GL_AMBIENT, light0_diffuse); 
   //LUZ 1
   //fonte de luz pontual
   //define características para a fonte de luz 1
   GLfloat light1_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
   GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light1_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
   
   //atribui as características para a fonte de luz 1
   //(experimentem remover alguns dos componentes abaixo)
   glLightfv(GL_LIGHT1, GL_POSITION, luz_pontual);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
   glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
   glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient); 
   
   //"acende" cada uma das luzes configuradas
   glEnable(GL_LIGHT2);
   glEnable(GL_LIGHT1);
}
void desenhar_luz(){
   glPushAttrib (GL_LIGHTING_BIT);
   
   GLfloat mat_diffuse[] = { 1.0, 1.0, 0.0, 1.0 };
   GLfloat mat_emission[] = { 1.0, 1.0, 0.0, 1.0 };
          
   //atribui características ao material
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
   glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
 
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
    
   glPushMatrix();
   glTranslatef(luz_pontual[0],luz_pontual[1],luz_pontual[2]); 
   
   glEnable(GL_LIGHTING);
   glColor3f (1.0, 1.0, 0.0);
   glActiveTexture(GL_TEXTURE1);
   glutSolidSphere(sol_raio,50,50);
   draw_tex_sphere(sol, texture[1]);
   glDisable(GL_LIGHTING);
   
   glPopAttrib();
   glPopMatrix();
}
void  draw_tex_sphere(Sphere sphere, GLuint tex){
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, tex);

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glVertexPointer(3, GL_FLOAT, 0, sphere.vertices.data());
   glNormalPointer(GL_FLOAT, 0, sphere.normals.data());
   glTexCoordPointer(2, GL_FLOAT, 0, sphere.texCoords.data());

   glPushMatrix();
      glColor3f (1, 1, 1);
      glRotatef(-90, 1, 0, 0);
      glDrawElements(GL_TRIANGLES, (unsigned int)sphere.indices.size(), GL_UNSIGNED_INT,  sphere.indices.data());
   glPopMatrix();
   glDisable(GL_TEXTURE_2D); 
   
   //glDisable(GL_TEXTURE_2D);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}
void desenhar_objeto(){	
   GLfloat mat_specular[] = {  1, 1, 1.0, 1.0 };
   GLfloat mat_diffuse[] = {1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 100.0 };
          
   glPushAttrib (GL_LIGHTING_BIT);
   
   //atribui características ao material
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
   
   glEnable(GL_LIGHTING);
   glActiveTexture(GL_TEXTURE0);
   draw_tex_sphere(planeta, texture[0]);
   glDisable(GL_LIGHTING);
   
   glPopAttrib();
   
}

void desenhar_eixos(){
   //glEnable(GL_LIGHTING);
    //não há efeitos de iluminação nos eixos
   float size = raio + 2;
	glLineWidth(3);
    glBegin(GL_POINTS);
        glColor3f (1.0, 0.0, 0.0);
        //glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(posCameraX, posCameraY, posCameraZ);
        
        glColor3f (0.0, 1.0, 0.0);
        //glVertex3f(posCameraX, posCameraY, posCameraZ);
        glVertex3f(upX, upY, upZ);
      
        glColor3f (0.0, 0.0, 1.0);
        //glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(refX, refY, refZ);
    glEnd(); //glDisable(GL_LIGHTING);
	}  
void init(void) 
{
  /** 
   cout << "Please enter the radius of the planet(ex: 5.0): ";
   cin >> raio; //ex: 5,0
   cout << "Please enter the sun's position(ex: 0 1 -225): ";
   cin >> solX >> solY >> solZ; //ex: 0 1 -225
  **/
   raio = 5.0;
   solX = 0;
   solY = 1;
   solZ = -225;
    
   sol_raio = 50;
   planeta = init(planeta, raio, 60, 60);
   sol = init(sol, sol_raio, 60, 60);
   luz_pontual[0] = solX;
   luz_pontual[1] = solY;
   luz_pontual[2] = solZ;
   luz_pontual[3] = 1.0;
   eixo_frenteX = 1.0;
   eixo_frenteY = 0.0;
   eixo_frenteZ = 0.0;
   refX = 0.0;
   refY = 0.0;
   refZ = -5.0;
   spinX = 1.0;
   spinY = 1.0;
   spinZ = 1.0;
   upX = 0.0;
   upY =1.0;
   upZ =0.0;
   posCameraX = 0.0;
   posCameraY = raio + h;
   posCameraZ = 1.0;
   h = 1.0;
   h_max = 2.0;
   h_min = 1.0;
   dh = 0.0;
   v = 0.0;
   stack_pos_atual = 0;
   sector_pos_atual = 7.5;
   stack_ref_atual = 3;
   sector_ref_atual = 7.5;
   
   glClearColor (0.0, 0.0, 0.0, 0.0);
   iluminar();
   glEnable(GL_DEPTH_TEST);
   glShadeModel (GL_SMOOTH);
   planeta = buildVerticesSphere(planeta);
   sol = buildVerticesSphere(sol);
   //get_polar_coordinates(planeta.radius + h,stack_pos_atual, sector_pos_atual, 30, 30, &posCameraX, &posCameraY, &posCameraZ);
   //get_polar_coordinates(planeta.radius + h,stack_ref_atual, sector_ref_atual, 30,30, &refX, &refY, &refZ);
   //get_polar_coordinates(planeta.radius + 2*h , stack_pos_atual , sector_pos_atual,30,30, &upX, &upY, &upZ);
   glGenTextures(2, texture);
   glActiveTexture(GL_TEXTURE0);
   texture[0] = loadTexture("textures/ds.bmp", true, texture[0]);
   glActiveTexture(GL_TEXTURE1);
   texture[1] = loadTexture("textures/sol.bmp", true,texture[1]);
   

}
void rotate_arbitrary(float *x,float *y,float *z,float a,float b,float c,float d, float e, float f,float angle){
   float u = a - d;
   float v = b - e;
   float w = c - f;
   float L = u*u + v*v + w*w;
   float x_v = *x;
   float y_v = *y;
   float z_v = *z;
   *x = ((a*(v*v + w*w) - u*(b*v + c*w - u*x_v- v*y_v - w*z_v))*(1-cos(angle)) + L*x_v*cos(angle)+ sqrt(L)*(-c*v + b*w - w*y_v + v*z_v)*sin(angle))/L;
   *y = ((b*(u*u + w*w) - u*(a*u + c*w - u*x_v- v*y_v - w*z_v))*(1-cos(angle)) + L*y_v*cos(angle)+ sqrt(L)*(-c*u + a*w - w*x_v + v*z_v)*sin(angle))/L;
   *z = ((c*(u*u + v*v) - w*(a*u + b*v - u*x_v- v*y_v - w*z_v))*(1-cos(angle)) + L*z_v*cos(angle)+ sqrt(L)*(-b*u + a*v - v*x_v + u*y_v)*sin(angle))/L;
}
void rotate(float *a, float *b, float angulo){
   float temp;
   temp = *a;
   *a =  temp*cos(angulo) + *b*sin(angulo);
   *b = -temp*sin(angulo) + *b*cos(angulo);
          
   
}
void print_rotacao(Rotacao rotacao){
   cout << "(" << rotacao.angle << ", " << rotacao.x << ", " << rotacao.y << ", " << rotacao.z << ")" << endl;
}
void atualiza_spin(float* spin, float inc){
   *spin += inc;
   if (*spin > 360.0)
      *spin = *spin - 360.0;
}
void spinDisplay(void)
{  
   if( h == h_max){
      //rotate_arbitrary(&refX,&refY, &refZ, 0.0, 0.0, 0.0, eixo_frenteX, eixo_frenteY, eixo_frenteZ, v);
      //rotate_arbitrary(&posCameraX,&posCameraY, &posCameraZ,  eixo_frenteX, eixo_frenteY, eixo_frenteZ,0.0, 0.0, 0.0, v);
      float diff_stack = (stack_pos_atual-stack_ref_atual);
      float diff_sector = (sector_pos_atual-sector_ref_atual);
      //cout << diff_stack << endl;
      //cout << diff_sector << endl;
      //cout << diff_stack << endl;
      //cout << diff_sector << endl;
      stack_pos_atual+=(v*0.01*diff_stack);
      stack_ref_atual+=(v*0.01*diff_stack);
      sector_pos_atual+=(v*0.01*diff_sector);
      sector_ref_atual+=(v*0.01*diff_sector);
      //get_polar_coordinates(planeta.radius + h, stack_pos_atual , sector_pos_atual,30, 30, &posCameraX, &posCameraY, &posCameraZ);
   //get_polar_coordinates(planeta.radius + h, stack_ref_atual , sector_ref_atual,30, 30, &refX, &refY, &refZ);
   //get_polar_coordinates(planeta.radius + (2*h), stack_pos_atual , sector_pos_atual,30, 30, &upX, &upY, &upZ);
      rotate(&posCameraZ, &posCameraY, 0.001);
      rotate(&refZ, &refY, 0.001);
      rotate(&upZ, &upY, 0.001);
      //rotate_arbitrary(&upX,&upY, &upZ, 0.0, 0.0, 0.0, eixo_frenteX, eixo_frenteY, eixo_frenteZ, v);
      //rotacoes.push_back(create_rotate(v, 1.0, 0.0, 0.0));
   }
   glutPostRedisplay();
}
void specialKeys(int key, int x, int y)
{  
   float temp;
   float angulo =2;
   angulo = 2*M_PI/180;
   if(h  == h_max){
      switch (key) {
         case GLUT_KEY_LEFT :
               //rotate(&refX, &refZ, -angulo);
               //rotate_arbitrary(&refX,&refY, &refZ, 0.0, 0.0, 0.0, upX, upY, upZ, angulo);
               //rotacoes.push_back(create_rotate(-angulo, 0, 1, 0));
               rotate(&refX, &refZ, -angulo);
               cout << "refX:"<< refX << endl;
               get_sector_stack(raio + h, refX, refY, refZ, 30, 30, &stack_ref_atual, &sector_ref_atual);
               break;
         case GLUT_KEY_RIGHT : 
               rotacoes.push_back(create_rotate(angulo, 0, 1, 0));
               break;     
         case GLUT_KEY_UP : 
               v += 0.01;
               if(v == 5)
                  v = 5;               
               break;     
         case GLUT_KEY_DOWN : 
               if( v > 0.01)
                  v -= 0.01;
               else
                  v = 0;
               break;     
         
      }
      glutPostRedisplay();
   }
}


void display(void)
{
   //limpeza do zbuffer deve ser feita a cada desenho da tela
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
  
   
   
   //cout << posCameraX << ", "<< posCameraY << ", "<< posCameraZ <<	endl;
   
   //gluLookAt (posCameraX, posCameraY, posCameraZ, refX, refY, refZ, upX, upY, upZ);
   // Por cima
   gluLookAt (0.0, 10.0, 0.0, 0, -5.0, 0.0, 0.0, 0.0, 1.0);	
   // Por trás
   //gluLookAt (0.0, 0.0, 10.0, 0, 0, -5.0, 0.0, 1.0, 0.0);	
   // De lado
   //gluLookAt (10.0, 0.0, 0.0, -5.0, 0.0, 0.0, 0.0, 1.0, 0.0);	
   //for(int i =  rotacoes.size()-1; i >= 0; i--){
      //glRotatef(rotacoes[i].angle, rotacoes[i].x, rotacoes[i].y, rotacoes[i].z);
      //}
   glLightfv(GL_LIGHT1, GL_POSITION, luz_pontual);
   
   desenhar_luz(); 
   
   desenhar_eixos();
   desenhar_objeto();
   
   glutSwapBuffers();
   
}
void update_h(int time){
   if(dh != 0){
      h += dh;
      if(h > h_max){
         h = h_max;
         dh = 0;
      }
      if(h < h_min){
         h = h_min;
         dh = 0;
      }
   }
   //posCameraY = raio+h;

   glutPostRedisplay();
   glutTimerFunc(10, update_h,1);
}

void keyboard(unsigned char key, int x, int y){
   if(dh == 0){
      switch (key)
      {
      case 'd':
         if(h == h_min){
            dh = 0.01;
            glutIdleFunc(spinDisplay);
         }
        break;
      case 's':
         if(h == h_max){
            dh = -0.01;
            glutIdleFunc(NULL);
         }
         break;
      
      default:
         break;
      }
   }
  
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   //ortográfica
   /**
   if (w <= h)
      glOrtho (-1.5, 1.5, -1.5*(GLfloat)h/(GLfloat)w,
         1.5*(GLfloat)h/(GLfloat)w, -10.0, 10.0);
   else
      glOrtho (-1.5*(GLfloat)w/(GLfloat)h,
         1.5*(GLfloat)w/(GLfloat)h, -1.5, 1.5, -10.0, 10.0);
   */
   //perspectiva
   gluPerspective(90.0, w/h, 0.3, 1000.0);
   glutPostRedisplay();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (500, 100);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutKeyboardFunc(keyboard);
   glutSpecialFunc(specialKeys);
   glutReshapeFunc(reshape);
   glutTimerFunc(10, update_h,1);
   glutMainLoop();
   return 0;
}