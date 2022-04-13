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

// void logLoop()
// {
//     while (true)
//     {
//         std::this_thread::sleep_for(std::chrono::milliseconds(1));
//         std::system("clear");
//         std::cout << count << std::endl;
//     }
// }

int main()
{
    CppCan::CanBitrate standardBitrate(CppCan::eCanBittimingMode_Raw, 2, 63, 16, 16, 0);
    CppCan::CanBitrate extendedBitrate(CppCan::eCanBittimingMode_Raw, 2, 5, 4, 4, 0);
    bool connected = controller_.Open(CppCan::CanOperatingMode::eCanOperatingMode_Standard,
                                CppCan::CanExtendedOperatingMode::eCanExtendedOperatingMode_FastDataRate
                                | CppCan::CanExtendedOperatingMode::eCanExtendedOperatingMode_ExtendedDataLength
                                | CppCan::CanExtendedOperatingMode::eCanExtendedOperatingMode_IsoCanFd,
                                standardBitrate,
                                extendedBitrate);

    // std::thread t(&logLoop);
    if (!connected)
    {
        std::cout << "Cannot connect" << std::endl;
        return 0;
    }
    // controller_.EnableIxaatFix();

    CppCan::CanMessage message;
    int count = 0;
    while (true)
    {
        if (!controller_.ReadMessage(message))
            continue;
        if (message.DataSize() == 8 && message[2] == 0x22)
            controller_.SendMessage(message);
    }
}
