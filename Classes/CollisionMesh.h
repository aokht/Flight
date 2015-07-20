//
//  CollisionMesh.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/18.
//
//

#ifndef __Flight__CollisionMesh__
#define __Flight__CollisionMesh__

#include "cocos2d.h"

class CollisionMesh : public cocos2d::Ref
{
public:
    struct Triangle {
        int v1;
        int v2;
        int v3;
    };

    // k-d tree 構築
    static CollisionMesh* create(const std::vector<cocos2d::Vec3>& positionList, const std::vector<Triangle>& triangleList);

    // collision detection
    bool getIntersection(
        int* index,                  // nearest triangle index
        float* tOut,                 // distance (0.0 ~ 1.0)
        const cocos2d::Vec3& begin,  // position
        const cocos2d::Vec3& vec,    // destination vector
        const std::vector<cocos2d::Vec3>& positionList,
        const std::vector<Triangle>& triangleList
    );

private:

    class Node {
    public:
        struct InternalNode {
            float line;
            Node* right;
            Node* left;
        };
        struct LeafNode {
            int* indices;
            int indexCount;
        };
        union {
            InternalNode internal;
            LeafNode leaf;
        };

        enum Axis {
            NONE = 0,
            X = 1,
            Y = 2,
            Z = 3,
        };
        Axis axis;

        Node();
        ~Node();

        // build k-d tree
        void build(const int* indices, int indicesCount, const std::vector<cocos2d::Vec3>& positionList, const std::vector<Triangle>& triangleList, Node** nodePosition, cocos2d::Vec3 minV, cocos2d::Vec3 maxV, int maxDepth);

        // collision detection
        void getIntersection(int* index, float* t, const cocos2d::Vec3& begin, const cocos2d::Vec3& vec, const std::vector<cocos2d::Vec3>& positionList, const std::vector<Triangle>& triangleList, float beginT = 0.f, float endT = 1.f) const;

        // utility
        bool isLeaf() const { return axis == Axis::NONE; };

    private:
        // build k-d tree
        void countDividedTriangles(int* tL, int* tR, bool* hitL, bool* hitR, const int* indices, int indexCount, const std::vector<cocos2d::Vec3>& positionList, const std::vector<Triangle>& triangleList);

        void buildChildNodeLeft(const int* indices, int indexCount, int indexCountChild, const bool* hit, const std::vector<cocos2d::Vec3>& positionList, const std::vector<Triangle>& triangleList, Node** nodePosition, const cocos2d::Vec3& minV, const cocos2d::Vec3& maxV, int maxDepth);
        void buildChildNodeRight(const int* indices, int indexCount, int indexCountChild, const bool* hit, const std::vector<cocos2d::Vec3>& positionList, const std::vector<Triangle>& triangleList, Node** nodePosition, const cocos2d::Vec3& minV, const cocos2d::Vec3& maxV, int maxDepth);
        void buildChildNode(const int* indices, int indexCount, int indexCountChild, const bool* hit, const std::vector<cocos2d::Vec3>& positionList, const std::vector<Triangle>& triangleList, Node** nodePosition, cocos2d::Vec3 minV, cocos2d::Vec3 maxV, int maxDepth, bool isLeft);

        // collision detection
        bool getIntersectionTriangleAndLineSegment(float* tOut, const cocos2d::Vec3& begin, const cocos2d::Vec3& vec, const std::vector<cocos2d::Vec3>& positionList, const std::vector<Triangle>& triangleList, int index) const;
        bool solveLinearSystem(cocos2d::Vec3* o, const cocos2d::Vec3& a, const cocos2d::Vec3& b, const cocos2d::Vec3& c, const cocos2d::Vec3& d) const;

        // utility
        static Axis getAxis(int axis);
        static float getAxisValue(const cocos2d::Vec3& vec, Axis axis);
        static float getAxisValue(const cocos2d::Vec3& vec, int axis);
        static void setAxisValue(cocos2d::Vec3* vec, Axis axis, float val);
    };

    const std::vector<cocos2d::Vec3>& positionList;
    const std::vector<Triangle>& triangleList;
    Node* nodeList;

    void init();

    CollisionMesh(const std::vector<cocos2d::Vec3>& positionList, const std::vector<Triangle>& triangleList);
    ~CollisionMesh();
};

#endif /* defined(__Flight__CollisionMesh__) */
