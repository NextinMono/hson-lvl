#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <plume_render_interface.h>
#include "../math/aabb.h"

namespace ulvl::gfx {
	struct BaseVertex {
		glm::vec3 position;
		glm::vec2 texcoord;
	};

	struct Mesh {
		int indexOffset;
		int indexCount;
		void* texture;

		Mesh(int indexOffset, int indexCount, void* texture) : indexOffset{ indexOffset }, indexCount{ indexCount }, texture{ texture } {}
	};

	class Model {
	public:
		glm::vec3 position{ 0, 0, 0 };
		glm::quat rotation{ 1, 0, 0, 0 };
		glm::vec3 scale{ 1, 1, 1 };
		math::Aabb aabb{};

	private:
		glm::mat4 worldMatrix{};
		void* vertices{ nullptr };
		size_t vertexCount{ 0 };
		std::vector<plume::RenderInputElement> vertexLayout;
	public:
		std::vector<unsigned short> indices;
	private:
		std::vector<Mesh> meshes;
		std::unique_ptr<plume::RenderPipeline> pipeline;
		std::unique_ptr<plume::RenderPipelineLayout> pipelineLayout;
		std::unique_ptr<plume::RenderBuffer> vertexBuffer;
		std::unique_ptr<plume::RenderBuffer> indexBuffer;
		std::unique_ptr<plume::RenderDescriptorSet> descriptor;
		plume::RenderVertexBufferView vertexBufferView;
		plume::RenderIndexBufferView indexBufferView;
		plume::RenderInputSlot inputSlot;
		unsigned int vertexStride{ sizeof(BaseVertex) };

		void updateWorldMatrix();
		void updateAabb();
	public:
		Model();

		void init();
		void shutdown();

		void setPosition(const glm::vec3& pos);
		void setRotation(const glm::quat& rot);
		void setRotation(const glm::vec3& rot);
		void setScale(const glm::vec3& scale);

		void addMesh(void* vertices, unsigned int vcount, unsigned short* indices, unsigned int icount, void* texture);
		size_t getVertexLayoutOffset(const char* semanticName) const;
		inline const glm::mat4& getWorldMatrix() const { return worldMatrix; }

		template<typename T>
		T& getVertexValue(size_t idx, size_t offset) {
			return *(T*)&((char*)vertices)[idx * vertexStride + offset];
		}

		void render();
	};
}
