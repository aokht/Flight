//
//  CollisionMesh.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/18.
//
//  参考: 平山尚 (2008). ゲームプログラマになる前に覚えておきたい技術 秀和システム
//

#include <iostream>
#include "CollisionMesh.h"
#include "Global.h"

using namespace std;
using namespace cocos2d;

#pragma mark -
#pragma mark CollisionMesh

CollisionMesh::CollisionMesh(const vector<Vec3>& positionList, const vector<Triangle>& triangleList) :
    nodeList(nullptr),
    positionList(positionList),
    triangleList(triangleList)
{
}

CollisionMesh::~CollisionMesh()
{
    if (nodeList) {
        delete[] nodeList;
    }
}

CollisionMesh* CollisionMesh::create(const vector<Vec3>& positionList, const vector<Triangle>& triangleList)
{
    CollisionMesh* collisionMesh = new CollisionMesh(positionList, triangleList);
    collisionMesh->init();
    collisionMesh->autorelease();

    return collisionMesh;
}

void CollisionMesh::init()
{
    // サイズ計算
    int triangleCount = (int)triangleList.size();
    int maxNodeCount = 1;
    int maxDepth = 0;
    while (maxNodeCount < triangleCount) {
        maxNodeCount <<= 1;
        maxDepth++;
    }

    // 三角形インデックス作成
    int indices[triangleCount];
    for (int i = 0; i < triangleCount; ++i) {
        indices[i] = i;
    }

    // 世界の大きさを検出
    Vec3 minV = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    Vec3 maxV = -minV;
    for (auto p = positionList.begin(), last = positionList.end(); p != last; ++p) {
        minV = Vec3(min(minV.x, p->x), min(minV.y, p->y), min(minV.z, p->z));
        maxV = Vec3(max(maxV.x, p->x), max(maxV.y, p->y), max(maxV.z, p->z));
    }

    // 領域作成
    this->nodeList = new CollisionMesh::Node[maxNodeCount * 2];
    Node* nodePos = nodeList;

    // ルートノード割り当て、生成
    Node* rootNode = nodePos++;
    rootNode->build(indices, triangleCount, positionList, triangleList, &nodePos, minV, maxV, maxDepth);
}

bool CollisionMesh::getIntersection(int* index, float* tOut, const Vec3& a, const Vec3& b, const vector<Vec3>& positionList, const vector<Triangle>& triangleList)
{
    CCASSERT(index, "invalid argument: index");
    CCASSERT(tOut, "invalid argument: tOut");

    int i = -1;
    float t = FLT_MAX;
    nodeList[0].getIntersection(&i, &t, a, b, positionList, triangleList);

    if (i != -1) {
        *index = i;
        *tOut = t;
        return true;
    } else {
        return false;
    }
}


#pragma mark -
#pragma mark Node

CollisionMesh::Node::Node() :
    leaf({ nullptr, 0 }),
    axis(Axis::NONE)
{
}

CollisionMesh::Node::~Node()
{
    if (isLeaf()) {
        delete[] leaf.indices;
    }
}

#pragma mark -
#pragma mark build k-d tree

void CollisionMesh::Node::build(const int* indices, int indexCount, const vector<Vec3>& positionList, const vector<CollisionMesh::Triangle>& triangleList, Node** nodePosition, Vec3 minV, Vec3 maxV, int maxDepth)
{
    // maxDepth に達していたらリーフノードとする
    if (maxDepth <= 1) {
        axis = Axis::NONE;
        leaf.indexCount = indexCount;
        leaf.indices = new int[indexCount];
        memcpy(leaf.indices, indices, sizeof(int) * indexCount);
    }
    // それ以外の場合は分割する
    else {
        // 分割軸(一番長い辺)を検出
        Vec3 world = maxV - minV;
        float maxLength = -FLT_MAX;
        for (int a = Axis::X; a <= Axis::Z; ++a) {
            if (maxLength < getAxisValue(world, a)) {
                maxLength = getAxisValue(world, a);
                axis = getAxis(a);
            }
        }
        // 分割値
        internal.line = (getAxisValue(minV, axis) + getAxisValue(world, axis) * 0.5f);

        // 左右に振り分ける三角形の数をカウント
        int tL = 0, tR = 0;
        bool hitL[indexCount], hitR[indexCount];
        this->countDividedTriangles(&tL, &tR, hitL, hitR, indices, indexCount, positionList, triangleList);

        // 左右それぞれ存在すれば、子ノードを作成する
        if (tL) {
            this->buildChildNodeLeft(indices, indexCount, tL, hitL, positionList, triangleList, nodePosition, minV, maxV, maxDepth - 1);
        }
        if (tR) {
            this->buildChildNodeRight(indices, indexCount, tR, hitR, positionList, triangleList, nodePosition, minV, maxV, maxDepth - 1);
        }
    }
}

void CollisionMesh::Node::countDividedTriangles(int* tL, int* tR, bool* hitL, bool* hitR, const int* indices, int indexCount, const vector<Vec3>& positionList, const vector<Triangle>& triangleList)
{
    for (int i = 0; i < indexCount; ++i) {
        const Triangle& t = triangleList[indices[i]];
        const Vec3& p0 = positionList[t.v1];
        const Vec3& p1 = positionList[t.v2];
        const Vec3& p2 = positionList[t.v3];
        float minP = min(min(getAxisValue(p0, axis), getAxisValue(p1, axis)), getAxisValue(p2, axis));
        float maxP = max(max(getAxisValue(p0, axis), getAxisValue(p1, axis)), getAxisValue(p2, axis));

        hitL[i] = (minP < internal.line);  // 左に所属
        hitR[i] = (internal.line < maxP);  // 右に所属
        if (minP == maxP && maxP == internal.line) {  // 分割線ぴったりの場合 => 右
            hitR[i] = true;
        }

        if (hitL[i]) {
            (*tL)++;
        }
        if (hitR[i]) {
            (*tR)++;
        }
    }
}

void CollisionMesh::Node::buildChildNodeLeft(const int* indices, int indexCount, int indexCountChild, const bool* hit, const vector<Vec3>& positionList, const vector<CollisionMesh::Triangle>& triangleList, Node** nodePosition, const Vec3& minV, const Vec3& maxV, int maxDepth)
{
    this->buildChildNode(indices, indexCount, indexCountChild, hit, positionList, triangleList, nodePosition, minV, maxV, maxDepth, true);
}

void CollisionMesh::Node::buildChildNodeRight(const int* indices, int indexCount, int indexCountChild, const bool* hit, const vector<Vec3>& positionList, const vector<CollisionMesh::Triangle>& triangleList, Node** nodePosition, const Vec3& minV, const Vec3& maxV, int maxDepth)
{
    this->buildChildNode(indices, indexCount, indexCountChild, hit, positionList, triangleList, nodePosition, minV, maxV, maxDepth, false);
}

void CollisionMesh::Node::buildChildNode(const int* indices, int indexCount, int indexCountChild, const bool* hit, const vector<Vec3>& positionList, const vector<CollisionMesh::Triangle>& triangleList, Node** nodePosition, Vec3 minV, Vec3 maxV, int maxDepth, bool isLeft)
{
    // 三角形のインデックスリスト作成
    int indicesChild[indexCountChild];
    for (int i = 0, childCount = 0; i < indexCount; ++i) {
        if (hit[i]) {
            indicesChild[childCount++] = indices[i];
        }
    }

    // 新しい範囲を作成
    setAxisValue((isLeft ? &maxV : &minV), axis, internal.line);

    // 子ノード割り当て、生成
    Node** child = isLeft ? &internal.left : &internal.right;
    *child = (*nodePosition)++;
    (*child)->build(indicesChild, indexCountChild, positionList, triangleList, nodePosition, minV, maxV, maxDepth);
}

#pragma mark -
#pragma mark Collision Detection

void CollisionMesh::Node::getIntersection(int* minIndex, float* minT, const Vec3& begin, const Vec3& vec, const vector<Vec3>& positionList, const vector<Triangle>& triangleList, float beginT, float endT) const
{
    // internal (葉以外の場合: 再帰的に葉までたどる)
    if (axis) {
        const Vec3& a = begin;
        const Vec3& b = vec;
        const Node* l = internal.left;
        const Node* r = internal.right;

        // 平面に当たるときの t を求める
        // d = a + bt より t = (d - a) / b
        float fa = getAxisValue(a, axis);
        float fb = getAxisValue(b, axis);
        float divT = (fb != 0.f) ? ((internal.line - fa) / fb) : FLT_MAX;  // 0割 == 軸と垂直

        float endT0, beginT1; // 分割点のt ( beginT -> endT0|beginT1 -> endT )
        if (endT <= divT) {        // 分割面が遠すぎて到達しない: 分割点のt == endT
            endT0 = endT;
            beginT1 = endT;
        }
        else if (divT > beginT) {  // 分割面が範囲内: 分割点のt == divT
            endT0 = divT;
            beginT1 = divT;
        }
        else {                     // 分割面は自分の後ろにあり到達しない: 分割点のt == endT
            endT0 = endT;
            beginT1 = endT;
        }

        // 判定開始点
        float start = fa + beginT * fb;

        // 誤差を求める
        float e = abs(fa) + abs(beginT * fb) + abs(internal.line);
        e *= 0.f;//EPSILON * 10; // 念のため10倍

        // 分割点より左にいる
        if (start + e < internal.line) {
            if (l) {
                l->getIntersection(minIndex, minT, begin, vec, positionList, triangleList, beginT, endT0);
            }
            // 分割点を間に挟むならその先(右)を検索
            if (beginT1 < endT) {
                if (r) {
                    r->getIntersection(minIndex, minT, begin, vec, positionList, triangleList, beginT1, endT);
                }
            }
        }
        // 分割点より右にいる
        else if (start - e > internal.line) {
            if (r) {
                r->getIntersection(minIndex, minT, begin, vec, positionList, triangleList, beginT, endT0);
            }
            // 分割点を挟むならその先(左)を検索
            if (beginT1 < endT) {
                if (l) {
                    l->getIntersection(minIndex, minT, begin, vec, positionList, triangleList, beginT1, endT);
                }
            }
        }
        // 分からないので両方検索
        else {
            if (l) {
                l->getIntersection(minIndex, minT, begin, vec, positionList, triangleList, beginT, endT);
            }
            if (r) {
                l->getIntersection(minIndex, minT, begin, vec, positionList, triangleList, beginT, endT);
            }
        }
    }
    // 葉の場合: 総当りで衝突判定
    else {  // Internal
        for (int i = 0; i < leaf.indexCount; ++i) {
            int triangleIndex = leaf.indices[i];
            float t;
            if (getIntersectionTriangleAndLineSegment(&t, begin, vec, positionList, triangleList, triangleIndex)) {
                // より近い方を選ぶ
                if (t < *minT) {
                    *minT = t;
                    *minIndex = triangleIndex;
                }
            }
        }
    }
}

bool CollisionMesh::Node::getIntersectionTriangleAndLineSegment(float *tOut, const cocos2d::Vec3 &begin, const cocos2d::Vec3 &vec, const std::vector<cocos2d::Vec3> &positionList, const std::vector<Triangle> &triangleList, int index) const
{
    const Triangle& triangle = triangleList[index];
    const Vec3& p0 = positionList[triangle.v1];
    const Vec3& p1 = positionList[triangle.v2];
    const Vec3& p2 = positionList[triangle.v3];

    // p0, p1, p2 からなる三角形と、 a, b からなる線分の交差点を求める
    //
    // p0 + u*(p1 - p0) + v*(p2 - p0) = a + t*b
    // ただし、 0 <= u + v <= 1 (三角形), 0 <= t <= 1 (線分)
    //
    // t*b - (u(p1 - p0) + v(p2 - p0)) = p0 - a
    //
    // d = p1 - p0
    // e = p2 - p0
    // f = p0 - a
    //
    // | bx -dx -ex | | t |   | fx |
    // | by -dy -ey | | u | = | fy |
    // | bz -dz -ez | | v |   | fz |
    //
    // を解く

    const Vec3& b = vec;
    Vec3 d = p1 - p0;
    Vec3 e = p2 - p0;
    Vec3 f = p0 - begin;

    Vec3 tuv;  // 解
    if (solveLinearSystem(&tuv, b, -d, -e, f)) {
        *tOut = tuv.x; // 必要なのは t (= Vec3.x)
        return true;
    } else {
        return false;  // 外れ
    }
}

bool CollisionMesh::Node::solveLinearSystem(Vec3* o, const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d ) const
{
    // | ax bx cx | | ox | = | dx |
    // | ay by cy | | oy | = | dy |
    // | az bz cz | | oz | = | dz |
    //
    // をクラメルの公式を用いて解く
    //       det(Ai)
    // xi = ---------
    //       det(A)

    // 行列式 det(A) を計算
    float m1021 = a.y * b.z;
    float m1022 = a.y * c.z;

    float m1120 = b.y * a.z;
    float m1122 = b.y * c.z;

    float m1220 = c.y * a.z;
    float m1221 = c.y * b.z;

    float m1122_m1221 = m1122 - m1221;
    float m1220_m1022 = m1220 - m1022;
    float m1021_m1120 = m1021 - m1120;

    //00*( 11*22 - 12*21 ) - 01*( 10*22 - 12*20 ) + 02*( 10*21 - 11*20 )
    float detA = a.x * m1122_m1221 + b.x * m1220_m1022 + c.x * m1021_m1120;
    if ( detA == 0.f ){ //解けない
        return false;
    }
    // 逆数(reciprocal) をとる
    float rcpDetA = 1.f / detA;

    // 桁落ちが 3桁にとどまると考え、EPSILON * 1000 を用いる
    // 桁落ちが 3桁 == 内積が0.001以下 == 0.05度以下の平行
    static const float zero = 0.f - EPSILON * 1000.f;
    static const float one  = 1.f + EPSILON * 1000.f;

    // det(Ai) を掛ける
    float m0221 = c.x * b.z;
    float m0122 = b.x * c.z;
    float m0112 = b.x * c.y;
    float m0211 = c.x * b.y;
    o->x = m1122_m1221 * d.x + (m0221-m0122) * d.y + (m0112-m0211) * d.z;

    // 0 <= t <= 1 を判定
    o->x *= rcpDetA;
    if (o->x < zero || o->x > one) {
        return false;
    }

    // u (i = 2) を計算
    float m0022 = a.x * c.z;
    float m0220 = c.x * a.z;
    float m0210 = c.x * a.y;
    float m0012 = a.x * c.y;
    o->y = m1220_m1022 * d.x + (m0022-m0220) * d.y + (m0210-m0012) * d.z;
    o->y *= rcpDetA;
    if (o->y < zero) {
        return false;
    }

    // v (i = 3) を計算
    float m0120 = b.x * a.z;
    float m0021 = a.x * b.z;
    float m0011 = a.x * b.y;
    float m0110 = b.x * a.y;
    o->z = m1021_m1120 * d.x + (m0120-m0021) * d.y + (m0011-m0110) * d.z;
    o->z *= rcpDetA;
    if (o->z < zero) {
        return false;
    }
    // 0 <= u + v <= 1 を判定
    if (o->y + o->z > one) {
        return false;
    }

    // 解けた
    return true;
}

#pragma mark -
#pragma mark Utility

CollisionMesh::Node::Axis CollisionMesh::Node::getAxis(int axis)
{
    switch (axis) {
        case Axis::X:
            return Axis::X;
        case Axis::Y:
            return Axis::Y;
        case Axis::Z:
            return Axis::Z;
        case Axis::NONE:
            return Axis::NONE;
        default:
            CCASSERT(false, "Invalid Axis");
    }
}

float CollisionMesh::Node::getAxisValue(const cocos2d::Vec3& vec, Axis axis)
{
    switch (axis) {
        case Axis::X:
            return vec.x;
        case Axis::Y:
            return vec.y;
        case Axis::Z:
            return vec.z;
        default:
            CCASSERT(false, "No Axis specified");
    }
}

float CollisionMesh::Node::getAxisValue(const cocos2d::Vec3& vec, int axis)
{
    return getAxisValue(vec, getAxis(axis));
}

void CollisionMesh::Node::setAxisValue(cocos2d::Vec3* vec, Axis axis, float val)
{
    switch (axis) {
        case Axis::X:
            vec->x = val;
            break;
        case Axis::Y:
            vec->y = val;
            break;
        case Axis::Z:
            vec->z = val;
            break;
        default:
            CCASSERT(false, "No Axis specified");
    }
}