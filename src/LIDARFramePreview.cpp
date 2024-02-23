#include "LIDARFramePreview.hpp"

#include "GLInclude.hpp"

LIDARFramePreview::LIDARFramePreview(const std::string& name) :
    SceneObject(LIDAR_FRAME_PREVIEW, name),
    var(0)
{
    VertexFormat vtxFmt;

    vtxFmt.size = 3;
    vtxFmt[0].data = EMVF_ATTRB_USAGE_POS | EMVF_ATTRB_TYPE_FLOAT | EMVF_ATTRB_SIZE(3);
    vtxFmt[1].data = EMVF_ATTRB_USAGE_UV | EMVF_ATTRB_TYPE_FLOAT | EMVF_ATTRB_SIZE(2);
    vtxFmt[2].data = EMVF_ATTRB_USAGE_COLOR | EMVF_ATTRB_TYPE_FLOAT | EMVF_ATTRB_SIZE(4);

    m_meshBuilder = std::make_unique<MeshBuilder>(vtxFmt);
}

void LIDARFramePreview::draw(Shader& shader)
{   
    m_meshBuilder->reset();

    for (int i = 0; i <= 50; i++) {
        float angle = glm::radians((360.0f / 50) * i);
        float x = cos(angle);
        float y = sin(angle);
        // Calculate x, y coordinates of a unit circle for each iteration
        // ...
        m_meshBuilder->index(1, 0);
        m_meshBuilder->vertex(NULL, x, y, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    shader.setModelViewMatrix(getTransform().getMatrix());

    glLineWidth(2.0f);
    shader.setColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    shader.use();
    m_meshBuilder->drawElements(GL_LINE_STRIP);

    glPointSize(3.0f);
    shader.setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    shader.use();
    m_meshBuilder->drawElements(GL_POINTS);
}

void LIDARFramePreview::update(float dt)
{
    // Update the LIDAR frame preview
    // ...
}