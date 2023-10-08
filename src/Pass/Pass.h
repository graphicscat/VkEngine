#pragma once
#include <macros.h>
#include <Framework/Descriptor.h>
#include <Framework/Buffer.h>

struct UboRes{
        glm::mat4 proj;
        glm::mat4 view;
        glm::vec4 camPos;
        glm::vec4 lightPos;
    };

class Pass
{
    public:
    Pass() = default;
    virtual ~Pass() = default;
    Pass(const char* v,const char *f);
    virtual void init() = 0;
    virtual void quit() = 0;
    virtual void initDescriptor() = 0;
    virtual void initBuffer() = 0;

    const char * vertexShader;
    const char * fragShader;

    Buffer m_buffer;
    Descriptor m_descriptor;
};