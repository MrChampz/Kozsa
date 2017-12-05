#include "TerrainCell.h"

TerrainCell::TerrainCell()
	: mVertices(0), mIndices(0), mLineVertices(0), mLineIndices(0), mVertexCount(0), mIndexCount(0),
	  mLineVertexCount(0), mLineIndexCount(0), mWidth(0), mHeight(0), mTerrainWidth(0)
{
}

TerrainCell::TerrainCell(TerrainVertex* vertices, GLuint* indices, int vertexCount, int indexCount, float width, float height,
						 float terrainWidth)
	: mVertices(0), mIndices(0), mLineVertices(0), mLineIndices(0), mVertexCount(0), mIndexCount(0),
	  mLineVertexCount(0), mLineIndexCount(0), mWidth(width), mHeight(height), mTerrainWidth(terrainWidth)
{
	// Set the vertex and index count
	this->mVertexCount = vertexCount;
	this->mIndexCount = indexCount;

	// Initialize the vertex and index arrays
	this->mVertices = new TerrainVertex[this->mVertexCount];
	this->mIndices = new GLuint[this->mIndexCount];

	// Copy vertex and index data on arrays
	this->mVertices = vertices;
	this->mIndices = indices;

	this->calculateCellDimensions();
	this->setupMesh();
	this->shutdownMesh();
	this->setupLinesMesh();
	this->shutdownLineMesh();
}

TerrainCell::~TerrainCell()
{
	// Delete the Line EBO, VBO and VAO
	glDeleteBuffers(1, &this->mLineEBO);
	glDeleteBuffers(1, &this->mLineVBO);
	glDeleteVertexArrays(1, &this->mLineVAO);

	// Delete the EBO, VBO and VAO
	glDeleteBuffers(1, &this->mEBO);
	glDeleteBuffers(1, &this->mVBO);
	glDeleteVertexArrays(1, &this->mVAO);
}

void TerrainCell::Update(GLfloat dt)
{
}

void TerrainCell::Draw(Renderer& renderer)
{
	// Render the terrain cell
	glBindVertexArray(this->mVAO);
	glDrawElements(GL_TRIANGLES, this->mIndexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void TerrainCell::DrawDebugLines(Renderer& renderer)
{
	// Render the terrain cell lines
	glBindVertexArray(this->mLineVAO);
	glDrawElements(GL_LINES, this->mLineIndexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void TerrainCell::calculateCellDimensions()
{
	float width, height, depth;

	// Initialize the dimensions of the node.
	mMaxWidth = -1000000.0f;
	mMaxHeight = -1000000.0f;
	mMaxDepth = -1000000.0f;

	mMinWidth = 1000000.0f;
	mMinHeight = 1000000.0f;
	mMinDepth = 1000000.0f;

	for (int i = 0; i < this->mVertexCount; i++)
	{
		width = this->mVertices[i].Position.x;
		height = this->mVertices[i].Position.y;
		depth = this->mVertices[i].Position.z;

		// Check if the width exceeds the minimum or maximum.
		if (width > mMaxWidth)
		{
			mMaxWidth = width;
		}
		if (width < mMinWidth)
		{
			mMinWidth = width;
		}

		// Check if the height exceeds the minimum or maximum.
		if (height > mMaxHeight)
		{
			mMaxHeight = height;
		}
		if (height < mMinHeight)
		{
			mMinHeight = height;
		}

		// Check if the depth exceeds the minimum or maximum.
		if (depth > mMaxDepth)
		{
			mMaxDepth = depth;
		}
		if (depth < mMinDepth)
		{
			mMinDepth = depth;
		}
	}

	// Calculate the center position of this cell.
	mPositionX = (mMaxWidth - mMinWidth) + mMinWidth;
	mPositionY = (mMaxHeight - mMinHeight) + mMinHeight;
	mPositionZ = (mMaxDepth - mMinDepth) + mMinDepth;
}

void TerrainCell::setupMesh()
{
	// Create buffers/arrays
	glGenVertexArrays(1, &this->mVAO);
	glGenBuffers(1, &this->mVBO);
	glGenBuffers(1, &this->mEBO);

	// Bind VAO
	glBindVertexArray(this->mVAO);

	// Load vertex data into vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->mVBO);
	glBufferData(GL_ARRAY_BUFFER, this->mVertexCount * sizeof(TerrainVertex), &this->mVertices[0], GL_STATIC_DRAW);

	// Load index data into index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->mIndexCount * sizeof(GLuint), &this->mIndices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (GLvoid*)0);
	
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (GLvoid*)offsetof(TerrainVertex, Normal));

	// Vertex Tangents
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (GLvoid*)offsetof(TerrainVertex, Tangent));

	// Vertex Binormals
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (GLvoid*)offsetof(TerrainVertex, Binormal));

	// Vertex Texture Coords
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (GLvoid*)offsetof(TerrainVertex, TexCoords));

	// Vertex Colors
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (GLvoid*)offsetof(TerrainVertex, Color));

	// Unbind VAO
	glBindVertexArray(0);
}

void TerrainCell::shutdownMesh()
{
	delete[] this->mVertices;
	this->mVertices = 0;

	delete[] this->mIndices;
	this->mIndices = 0;
}

void TerrainCell::setupLinesMesh()
{
	// Set the number of vertices in the vertex array.
	this->mLineVertexCount = 24;

	// Set the number of indices in the index array.
	this->mLineIndexCount = 24;

	this->mLineVertices = new LineVertex[this->mLineVertexCount];
	this->mLineIndices = new GLuint[this->mLineIndexCount];

	// Setup vertices, indices, and create buffers
	// 8 Horizontal lines.
	int index = 0;
	LineVertex vertex;
	glm::vec3 lineColor = glm::vec3(1.0f, 0.5f, 0.0f);

	vertex.Position = glm::vec3(mMinWidth, mMinHeight, mMinDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMaxWidth, mMinHeight, mMinDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMinWidth, mMinHeight, mMaxDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMaxWidth, mMinHeight, mMaxDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMinWidth, mMinHeight, mMinDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMinWidth, mMinHeight, mMaxDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMaxWidth, mMinHeight, mMinDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMaxWidth, mMinHeight, mMaxDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMinWidth, mMaxHeight, mMinDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMaxWidth, mMaxHeight, mMinDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMinWidth, mMaxHeight, mMaxDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMaxWidth, mMaxHeight, mMaxDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMinWidth, mMaxHeight, mMinDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMinWidth, mMaxHeight, mMaxDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMaxWidth, mMaxHeight, mMinDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMaxWidth, mMaxHeight, mMaxDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	// 4 Vertical lines.
	vertex.Position = glm::vec3(mMaxWidth, mMaxHeight, mMaxDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMaxWidth, mMinHeight, mMaxDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMinWidth, mMaxHeight, mMaxDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMinWidth, mMinHeight, mMaxDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMaxWidth, mMaxHeight, mMinDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMaxWidth, mMinHeight, mMinDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMinWidth, mMaxHeight, mMinDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;
	index++;

	vertex.Position = glm::vec3(mMinWidth, mMinHeight, mMinDepth);
	vertex.Color = lineColor;
	this->mLineVertices[index] = vertex;
	this->mLineIndices[index] = index;

	// Create buffers/arrays
	glGenVertexArrays(1, &this->mLineVAO);
	glGenBuffers(1, &this->mLineVBO);
	glGenBuffers(1, &this->mLineEBO);

	// Bind VAO
	glBindVertexArray(this->mLineVAO);

	// Load vertex data into vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->mLineVBO);
	glBufferData(GL_ARRAY_BUFFER, this->mLineVertexCount * sizeof(LineVertex), &this->mLineVertices[0], GL_STATIC_DRAW);

	// Load index data into index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mLineEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->mLineIndexCount * sizeof(GLuint), &this->mLineIndices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (GLvoid*)0);

	// Vertex Colors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (GLvoid*)offsetof(LineVertex, Color));

	// Unbind VAO
	glBindVertexArray(0);
}

void TerrainCell::shutdownLineMesh()
{
	delete[] this->mLineVertices;
	this->mLineVertices = 0;

	delete[] this->mLineIndices;
	this->mLineIndices = 0;
}
