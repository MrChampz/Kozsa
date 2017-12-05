/*******************************************************************************
* File:           QuadtreeTerrainFace.h
* Author:         Christian Alfons
* Date created:   2010-10-05
* Date modified:  2010-10-05
* Description:    Quadtree chunked level-of-detail terrain face class. Each
*                 face holds data shared by all of its nodes. Only the quadtree
*                 top node is visible to the face, but all nodes can access
*                 face data.
*******************************************************************************/

#ifndef QUADTREETERRAINFACE_H_INCLUDED
#define QUADTREETERRAINFACE_H_INCLUDED

#include "QuadtreeTerrainNode.h"

#include <math.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#define FACE_NEAR   0
#define FACE_LEFT   1
#define FACE_FAR    2
#define FACE_RIGHT  3
#define FACE_TOP    4
#define FACE_BOTTOM 5

#define M_PI 3.14159265358979323846264338327950288

class QuadtreeTerrain;

class QuadtreeTerrainFace
{
  protected:
    // Instance variables
	glm::vec3 position;
	glm::mat4 orientation;
    const QuadtreeTerrain *terrain;
    const unsigned int    faceType;
    double                positionZ;
    QuadtreeTerrainNode   *topNode;
    int                   splitsRemaining;

  public:
    // Constructor
    QuadtreeTerrainFace(const QuadtreeTerrain *terrain, const unsigned int faceType = FACE_NEAR, const double positionZ = 0.0);

    // Destructor
    virtual ~QuadtreeTerrainFace();

    // Instance methods
    bool                    HasSplitsLeft();
    void                    Rebuild();
    void                    Connect(const unsigned int side, QuadtreeTerrainFace *face);
    const QuadtreeTerrain*  GetTerrain() const;
    unsigned int            GetFaceType() const;
    void                    Render() const;
    void                    Update();
	glm::vec3               GetWorldPosition(const glm::vec2 &p) const;
};

#endif
