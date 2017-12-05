#pragma once

#include <GL/glew.h>
#include <GLM/glm.hpp>

#include "../Renderer.h"

// Represents a terrain vertex
struct TerrainVertex
{
	// Position
	glm::vec3 Position;

	// Normal
	glm::vec3 Normal;

	// Tangent
	glm::vec3 Tangent;

	// Binormal
	glm::vec3 Binormal;

	// TexCoords
	glm::vec2 TexCoords;

	// Color
	glm::vec3 Color;
};

// Represents a line vertex
struct LineVertex
{
	// Position
	glm::vec3 Position;

	// Color
	glm::vec3 Color;
};

struct TerrainModel
{
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
	float tx, ty, tz;
	float bx, by, bz;
	float r, g, b;
};

struct Heightmap
{
	float x, y, z;
	float nx, ny, nz;
	float r, g, b;
};

struct Vector
{
	float x, y, z;
};

struct TempVertexType
{
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
};

/**
 * Container object for holding all state relevant for a single
 * terrain cell entity.
 */
class TerrainCell 
{
public:
	// Render Data
	GLuint mVAO, mVBO, mEBO;
	GLuint mLineVAO, mLineVBO, mLineEBO;

	// Mesh Data
	TerrainVertex*	mVertices;
	GLuint*			mIndices;
	LineVertex*		mLineVertices;
	GLuint*			mLineIndices;
	int				mVertexCount, mIndexCount;
	int				mLineVertexCount, mLineIndexCount;

	// State
	// Width of the cell
	float mWidth;
	
	// Height of the cell
	float mHeight;

	// Width of the terrain
	float mTerrainWidth;

	// Used to calculate cell debug lines
	float mMaxWidth, mMaxHeight, mMaxDepth;
	float mMinWidth, mMinHeight, mMinDepth;
	float mPositionX, mPositionY, mPositionZ;

public:
	// Constructor
	TerrainCell();

	// Constructor
	TerrainCell(TerrainVertex* vertices, GLuint* indices, int vertexCount, int indexCount, float width, float height, float terrainWidth);

	// Destructor
	~TerrainCell();

	// Updates the state
	void Update(GLfloat dt);

	// Draws the terrain cell object
	void Draw(Renderer& renderer);

	// Draws the terrain cell debug lines
	void DrawDebugLines(Renderer& renderer);

private:
	void calculateCellDimensions();
	void setupMesh();
	void shutdownMesh();
	void setupLinesMesh();
	void shutdownLineMesh();
};