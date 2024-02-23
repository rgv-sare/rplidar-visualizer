#pragma once

#include "SceneObject.hpp"
#include "MeshBuilder.hpp"

using namespace em;

class LIDARFramePreview : public SceneObject
{
public:
    LIDARFramePreview(const std::string& name);

    void draw(Shader& shader) override;
private:
    std::unique_ptr<MeshBuilder> m_meshBuilder;

    int var;
protected:
    void update(float dt) override;
};