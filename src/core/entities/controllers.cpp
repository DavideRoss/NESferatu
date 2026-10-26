#include "controllers.h"

#include "utils/exception.h"

#include "savestates/serializer.h"
#include "savestates/deserializer.h"

void Controllers::Reset()
{
    strobe_ = false;

    ctrl1_status_ = 0;
    ctrl2_status_ = 0;

    ctrl1_shifter_ = 0;
    ctrl2_shifter_ = 0;
}

u8 Controllers::CPURead(u16 address)
{
    switch (address)
    {
        case 0x4016:
        {
            if (strobe_) return ctrl1_status_ & 0x1;
            
            u8 value = ctrl1_shifter_ & 0x01;
            ctrl1_shifter_ >>= 1;
            ctrl1_shifter_ |= 0x80;

            // Emulating bit 6 high (open bus)
            return 0x40 | value;
        }

        case 0x4017:
        {
            if (strobe_) return ctrl2_status_ & 0x1;
            
            u8 value = ctrl2_shifter_ & 0x01;
            ctrl2_shifter_ >>= 1;
            ctrl2_shifter_ |= 0x80;

            // Emulating bit 6 high (open bus)
            return 0x40 | value;
        }

        default: NESFERATU_THROW("Unmapped address: 0x{:04x}", address);
    }
}

void Controllers::CPUWrite(u16 address, u8 data)
{
    if (address != 0x4016)
    {
        NESFERATU_THROW("Unmapped address: 0x{:04x}", address);
    }

    bool new_strobe = data & 0x1;

    // Strobe falling edge
    if (strobe_ && !new_strobe)
    {
        ctrl1_shifter_ = ctrl1_status_;
        ctrl2_shifter_ = ctrl2_status_;
    }

    strobe_ = new_strobe;
}

void Controllers::SetController1Status(u8 status)
{
    ctrl1_status_ = status;
}

void Controllers::SetController2Status(u8 status)
{
    ctrl2_status_ = status;
}

void Controllers::Serialize(Serializer &serializer)
{
    serializer.Write(strobe_);

    serializer.Write(ctrl1_status_);
    serializer.Write(ctrl2_status_);

    serializer.Write(ctrl1_shifter_);
    serializer.Write(ctrl2_shifter_);
}

void Controllers::Deserialize(Deserializer &deserializer)
{
    deserializer.Read(strobe_);

    deserializer.Read(ctrl1_status_);
    deserializer.Read(ctrl2_status_);
    
    deserializer.Read(ctrl1_shifter_);
    deserializer.Read(ctrl2_shifter_);
}
