#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include <vector>

#include "Texture.h"

namespace fgr {
	///<summary>
	/// A struct for handling 3D vertices. Used for 3D meshes.
	///</summary>
	struct Vertex3D {
		glm::vec3 position = glm::vec3(0);
		glm::vec3 color = glm::vec3(0);
		glm::vec3 normal = glm::vec3(0);
		glm::vec2 tex_coord = glm::vec3(0);

		Vertex3D() = default;

		///<summary>
		/// Construct a 3D vertex.
		///</summary>
		Vertex3D(const glm::vec3& position, const glm::vec3& color, const glm::vec3& normal, const glm::vec2& texCoord);
	};

	///<summary>
	/// A struct for handling 3D views.
	///</summary>
	struct View {
		///<summary>
		/// The corresponding matrix associated with the view.
		///</summary>
		glm::mat4 view, projection;

		View() = default;

		///<summary>
		/// Set the projection matrix according to screen size.
		///</summary>
		///<param name="fov">The field of view in radiants.</param>
		///<param name="screen_width">The width of the framebuffer to render to.</param>
		///<param name="screen_height">The height of the framebuffer to render to.</param>
		void setPerspective(const float fov, const float screen_width, const float screen_height);

		///<summary>
		/// Set the view matrix according to camera parameters.
		///</summary>
		///<param name="camera_pos">Where is the camera located?</param>
		///<param name="camera_front">Where is the camera facing?</param>
		///<param name="camera_up">Where is up for the camera?</param>
		void setOrientation(const glm::vec3& camera_pos, const glm::vec3& camera_front, const glm::vec3& camera_up);
	};

	///<summary>
	/// Basic information regarding how a mesh will be rendered.
	///</summary>
	struct MaterialInfo {
		Shader* shader;
		TextureHandle* texture;
		///<summary>
		/// Lighting information that must be set.
		///</summary>
		glm::vec3 sun_direction, sun_color, ambient_color;

		///<summary>
		/// Construct the information struct. Note that lighting info has to be manually set.
		///</summary>
		///<param name="shader">The shader to render with.</param>
		///<param name="texture">The texture to use for rendering.</param>
		MaterialInfo(Shader& shader, TextureHandle& texture);

		///<summary>
		/// Set the shader uniforms. Here a the uniform IDs that will be set:
		/// 0: Texture unit
		/// 1: Sun direction
		/// 2: Sun color
		/// 3: Ambient color
		///</summary>
		///<param name=""> </param>
		void setUniforms();
	};

	///<summary>
	/// A struct for rendering 3D meshes.
	///</summary>
	struct Mesh {
	protected:
		Shader* shader;

		uint VAO, VBO, EBO;
		uint projection_uni, model_uni, view_uni;
		uint vertices_allocated = 0, indices_allocated = 0;

		View* view;

		bool apply_matrices = true;

	public:
		///<summary>
		/// The model matrix. By default this has no effect.
		///</summary>
		glm::mat4 model = glm::mat4(1.0);

		///<summary>
		/// The vertex pool of the mesh.
		///</summary>
		std::vector<Vertex3D> vertices;

		///<summary>
		/// The indices of the vertices that will make up the mesh. Note that these must make individual triangles!
		///</summary>
		std::vector<uint> indices;

		Mesh() = default;

		///<summary>
		/// Create all the OpenGL buffers and objects required for rendering.
		///</summary>
		///<param name="Shader">The shader to use for rendering.</param>
		///<param name="View">The view from which the mesh will be viewed. Note that this is a reference, so alterations will be effective.</param>
		void init(Shader& shader, View& view);

		///<summary>
		/// Set all vertex data.
		///</summary>
		///<param name="vertices">The vertex pool of the mesh.</param>
		///<param name="indices">The indices of the vertices that will make up the mesh. Note that these must make individual triangles!</param>
		void setMesh(const std::vector<Vertex3D>& vertices, const std::vector<uint>& indices);

		///<summary>
		/// Load a mesh from a .obj file.
		///</summary>
		///<param name="path">The path of the file.</param>
		void loadFromOBJ(const std::string& path);

		///<summary>
		/// Update the vertex data. This must only be called when this data is manually altered.
		///</summary>
		void update();

		///<summary>
		/// Draw the mesh to the framebuffer.
		///</summary>
		void render();

		///<summary>
		/// Destroy all allocated contents.
		///</summary>
		void dispose();
	};
}