#include <vector>
using namespace std;
typedef struct Sphere
{
  float radius;
  int stacks;
  int sectors;
  vector<float> vertices;
  vector<float> normals;
  vector<float> texCoords;
  vector<unsigned int> indices;
  vector<unsigned int> lineIndices;
} Sphere;

Sphere buildVerticesSphere(Sphere sphere); 
Sphere init(Sphere Sphere, float new_radius, int new_stacks, int new_sectors);