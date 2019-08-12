#include <vector>
using namespace std;
typedef struct Vetor
{
  float x;
  float y;
  float z;
  
} Vetor;
typedef struct Sphere
{
  float radius;
  int stacks;
  int sectors;
  float sectorStep;
  float stackStep;
  vector<float> vertices;
  vector<float> normals;
  vector<Vetor> heights;
  vector<float> texCoords;
  vector<unsigned int> indices;
  vector<unsigned int> lineIndices;
} Sphere;

Sphere buildVerticesSphere(Sphere sphere, const char* hmfileName, int is_img, const char* hmfileNameTxt); 
Sphere init(Sphere Sphere, float new_radius, int new_stacks, int new_sectors);
Vetor getPolarCoordinates(Sphere sphere, Vetor vetor, float h);
Vetor new_vetor(float x, float y, float z);
void print_vetor(Vetor a);
Vetor rotate_vec(Vetor v, float angle, char axis);
Vetor get_height(Sphere sphere, Vetor polar);
float get_norm(Vetor vetor);
Vetor sub_vec(Vetor a, Vetor b);
Vetor sum_vec(Vetor a, Vetor b);
Vetor direction_vec(Vetor a, Vetor b);
Vetor norm(Vetor a);
Vetor rotate_in_arbitrary_axis(Vetor xyz, Vetor p1, Vetor p2, float angle);
Vetor inc_vec(Vetor a, Vetor direction, float taxa);