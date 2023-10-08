#include<stdio.h>
#include<vector>
#include<vulkan/vulkan.h>
#include<Framework/Logger.h>
#include<Framework/Engine.h>
int main(int argc, char *argv[])
{
    
    Engine* e = new Engine();
    e->run();
    delete e;
    
    return 0;
}