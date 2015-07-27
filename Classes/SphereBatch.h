//
//  SphereBatch.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/26.
//
//

#ifndef __Flight__SphereBatch__
#define __Flight__SphereBatch__

#include "cocos2d.h"
#include "Sphere.h"

class ExSprite3D;

class SphereBatch : public cocos2d::Sprite3D
{
public:
    static SphereBatch* create();

    void addSphere(cocos2d::Vec3 position);
    void buildBatch();

    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;


protected:
    std::vector<ExSprite3D*> sphereList;
    std::vector<std::vector<float>> sphereVertexList;
    std::vector<std::vector<unsigned short>> sphereIndexList;

    cocos2d::MeshVertexData* meshVertexData;

private:
    SphereBatch();
    ~SphereBatch();

    int vertexCount;

};




#endif /* defined(__Flight__SphereBatch__) */
