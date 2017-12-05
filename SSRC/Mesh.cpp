#include "Mesh.h"

Mesh::Mesh()
	: mVertices(), mIndices(), mTextures()
{
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture2D> textures)
{
	this->mVertices = vertices;
	this->mIndices = indices;
	this->mTextures = textures;

	this->setupMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture2D> textures, std::vector<BoneData> boneData)
{
	this->mVertices = vertices;
	this->mIndices = indices;
	this->mTextures = textures;
	this->mBoneData = boneData;

	this->setupMesh();
}

Mesh::~Mesh()
{
}

void Mesh::Draw(Shader& shader)
{
	// Bind appropriate textures
	GLuint albedoNr = 1;
	GLuint normalNr = 1;
	GLuint roughnessNr = 1;
	GLuint metallicNr = 1;
	GLuint aoNr = 1;
	for (GLuint i = 0; i < this->mTextures.size(); i++)
	{
		// Active proper texture unit before binding
		glActiveTexture(GL_TEXTURE0 + i);

		// Retrieve texture number (the N in diffuse_textureN)
		TextureType type = mTextures[i].mType;
		std::string number;
		std::string name;
		if (type == TEXTURE_TYPE_ALBEDO)
		{
			number = std::to_string(albedoNr++);
			name = "texture_albedo";
		}
		else if (type == TEXTURE_TYPE_NORMAL)
		{
			number = std::to_string(normalNr++);
			name = "texture_normal";
		}
		else if (type == TEXTURE_TYPE_ROUGHNESS)
		{
			number = std::to_string(roughnessNr++);
			name = "texture_roughness";
		}
		else if (type == TEXTURE_TYPE_METALLIC)
		{
			number = std::to_string(metallicNr++);
			name = "texture_metallic";
		}
		else if (type == TEXTURE_TYPE_AO)
		{
			number = std::to_string(aoNr++);
			name = "texture_ao";
		}

		// Now set the sampler to the correct texture unit
		shader.SetInteger((name + number).c_str(), i);

		// And finally bind the texture
		this->mTextures[i].Bind();
	}

	// Draw mesh 
	glBindVertexArray(this->mVAO);
	glDrawElements(GL_TRIANGLES, this->mIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Set everything back to defaults once configured
	for (GLuint i = 0; i < this->mTextures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		this->mTextures[i].Unbind();
	}
}

void Mesh::setupMesh()
{
	// Create buffers/arrays
	glGenVertexArrays(1, &this->mVAO);
	glGenBuffers(1, &this->mVBO);
	glGenBuffers(1, &this->mEBO);

	// Bind VAO
	glBindVertexArray(this->mVAO);

	// Load vertex data into vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->mVBO);
	glBufferData(GL_ARRAY_BUFFER, this->mVertices.size() * sizeof(Vertex), &this->mVertices[0], GL_STATIC_DRAW);

	// Load index data into index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->mIndices.size() * sizeof(GLuint), &this->mIndices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

	// Vertex Tangent
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));

	// Vertex BiTangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, BiTangent));

	// Vertex Texture Coords
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

	if (mBoneData.size() > 0)
	{
		GLuint boneVBO;
		glGenBuffers(1, &boneVBO);
		glBindBuffer(GL_ARRAY_BUFFER, boneVBO);
		glBufferData(GL_ARRAY_BUFFER, this->mBoneData.size() * sizeof(BoneData), &this->mBoneData[0], GL_STATIC_DRAW);

		// BoneIndices1
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(BoneData), (GLvoid*)0);

		// BoneIndices2
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(BoneData), (GLvoid*)offsetof(BoneData, BoneIndices2));

		// BoneWeights1
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(BoneData), (GLvoid*)offsetof(BoneData, BoneWeights1));

		// BoneWeights2
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(BoneData), (GLvoid*)offsetof(BoneData, BoneWeights2));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// Unbind VAO
	glBindVertexArray(0);
}

void Mesh::shutdownMesh()
{
	//delete[] mVertices;
	//mVertices = 0;

	//delete[] mIndices;
	//mIndices = 0;
}
