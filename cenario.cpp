// para compilar: g++ -o cenario Geometry.cpp Bmp.cpp cenario.cpp -I ./CImg -lm -lpthread -lX11 -lGL -lGLU -lglut
#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
#include <string>
#include "Geometry.h"
#include <string.h>
#include "Bmp.h"
#include "CImg.h"
#include <vector>
#define WINDOW_TITLE_PREFIX "Eu, avião"
using namespace std;
using namespace cimg_library;
unsigned FrameCount = 0;

Sphere planeta, sol;
GLuint texture[2];
int aterrissando;
float sol_raio, solX,solY, solZ, raio, angulo, h,dh, h_max, h_min, h_atual, h_ant, v, h_max_atual;
Vetor camera, up, ref, polar, dhs, eixo_frente, eixo_x, eixo_y, eixo_z, origin;
void  draw_tex_sphere(Sphere sphere, GLuint tex);
const float PI = 3.1415926f;
GLfloat luz_pontual[] = { 0.0, 1.0, 50.0, 1.0 };
Vetor camera_exib, ref_exib;

float getPixel( const char* fileName, int i, int j){
    Image::Bmp bmp;
    if(!bmp.read(fileName)){
        return 0;     // exit if failed load image
    }
    return float(bmp.getData()[i*bmp.getWidth() + j])/255;

}


GLuint loadTexture(const char* fileName, bool wrap, GLuint texture)
{
    Image::Bmp bmp;
    if(!bmp.read(fileName)){
        cout << "error" << endl;
        return 0;     // exit if failed load image

    }
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
      //glRotatef(-90, 1, 0, 0);
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
    glBegin(GL_LINES);
        glColor3f (1.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(eixo_x.x, eixo_x.y, eixo_x.z);
        
        glColor3f (0.0, 1.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(eixo_y.x, eixo_y.y, eixo_y.z);
      
        glColor3f (0.0, 0.0, 1.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(eixo_z.x, eixo_z.y, eixo_z.z);    
    glEnd(); //glDisable(GL_LIGHTING);
	}  
void desenhar_pontos(){
   //glEnable(GL_LIGHTING);
    //não há efeitos de iluminação nos eixos
	glLineWidth(3);
    glBegin(GL_POINTS);
        glColor3f (1.0, 0.0, 0.0);
        glVertex3f(camera.x, camera.y, camera.z);
        
        glColor3f (0.0, 1.0, 0.0);
        glVertex3f(ref.x, ref.y, ref.z);
      
        glColor3f (0.0, 0.0, 1.0);
        glVertex3f(up.x, up.y, up.z);    
        
        glColor3f (0.0, 1.0, 1.0);
        glVertex3f(eixo_frente.x, eixo_frente.y, eixo_frente.z); 
    glEnd(); //glDisable(GL_LIGHTING);
	}  

void init_camera(){
   ref = new_vetor(0.0, 0.0, -5.0);
   up = new_vetor(0.0, raio + h_min + 1 , 0.0);
   camera = new_vetor(0.0, raio + h_min, 0.0);

}
void init(void) 
{
  
   cout << "Please enter the radius of the planet(ex: 5.0): ";
   cin >> raio; //ex: 5,0
   cout << "Please enter the sun's position(ex: 0 1 -225): ";
   cin >> solX >> solY >> solZ; //ex: 0 1 -225
   /** 
   raio = 5.0;
   solX = 0;
   solY = 1;
   solZ = -225;
     **/
   sol_raio = 40;
   planeta = init(planeta, raio, 60, 60);
   sol = init(sol, sol_raio, 30, 30);
   luz_pontual[0] = solX;
   luz_pontual[1] = solY;
   luz_pontual[2] = solZ;
   luz_pontual[3] = 1.0;
   h = 1.0;
   h_max = 1.5;
   h_max_atual = h_max;
   h_ant = h_max;
   h_min = 1.0;
   dh = 0.0;
   v = 0.0;
   init_camera();
   eixo_x = new_vetor(1.0, 0.0, 0.0);
   eixo_y = new_vetor(0.0, 1.0, 0.0);
   eixo_z = new_vetor(0.0, 0.0, 1.0);
   origin = new_vetor(0.0, 0.0, 0.0);
   glClearColor (0.0, 0.0, 0.0, 0.0);
   iluminar();
   glEnable(GL_DEPTH_TEST);
   glShadeModel (GL_SMOOTH);
   planeta = buildVerticesSphere(planeta,"textures/marsbump.jpg", 1, "textures/bump.txt");
   sol = buildVerticesSphere(sol,NULL, 0, NULL);
    
   glGenTextures(2, texture);
   glActiveTexture(GL_TEXTURE0);
   texture[0] = loadTexture("textures/mars.bmp", true, texture[0]);
  

}


void spinDisplay(void)
{ 

   if(h != h_min){
      camera = rotate_in_arbitrary_axis(camera, origin, eixo_x, -v);
      ref = rotate_in_arbitrary_axis(ref, origin, eixo_x,-v);
      up = rotate_in_arbitrary_axis(up, origin, eixo_x, -v);
      eixo_z = rotate_in_arbitrary_axis(eixo_z, origin,  eixo_x, -v);
      eixo_y = rotate_in_arbitrary_axis(eixo_y, origin,  eixo_x, -v);
      polar = getPolarCoordinates(planeta, ref, 0);
      
      h_atual = get_height(planeta, polar)*0.1;
      
      if(v > 0 && h_atual != h_ant){
         h_max_atual = h_max + h_atual;
         h_ant = h_atual;
         if(h_atual > 0)
            dh = 0.01;
         else if(h_atual < 0)
            dh = -0.01;
         
      }
   }
   glutPostRedisplay();
}
void specialKeys(int key, int x, int y){  
   float temp;
   float angulo =0.08;
   if(h  == h_max_atual){
      switch (key) {
         case GLUT_KEY_LEFT :
               ref = rotate_in_arbitrary_axis(ref, origin, eixo_y, angulo);
               eixo_x = rotate_in_arbitrary_axis(eixo_x, origin,  eixo_y, angulo);
               eixo_z = rotate_in_arbitrary_axis(eixo_z, origin,  eixo_y, angulo);
               break;
         case GLUT_KEY_RIGHT : 
               ref = rotate_in_arbitrary_axis(ref, origin, eixo_y, -angulo);
               eixo_x = rotate_in_arbitrary_axis(eixo_x, origin,  eixo_y, -angulo);
               eixo_z = rotate_in_arbitrary_axis(eixo_z, origin,  eixo_y, -angulo);
               break;     
         case GLUT_KEY_UP : 
               v += 0.001;
               if(v == 5)
                  v = 5;               
               break;     
         case GLUT_KEY_DOWN : 
               if( v > 0.001)
                  v -= 0.001;
               else
                  v = 0;
               break;     
      }
      glutPostRedisplay();
   }
}


void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt (camera.x, camera.y, camera.z, ref.x, ref.y, ref.z, up.x, up.y, up.z);
   
   glLightfv(GL_LIGHT1, GL_POSITION, luz_pontual);
   desenhar_luz(); 
   desenhar_objeto();
   	
   glutSwapBuffers();
}
void update_h(int time){
   if(dh != 0){
      h += dh;
      if(dh < 0 && !aterrissando){
         if(h < h_max_atual){
            h = h_max_atual;
            dh = 0;
         }
         time = 3;
      }else if(dh < 0 && aterrissando){
            if(h < h_min){
               h = h_min;
               dh = 0;
            }
      }else  if(dh > 0){
          if(h >h_max_atual ){
            h =h_max_atual;
            dh = 0;
            }
      }
   }
   if(dh != 0){
      camera = inc_vec(camera, eixo_y, dh);
      ref = inc_vec(ref, eixo_y, dh);
      up = inc_vec(up, eixo_y, dh);
   }
   glutPostRedisplay();
   glutTimerFunc(10, update_h,1);
}

void keyboard(unsigned char key, int x, int y){
   
      switch (key)
      {
      case 'd':
         if(h == h_min){
            dh = 0.01;
            aterrissando = 0;
            glutIdleFunc(spinDisplay);
            
         }
        break;
      case 's':
         if(h == h_max_atual){
            dh = -0.01;
            v = 0;
            aterrissando = 1;
            glutIdleFunc(NULL);
         }
         break;
      
      default:
         break;
      }
   
  
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(90.0, w/h, 0.3, 1000.0);
   glutPostRedisplay();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (800, 600); 
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