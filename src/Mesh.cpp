#include "Mesh.hpp"

#include <stb_image.h>
#include <filesystem>

#include "Logger.hpp"

namespace fs = std::filesystem;
using namespace em;

Logger m_logger("Meshes");

std::vector<std::shared_ptr<Mesh>> Mesh::load(const char* filePath)
{
    std::vector<std::shared_ptr<Mesh>> meshes;

    return meshes;
}

Mesh::Mesh() :
    m_numVerticies(0),
    m_numIndicies(0),
    m_isRenderable(false),
    m_hasAdvisedAboutNotBeingRenderableYet(false),
    m_glVAO(0),
    m_glVBO(0),
    m_glEBO(0),
    m_glTexture(0)
{

}

Mesh::~Mesh()
{
    m_logger.infof("Deleting mesh \"%s\"", m_name.c_str());

    if(m_isRenderable)
    {
        glDeleteVertexArrays(1, &m_glVAO);
        glDeleteBuffers(1, &m_glVBO);
        glDeleteBuffers(1, &m_glEBO);
    }

    if(m_glTexture)
    {
        glDeleteTextures(1, &m_glTexture);
    }
}

uint32_t Mesh::getTextureHandle() const
{
    return m_glTexture;
}

void Mesh::putMeshArrays(MeshBuilder& meshBuilder) const
{
    const VertexFormat& vtxFmt = meshBuilder.getVertexFormat();
    
    if(m_positions.empty() || m_indicies.empty()) return;

    for(size_t i = 0; i < m_numIndicies; i++)
    {
        uint32_t index = m_indicies[i];

        putVertex(meshBuilder, vtxFmt, index);
    }
}

void Mesh::putMeshElements(MeshBuilder& meshBuilder) const
{
    const VertexFormat& vtxFmt = meshBuilder.getVertexFormat();

    if(m_positions.empty() || m_indicies.empty()) return;

    meshBuilder.indexv(m_numIndicies, m_indicies.data());

    for(size_t i = 0; i < m_numVerticies; i++)
    {
        putVertex(meshBuilder, vtxFmt, (uint32_t) i);
    }
}

void Mesh::makeRenderable(VertexFormat vtxFmt)
{
    if(m_isRenderable) return;

    MeshBuilder meshBuilder(vtxFmt);

    putMeshElements(meshBuilder);

    size_t vertexBufferSize, indexBufferSize;

    const uint8_t* vertexData = meshBuilder.getVertexBuffer(&vertexBufferSize);
    const uint32_t* indexData = meshBuilder.getIndexBuffer(&indexBufferSize);

    glGenVertexArrays(1, &m_glVAO);
    glGenBuffers(1, &m_glVBO);
    glGenBuffers(1, &m_glEBO);

    glBindVertexArray(m_glVAO);
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_glVBO);
        glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertexData, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, indexData, GL_STATIC_DRAW);

        vtxFmt.apply();
    }
    glBindVertexArray(0);

    m_isRenderable = true;
}

void Mesh::render(int mode) const
{
    if(!m_isRenderable)
    {
        if(!m_hasAdvisedAboutNotBeingRenderableYet)
        {
            m_hasAdvisedAboutNotBeingRenderableYet = true;
            m_logger.submodule(getName()).warnf("Mesh has not yet been made renderable. Call makeRenderable first.");
        }
        return;
    }

    if(m_glTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_glTexture);
    }

    glBindVertexArray(m_glVAO);
    {
        glDrawElements(mode, m_numIndicies, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

void Mesh::renderInstanced(int mode, int instances) const
{
    if(!m_isRenderable)
    {
        if(!m_hasAdvisedAboutNotBeingRenderableYet)
        {
            m_hasAdvisedAboutNotBeingRenderableYet = true;
            m_logger.submodule(getName()).warnf("Mesh has not yet been made renderable. Call makeRenderable first.");
        }
        return;
    }

    if(m_glTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_glTexture);
    }

    glBindVertexArray(m_glVAO);
    {
        glDrawElementsInstanced(mode, m_numIndicies, GL_UNSIGNED_INT, 0, instances);
    }
    glBindVertexArray(0);
}

const glm::vec3* Mesh::getPositions() const
{
    if(m_positions.empty()) return NULL;
    return m_positions.data();
}

const glm::vec2* Mesh::getUVs() const
{
    if(m_uvs.empty()) return NULL;
    return m_uvs.data();
}

const glm::vec3* Mesh::getNormals() const
{
    if(m_normals.empty()) return NULL;
    return m_normals.data();
}

const glm::vec4* Mesh::getColors() const
{
    if(m_colors.empty()) return NULL;
    return m_colors.data();
}

const uint32_t* Mesh::getIndicies() const
{
    if(m_indicies.empty()) return NULL;
    return m_indicies.data();
}

size_t Mesh::numVerticies() const
{
    return m_numVerticies;
}

size_t Mesh::numIndicies() const
{
    return m_numIndicies;
}

const char* Mesh::getName() const
{
    return m_name.c_str();
}

void Mesh::putVertex(MeshBuilder& meshBuilder, const VertexFormat& vtxFmt, uint32_t vertexID) const
{
    bool hasUVs = !m_uvs.empty();
    bool hasNormals = !m_normals.empty();
    bool hasColors = !m_colors.empty();

    for(int attrb = 0; attrb < vtxFmt.size; attrb++)
    {
        uint32_t attrbUsage = vtxFmt.attributes[attrb].getUsage();

        glm::vec2 vec2;
        glm::vec3 vec3;
        glm::vec4 vec4;

        switch (attrbUsage)
        {
        case EMVF_ATTRB_USAGE_POS:
            vec3 = m_positions[vertexID];
            meshBuilder.position(vec3.x, vec3.y, vec3.z);
            break;
        case EMVF_ATTRB_USAGE_UV:
            if(hasUVs)
            {
                vec2 = m_uvs[vertexID];
                meshBuilder.uv(vec2.s, vec2.t);
            }
            else meshBuilder.uvDefault();
            break;
        case EMVF_ATTRB_USAGE_NORMAL:
            if(hasNormals)
            {
                vec3 = m_normals[vertexID];
                meshBuilder.normal(vec3.x, vec3.y, vec3.z);
            }
            else meshBuilder.normalDefault();
            break;
        case EMVF_ATTRB_USAGE_COLOR:
            if(hasColors)
            {
                vec4 = m_colors[vertexID];
                meshBuilder.colorRGBA(vec4.r, vec4.g, vec4.b, vec4.a);
            }
            else meshBuilder.colorDefault();
            break;
        case EMVF_ATTRB_USAGE_TEXID:
            meshBuilder.texid(0);
            break;
        }
    }
}