#pragma once
#include<Framework/Mesh.h>
#include<macros.h>
#include<unordered_map>

class Scene
{
    public:
    Scene() = default;
    ~Scene() = default;

    void add(const std::string& filename, const std::string& meshname);

    void init();

    void quit();

    std::unordered_map<std::string, Mesh*> m_scene;

    private:
    void updateMesh(Mesh*);

    
};