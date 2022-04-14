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

void SetCursorPosition(int XPos, int YPos) {
    printf("\033[%d;%dH",YPos+1,XPos+1);
}

void logLoop()
{
    auto startTime = std::chrono::system_clock::now();
    while (true)
    {
        auto now = std::chrono::system_clock::now();
        SetCursorPosition(0, 0);
        std::cout << count << std::endl;
        double seconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count() / 1000.0;
        std::cout << (double)count / seconds << " / s                          " << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

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

    std::thread t(&logLoop);
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
