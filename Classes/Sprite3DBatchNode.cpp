//
//  Sprite3DBatchNode.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/08/01.
//
//

#include "Sprite3DBatchNode.h"
#include "ExMesh.h"

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

Sprite3DBatchNode* Sprite3DBatchNode::create(const Sprite3DBatchNode& src)
{
    return create(src.modelPath, src.shaderPathVert, src.shaderPathVert);
}

void Sprite3DBatchNode::makeBatchNodeShared(const Sprite3DBatchNode& src, Sprite3DBatchNode* dst)
{
    dst->nodeCount = src.nodeCount;
    dst->positionOffsetList = src.positionOffsetList;
    dst->visibleList = src.visibleList;
    dst->statusList = src.statusList;
    dst->isShared = true;
}

Sprite3DBatchNode::Sprite3DBatchNode() :
    nodeCount(0),
    isBuilt(false),
    isShared(false),
    positionOffsetList(new vector<float>()),
    visibleList(new vector<float>()),
    statusList(new vector<Sprite3DBatchNode::NodeStatus>())
{
}

Sprite3DBatchNode::~Sprite3DBatchNode()
{
    if (! isShared) {
        delete positionOffsetList;
        delete visibleList;
        delete statusList;
    }
}

bool Sprite3DBatchNode::initWithFileAndShaders(const string& modelPath, const string& vertShader, const string& fragShader)
{
    bool ret = ExSprite3D::initWithFile(modelPath);
    this->modelPath = modelPath;

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

    this->shaderPathVert = vertShader;
    this->shaderPathFrag = fragShader;

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
    positionOffsetList->push_back(offset.x);
    positionOffsetList->push_back(offset.y);
    positionOffsetList->push_back(offset.z);

    statusList->push_back({
        true,
        offset
    });
    visibleList->push_back(1.f);
}

int Sprite3DBatchNode::getNodeCount() const
{
    return nodeCount;
}

void Sprite3DBatchNode::build()
{
    this->getGLProgramState()->setUniformCallback("u_offset", [&](GLProgram* program, Uniform* uniform) {
        program->setUniformLocationWith3fv(uniform->location, positionOffsetList->data(), nodeCount);
    });

    this->getGLProgramState()->setUniformCallback("u_visible", [&](GLProgram* program, Uniform* uniform) {
        program->setUniformLocationWith1fv(uniform->location, visibleList->data(), nodeCount);
    });

    AABB aabb;
    for (const NodeStatus& node : *statusList) {
        aabb._min.x = min(aabb._min.x, node.position.x);
        aabb._min.y = min(aabb._min.y, node.position.y);
        aabb._min.z = min(aabb._min.z, node.position.z);
        aabb._max.x = max(aabb._max.x, node.position.x);
        aabb._max.y = max(aabb._max.y, node.position.y);
        aabb._max.z = max(aabb._max.z, node.position.z);
    }
    ((ExMesh*)this->getMesh())->setAABB(aabb);
    this->_aabbDirty = true;

    this->isBuilt = true;
}

void Sprite3DBatchNode::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    // 初回描画時に各種 uniform 変数と、AABBの設定を行う
    if (! isBuilt) {
        this->build();
    }
    // 初めて描くときの処理が重くて一瞬止まるので、初回描画時は全部描き、以降はクリッピング判定を行う
    else if (! Camera::getVisitingCamera()->isVisibleInFrustum(&this->getAABB())) {
        return;
    }

    command.init(_globalZOrder, transform, flags);
    command.func = [&](){
        if (! glIsEnabled(GL_CULL_FACE)) {
            glEnable(GL_CULL_FACE);
        }
        GLint cullFace;
        glGetIntegerv(GL_CULL_FACE_MODE, &cullFace);
        if (cullFace != GL_BACK) {
            glCullFace(GL_BACK);
        }
        if (! glIsEnabled(GL_DEPTH_TEST)) {
            glEnable(GL_DEPTH_TEST);
        }

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


const vector<Sprite3DBatchNode::NodeStatus>& Sprite3DBatchNode::getNodeStatusList() const
{
    return *statusList;
}

void Sprite3DBatchNode::setNodeVisible(int index, bool visible)
{
    CCASSERT(0 <= index && index < statusList->size(), "Invalid index for statusList");

    statusList->at(index).isVisible = visible;
    visibleList->at(index) = visible ? 1.f : 0.f;
}

int Sprite3DBatchNode::getRemainingNodeCount() const
{
    int count = 0;
    if (statusList) {
        for (const Sprite3DBatchNode::NodeStatus& status : *statusList) {
            count += status.isVisible ? 1 : 0;
        }
    }

    return count;
}