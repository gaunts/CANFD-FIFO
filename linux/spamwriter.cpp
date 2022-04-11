#include <iostream>
#include <thread>
#include <cstdlib>
#include "CanBitrate.hh"
#include "CanController.hh"
#include "CanMessage.hh"
#include "CanOperatingModes.hh"

CppCan::CanController controller_;
uint64_t count = 0;
std::thread logthread;


void logLoop()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::system("clear");
        std::cout << count << std::endl;
    }
}

int main()
{
    CppCan::CanBitrate standardBitrate(CppCan::eCanBittimingMode_Raw, 2, 63, 16, 16, 0);
    CppCan::CanBitrate extendedBitrate(CppCan::eCanBittimingMode_Raw, 2, 7, 2, 2, 16);
    bool connected = controller_.Open(CppCan::CanOperatingMode::eCanOperatingMode_Standard,
                                CppCan::CanExtendedOperatingMode::eCanExtendedOperatingMode_FastDataRate
                                | CppCan::CanExtendedOperatingMode::eCanExtendedOperatingMode_ExtendedDataLength
                                | CppCan::CanExtendedOperatingMode::eCanExtendedOperatingMode_IsoCanFd,
                                standardBitrate,
                                extendedBitrate);
    if (!connected)
    {
        std::cout << "Cannot connect" << std::endl;
        return 0;
    }

    logthread = std::thread(&logLoop);

    while (true)
    {
        CppCan::CanMessage canMessage;
        canMessage.destinationId = 160;
        canMessage.SetDataSize(8);
        controller_.SendMessage(canMessage);
        controller_.SendMessage(canMessage);
        
        canMessage[0] = 0x61;
        canMessage[1] = 0x10;
        canMessage[2] = 0x20;
        controller_.SendMessage(canMessage);

        canMessage[0] = 0x69;
        canMessage[1] = 0x10;
        canMessage[2] = 0xE0;
        controller_.SendMessage(canMessage);
        count += 4;
    }
}
