//
//  SkyDome.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/13.
//
//

#include "SkyDome.h"

using namespace std;
using namespace cocos2d;

SkyDome* SkyDome::create(const std::string &modelPath)
{
    auto sprite = new (std::nothrow) SkyDome();
    if (sprite && sprite->initWithFile(modelPath)) {
        sprite->_contentSize = sprite->getBoundingBox().size;
        sprite->autorelease();
        return sprite;
    }

    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool SkyDome::initWithFile(const std::string& modelPath)
{
    if (! ExSprite3D::initWithFile(modelPath)) {
        return false;
    }

    this->setupShader();

    return true;
}

void SkyDome::setupShader()
{
    GLProgram* glProgram = GLProgram::createWithFilenames("SkyDomeShader.vert", "SkyDomeShader.frag");
    GLProgramState* glProgramState = GLProgramState::create(glProgram);
    this->setGLProgramState(glProgramState);

    // メッシュ取得(obj形式では1つのみの前提)
    MeshVertexData* mesh = this->_meshVertexDatas.at(0);
    for (ssize_t i = 0, attributeCount = mesh->getMeshVertexAttribCount(), offset = 0; i < attributeCount; i++) {
        const MeshVertexAttrib& meshattribute = mesh->getMeshVertexAttrib(i);
        glProgramState->setVertexAttribPointer(
                                               s_attributeNames[meshattribute.vertexAttrib],
                                               meshattribute.size,
                                               meshattribute.type,
                                               GL_FALSE,
                                               mesh->getVertexBuffer()->getSizePerVertex(),
                                               (GLvoid*)offset
                                               );

        offset += meshattribute.attribSizeBytes;
    }
}