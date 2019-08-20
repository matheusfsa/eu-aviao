#include "Geometry.h" 
#include <iostream>
#include <math.h>
#include "Bmp.h"
#include "CImg.h"
#include <fstream>

using namespace cimg_library;
using namespace std; 
const float PI = 3.1415926f;
float getHeight(CImg<unsigned char> src, float s, float t){
    int width = src.width();
    int height = src.height();
    int x = round(s*width);
    int y = round(t*height);   
    float height_res = ((float)src(x,y,0,0)/128) - 1;
    return height_res;

}

vector<float> load_heights(const char* fileName){
    vector<float> heights;
    ifstream myfile;
    myfile.open (fileName);
    float a;

    if (myfile.is_open())
    {
        while (!myfile.eof())
        {
            myfile >> a;
            heights.push_back(a);
        }
        
    }
    myfile.close();
    return heights;
}
int save_heights(vector<float> heights, const char* fileName){
    ofstream myfile;
    myfile.open (fileName);
    for(int i = 0; i< heights.size(); i++)
        myfile << heights[i]<<  endl;
    myfile.close();
}
Sphere init(Sphere sphere, float new_radius, int new_stacks, int new_sectors){
    sphere.radius = new_radius;
    sphere.stacks = new_stacks;
    sphere.sectors = new_sectors;
    sphere.sectorStep = 2 * PI / new_sectors;
    sphere.stackStep = PI / new_stacks;
    return sphere;
}
Vetor new_vetor(float x, float y, float z){
    Vetor res;
    res.x = x;
    res.y = y;
    res.z = z;
    return res;
}
void print_vetor(Vetor a){
    cout << "(" << a.x << "," << a.y << ", " << a.z << ")" << endl;
}
Vetor rotate_vec(Vetor v, float angle, char axis){
    Vetor res = new_vetor(v.x, v.y, v.z);
    switch (axis)
    {
    case 'x':
        res.y =  v.y * cos(angle) + v.z * sin(angle);
        res.z = -v.y * sin(angle) + v.z * cos(angle);
        break;
    case 'y':
        res.x =  v.x * cos(angle) + v.z * sin(angle);
        res.z = -v.x * sin(angle) + v.z * cos(angle);
        break;
    case 'z':
        res.x =  v.x * cos(angle) + v.y * sin(angle);
        res.y = -v.x * sin(angle) + v.y * cos(angle);
        break;
    default:
        break;
    }
    return res;
}
float getPixel(const unsigned char* data, int width, int height, float s, float t, int channel){
    int j = round(s*width);
    int i = round(t*height);
    return float(data[(i * width + j)+ channel] )/255;
}
Sphere buildVerticesSphere(Sphere sphere, const char* hmfileName, int is_img, const char* hmfileNameTxt)
{
    int has_hm = 0;
    int width_img = 0;
    int height_img = 0;
    float height = 0;
    CImg<unsigned char> height_map;
    const unsigned char* data;
    vector<float> heights;
    if(hmfileName != NULL){
        has_hm = 1;
        if(is_img){
            height_map.load(hmfileName);
            data = height_map.data();
            height_img = height_map.height();
            width_img = height_map.width();
        }else{
            heights = load_heights(hmfileNameTxt);
        }
    }else{
        has_hm = 0;
    }
    int sectorCount = sphere.sectors;
    int stackCount = sphere.stacks;
    std::vector<float>().swap(sphere.heights);
    std::vector<float>().swap(sphere.vertices);
    std::vector<float>().swap(sphere.normals);
    std::vector<float>().swap(sphere.texCoords);
    std::vector<unsigned int>().swap(sphere.indices);
    std::vector<unsigned int>().swap(sphere.lineIndices);

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / sphere.radius;    // normal
    float s, t;
    int j_a;                                     // texCoord

    
    float sectorAngle, stackAngle;
    float height_polo_n =  (rand() % 50)/(float)1000; 
    float height_polo_s =  (rand() % 50)/(float)1000; 

    for(int i = 0; i <= stackCount; ++i)
    {    
        stackAngle = PI / 2 - i * sphere.stackStep;        // starting from pi/2 to -pi/2
        xy = sphere.radius * cosf(stackAngle);             // r * cos(u)
        y = -sphere.radius * sinf(stackAngle);              // r * sin(u)
        
        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        
        
        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sphere.sectorStep;           // starting from 0 to 2pi

            // vertex position
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            //y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            z= xy * sinf(sectorAngle);
            Vetor v2 = getPolarCoordinates(sphere, new_vetor(x, y, z), 0); 
            if( int(v2.x) - i != 0){
                cout << "Coordenadas reais: " << "(" << i << ", "<< j << ", "<< stackAngle << ", " << sectorAngle << ")" << endl;
                cout << "Coordenadas estimadas: ";
                print_vetor(v2);                
            }
            
            // normalized vertex normal
            nx = x * lengthInv;
            ny = y * lengthInv; 
            nz = z * lengthInv; 
            t = (float)i / stackCount;
            s = (float)j / (sectorCount);

            if(i == 0 || i == stackCount || j == 0 || j == stackCount){
                    j_a = 0;
            }
            if(has_hm){
                if(is_img){
                        height = getPixel(height_map,width_img, height_img, j_a, t, 0);
                        heights.push_back(height);
                }else{
                    height = heights[i*stackCount + j_a];    
                }
            }else{
                height = 0;   
            } 
            sphere.heights.push_back(height);
            height *= 0.5;
            Vetor v;        
            v.x = nx*height;
            v.y = ny*height;
            v.z = nz*height;
            
            
            sphere.vertices.push_back(x+v.x);
            sphere.vertices.push_back(y+v.y);
            sphere.vertices.push_back(z+v.y);
            //adiciona vetores normais ao array de normais
            sphere.normals.push_back(nx);
            sphere.normals.push_back(ny);
            sphere.normals.push_back(nz);
            sphere.texCoords.push_back(s);
            sphere.texCoords.push_back(t);
            
        }
    }
    if(is_img && has_hm){
        save_heights(heights, hmfileNameTxt);
    }
    // indices
    //  k1--k1+1
    //  |  / |
    //  | /  |
    //  k2--k2+1
    unsigned int k1, k2, k11, k21;
    for(int i = 0; i < stackCount; ++i)
    {   
        
        k1 = i * (sectorCount + 1);     // beginning of current stack
        // beginning of next stack
        
        k2 = k1 + sectorCount + 1;  
        
        
        for(int j = 0; j <sectorCount; ++j, ++k1, ++k2)
        {
            k11 = k1+1;
            k21 = k2+1; 
            // 2 triangles per sector excluding 1st and last stacks
            if(i != 0)
            {
			// k1---k2---k1+1
			sphere.indices.push_back(k1);
			sphere.indices.push_back(k2);
			sphere.indices.push_back(k11);               
            }

            if(i != (stackCount-1))
            {
			// k1+1---k2---k2+1
			sphere.indices.push_back(k11);
			sphere.indices.push_back(k2);
			sphere.indices.push_back(k21);
            }
            // vertical lines for all stacks
            sphere.lineIndices.push_back(k1);
            sphere.lineIndices.push_back(k2);
            if(i != 0)  // horizontal lines except 1st stack
            {
                sphere.lineIndices.push_back(k1);
                sphere.lineIndices.push_back(k11);
            }
        }
    }
    return sphere;
}
float get_height(Sphere sphere, Vetor polar){
    int stack = round(polar.x);
    int sector = round(polar.y);
    return sphere.heights[stack*sector]; 
}
Vetor getPolarCoordinates(Sphere sphere, Vetor vetor, float h){
    /**
     * stackAngle = PI / 2 - i * sphere.stackStep;        
        xy = sphere.radius * cosf(stackAngle);            
        y = -sphere.radius * sinf(stackAngle);            
        sectorAngle = j * sphere.sectorStep;           
        x = xy * cosf(sectorAngle);             
        z = xy * sinf(sectorAngle);
     * **/
    const float PI = 3.1415926f;
    int sectorCount = sphere.sectors;
    int stackCount = sphere.stacks;
    float stackAngle, sectorAngle; 
    float v = -(vetor.y)/(sphere.radius+h);
    if(v > 1){
        v = 1;
    }
    if(v < -1){
        v = -1;
    }
    stackAngle = asinf(v);
    float xy = (sphere.radius+h) * cosf(stackAngle); 
    v = vetor.x/xy;

    if(v > 1){
        v = 1;
    }
    if(v < -1){
        v = -1;
    }
    sectorAngle = acosf(v);
    if(vetor.z < 0){
        sectorAngle  = 2*PI - sectorAngle;
    }
    float stack   = (PI / 2 - stackAngle)/sphere.stackStep;
    float sector = sectorAngle / sphere.sectorStep; 
    
    //cout << "Coordenadas estimadas:" << 
      //              " ( "<< stack << ", "<< sector << ", "<< stackAngle << ", "<< sectorAngle <<  ")" << endl << endl;
    
    return new_vetor(round(stack), round(sector), 0.0);
}
Vetor inc_vec(Vetor a, Vetor direction, float taxa){
    return new_vetor(a.x + (direction.x)*taxa, a.y + (direction.y)*taxa, a.z + (direction.z)*taxa);
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
Vetor direction_vec(Vetor a, Vetor b){
    Vetor sub = sub_vec(b, a);
    float norm = get_norm(sub);
    return new_vetor(sub.x/norm, sub.y/norm, sub.z/norm);
}
Vetor norm(Vetor a){
    float norma = get_norm(a);
    return new_vetor(a.x/norma, a.y/norma,a.z/norma);
}
Vetor rotate_in_arbitrary_axis(Vetor xyz, Vetor p1, Vetor p2, float angle){
   //Vetor abc = p2;
   double m11, m12, m13, m14, m21, m22, m23, m24, m31, m32,m33,m34;
   Vetor abc =sub_vec(p2, p1);
   Vetor uvw = norm(abc);
   double x = xyz.x;
   double y = xyz.y;
   double z = xyz.z;
   double a = abc.x;
   double b = abc.y;
   double c = abc.z;
   double u = uvw.x;
   double v = uvw.y;
   double w = uvw.z;
   double u2 = u*u;
   double v2 = v*v;
   double w2 = w*w;
   double cosT = cos(angle);
   double oneMinusCosT = 1-cosT;
   double sinT = sin(angle);
   // Build the matrix entries element by element.
   m11 = u2 + (v2 + w2) * cosT;
   m12 = u*v * oneMinusCosT - w*sinT;
   m13 = u*w * oneMinusCosT + v*sinT;
   m14 = (a*(v2 + w2) - u*(b*v + c*w))*oneMinusCosT
            + (b*w - c*v)*sinT;

   m21 = u*v * oneMinusCosT + w*sinT;
   m22 = v2 + (u2 + w2) * cosT;
   m23 = v*w * oneMinusCosT - u*sinT;
   m24 = (b*(u2 + w2) - v*(a*u + c*w))*oneMinusCosT
            + (c*u - a*w)*sinT;

   m31 = u*w * oneMinusCosT - v*sinT;
   m32 = v*w * oneMinusCosT + u*sinT;
   m33 = w2 + (u2 + v2) * cosT;
   m34 = (c*(u2 + v2) - w*(a*u + b*v))*oneMinusCosT
            + (a*v - b*u)*sinT;
   Vetor res;
   res.x = m11*x + m12*y + m13*z + m14;
   res.y = m21*x + m22*y + m23*z + m24;
   res.z = m31*x + m32*y + m33*z + m34;
   return res;
}
 