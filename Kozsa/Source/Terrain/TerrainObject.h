#pragma once

#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtx/quaternion.hpp>

#include "../Physics/Physics.h"
#include "../GameObject.h"
#include "../Renderer.h"
#include "../Frustum.h"
#include "TerrainCell.h"

/**
 * Container object for holding all state relevant for a single
 * terrain object entity.
 */
class TerrainObject : public GameObject
{
public:
	// Mesh Data
	TerrainCell*	mCells;
	Heightmap*		mHeightmap;
	TerrainModel*	mModel;
	std::vector<Texture2D> mTextures;
	int mVertexCount, mIndexCount;

	// State
	// Width of the terrain
	float mWidth;
	
	// Height of the terrain
	float mHeight;

	// Scale of the terrain height
	float mHeightScale;

	// RigidBody of the terrain
	PxRigidStatic* mRigidBody;

	// Number of cells draw, culled and total
	int mCellsCount;
	int mCellsCulledCount;
	int mCellsDrawnCount;

	unsigned short* rawImage;

public:
	// Constructor
	TerrainObject(const float width = 1025.0f, const float height = 1025.0f,
		const float heightScale = 300.0f, const glm::vec3 position = glm::vec3(0.0f), const glm::vec3 size = glm::vec3(1.0f));
	// Constructor
	TerrainObject(std::string file, std::string blendMap, const float width = 1025.0f, const float height = 1025.0f,
		const float heightScale = 300.0f, const glm::vec3 position = glm::vec3(0.0f), const glm::vec3 size = glm::vec3(1.0f));

	// Destructor
	~TerrainObject() override;

	// Updates the state
	void Update(GLfloat dt) override;

	// Draws the terrain object
	void Draw(Renderer& renderer, Frustum* frustum);

	// Draws the terrain cell lines
	void DrawDebugLines(Renderer& renderer);

private:
	void loadPlaneHeightmap();
	void loadHeightmap(std::string file);
	void loadTargaHeightmap(std::string file);
	void shutdownHeightMap();
	void setTerrainCoordinates();
	void calculateNormals();
	void loadBlendMap(std::string path);
	void buildTerrainModel();
	void shutdownTerrainModel();
	void calculateTerrainVectors();
	void calculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, Vector& tangent, Vector& binormal);
	void loadTerrainCells();
	void setupRigidBody();
};