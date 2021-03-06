#include "Mesh.h"
#include <glm/gtc/constants.hpp>

Mesh::Mesh(int vertexBuffers, int numVerts, int numFaces) :
	IBO(&VAO, numFaces),
	NumVerts(numVerts),
	NumTris(numFaces)
{
	for (int i = 0; i < vertexBuffers; i++)
		VBOs.push_back(FloatBuffer(&VAO, 1, numVerts));
	VAO.Bind([&](){
		for (std::size_t i = 0, n = vertexBuffers; i < n; i++)
			glEnableVertexAttribArray(i);
	});
}

Mesh::Mesh(
		std::vector<GLfloat> vbo,
		std::vector<GLfloat> nbo,
		std::vector<GLuint> ibo) : Mesh(2, vbo.size() / 3, ibo.size() / 3)
{
	SetVertexData(0, &vbo[0], 3);
	SetVertexData(1, &nbo[0], 3);
	SetIndexData(&ibo[0]);
}

void Mesh::Draw() const
{
	VAO.Bind([&](){
		glDrawElements(
				GL_TRIANGLES,
				NumTris * 3,
				GL_UNSIGNED_INT,
				(void *)0
				);
	});
}

void Mesh::SetVertexData(
		unsigned int attribute, GLfloat *data, int itemSize)
{
	VBOs[attribute] = FloatBuffer(&VAO, itemSize, NumVerts);
	VBOs[attribute].SetData(data);
	VBOs[attribute].VertexAttribPointer(attribute);
}

void Mesh::SetIndexData(unsigned int *iboData)
{
	IBO = ElementBuffer(&VAO, NumTris);
	IBO.SetData(iboData);
	IBO.BindBuffer();
}

void Mesh::ComputeAABB(
		float& min_x, float& min_y, float& min_z,
		float& max_x, float& max_y, float& max_z) const
{
	min_x = VBOs[0].Data[0];
	min_y = VBOs[0].Data[1];
	min_z = VBOs[0].Data[2];
	max_x = VBOs[0].Data[0];
	max_y = VBOs[0].Data[1];
	max_z = VBOs[0].Data[2];
	for (int i = 1; i < NumVerts; i++)
	{
		float x = VBOs[0].Data[i * 3];
		if (x < min_x) min_x = x;
		if (x > max_x) max_x = x;
		float y = VBOs[0].Data[i * 3 + 1];
		if (y < min_y) min_y = y;
		if (y > max_y) max_y = y;
		float z = VBOs[0].Data[i * 3 + 2];
		if (z < min_z) min_z = z;
		if (z > max_z) max_z = z;
	}
}

float Mesh::ComputeRadius(glm::vec3 center) const
{
	float max_square_distance = 0.f;
	for (int i = 0; i < NumVerts; i++)
	{
		float x = VBOs[0].Data[i * 3] - center.x;
		float y = VBOs[0].Data[i * 3 + 1] - center.y;
		float z = VBOs[0].Data[i * 3 + 2] - center.z;
		float square_distance = x * x + y * y + z * z;
		if (square_distance > max_square_distance)
			max_square_distance = square_distance;
	}
	return glm::sqrt(max_square_distance);
}

Mesh *Mesh::CreateCube()
{
	Mesh *cube = new Mesh(1, 8, 12);
	GLfloat vbo[] = {
		-1, -1, -1,   // 0
		-1, -1,  1,   // 1
		-1,  1, -1,   // 2
		-1,  1,  1,   // 3
		 1, -1, -1,   // 4
		 1, -1,  1,   // 5
		 1,  1, -1,   // 6
		 1,  1,  1 }; // 7
	cube->SetVertexData(0, vbo, 3);
	unsigned int ibo[] = {
		// front (-Z)
		0, 6, 2,
		6, 0, 4,
		// back (+Z)
		5, 3, 7,
		3, 5, 1,
		// left (-X)
		1, 2, 3,
		2, 1, 0,
		// right (+X)
		4, 7, 6,
		7, 4, 5,
		// top (+Y)
		2, 7, 3,
		7, 2, 6,
		// bottom (-Y)
		1, 4, 0,
		4, 1, 5 };
	cube->SetIndexData(ibo);

	return cube;
}

Mesh *Mesh::CreateCubeWithNormals()
{
	Mesh *cube = new Mesh(3, 24, 12);
	GLfloat vbo[] = {
		-1, -1, -1,   // 0
		-1, -1,  1,   // 1
		-1,  1, -1,   // 2
		-1,  1,  1,   // 3
		 1, -1, -1,   // 4
		 1, -1,  1,   // 5
		 1,  1, -1,   // 6
		 1,  1,  1,	  // 7

		-1, -1, -1,   // 8
		-1, -1,  1,   // 9
		-1,  1, -1,   // 10
		-1,  1,  1,   // 11
		 1, -1, -1,   // 12
		 1, -1,  1,   // 13
		 1,  1, -1,   // 14
		 1,  1,  1,   // 15

		-1, -1, -1,   // 16
		-1, -1,  1,   // 17
		-1,  1, -1,   // 18
		-1,  1,  1,   // 19
		 1, -1, -1,   // 20
		 1, -1,  1,   // 21
		 1,  1, -1,   // 22
		 1,  1,  1	  //23
	};
	cube->SetVertexData(0, vbo, 3);

	GLfloat normals[] = {
		0, 0, -1,	//front/back
		0, 0, 1,
		0, 0, -1,
		0, 0, 1,
		0, 0, -1,
		0, 0, 1,
		0, 0, -1,
		0, 0, 1,

		-1, 0, 0, //left/right
		-1, 0, 0,
		-1, 0, 0,
		-1, 0, 0,
		1, 0, 0,
		1, 0, 0,
		1, 0, 0,
		1, 0, 0,

		0, -1, 0, //top/bottom
		0, -1, 0,
		0, 1, 0,
		0, 1, 0,
		0, -1, 0,
		0, -1, 0,
		0, 1, 0,
		0, 1, 0
	};
	cube->SetVertexData(1, normals, 3);

	GLfloat texCoords[] = {
		0, 0,	//front/back
		1, 0,
		0, 1,
		1, 1,
		1, 0,
		0, 0,
		1, 1,
		0, 1,

		1, 0,	//left/right
		0, 0,
		1, 1,
		0, 1,
		0, 0,
		1, 0,
		0, 1,
		1, 1,

		0, 1,	//top/bottom
		0, 0,
		0, 0,
		0, 1,
		1, 1,
		1, 0,
		1, 0,
		1, 1,
	};
	cube->SetVertexData(2, texCoords, 2);

	unsigned int ibo[] = {
		// front (-Z)
		6, 0, 2,
		0, 6, 4,
		// back (+Z)
		3, 5, 7,
		5, 3, 1,
		// left (-X)
		10, 9, 11,
		9, 10, 8,
		// right (+X)
		15, 12, 14,
		12, 15, 13,
		// top (+Y)
		23, 18, 19,
		18, 23, 22,
		// bottom (-Y)
		20, 17, 16,
		17, 20, 21 };
	cube->SetIndexData(ibo);

	return cube;
}

Mesh *Mesh::CreateTriplePlane()
{
	Mesh *triple_plane = new Mesh(1, 12, 6);
	GLfloat vbo[] = {
		-1, -1,  0,
		 1, -1,  0,
		-1,  1,  0,
		 1,  1,  0,
		-1,  0, -1,
		-1,  0,  1,
		 1,  0, -1,
		 1,  0,  1,
		 0, -1, -1,
		 0,  1, -1,
		 0, -1,  1,
		 0,  1,  1 };
	triple_plane->SetVertexData(0, vbo, 3);
	unsigned int ibo[] = {
		 0,  3,  2,
		 3,  0,  1,
		 7,  4,  6,
		 4,  7,  5,
		11,  8, 10,
		 8, 11,  9 };
	triple_plane->SetIndexData(ibo);

	return triple_plane;
}

Mesh *Mesh::CreateQuad()
{
	Mesh *quad = new Mesh(2, 4, 2);

	GLfloat vbo[] = {
		-1.0f,  1.0f, 0.0f,   // 0
		-1.0f, -1.0f, 0.0f,   // 1
		 1.0f,  1.0f, 0.0f,   // 2
		 1.0f, -1.0f, 0.0f }; // 3
	quad->SetVertexData(0, vbo, 3);

	GLfloat texCoords[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f };
	quad->SetVertexData(1, texCoords, 2);

	unsigned int ibo[] = {
		2, 0, 1,
		1, 3, 2 };
	quad->SetIndexData(ibo);

	return quad;
}

Mesh *Mesh::CreateCylinderWithNormals(int segments)
{
	const float height = 1.0f; 
	const float radiusTop = 1.0f;
	const float radiusBottom = 1.0f;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<GLuint> indices;
	
	double angle = 0.0;

	vertices.push_back(glm::vec3(0, height, 0));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	for (unsigned int i = 0; i<segments; i++)
	{
		angle = ((double)i) / ((double)segments) * 2.0 * 3.14;
		
		glm::vec3 v = glm::vec3(radiusTop*cos(angle), height, 
			radiusTop*sin(angle));
		vertices.push_back(v);
		
		normals.push_back(glm::vec3(cos(angle), 0.0f, sin(angle)));
		
		indices.push_back(0);
		indices.push_back((i + 1) % segments + 1);
		indices.push_back(i + 1);
	}

	vertices.push_back(glm::vec3(0, 0, 0));
	normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));

	for (unsigned int i = 0; i<segments; i++)
	{
		angle = ((double)i) / ((double)segments) * 2.0 * 3.14;
		vertices.push_back(glm::vec3(radiusBottom*cos(angle), -height, 
			radiusBottom*sin(angle)));
		normals.push_back(glm::vec3(cos(angle), 0.0f, sin(angle)));
		indices.push_back(segments + 1);
		indices.push_back(segments + 2 + (i + 1) % segments);
		indices.push_back(segments + i + 2);
	}

	for (unsigned int i = 0; i<segments; i++)
	{
		indices.push_back(i + 1);
		indices.push_back((i + 1) % segments + 1);
		indices.push_back(segments + 2 + (i + 1) % segments);

		indices.push_back(i + 1);
		indices.push_back(segments + 2 + (i + 1) % segments);
		indices.push_back(segments + i + 2);
	}

	Mesh *cylinder = new Mesh(2, vertices.size(), indices.size() / 3);
	cylinder->SetVertexData(0, &vertices[0][0], 3);
	cylinder->SetVertexData(1, &normals[0][0], 3);
	cylinder->SetIndexData(&indices[0]);

	return cylinder;
}

void Mesh::computeTangentBasis(float* vbo, float* texCoords, std::vector<glm::vec3> & tangents, std::vector<glm::vec3> & bitangents)
{
	std::vector<glm::vec3> vertices;
	for (int i = 0; i < 24; i += 3) {
		vertices.push_back(glm::vec3(vbo[i], vbo[i + 1], vbo[i + 2]));
	}

	std::vector<glm::vec2> uvs;
	for (int i = 0; i < 24; i += 2) {
		uvs.push_back(glm::vec2(texCoords[i], texCoords[i + 1]));
	}

	for (unsigned int i = 0; i<vertices.size(); i += 3)
	{
		glm::vec3 & v0 = vertices[i + 0];
		glm::vec3 & v1 = vertices[i + 1];
		glm::vec3 & v2 = vertices[i + 2];

		// Shortcuts for UVs
		glm::vec2 & uv0 = uvs[i + 0];
		glm::vec2 & uv1 = uvs[i + 1];
		glm::vec2 & uv2 = uvs[i + 2];

		// Edges of the triangle : postion delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

		// Set the same tangent for all three vertices of the triangle
		tangents.push_back(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);

		// Same thing for binormals
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
	}
}