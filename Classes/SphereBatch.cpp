//
//  SphereBatch.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/26.
//
//

#include "SphereBatch.h"
#include "ExSprite3D.h"
#include "ExVertexIndexBuffer.h"
#include "ExMesh.h"

using namespace std;
using namespace cocos2d;

SphereBatch* SphereBatch::create()
{
    SphereBatch* sphereBatch = new SphereBatch();

    return sphereBatch;
}

SphereBatch::SphereBatch() :
    meshVertexData(nullptr),
    vertexCount(0)
{
}

SphereBatch::~SphereBatch()
{
    if (meshVertexData) {
        meshVertexData->release();
    }
}

void SphereBatch::addSphere(cocos2d::Vec3 position)
{
    ExSprite3D* sphere = ExSprite3D::create("objects/diamond.obj");
    sphere->retain();
    sphereList.push_back(sphere);

    const MeshVertexData* meshVertexData = sphere->getMeshVertexData();

    // vertex 抽出
    const vector<unsigned char>& rawVertexList = sphere->getVertexBuffer()->getShadowCopy();
    const float* vertexListArray = reinterpret_cast<const float*>(rawVertexList.data());
    const vector<float> fVertexList(vertexListArray, vertexListArray + rawVertexList.size() / sizeof(float));
    sphereVertexList.push_back(fVertexList);

    // index 抽出
    for (ssize_t i = 0, last = meshVertexData->getMeshIndexDataCount(); i < last; ++i) {
        const ExIndexBuffer* indexBuffer = (const ExIndexBuffer*)meshVertexData->getMeshIndexDataByIndex((int)i)->getIndexBuffer();
        const vector<unsigned char>& rawIndexList = indexBuffer->getShadowCopy();
        // unsigned char -> unsigned short 変換 orz
        const unsigned short* indexListArray = reinterpret_cast<const unsigned short*>(rawIndexList.data());
        const vector<unsigned short> usIndexList(indexListArray, indexListArray + rawIndexList.size() / sizeof(unsigned short));

        // vertexList にまとめている分の offset を足す
        vector<unsigned short> newIndexList;
        for (unsigned short index : usIndexList) {
            newIndexList.push_back(index + vertexCount);
        }
        sphereIndexList.push_back(newIndexList);
    }

    vertexCount += fVertexList.size();
}

void SphereBatch::buildBatch()
{
    if (! sphereList.size()) {
        return;
    }

    MeshData meshData;

    // vector<MeshVertexAttrib>
    ExSprite3D* sphere = sphereList[0]; // サンプル
    for (size_t index = 0, last = sphere->getMesh()->getMeshVertexAttribCount(); index < last; ++index) {
        meshData.attribs.push_back(sphere->getMesh()->getMeshVertexAttribute((int)index));
    }
    meshData.attribCount = (int)meshData.attribs.size();


    // vertex, index
    vector<float> vertexList;
    vector<unsigned short> indexList;
    for (size_t i = 0, sphereListSize = sphereList.size(); i < sphereListSize; ++i) {
        for (float vertex : sphereVertexList[i]) {
            vertexList.push_back(vertex);
        }
        for (unsigned short index : sphereIndexList[i]) {
            indexList.push_back(index);
        }
    }
    meshData.vertex = vertexList;
    meshData.vertexSizeInFloat = (int)vertexList.size() * sizeof(float);
    meshData.subMeshIndices.push_back(indexList);
    meshData.numIndex = 1;
    meshData.subMeshIds.push_back("");
    meshData.subMeshAABB.push_back(AABB());

    meshVertexData = MeshVertexData::create(meshData);
    meshVertexData->retain();

    // shader
    auto glProgram = GLProgram::createWithFilenames("SphereShader.vert", "SphereShader.frag");
    auto glProgramState = GLProgramState::create(glProgram);
    this->setGLProgramState(glProgramState);

    long offset = 0;
    MeshVertexData* mesh = meshVertexData;
    for (MeshVertexAttrib meshattribute : meshData.attribs) {
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
    GLint locationMVPMatrix = glProgram->getUniformLocation("u_mvpMatrix");
    float mat4List[16 * sphereList.size()];
    for (size_t i = 0, last = sphereList.size(); i < last; ++i) {
        const Mat4& mv = sphereList[i]->getNodeToWorldTransform();
        memcpy(&mat4List[i * 16], mv.m, sizeof(float) * 16);
    }
    glProgram->setUniformLocationWithMatrix4fv(locationMVPMatrix, mat4List, sphereList.size());


    vector<int> indexes;
    for (int i = 0, last = sphereList.size(); i < last; ++i) {
        indexes.push_back(i);
    }
    glProgram->bindAttribLocation("a_index", 100);
    glEnableVertexAttribArray(100);
    glProgramState->setVertexAttribPointer(
        "a_index",
        indexes.size(),
        GL_FLOAT,
        GL_FALSE,
        sizeof(int),
        indexes.data()
    );

//    glProgramState->setUniformCallback("u_mvpMatrix", [this](GLProgram* program, Uniform* uniform) {
//        float mat4List[16 * sphereList.size()];
//        for (size_t i = 0, last = sphereList.size(); i < last; ++i) {
//            const Mat4& mv = sphereList[i]->getNodeToWorldTransform();
//            memcpy(&mat4List[i * 16], mv.m, sizeof(float) * 16);
//        }
//        program->setUniformLocationWithMatrix4fv(uniform->location, mat4List, sphereList.size());
//    });
}

void SphereBatch::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    ExMesh* mesh = (ExMesh*)Mesh::create("hoge", meshVertexData->getMeshIndexDataByIndex(0));
    MeshCommand& meshCommand = mesh->getMeshCommand();

    meshCommand.init(_globalZOrder, 0, this->getGLProgramState(), _blend, mesh->getVertexBuffer(), mesh->getIndexBuffer(), mesh->getPrimitiveType(), mesh->getIndexFormat(), mesh->getIndexCount(), transform, flags);
    meshCommand.setDepthTestEnabled(true);

    renderer->addCommand(&meshCommand);
}

