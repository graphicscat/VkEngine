#include<Pass/BPhongPass.h>

void BPhongPass::init() 
{
    initDescriptor();
    initBuffer();
    defaultPipeline.defaultGraphicsPipeline(vertexShader,fragShader,m_descriptor.m_desLayout);
}

void BPhongPass::initDescriptor()
{
    m_descriptor.init();
    m_descriptor.bind(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT,0);
    m_descriptor.updateDescriptorLayout();
}

void BPhongPass::initBuffer()
{
    uint32_t size = sizeof(UboRes);
    m_buffer.size = size;
    m_buffer.create();
    m_buffer.upload(m_descriptor.m_set,0);
}

void BPhongPass::quit()
{
    m_buffer.clean();
    m_descriptor.clean();
    defaultPipeline.quit();
}

BPhongPass::BPhongPass(const char* v,const char *f):Pass(v,f){}