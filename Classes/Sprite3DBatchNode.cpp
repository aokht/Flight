//
//  Sprite3DBatchNode.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/01.
//
//

#include "Sprite3DBatchNode.h"

using namespace std;
using namespace cocos2d;

Sprite3DBatchNode* Sprite3DBatchNode::create(const std::string& modelPath)
{
    return Sprite3DBatchNode::create(modelPath, "Sprite3DBatchNodeShader.vert", "Sprite3DBatchNodeShader.frag");
}

Sprite3DBatchNode* Sprite3DBatchNode::create(const string& modelPath, const string& vertShader, const string& fragShader)
{
    Sprite3DBatchNode* sprite3DBatchNode = new Sprite3DBatchNode();
    if (sprite3DBatchNode && sprite3DBatchNode->initWithFileAndShaders(modelPath, vertShader, fragShader)) {
        sprite3DBatchNode->autorelease();
        return sprite3DBatchNode;
    }
    CC_SAFE_DELETE(sprite3DBatchNode);
    return nullptr;


}

Sprite3DBatchNode::Sprite3DBatchNode() :
    nodeCount(0),
    isBuilt(false)
{
}

Sprite3DBatchNode::~Sprite3DBatchNode()
{
}

bool Sprite3DBatchNode::initWithFileAndShaders(const string& modelPath, const string& vertShader, const string& fragShader)
{
    bool ret = ExSprite3D::initWithFile(modelPath);

    this->setupShaders(vertShader, fragShader);

    return ret;
}

void Sprite3DBatchNode::setupShaders(const string& vertShader, const string& fragShader)
{
    GLProgram* glProgram = GLProgram::createWithFilenames(vertShader, fragShader);
    GLProgramState* glProgramState = GLProgramState::create(glProgram);
    this->setGLProgramState(glProgramState);

    // set vertex attributes
    const MeshVertexData* meshVertexData = this->getMeshVertexData();
    for (int i = 0, offset = 0, last = meshVertexData->getMeshVertexAttribCount(); i < last; ++i) {
        const MeshVertexAttrib& attrib = meshVertexData->getMeshVertexAttrib(i);
        glProgramState->setVertexAttribPointer(
           s_attributeNames[attrib.vertexAttrib],
           attrib.size,
           attrib.type,
           GL_FALSE,
           meshVertexData->getVertexBuffer()->getSizePerVertex(),
           (GLvoid*)offset
        );
        offset += attrib.attribSizeBytes;
    }

// メモ： GL_EXT_instanced_arrays がサポートされていないっぽいけど使える場合の例
//     glGenBuffers(1, &offsetBuffer);
//     glBindBuffer(GL_ARRAY_BUFFER, offsetBuffer);
//     glBufferData(12, sizeof(float) * offsetArray.size(), offsetArray.data(), GL_DYNAMIC_DRAW);
//     glProgram->bindAttribLocation("a_offset", 12);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Sprite3DBatchNode::add(cocos2d::Vec3 position)
{
    // TODO: uniform 容量チェック
    ++nodeCount;

    Vec3 offset = position - this->getPosition3D();
    positionOffsetList.push_back(offset.x);
    positionOffsetList.push_back(offset.y);
    positionOffsetList.push_back(offset.z);
}

void Sprite3DBatchNode::build()
{
    this->getGLProgramState()->setUniformCallback("u_offset", [&](GLProgram* program, Uniform* uniform) {
        program->setUniformLocationWith3fv(uniform->location, positionOffsetList.data(), nodeCount);
    });

    this->isBuilt = true;
}

void Sprite3DBatchNode::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    if (! isBuilt) {
        this->build();
    }

    command.init(_globalZOrder, transform, flags);
    command.func = [&](){
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);

        Mesh* mesh = this->getMesh();
        MeshIndexData* meshIndexData = mesh->getMeshIndexData();
        GLuint vertexBuffer = meshIndexData->getVertexBuffer()->getVBO();
        GLuint indexBuffer = meshIndexData->getIndexBuffer()->getVBO();

        if (mesh->getTexture()) {
            GL::bindTexture2D(mesh->getTexture()->getName());
        }

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        this->getGLProgramState()->apply(transform);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

        // instanced drawing
        int indexNumber = meshIndexData->getIndexBuffer()->getIndexNumber();
        glDrawElementsInstancedEXT(GL_TRIANGLES, indexNumber, GL_UNSIGNED_SHORT, 0, nodeCount);

        CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, indexNumber * nodeCount);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    };
    renderer->addCommand(&command);
}

// メモ： GL_EXT_instanced_arrays がサポートされていないっぽいけど使える場合の例
//        glBindBuffer(GL_ARRAY_BUFFER, offsetBuffer);
//        glVertexAttribPointer(12, 3, GL_FLOAT, GL_FALSE, 12, 0);
//        glVertexAttribDivisorEXT(0, 0);
//        glVertexAttribDivisorEXT(12, 1);