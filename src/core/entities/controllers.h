#pragma once

#include "utils/types.h"

enum ControllerButtons : u8
{
    ButtonA = 1 << 0,
    ButtonB = 1 << 1,
    ButtonSelect = 1 << 2,
    ButtonStart = 1 << 3,
    ButtonUp = 1 << 4,
    ButtonDown = 1 << 5,
    ButtonLeft = 1 << 6,
    ButtonRight = 1 << 7
};

class Controllers
{
public:
    void Reset();

    u8 CPURead(u16 address);
    void CPUWrite(u16 address, u8 data);

    void SetController1Status(u8 status);
    void SetController2Status(u8 status);

    void Serialize(class Serializer& serializer);
    void Deserialize(class Deserializer& deserializer);

private:
    bool strobe_ = false;

    u8 ctrl1_status_ = 0x00;
    u8 ctrl2_status_ = 0x00;

    u8 ctrl1_shifter_ = 0x00;
    u8 ctrl2_shifter_ = 0x00;
};
