#pragma once

class Memory
{
public:
    void ConnectBus(Bus* bus);

private:
    Bus* bus_ = nullptr;
};
