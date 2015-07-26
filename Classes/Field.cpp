//
//  Field.cpp
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/17.
//
//

#include <iostream>
#include "Field.h"
#include "ExSprite3D.h"
#include "FieldDataSource.h"
#include "Airplane.h"
#include "Global.h"
#include "Sphere.h"

using namespace std;
using namespace cocos2d;

Field* Field::createById(int id, bool collisionMesh)
{
    const FieldData FieldData = FieldDataSource::findById(id);

    if (FieldData.id == FieldData::NOT_FOUND) {
        return nullptr;
    }

    return Field::createWithData(FieldData, collisionMesh);
}

Field* Field::createWithData(const FieldData& data, bool collisionMesh)
{
    Field* field = new Field();

    if (field) {
        field->enableCollisionDetection(collisionMesh);

        if (field->initWithFile(data.filenameTerrain)) {
            field->autorelease();
        }
    } else {
        delete field;
        field = nullptr;
        return nullptr;
    }

    field->fieldId = data.id;
    field->fieldName = data.name;

    return field;
}

#pragma mark -
#pragma mark パラメータ

int Field::getFieldId() const
{
    return fieldId;
}

const string& Field::getFieldName() const
{
    return fieldName;
}