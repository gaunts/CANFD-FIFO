#include <iostream>
#include <thread>
#include <cstdlib>
#include "canhelper.hh"

uint64_t count = 0;
int lastReceivedTrack = -1;
std::thread logthread;
CanHelper canHelper;

void SetCursorPosition(int XPos, int YPos) {
    printf("\033[%d;%dH",YPos+1,XPos+1);
}

void logLoop()
{
    auto startTime = std::chrono::system_clock::now();
    while (true)
    {
        if (count == 0)
            startTime = std::chrono::system_clock::now();
        auto now = std::chrono::system_clock::now();
        SetCursorPosition(0, 0);
        std::cout << count << std::endl;
        double seconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count() / 1000.0;
        std::cout << (double)count / seconds << " / s                          " << std::endl;
        std::cout << lastReceivedTrack << "  " << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main()
{
    system("clear");
    if (!canHelper.Connect())
    {
        std::cout << "Cannot connect" << std::endl;
        return 0;
    }

    std::thread t(&logLoop);
    ECI_CTRL_MESSAGE message = {0};
    while (true)
    {
        if (!canHelper.ReadMessage(&message))
            continue;
        if (message.u.sCanMessage.u.V1.uMsgInfo.Bits.dlc == 8 && message.u.sCanMessage.u.V1.abData[2] == 0x22)
        {
            count++;
            // std::cout << "Sending" << std::endl;
            ECI_CTRL_MESSAGE msg = {0};
            msg.u.sCanMessage.dwVer = ECI_STRUCT_VERSION_V1;
            msg.u.sCanMessage.u.V1.dwMsgId = 1;
            msg.u.sCanMessage.u.V1.uMsgInfo.Bits.dlc = 8;
            msg.u.sCanMessage.u.V1.uMsgInfo.Bits.type = ECI_CAN_MSGTYPE_DATA;
            msg.u.sCanMessage.u.V1.uMsgInfo.Bits.ext = false;
            msg.u.sCanMessage.u.V1.uMsgInfo.Bits.srr = false;
            msg.u.sCanMessage.u.V1.uMsgInfo.Bits.edl = 0;
            msg.u.sCanMessage.u.V1.abData[0] = message.u.sCanMessage.u.V1.abData[0];
            msg.u.sCanMessage.u.V1.abData[2] = message.u.sCanMessage.u.V1.abData[2];
            canHelper.SendMessage(&message);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}
