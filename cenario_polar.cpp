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
typedef struct Vetor
{
  float x;
  float y;
  float z;
  
} Vetor;

vector<float> lasers;
vector<float> lasers_color;
vector<Rotacao> rotacoes;
Sphere planeta, sol;
GLuint texPlanetaId, texSolId;
GLuint texture[2];
float sol_raio, solX,solY, solZ, raio, angulo, h,dh, h_max, h_min, v;
Vetor camera_xyz, up_xyz, ref_xyz;
Vetor camera_polar, up_polar, ref_polar;
Vetor direction;
void  draw_tex_sphere(Sphere sphere, GLuint tex);

GLfloat luz_pontual[] = { 0.0, 1.0, 50.0, 1.0 };
void print_vetor(Vetor a){
    cout << "(" << a.x << "," << a.y << ", " << a.z << ")" << endl;
}
Vetor new_vetor(float x, float y, float z){
    Vetor res;
    res.x = x;
    res.y = y;
    res.z = z;
    return res;
}
float get_norm(Vetor vetor){
    return sqrt((vetor.x*vetor.x) + (vetor.y*vetor.y) + (vetor.z*vetor.z));
}
Vetor sub_vec(Vetor a, Vetor b){
    return new_vetor(a.x - b.x, a.y - b.y, a.z - b.z);
}
Vetor sum_vec(Vetor a, Vetor b){
    return new_vetor(a.x + b.x, a.y + b.y, a.z + b.z);
}
Vetor inc_vec(Vetor a, Vetor direction, float taxa){
    return new_vetor(a.x + (direction.x)*taxa, a.y + (direction.y)*taxa, a.z + (direction.z)*taxa);
}
Vetor direction_vec(Vetor a, Vetor b){
    Vetor sub = sub_vec(b, a);
    float norm = get_norm(sub);
    return new_vetor(sub.x/norm, sub.y/norm, sub.z/norm);
}
Vetor rotate_vec(Vetor v, float angle, char axis){
    Vetor res = new_vetor(v.x, v.y, v.z);
    switch (axis)
    {
    case 'x':
        res.y = v.y * cos(angle) - v.z * cos(angle);
        res.z = v.y * sin(angle) + v.z * cos(angle);
        break;
    case 'y':
        res.x = v.x * cos(angle) + v.z * sin(angle);
        res.z = -v.x * sin(angle) + v.z * cos(angle);
        break;
    case 'z':
        res.x = v.x * cos(angle) + v.y * sin(angle);
        res.y = -v.x * sin(angle) + v.y * cos(angle);
        break;
    default:
        break;
    }
    return res;
}
Rotacao create_rotate(float angle, float x, float y, float z){
   Rotacao rot;
   rot.angle = angle;
   rot.x = x;
   rot.y = y;
   rot.z = z;
   return rot;
}
Vetor polar_to_xyz(Sphere planeta, float h, Vetor polar){
    const float PI = 3.1415926f;
    float sectorStep = 2 * PI / planeta.sectors; 
    float stackStep = PI / planeta.stacks;
    float stackAngle = PI / 2 - polar.x * stackStep; //starting from pi/2 to -pi/2
    float sectorAngle = polar.y * sectorStep; // starting from 0 to 2pi
    float xy; 
    Vetor res;
    xy = (planeta.radius + h) * cosf(stackAngle);
    res.x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
    res.y = (planeta.radius + h) * sinf(stackAngle);             // r * cos(u) * sin(v)
    res.z = -xy * sinf(sectorAngle); 
    if(res.x < 0.00001 && res.x > -0.00001)
       res.x = 0;
    if(res.z < 0.00001 && res.z > -0.00001)
       res.z = 0;
   if(res.y < 0.00001 && res.y > -0.00001)
       res.y = 0;
    return res;
}
Vetor rotacionar_lados(Vetor camera_polar, Vetor ref_polar, float angle){
    Vetor diff  = sub_vec(ref_polar, camera_polar);
    cout << endl;
    cout << "ref_polar:";
    print_vetor(ref_polar);
    cout << "camera_polar:";
    print_vetor(camera_polar);
    cout << "diff:";
    print_vetor(diff);
    
    diff = rotate_vec(diff, angle, 'z');
     cout << "diff_rot:";
    print_vetor(diff);
    Vetor res = sum_vec(camera_polar, diff);
    cout << "res:";
    print_vetor(res);
    if(res.x > 60 )
        res.x = 60 - res.x;
    if(res.y > 60)
        res.y = 60 - res.x;
    res.x = abs(res.x);
    res.y = abs(res.y);
    
    return res;
}

Vetor xyz_to_vec(Sphere planeta, float h,Vetor v){

    const float PI = 3.1415926f;
    float sectorStep = 2 * PI / planeta.sectors; 
    float stackStep = PI / planeta.stacks;
    float stackAngle = asin(v.y/(planeta.radius + h));
    float xy; 
    xy =(planeta.radius + h) * cosf(stackAngle);
   
    float xxy = v.x/xy;
    float sectorAngle;
    cout << xxy << endl;
    if(xxy > 0.99999){
        cout << xxy << endl;
        sectorAngle = 0;
    }else{
        sectorAngle = acos(xxy);
    }
    return new_vetor((PI/2 - stackAngle)/stackStep, sectorAngle/sectorStep, 0.0);
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
   float size = raio + 2;
	glLineWidth(3);
    glBegin(GL_POINTS);
        glColor3f (1.0, 0.0, 0.0);
        //glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(camera_xyz.x, camera_xyz.y, camera_xyz.z);
        
        glColor3f (0.0, 1.0, 0.0);
        //glVertex3f(posCameraX, posCameraY, posCameraZ);
        glVertex3f(up_xyz.x, up_xyz.y, up_xyz.z);
      
        glColor3f (0.0, 0.0, 1.0);
        //glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(ref_xyz.x, ref_xyz.y, ref_xyz.z);
    glEnd();
	}  
void init(void) {
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
   h = 1.0;
   h_max = 2.0;
   h_min = 1.0;
   dh = 0.0;
   v = 0.0;
   glClearColor (0.0, 0.0, 0.0, 0.0);
   iluminar();
   glEnable(GL_DEPTH_TEST);
   glShadeModel (GL_SMOOTH);
   planeta = buildVerticesSphere(planeta);
   sol = buildVerticesSphere(sol);
   camera_xyz = new_vetor(0.0, planeta.radius + h, 1.0);
   camera_polar = new_vetor(0.0, planeta.sectors/4, 0);
   ref_xyz = new_vetor(0.0, 0.0, -5.0);
   ref_polar = new_vetor(3, planeta.sectors/4, 0);
   up_xyz = new_vetor(0.0, planeta.radius + h, 1.0); 
   up_polar = new_vetor(0.0, planeta.sectors/4, 0.0); 
   glGenTextures(2, texture);
   glActiveTexture(GL_TEXTURE0);
   texture[0] = loadTexture("textures/ds.bmp", true, texture[0]);
   glActiveTexture(GL_TEXTURE1);
   texture[1] = loadTexture("textures/sol.bmp", true,texture[1]);
   

}
void atualiza_spin(float* spin, float inc){
   *spin += inc;
   if (*spin > 360.0)
      *spin = *spin - 360.0;
}
void spinDisplay(void)
{  
   if( h == h_max){
       Vetor direction = direction_vec(camera_polar, ref_polar);
       
       camera_polar = inc_vec(camera_polar, direction, v);
       camera_xyz = polar_to_xyz(planeta, h, camera_polar);
       ref_polar = inc_vec(ref_polar, direction, v);
       ref_xyz = polar_to_xyz(planeta, h, ref_polar);
       up_polar = inc_vec(up_polar, direction, v);
       up_xyz = polar_to_xyz(planeta, 2*h, up_polar);
   }
   glutPostRedisplay();
}
void specialKeys(int key, int x, int y)
{  
   float temp;
   float angulo;
   angulo = M_PI/2;
   if(h  == h_max){
      switch (key) {
         case GLUT_KEY_LEFT :
               //print_vetor(ref_polar); 
               //ref_polar = rotacionar_lados(camera_polar, ref_polar, angulo);
               //print_vetor(ref_polar);
               //ref_xyz = polar_to_xyz(planeta, h, ref_polar);
               print_vetor(ref_xyz);
               ref_xyz = rotate_vec(ref_xyz, -angulo, 'y');
               print_vetor(ref_xyz);
               ref_polar = xyz_to_vec(planeta, h, ref_xyz);
               break;
         case GLUT_KEY_RIGHT : 
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
   camera_xyz = polar_to_xyz(planeta, h, camera_polar);
   ref_xyz = polar_to_xyz(planeta, h, ref_polar);
   up_xyz = polar_to_xyz(planeta, 2*h, up_polar);
   //cout << posCameraX << ", "<< posCameraY << ", "<< posCameraZ <<	endl;
   //gluLookAt (posCameraX, posCameraY, posCameraZ, refX, refY, refZ, upX, upY, upZ);
   // Por cima
   //gluLookAt (0.0, 10.0, 0.0, 0, -5.0, 0.0, 0.0, 0.0, 1.0);	
   // Por trás
   //gluLookAt (0.0, 0.0, 10.0, 0, 0, -5.0, 0.0, 1.0, 0.0);	
   // De lado
   gluLookAt (10.0, 0.0, 0.0, -5.0, 0.0, 0.0, 0.0, 1.0, 0.0);	
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