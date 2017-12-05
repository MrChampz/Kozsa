#include "QuadtreeTerrainPatch.h"
#include "QuadtreeTerrainFace.h"
#include "QuadtreeTerrain.h"
#include "../../Shaders/ShaderManager.h"
#include "../../Math/PerlinNoise.h"

#define BUFFER_OFFSET(i) ((char*)0 + (i))
#define TEXTURE_SIZE        192
#define TEXTURE_SIZE_BORDER (TEXTURE_SIZE + 2)
#define TEXEL_SIZE          (1.0 / TEXTURE_SIZE)
#define TEXEL_SIZE_BORDER   (1.0 / TEXTURE_SIZE_BORDER)

using std::max;
using std::min;

unsigned int QuadtreeTerrainPatch::instanceCount = 0;
QuadtreeTerrainPatchTopology* QuadtreeTerrainPatch::topologies[MAX_DETAIL_DIFFERENCE + 1][MAX_DETAIL_DIFFERENCE + 1][MAX_DETAIL_DIFFERENCE + 1][MAX_DETAIL_DIFFERENCE + 1] = { { { { 0 } } } };
GLuint QuadtreeTerrainPatch::fbos[NUM_FBOS] = { 0 };
GLuint QuadtreeTerrainPatch::texPositionmap = 0;

// Generates all ((MAX_DETAIL_DIFFERENCE + 1)^4) possible topologies
void QuadtreeTerrainPatch::GenerateTopologies()
{
  // Generate topology information for all possible detail difference combinations
  for (unsigned int a = 0; a <= MAX_DETAIL_DIFFERENCE; a++)
    for (unsigned int b = 0; b <= MAX_DETAIL_DIFFERENCE; b++)
      for (unsigned int c = 0; c <= MAX_DETAIL_DIFFERENCE; c++)
        for (unsigned int d = 0; d <= MAX_DETAIL_DIFFERENCE; d++)
          topologies[a][b][c][d] = new QuadtreeTerrainPatchTopology(a, b, c, d);
}

void QuadtreeTerrainPatch::DeleteTopologies()
{
  for (unsigned int a = 0; a <= MAX_DETAIL_DIFFERENCE; a++)
    for (unsigned int b = 0; b <= MAX_DETAIL_DIFFERENCE; b++)
      for (unsigned int c = 0; c <= MAX_DETAIL_DIFFERENCE; c++)
        for (unsigned int d = 0; d <= MAX_DETAIL_DIFFERENCE; d++)
        {
          delete topologies[a][b][c][d];
          topologies[a][b][c][d] = 0;
        }
}

void QuadtreeTerrainPatch::RenderFullscreenSquare(const Vector3<double> patchCorners[4])
{
  // Render a square (actually a triangle strip with only two triangles)
  glBegin(GL_TRIANGLE_STRIP);
  glTexCoord3dv(patchCorners[SOUTH_WEST].data);
  glVertex2i(-1, -1);
  glTexCoord3dv(patchCorners[SOUTH_EAST].data);
  glVertex2i(1, -1);
  glTexCoord3dv(patchCorners[NORTH_WEST].data);
  glVertex2i(-1, 1);
  glTexCoord3dv(patchCorners[NORTH_EAST].data);
  glVertex2i(1, 1);
  glEnd();
}

void QuadtreeTerrainPatch::RenderTextures(const QuadtreeTerrainNode *node)
{
  // Generate FBOs
  if (!fbos[0])
    glGenFramebuffers(NUM_FBOS, fbos);

  // Create positionmap texture
  if (!texPositionmap)
  {
    // Create position/height texture (one pixel padding for normal calculations)
    glGenTextures(1, &texPositionmap);
    glBindTexture(GL_TEXTURE_2D, texPositionmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_SIZE_BORDER, TEXTURE_SIZE_BORDER, 0, GL_BGRA, GL_FLOAT, 0);
  }

  // Create normalmap texture
  glGenTextures(1, &texNormalmap);
  glBindTexture(GL_TEXTURE_2D, texNormalmap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_BGR, GL_UNSIGNED_BYTE, 0);

  // Create heightmap texture
  glGenTextures(1, &texHeightmap);
  glBindTexture(GL_TEXTURE_2D, texHeightmap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_LUMINANCE, GL_FLOAT, 0);

  // Get node properties
  const double scale              = node->GetScale();
  const Vector2<double> nodePos   = node->GetPosition();
  const QuadtreeTerrainFace *face = node->GetFace();
  Vector3<double> patchCornersBorder[4];
  for (int i = 0; i < 4; i++)
  {
    // Calculate corner vertex position
    const int east  = ADJACENT(EAST,  i) ? 1 : -1;
    const int north = ADJACENT(NORTH, i) ? 1 : -1;
    const Vector2<double> offset(east * scale, north * scale);
    const Vector2<double> treePos       = nodePos + offset;

    // A texel in world space is 2.0 [cube size] * scale / (TEXTURE_SIZE - 1), since only half of the outer texel is used
    // Add the texel size multiplied by 1.5 to get the extra one-and-a-half border texel
    const Vector2<double> treePosBorder = treePos + offset * 3.0 / (TEXTURE_SIZE - 1);
    patchCornersBorder[i]               = face->GetWorldPosition(treePosBorder);
  }

  // Bind the FBO and attach texture to FBO attachment point
  glBindFramebuffer(GL_FRAMEBUFFER, fbos[0]);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texPositionmap, 0);

  // Store render state
  glPushAttrib(GL_LIGHTING_BIT | GL_VIEWPORT_BIT | GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);
  glViewport(0, 0, TEXTURE_SIZE_BORDER, TEXTURE_SIZE_BORDER);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // Render positionmap
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, PerlinNoise<double>::GetPermutationTexture());
  ShaderManager::UseShaderProgram(PATCH_POSITIONMAP);
  static const GLint uniformPermTex = glGetUniformLocation(ShaderManager::GetShaderProgramHandle(PATCH_POSITIONMAP), "permTex");
  glUniform1i(uniformPermTex, 0);
  RenderFullscreenSquare(patchCornersBorder);

  // Change FBO
  glBindFramebuffer(GL_FRAMEBUFFER, fbos[1]);
  glViewport(0, 0, TEXTURE_SIZE, TEXTURE_SIZE);

  // Use positionmap
  glBindTexture(GL_TEXTURE_2D, texPositionmap);
  static const Vector3<double> heightmapCorners[4] = { Vector3<double>(       TEXEL_SIZE_BORDER, 1.0 - TEXEL_SIZE_BORDER, 0.0),
                                                       Vector3<double>( 1.0 - TEXEL_SIZE_BORDER, 1.0 - TEXEL_SIZE_BORDER, 0.0),
                                                       Vector3<double>( 1.0 - TEXEL_SIZE_BORDER,       TEXEL_SIZE_BORDER, 0.0),
                                                       Vector3<double>(       TEXEL_SIZE_BORDER,       TEXEL_SIZE_BORDER, 0.0) };

  // Attach both normalmap and heightmap textures to FBO attachment points
  static const GLenum drawbuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers(2, drawbuffers);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texNormalmap, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texHeightmap, 0);

  // Render normalmap and heightmap
  ShaderManager::UseShaderProgram(PATCH_NORMALMAP_HEIGHTMAP);
  static const GLint uniformPositionmap = glGetUniformLocation(ShaderManager::GetShaderProgramHandle(PATCH_NORMALMAP_HEIGHTMAP), "positionmap");
  glUniform1i(uniformPositionmap, 0);
  static const GLint uniformTexelSize = glGetUniformLocation(ShaderManager::GetShaderProgramHandle(PATCH_NORMALMAP_HEIGHTMAP), "texelSize");
  glUniform1f(uniformTexelSize, TEXEL_SIZE_BORDER);
  RenderFullscreenSquare(heightmapCorners);

  // Go back to perspective projection and restore render state
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopAttrib();

  const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
    printf("FBO ERROR: %d\n", status);

  // Unbind the FBO; return to default state
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
}

QuadtreeTerrainPatch::QuadtreeTerrainPatch(const QuadtreeTerrainNode *node)
  : node(node), vbo(0), verts(), texHeightmap(0), texNormalmap(0)
{
  // If first instance to be created
  if (!instanceCount++)
    GenerateTopologies();

  // Get scale and position
  const double scale                          = node->GetScale();
  const Vector2<double> position              = node->GetPosition();
  const QuadtreeTerrainFace *face             = node->GetFace();
  const unsigned int quadrant                 = node->GetQuadrant();
  const QuadtreeTerrainNode *nodeParent       = node->GetParent();
  const QuadtreeTerrainPatch *nodeParentPatch = nodeParent ? nodeParent->GetPatch() : 0;
  const Vector2<int> nodeParentPatchOffset    = nodeParentPatch ? Vector2<int>(ADJACENT(EAST,  quadrant) ? PATCH_SIZE_HALF : 0, ADJACENT(NORTH,  quadrant) ? PATCH_SIZE_HALF : 0) : Vector2<int>(0, 0);

  for (int x = 0; x < PATCH_VERTICES; x++)
  {
    for (int y = 0; y < PATCH_VERTICES; y++)
    {
      // If even row and column; use parent's height value
      if (nodeParentPatch && x % 2 == 0 && y % 2 == 0)
        verts[INDEX1D(x,y)] = nodeParentPatch->verts[INDEX1D(nodeParentPatchOffset.x + x / 2, nodeParentPatchOffset.y + y / 2)];
      else
      {
        // Get the vertex position in the quad tree
        const Vector2<double> treePos = (Vector2<double>((double)x, (double)y) - PATCH_SIZE_HALF) * scale / PATCH_SIZE_HALF + position;
        const Vector3<double> vertPos = face->GetTerrain()->ProjectOnSurface(face->GetWorldPosition(treePos));
        verts[INDEX1D(x,y)].position = Vector3<float>(vertPos.x, vertPos.y, vertPos.z);
        verts[INDEX1D(x,y)].normal = verts[INDEX1D(x,y)].position.GetNormalized();
      }

      // Calculate texture coordinates [0,1]
      Vector2<float> texCoord = Vector2<float>(x, y) / PATCH_SIZE;

      // Use only half of the outer texel
      texCoord = texCoord * (1.0 - TEXEL_SIZE) + 0.5 * TEXEL_SIZE;
      verts[INDEX1D(x,y)].texCoord = texCoord;
    }
  }

  RenderTextures(node);

  // Generate the VBO
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex<float>) * PATCH_VERTICES_TOTAL, 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex<float>) * PATCH_VERTICES_TOTAL, verts);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

QuadtreeTerrainPatch::~QuadtreeTerrainPatch()
{
  // If last instance to be deleted
  if (!--instanceCount)
  {
    glDeleteFramebuffers(NUM_FBOS, fbos);
    for (int i = 0; i < NUM_FBOS; i++)
      fbos[i] = 0;

    glDeleteTextures(1, &texPositionmap);
    texPositionmap = 0;

    DeleteTopologies();
  }

  glDeleteBuffers(1, &vbo);
  glDeleteTextures(1, &texHeightmap);
  glDeleteTextures(1, &texNormalmap);
}

const QuadtreeTerrainPatchTopology* QuadtreeTerrainPatch::GetTopology() const
{
  if (!node)
    return topologies[0][0][0][0];
  return topologies[node->GetNeighborDetailDifference(NORTH)][node->GetNeighborDetailDifference(EAST)][node->GetNeighborDetailDifference(SOUTH)][node->GetNeighborDetailDifference(WEST)];
}

void QuadtreeTerrainPatch::Render() const
{
  // Get the patch topology for the current neighbor detail differences
  const QuadtreeTerrainPatchTopology *topology = GetTopology();
  if (!topology)
    return;

  // Bind VBO and IBO
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topology->GetIbo());

  // Set data pointers
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glTexCoordPointer(2,  GL_FLOAT, sizeof(Vertex<float>), BUFFER_OFFSET(sizeof(Vector3<float>) * 2));
  glNormalPointer(      GL_FLOAT, sizeof(Vertex<float>), BUFFER_OFFSET(sizeof(Vector3<float>)));
  glVertexPointer(  3,  GL_FLOAT, sizeof(Vertex<float>), BUFFER_OFFSET(0));

  // Bind texture
  static const GLint uniformHeightmap = glGetUniformLocation(ShaderManager::GetShaderProgramHandle(PLANET), "heightmap");
  glUniform1i(uniformHeightmap, 1);
  static const GLint uniformNormalmap = glGetUniformLocation(ShaderManager::GetShaderProgramHandle(PLANET), "normalmap");
  glUniform1i(uniformNormalmap, 2);

  // Render
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texHeightmap);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, texNormalmap);
  glDrawElements(GL_TRIANGLES, topology->GetVertexCount(), GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0);

  // Unbind VBO and IBO
  glBindBuffer(GL_ARRAY_BUFFER,         0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void QuadtreeTerrainPatch::GetBoundingSphere(Vector3<double> &boundingSphereCenter, double &boundingSphereRadius) const
{
  // Set initial values
  Vector3<float>  boundingSphereCenterF = Vector3<float>(0.0, 0.0, 0.0);
  float           boundingSphereRadiusF = 0.0;

  // Calculate mean position and use as bounding sphere center
  for (int i = 0; i < PATCH_VERTICES_TOTAL; i++)
    boundingSphereCenterF += verts[i].position;
  boundingSphereCenterF /= (float)PATCH_VERTICES_TOTAL;

  // Find the largest distance from the center to a vertex
  for (int i = 0; i < PATCH_VERTICES_TOTAL; i++)
    boundingSphereRadiusF = max(boundingSphereRadiusF, (verts[i].position - boundingSphereCenterF).GetSquaredLength());
  boundingSphereRadiusF = sqrt(boundingSphereRadiusF);

  // Convert from float to double (we prefer double, but the vertices aren't stored in double precision)
  boundingSphereCenter = Vector3<double>(boundingSphereCenterF.x, boundingSphereCenterF.y, boundingSphereCenterF.z);
  boundingSphereRadius = (double)boundingSphereRadiusF;
}
