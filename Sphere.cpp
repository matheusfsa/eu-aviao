#include "Sphere.h" 
#include <iostream>
#include <math.h>
using namespace std; 
Sphere init(Sphere sphere, float new_radius, int new_stacks, int new_sectors){
    sphere.radius = new_radius;
    sphere.stacks = new_stacks;
    sphere.sectors = new_sectors;
    return sphere;
}

Sphere buildVerticesSphere(Sphere sphere)
{
    const float PI = 3.1415926f;
    int sectorCount = sphere.sectors;
    int stackCount = sphere.stacks;
    std::vector<float>().swap(sphere.vertices);
    std::vector<float>().swap(sphere.normals);
    std::vector<float>().swap(sphere.texCoords);
    std::vector<unsigned int>().swap(sphere.indices);
    std::vector<unsigned int>().swap(sphere.lineIndices);

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / sphere.radius;    // normal
    float s, t;                                     // texCoord

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;
    float height_polo_n =  (rand() % 10)/(float)1000; 
    float height_polo_s =  (rand() % 10)/(float)1000; 
    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = sphere.radius * cosf(stackAngle);             // r * cos(u)
        z = sphere.radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        float height_0n =  (rand() % 10)/(float)1000; 

        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

            // vertex tex coord between [0, 1]
       
            // normalized vertex normal
            nx = x * lengthInv;
            ny = y * lengthInv; 
            nz = z * lengthInv; 
             
            //valor que eu inseri para perturbar a superfície da esfera. 
            //Vocês devem usar valores de um mapa de altitude (heightmap) no lugar de height.
            float height;
            height = (rand() % 50)/(float)1000; 
            
            if(j == 0 || j == sectorCount){
                height =height_0n;
            }
            if(i == 0)
                height = height_polo_n;
            if(i == stackCount)
                height = height_polo_s;
            

            
            sphere.vertices.push_back(x+nx*height);
            sphere.vertices.push_back(y+ny*height);
            sphere.vertices.push_back(z+nz*height);
            //adiciona vetores normais ao array de normais
            sphere.normals.push_back(nx);
            sphere.normals.push_back(ny);
            sphere.normals.push_back(nz);

                
            s = (float)j / (sectorCount);
            t = (float)i / stackCount;   

           
            sphere.texCoords.push_back(s);
            sphere.texCoords.push_back(t);
            
        }
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
