#include "debug-visual-service.h"
#include "../graphics/graphics.h"
#include "../app.h"
#include "cleaner-service.h"
#include <numbers>

using namespace ulvl::app;

DebugVisualService::~DebugVisualService() {
    Application::instance->getService<CleanerService>()->deleteModel(cube);
}

void DebugVisualService::initCube() {
    cube = new gfx::InstancedModel{};

    gfx::PosVertex cubeVertices[]{
        {{-0.5f, -0.5f,  0.5f}},
        {{ 0.5f, -0.5f,  0.5f}},
        {{ 0.5f,  0.5f,  0.5f}},
        {{-0.5f,  0.5f,  0.5f}},
        {{ 0.5f, -0.5f, -0.5f}},
        {{-0.5f, -0.5f, -0.5f}},
        {{-0.5f,  0.5f, -0.5f}},
        {{ 0.5f,  0.5f, -0.5f}},
        {{-0.5f, -0.5f, -0.5f}},
        {{-0.5f, -0.5f,  0.5f}},
        {{-0.5f,  0.5f,  0.5f}},
        {{-0.5f,  0.5f, -0.5f}},
        {{ 0.5f, -0.5f,  0.5f}},
        {{ 0.5f, -0.5f, -0.5f}},
        {{ 0.5f,  0.5f, -0.5f}},
        {{ 0.5f,  0.5f,  0.5f}},
        {{-0.5f,  0.5f,  0.5f}},
        {{ 0.5f,  0.5f,  0.5f}},
        {{ 0.5f,  0.5f, -0.5f}},
        {{-0.5f,  0.5f, -0.5f}},
        {{-0.5f, -0.5f, -0.5f}},
        {{ 0.5f, -0.5f, -0.5f}},
        {{ 0.5f, -0.5f,  0.5f}},
        {{-0.5f, -0.5f,  0.5f}}
    };

    unsigned short cubeIndices[]{
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9,10,10,11, 8,
        12,13,14,14,15,12,
        16,17,18,18,19,16,
        20,21,22,22,23,20
    };

    cube->setModel(cubeVertices, ARRAY_SIZE(cubeVertices), cubeIndices, ARRAY_SIZE(cubeIndices));
}

void DebugVisualService::initSphere() {
    sphere = new gfx::InstancedModel{};

    static const int X_SEGMENTS{ 8 };
    static const int Y_SEGMENTS{ 8 };

    gfx::PosVertex sphereVertices[(X_SEGMENTS + 1) * (Y_SEGMENTS + 1)]{};
    unsigned short sphereIndices[X_SEGMENTS * Y_SEGMENTS * 6]{};

    int v = 0;
    for (int y = 0; y <= Y_SEGMENTS; y++)
    {
        float vRatio = (float)y / Y_SEGMENTS;
        float phi = vRatio * 3.14159265f;

        for (int x = 0; x <= X_SEGMENTS; x++)
        {
            float uRatio = (float)x / X_SEGMENTS;
            float theta = uRatio * 2.0f * 3.14159265f;

            float px = std::cosf(theta) * std::sinf(phi);
            float py = std::cosf(phi);
            float pz = std::sinf(theta) * std::sinf(phi);

            sphereVertices[v++] = { { px * 0.5f, py * 0.5f, pz * 0.5f } };
        }
    }

    int idx = 0;
    for (int y = 0; y < Y_SEGMENTS; y++)
    {
        for (int x = 0; x < X_SEGMENTS; x++)
        {
            int i0 = y * (X_SEGMENTS + 1) + x;
            int i1 = i0 + 1;
            int i2 = i0 + (X_SEGMENTS + 1);
            int i3 = i2 + 1;

            sphereIndices[idx++] = (unsigned short)i0;
            sphereIndices[idx++] = (unsigned short)i2;
            sphereIndices[idx++] = (unsigned short)i1;

            sphereIndices[idx++] = (unsigned short)i1;
            sphereIndices[idx++] = (unsigned short)i2;
            sphereIndices[idx++] = (unsigned short)i3;
        }
    }

    sphere->setModel(sphereVertices, ARRAY_SIZE(sphereVertices), sphereIndices, ARRAY_SIZE(sphereIndices));
}

void DebugVisualService::initCylinder() {
    cylinder = new gfx::InstancedModel{};

    static const int SEGMENTS{ 16 };
    static const int RINGS{ 1 };
    static const float RADIUS{ 0.5f };
    static const float HALF_HEIGHT{ 0.5f };

    static const int STRIDE{ SEGMENTS + 1 };
    static const int VERTEX_COUNT{ (RINGS + 1) * STRIDE + 2 * (SEGMENTS + 1) };
    static const int INDEX_COUNT{ SEGMENTS * 6 * RINGS + SEGMENTS * 6 };

    gfx::PosVertex cylinderVertices[VERTEX_COUNT]{};
    unsigned short cylinderIndices[INDEX_COUNT]{};

    const double PI = std::numbers::pi;
    int v = 0;
    int i = 0;

    for (int r = 0; r <= RINGS; r++)
    {
        float y = HALF_HEIGHT - r * (2.0f * HALF_HEIGHT) / RINGS;
        for (int s = 0; s <= SEGMENTS; s++)
        {
            float theta = (float)s / SEGMENTS * (PI * 2.0f);
            float x = cosf(theta) * RADIUS;
            float z = sinf(theta) * RADIUS;
            cylinderVertices[v++] = { { x, y, z } };
        }
    }

    int startSide{ 0 };

    for (int s = 0; s <= SEGMENTS; s++)
    {
        float theta = (float)s / SEGMENTS * (PI * 2.0f);
        float x = cosf(theta) * RADIUS;
        float z = sinf(theta) * RADIUS;
        float y = HALF_HEIGHT;
        cylinderVertices[v++] = { { x, y, z } };
    }
    int startTop = v - (SEGMENTS + 1);

    for (int s = 0; s <= SEGMENTS; s++)
    {
        float theta = (float)s / SEGMENTS * (PI * 2.0f);
        float x = cosf(theta) * RADIUS;
        float z = sinf(theta) * RADIUS;
        float y = -HALF_HEIGHT;
        cylinderVertices[v++] = { { x, y, z } };
    }
    int startBottom = v - (SEGMENTS + 1);

    for (int r = 0; r < RINGS; r++)
    {
        for (int s = 0; s < SEGMENTS; s++)
        {
            int i0 = startSide + r * STRIDE + s;
            int i1 = i0 + 1;
            int i2 = i0 + STRIDE;
            int i3 = i2 + 1;

            cylinderIndices[i++] = i0;
            cylinderIndices[i++] = i2;
            cylinderIndices[i++] = i1;

            cylinderIndices[i++] = i1;
            cylinderIndices[i++] = i2;
            cylinderIndices[i++] = i3;
        }
    }

    for (int s = 0; s < SEGMENTS; s++)
    {
        int i0 = startTop + s;
        int i1 = startTop + s + 1;
        int i2 = startTop + SEGMENTS;
        cylinderIndices[i++] = i0;
        cylinderIndices[i++] = i1;
        cylinderIndices[i++] = i2;
    }

    for (int s = 0; s < SEGMENTS; s++)
    {
        int i0 = startBottom + s;
        int i1 = startBottom + s + 1;
        int i2 = startBottom + SEGMENTS;
        cylinderIndices[i++] = i1;
        cylinderIndices[i++] = i0;
        cylinderIndices[i++] = i2;
    }

    cylinder->setModel(cylinderVertices, ARRAY_SIZE(cylinderVertices), cylinderIndices, ARRAY_SIZE(cylinderIndices));
}

void DebugVisualService::AddCallback() {
    auto* gfx = gfx::Graphics::instance;

    initCube();
    initSphere();
    initCylinder();
	
    gfx->models.push_back(cube);
    gfx->models.push_back(sphere);
    gfx->models.push_back(cylinder);
}

void DebugVisualService::addCube(gfx::InstancedMesh mesh) {
    cube->addMesh(mesh);
}

void DebugVisualService::addSphere(gfx::InstancedMesh mesh) {
    sphere->addMesh(mesh);
}

void DebugVisualService::addCylinder(gfx::InstancedMesh mesh) {
    cylinder->addMesh(mesh);
}

void DebugVisualService::removeMeshes(int id) {
    cube->removeMeshes(id);
    sphere->removeMeshes(id);
    cylinder->removeMeshes(id);
}
