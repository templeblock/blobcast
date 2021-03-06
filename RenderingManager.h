#pragma once
#include "ShaderProgram.h"
#include "config.h"
#include "Light.hpp"
#include "Skybox.h"
#include "IOBuffer.h"
#include "Blob.h"
#include "Level.h"
#include "BlobDisplay.h"
#include "glm/glm.hpp"
#include <glm/gtx/transform.hpp>
#include <random>

class RenderingManager
{
public:
	bool init();

	void drawBlob(Blob *blob, glm::vec3 camPos, glm::mat4 viewMatrix, 
		glm::mat4 projMatrix);
	void drawLevel(Level *level, glm::vec3 camPos, glm::mat4 viewMatrix, 
		glm::mat4 projMatrix);

	void depthPass(Blob *blob, Level *level, glm::vec3 camPos);
	void dynamicCubeMapPass(Blob *blob, Level *level);

	void geometryPass(Level *level, glm::mat4 viewMatrix, glm::mat4 projMatrix);
	void SSAOPass(glm::mat4 projMatrix, glm::vec3 camPos);
	void blurPass();

	void drawSkybox(glm::mat4 viewMatrix, glm::mat4 projMatrix);
	void debugQuadDraw();

	void drawParticles(Level *level, glm::mat4 viewMatrix,
		glm::mat4 projMatrix);

protected:

	ShaderProgram *blobShader;
	ShaderProgram *platformShader;
	ShaderProgram *skyboxShader;
	ShaderProgram *depthShader;
	ShaderProgram *geomPassShader;
	ShaderProgram *SSAOShader;
	ShaderProgram *blurShader;
	ShaderProgram *quadShader;
	ShaderProgram *particleShader;

	IOBuffer cubeMapBuffer;
	IOBuffer gBuffer;
	IOBuffer aoBuffer;
	std::vector<IOBuffer> pingpongBuffers;
	IOBuffer depthBuffer;
	IOBuffer CSMBuffer;

	glm::mat4 lightSpaceMatrix;

	Mesh* quad;

	GLuint noiseTexture;
	std::vector<glm::vec3> ssaoKernel;

	DirectionalLight dirLight;

	float width, height;

	Skybox skybox;

	bool initFrameBuffers();
	bool initSkybox();

	void drawCubeFace(glm::vec3 position, glm::vec3 direction, glm::vec3 up,
		GLenum face, glm::mat4 projMatrix, Level *level);

	GLuint wallTexture;
};