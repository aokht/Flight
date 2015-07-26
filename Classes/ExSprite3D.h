//
//  ExSprite3D.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/17.
//
//

#ifndef __Flight__ExSprite3D__
#define __Flight__ExSprite3D__

#include "cocos2d.h"
#include "CollisionMesh.h"

class ExSprite3D : public cocos2d::Sprite3D
{
public:
    // Life Cycle
    static ExSprite3D* create(const std::string &modelPath);

    // Collision Detection
    void enableCollisionDetection(bool flag);
    bool collisionDetectionEnabled() const;
    bool getIntersection(int* triangleIndex, float *tOut, const cocos2d::Vec3& begin, const cocos2d::Vec3& vec) const;
    bool isIntersect(const cocos2d::Vec3& begin, const cocos2d::Vec3& vec) const;

protected:
    // Life Cycle
    ExSprite3D();
    ~ExSprite3D();
    bool initWithFile(const std::string &path);

    // Collision Detection
    bool _collisionDetectionEnabled;
    void extractVertexInfo();
    void buildCollisionMesh();
    std::vector<cocos2d::Vec3> positionList;
    std::vector<cocos2d::Vec3> normalList;
    std::vector<CollisionMesh::Triangle> triangleList;
    CollisionMesh* collisionMesh;

};

#endif /* defined(__Flight__ExSprite3D__) */
