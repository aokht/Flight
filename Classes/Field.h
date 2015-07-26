//
//  Field.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/17.
//
//

#ifndef __Flight__Field__
#define __Flight__Field__

#include "cocos2d.h"
#include "ExSprite3D.h"
#include "FieldDataSource.h"

class Field : public ExSprite3D
{
public:
    // Life Cycle
    static Field* createById(int i, bool collisionMesh = false);
    static Field* createWithData(const FieldData& data, bool collisionMesh = false);

    // パラメータ
    int getFieldId() const;
    const std::string& getFieldName() const;

protected:

    // パラメータ
    int fieldId;
    std::string fieldName;
};

#endif /* defined(__Flight__Field__) */
