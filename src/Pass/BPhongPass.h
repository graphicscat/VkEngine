#pragma once
#include<Pass/Pass.h>
#include<Framework/Camera.h>
#include <Framework/Pipeline.h>

class BPhongPass : public Pass
{
    public:

    virtual void init() override;
    virtual void initDescriptor() override;
    virtual void quit() override;
    virtual void initBuffer() override;

    BPhongPass() = default;
    BPhongPass(const char* v,const char *f );

    GraphicsPipeline defaultPipeline;

    UboRes m_ubo;

    //Camera m_cam;
    
};