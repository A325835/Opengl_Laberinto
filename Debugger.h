#ifndef DEBUGGER_H
#define DEBUGGER_H

using namespace reactphysics3d;

struct DebugVertex
{
    float pos[3];
    float color[3];
};

class Debugger
{
public:
    Debugger(PhysicsWorld *world)
    {
        this->world = world;
        this->debugRenderer = &(this->world->getDebugRenderer());
    }

    ~Debugger()
    {
        glDeleteVertexArrays(1, &(this->VAO));
        glDeleteBuffers(1, &(this->VBO));
    }

    void enableDebugRendering()
    {
        this->world->setIsDebugRenderingEnabled(true);
        generateDynamicBuffer();
        this->debugRenderer->setIsDebugItemDisplayed(DebugRenderer::DebugItem::COLLISION_SHAPE, true);
    }

    void disableDebugRendering()
    {
        this->world->setIsDebugRenderingEnabled(false);
        this->debugRenderer->setIsDebugItemDisplayed(DebugRenderer::DebugItem::COLLISION_SHAPE, false);
    }

    void drawColliders()
    {
        const PhysicsWorld& temp = *world;
        this->debugRenderer->computeDebugRenderingPrimitives(temp);
        uint32 size = this->debugRenderer->getNbTriangles();
        uint32 nVertices = size * 3;
        Array<DebugRenderer::DebugTriangle> triArray = this->debugRenderer->getTriangles();

        uint32 left = nVertices % this->bufferSize;
        uint32 index = 0;
        bool justStarted = true;

       
        glBindVertexArray(this->VAO);

        //Triangles
        for (uint32 i = 0; i < size; i++)
        {
            index = (i * 3) % this->bufferSize;
            if (index == 0 && !justStarted)
            {
                updateBuffer(this->VBO, 0, this->debugVertices, sizeof(DebugVertex) * this->bufferSize, GL_ARRAY_BUFFER);
                glDrawArrays(GL_LINES, 0, this->bufferSize);

            }
            else justStarted = false;

            this->debugVertices[index].pos[0] = triArray[i].point1.x;
            this->debugVertices[index].pos[1] = triArray[i].point1.y;
            this->debugVertices[index].pos[2] = triArray[i].point1.z;

            this->debugVertices[index].color[0] = 0.0f;
            this->debugVertices[index].color[1] = 1.0f;
            this->debugVertices[index].color[2] = 1.0f;

            this->debugVertices[index + 1].pos[0] = triArray[i].point2.x;
            this->debugVertices[index + 1].pos[1] = triArray[i].point2.y;
            this->debugVertices[index + 1].pos[2] = triArray[i].point2.z;

            this->debugVertices[index + 1].color[0] = 0.0f;
            this->debugVertices[index + 1].color[1] = 1.0f;
            this->debugVertices[index + 1].color[2] = 1.0f;

            this->debugVertices[index + 2].pos[0] = triArray[i].point3.x;
            this->debugVertices[index + 2].pos[1] = triArray[i].point3.y;
            this->debugVertices[index + 2].pos[2] = triArray[i].point3.z;

            this->debugVertices[index + 2].color[0] = 0.0f;
            this->debugVertices[index + 2].color[1] = 1.0f;
            this->debugVertices[index + 2].color[2] = 1.0f;
        }
        updateBuffer(this->VBO, 0, this->debugVertices, sizeof(DebugVertex) * left, GL_ARRAY_BUFFER);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, left);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

private:
	unsigned int VAO = 0, VBO = 0;
    const uint bufferSize = 1200;
    DebugVertex debugVertices[1200];
    
    PhysicsWorld* world;
    DebugRenderer* debugRenderer;

    void updateBuffer(uint& id, uint offset, void* data, uint size, uint type)
    {
        glBindBuffer(type, id);
        glBufferSubData(type, offset, size, data);
    }

    void generateDynamicBuffer()
    {
        glGenVertexArrays(1, &(this->VAO));
        glGenBuffers(1, &(this->VBO));
        glBindVertexArray(this->VAO);

        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(DebugVertex) * bufferSize, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, pos));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, color));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

#endif