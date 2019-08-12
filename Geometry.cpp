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
    float height_res = (float)src(x,y,0,0)/255;
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
    return float(data[(i * width + j)+ channel]- 128) /128;
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
    std::vector<Vetor>().swap(sphere.heights);
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
         
        
        float height_0n =  (rand() % 500)/(float)1000; 
        
        
        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sphere.sectorStep;           // starting from 0 to 2pi

            // vertex position
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            //y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            z= xy * sinf(sectorAngle);
          
                                   
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
            height *= 0.08;
            Vetor v;        
            v.x = nx*height;
            v.y = ny*height;
            v.z = nz*height;
            
            sphere.heights.push_back(v);
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
Vetor get_height(Sphere sphere, Vetor polar){
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
                   // " ( "<< vetor.x << ", "<< vetor.y << ", "<< vetor.z << ", "<< stackAngle << ", "<< stack << ", "<< sectorAngle << ", "<< sector <<  ")" << endl << endl;
    
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
   Vetor abc =sub_vec(p1, p2);
   Vetor uvw = norm(abc);
   print_vetor(uvw);
   print_vetor(xyz);
   Vetor res;
   res.x = (abc.x*(uvw.y*uvw.y + uvw.z*uvw.z) - uvw.x*(abc.y*uvw.y + abc.z*uvw.z - uvw.x*xyz.x - uvw.y*xyz.y - uvw.z*xyz.z))*(1-cos(angle)) + xyz.x*cos(angle) + (-abc.z*uvw.y + abc.y*uvw.z - uvw.z*xyz.y + uvw.y*xyz.z)*sin(angle);
   res.y = (abc.y*(uvw.x*uvw.x + uvw.z*uvw.z) - uvw.y*(abc.x*uvw.x + abc.z*uvw.z - uvw.x*xyz.x - uvw.y*xyz.y - uvw.z*xyz.z))*(1-cos(angle)) + xyz.y*cos(angle) + (abc.z*uvw.x + abc.x*uvw.z - uvw.z*xyz.x + uvw.x*xyz.z)*sin(angle);
   res.x = (abc.z*(uvw.x*uvw.x + uvw.y*uvw.y) - uvw.z*(abc.x*uvw.x + abc.y*uvw.y - uvw.x*xyz.x - uvw.y*xyz.y - uvw.z*xyz.z))*(1-cos(angle)) + xyz.z*cos(angle) + (-abc.y*uvw.x + abc.x*uvw.y - uvw.y*xyz.x + uvw.x*xyz.y)*sin(angle);
   print_vetor(res);
   return res;
}
 