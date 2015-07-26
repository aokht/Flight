//
//  SceneData.h
//  Flight
//
//  Created by Akihito OKUHATA on 2015/07/25.
//
//

#ifndef Flight_SceneData_h
#define Flight_SceneData_h


class SceneData
{
public:
    enum Mode {
        SINGLE = 1,
        MULTI  = 2,
    };
    Mode mode;

    int airplaneId;
    int stageId;

};

#endif
