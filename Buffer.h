#pragma once

#include <GL/glew.h>
#include "VertexArray.h"
#include <cstring>

template <class T>
class Buffer
{
	public:
		GLuint Name;
		VertexArray *VAO;
		GLenum Target;
		GLenum Usage;
		unsigned int ItemSize;
		unsigned int NumItems;
		T *Data;

		Buffer(
				VertexArray *vao,
				GLenum target,
				GLenum usage,
				unsigned int itemSize,
				unsigned int numItems);
		~Buffer();
		Buffer(const Buffer<T>&) = delete;
		Buffer<T>& operator=(const Buffer<T>&) = delete;
		Buffer(Buffer<T>&& other);
		Buffer<T>& operator=(Buffer<T>&& other);

		virtual void SetData(T *data, bool copy = true);
		virtual void BindBuffer();
		virtual void VertexAttribPointer(GLuint attribute, size_t stride = 0)
		{ }

	private:
		bool managed_data = false;
};
#include "Buffer.inl"

class FloatBuffer : public Buffer<GLfloat>
{
	public:
		FloatBuffer(
				VertexArray *vao,
				unsigned int itemSize,
				unsigned int numItems);

		void VertexAttribPointer(GLuint attribute, size_t stride = 0);
};

class ElementBuffer : public Buffer<unsigned int>
{
	public:
		ElementBuffer(VertexArray *vao, int numItems);
};
