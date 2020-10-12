#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include <vector>

namespace fgr {
    ///<summary>
    ///A struct for handling vertices.
    ///</summary>
	struct Vertex {
		glm::vec3 position;
		glm::vec2 tex_coords;
		glm::vec4 color;

		Vertex() = default;

		Vertex(glm::vec3 position, glm::vec2 tex_coords, glm::vec4 color);
	};

    ///<summary>
    ///A struct for handling instances.
    ///</summary>
	struct Instance {
		glm::mat3 transformations = glm::mat3(1.0);
		glm::mat3 tex_transform = glm::mat3(1.0);
		glm::vec4 color = glm::vec4(1.0);

		Instance() = default;

		Instance(glm::mat3 transformations, glm::mat3 tex_transform, glm::vec4 color);
	};

    ///<summary>
    ///All usable render modes.
    ///</summary>
	enum RendeMode {
		triangles = GL_TRIANGLES,
		triangle_strip = GL_TRIANGLE_STRIP,
		triangle_fan = GL_TRIANGLE_FAN,
		lines = GL_LINES,
		line_strip = GL_LINE_STRIP,
		line_loop = GL_LINE_LOOP,
		points = GL_POINTS,
		thick_line = GL_LINE_STRIP_ADJACENCY
	};

    ///<summary>
    ///A struct for handling and drawing vertices.
    ///</summary>
	struct VertexArray {
        ///<summary>
        ///The vertices of the array. May be changed so long as "vertices_size" is set appropriately and "update()" is called.
        ///</summary>
		Vertex* vertices = nullptr;
        
        ///<summary>
        ///The amount of vertices stored in the array.
        ///</summary>
		unsigned int vertices_size = 0;

        ///<summary>
        ///The associated OpenGL objects. WARNING: read-only!
        ///</summary>
		unsigned int VAO = 0, VBO = 0;
        
        ///<summary>
        ///The amount of vertices allocated in graphics memory. WARNING: read-only!
        ///</summary>
		unsigned int vertices_allocated = 0;

        ///<summary>
        ///The transformations of the array. WARNING: read-only!
        ///</summary>
		glm::mat3 transform;

        ///<summary>
        ///Should be set to true if the vertices are often changed, to false otherwhise.
        ///</summary>
		bool dynamic_allocation = false;

		VertexArray() = default;

        ///<summary>
        ///Must be called before changing data or drawing.
        ///</summary>
		void init();

        ///<summary>
        ///Draw the array.
        ///</summary>
        ///<param name="shader">The shader to be used.</param>
        ///<param name="mode">The render mode to draw with.</param>
		void draw(const Shader& shader, const unsigned int mode = GL_TRIANGLES);

        ///<summary>
        ///Update the vertices for drawing.
        ///</summary>
		void update();

        ///<summary>
        ///Truncate the array or allocate new vertices. "update()" does not need to be called.
        ///</summary>
        ///<param name="vertices_size">The desired new size.</param>
		void resize(const unsigned int vertices_size);

        ///<summary>
        ///Set a new set of vertices. "update()" does not need to be called.
        ///</summary>
        ///<param name="vertices">The vertices to be used.</param>
        ///<param name="size">The amount of vertices from the input array to be copied into the array.</param>
		void setVertices(Vertex* vertices, const unsigned int size);

        ///<summary>
        ///Append a vertex to the preexisting ones. "update()" does not need to be called.
        ///</summary>
        ///<param name="vertex">The vertex to be added.</param>
		void append(Vertex vertex);

        ///<summary>
        ///Set the transformations of the array.
        ///</summary>
        ///<param name="transform">The transformations to apply.</param>
		void setTransformations(const glm::mat3& transform);

        ///<summary>
        ///Destroy all stored data.
        ///</summary>
		void dispose();
	};

    ///<summary>
    ///A struct for handling and drawing instances of a VertexArray.
    ///</summary>
	struct InstanceArray {
        ///<summary>
        ///All the stored instances. May be changed so long as "update()" is subsequently called.
        ///</summary>
		std::vector<Instance> instances;

        ///<summary>
        ///The VBO associated with the array. WARNING: read-only!
        ///</summary>
		unsigned int VBO = 0;
        
        ///<summary>
        ///The amount of instances allocated in graphics memory. WARNING: read-only!
        ///</summary>
		unsigned int instances_allocted = 0;

        ///<summary>
        ///The vertex array to be instanced. May be modified freely.
        ///</summary>
		VertexArray va;

        ///<summary>
        ///Should be set to true if the instances are often changed, to false otherwhise.
        ///</summary>
		bool dynamic_allocation = false;

		InstanceArray() = default;

        ///<summary>
        ///Must be called before changing data or drawing.
        ///</summary>
		void init();

        ///<summary>
        ///Draw the array.
        ///</summary>
        ///<param name="shader">The shader to be used.</param>
        ///<param name="mode">The render mode to be drawn with.</param>
		void draw(const Shader& shader, const unsigned int mode = GL_TRIANGLES);

        ///<summary>
        ///Update the graphics memory for drawing after changes have been made to the instances.
        ///</summary>
		void update();

        ///<summary>
        ///Destroy all stored data.
        ///</summary>
		void dispose();
	};
}
