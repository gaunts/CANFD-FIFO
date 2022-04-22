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
        std::cout << "\33[2K\r" << count << std::endl;
        double seconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count() / 1000.0;
        std::cout << "\33[2K\r" << (double)count / seconds << " / s" << std::endl;
        std::cout << "\33[2K\r" << lastReceivedTrack << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void sendMessage(BYTE trackingValue)
{
    ECI_CTRL_MESSAGE msg = {0};
    msg.wCtrlClass = ECI_CTRL_CAN;
    msg.u.sCanMessage.dwVer = ECI_STRUCT_VERSION_V1;
    msg.u.sCanMessage.u.V1.dwMsgId = 1;
    msg.u.sCanMessage.u.V1.uMsgInfo.Bits.dlc = 8;
    msg.u.sCanMessage.u.V1.uMsgInfo.Bits.type = ECI_CAN_MSGTYPE_DATA;
    msg.u.sCanMessage.u.V1.uMsgInfo.Bits.ext = false;
    msg.u.sCanMessage.u.V1.uMsgInfo.Bits.srr = false;
    msg.u.sCanMessage.u.V1.uMsgInfo.Bits.edl = 0;
    msg.u.sCanMessage.u.V1.abData[0] = trackingValue;
    msg.u.sCanMessage.u.V1.abData[1] = 0x11;
    msg.u.sCanMessage.u.V1.abData[2] = 0x22;
    msg.u.sCanMessage.u.V1.abData[3] = 0x33;
    msg.u.sCanMessage.u.V1.abData[4] = 0x44;
    msg.u.sCanMessage.u.V1.abData[5] = 0x55;
    msg.u.sCanMessage.u.V1.abData[6] = 0x66;
    msg.u.sCanMessage.u.V1.abData[7] = 0x77;
    if (!canHelper.SendMessage(&msg))
        throw std::runtime_error("CAN write error");
}

int main()
{
    system("clear");
    std::thread t(&logLoop);
 
    if (!canHelper.Connect())
    {
        std::cout << "Cannot connect" << std::endl;
        return 0;
    }

    ECI_CTRL_MESSAGE message = {0};
    while (true)
    {
        if (!canHelper.ReadMessage(&message))
            continue;
        if (message.u.sCanMessage.u.V1.uMsgInfo.Bits.dlc == 8 && message.u.sCanMessage.u.V1.abData[2] == 0x22)
        {
            count++;
            lastReceivedTrack = message.u.sCanMessage.u.V1.abData[0];
            sendMessage(message.u.sCanMessage.u.V1.abData[0]);
        }
        else
        {
            if (message.u.sCanMessage.u.V1.abData[0] != 0)
                std::cout << (int)message.u.sCanMessage.u.V1.uMsgInfo.Bytes.bType << " : " << (int)message.u.sCanMessage.u.V1.abData[0] << std::endl;
        }
    }
}
