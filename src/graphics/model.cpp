#include "model.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "graphics.h"
#include "shaders/vs.h"
#include "shaders/ps_color.h"

using namespace ulvl::gfx;

void Model::updateWorldMatrix() {
	worldMatrix =
		glm::translate(glm::mat4(1), position) *
		glm::toMat4(rotation) *
		glm::scale(glm::mat4(1), scale);
}

void Model::updateAabb() {
    size_t posOffset{ getVertexLayoutOffset("POSITION") };

    for (size_t idx = 0; idx < vertexCount; idx++) {
        auto& position = getVertexValue<glm::vec3>(idx, posOffset);
        aabb.min = glm::min(aabb.min, position);
        aabb.max = glm::min(aabb.max, position);
    }
}

Model::Model() {
	init();
}

void Model::init() {
	auto* graphics = Graphics::instance;
	auto& ctx = graphics->renderCtx;

    plume::RenderDescriptorRange range{};
    range.type = plume::RenderDescriptorRangeType::CONSTANT_BUFFER;
    range.binding = 0;
    range.count = 1;

    plume::RenderDescriptorSetDesc descriptorDesc{};
    descriptorDesc.descriptorRanges = &range;
    descriptorDesc.descriptorRangesCount = 1;

    descriptor = ctx.device->createDescriptorSet(descriptorDesc);

    plume::RenderPushConstantRange pushConstantRange{};
    pushConstantRange.binding = 0;
    pushConstantRange.set = 0;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(glm::mat4);
    pushConstantRange.stageFlags = plume::RenderShaderStageFlag::VERTEX;

    plume::RenderPipelineLayoutDesc layoutDesc{};
    layoutDesc.allowInputLayout = true;
    layoutDesc.descriptorSetDescs = &descriptorDesc;
    layoutDesc.descriptorSetDescsCount = 1;
    layoutDesc.pushConstantRanges = &pushConstantRange;
    layoutDesc.pushConstantRangesCount = 1;

    pipelineLayout = ctx.device->createPipelineLayout(layoutDesc);

    plume::RenderShaderFormat shaderFormat = ctx.renderInterface->getCapabilities().shaderFormat;

    std::unique_ptr<plume::RenderShader> vertexShader;
    std::unique_ptr<plume::RenderShader> fragmentShader;

    switch (shaderFormat) {
    case plume::RenderShaderFormat::SPIRV:
        vertexShader = ctx.device->createShader(vs_shader, sizeof(vs_shader), "main", shaderFormat);
        fragmentShader = ctx.device->createShader(ps_color_shader, sizeof(ps_color_shader), "main", shaderFormat);
        break;
    default:
        assert(false && "Unknown shader format");
    }

    inputSlot = plume::RenderInputSlot{ 0, vertexStride };

    vertexLayout = {
        { "POSITION", 0, 0, plume::RenderFormat::R32G32B32_FLOAT, 0, 0                 },
        { "TEXCOORD", 0, 1, plume::RenderFormat::R32G32_FLOAT,    0, sizeof(float) * 3 }
    };

    plume::RenderGraphicsPipelineDesc pipelineDesc{};
    pipelineDesc.inputSlots = &inputSlot;
    pipelineDesc.inputSlotsCount = 1;
    pipelineDesc.inputElements = vertexLayout.data();
    pipelineDesc.inputElementsCount = static_cast<uint32_t>(vertexLayout.size());
    pipelineDesc.pipelineLayout = pipelineLayout.get();
    pipelineDesc.vertexShader = vertexShader.get();
    pipelineDesc.pixelShader = fragmentShader.get();
    pipelineDesc.renderTargetFormat[0] = plume::RenderFormat::B8G8R8A8_UNORM;
    pipelineDesc.renderTargetBlend[0] = plume::RenderBlendDesc::Copy();
    pipelineDesc.renderTargetCount = 1;
    pipelineDesc.primitiveTopology = plume::RenderPrimitiveTopology::TRIANGLE_LIST;

    pipeline = ctx.device->createGraphicsPipeline(pipelineDesc);

	updateWorldMatrix();
    updateAabb();
}

void Model::shutdown() {
    pipeline.reset();
    descriptor.reset();
    pipelineLayout.reset();
    vertexBuffer.reset();
    indexBuffer.reset();
}
 
void Model::setPosition(const glm::vec3& pos) {
	position = pos;
	updateWorldMatrix();
}

void Model::setRotation(const glm::quat& rot) {
	rotation = rot;
	updateWorldMatrix();
}

void Model::setRotation(const glm::vec3& rot) {
	setRotation(glm::quat(rot));
}

void Model::setScale(const glm::vec3& scale) {
	this->scale = scale;
	updateWorldMatrix();
}

void Model::addMesh(void* vertices, unsigned int vcount, unsigned short* indices, unsigned int icount, void* texture) {
	int indexOffset = this->indices.size();
	int vertexOffset = vertexCount;
	meshes.emplace_back(indexOffset, icount, texture);

    char* newVerts = new char[(vertexCount + vcount) * vertexStride];
    if (this->vertices) {
        memcpy(newVerts, this->vertices, vertexCount * vertexStride);
        delete this->vertices;
    }
    this->vertices = newVerts;
    memcpy(&newVerts[vertexCount * vertexStride], vertices, vcount * vertexStride);
    vertexCount += vcount;

	this->indices.insert(this->indices.end(), indices, indices + icount);

	for (int x = 0; x < icount; x++) {
		auto& y = this->indices[indexOffset + x];
		y += vertexOffset;
	}

    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    unsigned int verticesSize = vertexCount * vertexStride;
    vertexBuffer = ctx.device->createBuffer(plume::RenderBufferDesc::VertexBuffer(verticesSize, plume::RenderHeapType::UPLOAD));
    void* bufferData = vertexBuffer->map();
    std::memcpy(bufferData, this->vertices, verticesSize);
    vertexBuffer->unmap();

    vertexBufferView = plume::RenderVertexBufferView{ { vertexBuffer.get() }, verticesSize };

    unsigned int indicesSize = this->indices.size() * sizeof(unsigned short);
    indexBuffer = ctx.device->createBuffer(plume::RenderBufferDesc::IndexBuffer(indicesSize, plume::RenderHeapType::UPLOAD));
    bufferData = indexBuffer->map();
    std::memcpy(bufferData, this->indices.data(), indicesSize);
    indexBuffer->unmap();

    indexBufferView = plume::RenderIndexBufferView{ { indexBuffer.get() }, indicesSize, plume::RenderFormat::R16_UINT };

    updateAabb();
}

size_t Model::getVertexLayoutOffset(const char* semanticName) const {
    for (auto& l : vertexLayout)
        if (strcmp(l.semanticName, semanticName) == 0)
            return l.alignedByteOffset;

    return 0;
}

void Model::render() {
	auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    ctx.commandList->setGraphicsPipelineLayout(pipelineLayout.get());
    ctx.commandList->setPipeline(pipeline.get());
    ctx.commandList->setVertexBuffers(0, &vertexBufferView, 1, &inputSlot);
    ctx.commandList->setIndexBuffer(&indexBufferView);

    ctx.commandList->setGraphicsPushConstants(0, &worldMatrix, 0, sizeof(glm::mat4));

    descriptor->setBuffer(0, ctx.mainCBuffer.get(), sizeof(MainCBuffer));
    ctx.commandList->setGraphicsDescriptorSet(descriptor.get(), 0);

	for (auto& mesh : meshes) {
        ctx.commandList->drawIndexedInstanced(mesh.indexCount, 1, mesh.indexOffset, 0, 0);
	}
}
