#include "LIDARFramePreview.hpp"

#include "GLInclude.hpp"
#include "Visualizer.hpp"

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

    LIDARFrameGrabber* grabber =  VisualizerApp::getInstance().getLIDARFrameGrabber();

    if(!grabber || !grabber->isConnected())
        return;

    const std::vector<LIDARFrameGrabber::Node>& nodes = grabber->getNodes();
    LIDARFrameGrabber::Node longestNode = grabber->longestNode();

    m_meshBuilder->index(1, 0);
    m_meshBuilder->vertex(NULL, 0.0f, 0.0f, 0.0f, 0.0, 0.0, 0.0f, 0.5f, 0.0f, 1.0f);

    for(const LIDARFrameGrabber::Node& node : nodes)
    {
        if (node.distance < 5.0f)
            continue;

        float x = node.distance * cos(glm::radians(node.angle));
        float y = node.distance * sin(glm::radians(node.angle));

        x = x / longestNode.distance;
        y = y / longestNode.distance;

        m_meshBuilder->index(1, 0);
        m_meshBuilder->vertex(NULL, x, y, 0.0f, 0.0, 0.0, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    shader.setModelViewMatrix(getTransform().getMatrix());

    glLineWidth(2.0f);
    shader.setColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    shader.use();
    m_meshBuilder->drawElements(GL_LINE_STRIP);

    glPointSize(3.0f);
    shader.setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    shader.use();
    shader.setVertexColorEnabled(true);
    m_meshBuilder->drawElements(GL_POINTS);
}

void LIDARFramePreview::update(float dt)
{
    // Update the LIDAR frame preview
    // ...
}