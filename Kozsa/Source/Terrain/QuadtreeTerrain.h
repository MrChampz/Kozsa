/*******************************************************************************
* File:           QuadtreeTerrain.h
* Author:         Christian Alfons
* Date created:   2010-06-31
* Date modified:  2011-02-13
* Description:    Quadtree chunked level-of-detail terrain class. Each instance
*                 has a number of faces, which in turn hold quadtree nodes.
*                 This class also holds data shared by the faces, such as
*                 camera position and view frustum.
*                 Perhaps most importantly, this class is used to communicate
*                 terrain data outside the quadtree structure.
*******************************************************************************/

#ifndef QUADTREETERRAIN_H_INCLUDED
#define QUADTREETERRAIN_H_INCLUDED

#include "QuadtreeTerrainFace.h"

#include <GLM/glm.hpp>

class QuadtreeTerrain
{
  protected:
    // Instance variables
    vector<QuadtreeTerrainFace*>  faces;
    glm::vec3					  cameraPosition;

  public:
    // Constructor
    QuadtreeTerrain();

    // Destructor
    virtual ~QuadtreeTerrain();

    // Instance methods
    virtual void            Rebuild();
    glm::vec3               GetCameraPositionLocal() const;
    void                    SetCameraPosition(const glm::vec3 &cameraPosition);
    virtual double          GetTerrainHeight(const glm::vec3 &point) const = 0;
    virtual glm::vec3       ProjectOnSurface(const glm::vec3 &point) const = 0;
    virtual void            Render() const;
    virtual void            Update();
};

#endif
