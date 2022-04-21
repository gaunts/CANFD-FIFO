#ifndef CANHELPER_HH_
#define CANHELPER_HH_

extern "C"
{
    #include <ECI116.h>
}

class CanHelper
{
    ECI_CTRL_HDL _controllerHandle = ECI_INVALID_HANDLE;

 public:
    inline bool ReadMessage(ECI_CTRL_MESSAGE *message)
    {
        DWORD receiveCount = 1;
        message->u.sCanMessage.dwVer = ECI_STRUCT_VERSION_V1;

        HRESULT result = ECI116_CtrlReceive(this->_controllerHandle, &receiveCount, message, 10);
        if (receiveCount == 0 || result != ECI_OK)
            return false;

        return true;
    }

    bool SendMessage(const ECI_CTRL_MESSAGE *message)
    {
        HRESULT result = ECI116_CtrlSend(_controllerHandle, message, 500);
        return result == ECI_OK;
    }

    inline bool Connect()
    {
        ECI_HW_PARA astcHwPara  = {0};
        ECI_HW_INFO stcHwInfo   = {0};

        astcHwPara.wHardwareClass = ECI_HW_USB;
        HRESULT result = ECI116_Initialize(1, &astcHwPara);
        if (result)
            result = ECI116_GetInfo(0, &stcHwInfo);

        if (result != ECI_OK)
            return false;

        ECI_CTRL_CONFIG stcCtrlConfig = {0};
        ECI_CANBTP stcBtpSdr     = {ECI_CAN_BTMODE_NATIVE, 2, 63, 16, 16, 0};
        ECI_CANBTP stcBtpFdr     = {ECI_CAN_BTMODE_NATIVE, 2, 5, 4, 4, 0};

        stcCtrlConfig.wCtrlClass                       = ECI_CTRL_CAN;
        stcCtrlConfig.u.sCanConfig.dwVer               = ECI_STRUCT_VERSION_V1;
        stcCtrlConfig.u.sCanConfig.u.V1.bOpMode        = static_cast<int>(ECI_CAN_OPMODE_STANDARD);
        stcCtrlConfig.u.sCanConfig.u.V1.bExMode        = static_cast<int>(ECI_CAN_EXMODE_FASTDATA | ECI_CAN_EXMODE_EXTDATA | ECI_CAN_EXMODE_ISOFD);
        stcCtrlConfig.u.sCanConfig.u.V1.sBtpSdr        = stcBtpSdr;
        stcCtrlConfig.u.sCanConfig.u.V1.sBtpFdr = stcBtpFdr;

        HRESULT hResult = ECI116_CtrlOpen(&_controllerHandle, 0, 0, &stcCtrlConfig);
        if (hResult != ECI_OK)
            return false;
        hResult = ECI116_CtrlStart(_controllerHandle);

        return hResult == ECI_OK;
    }
};

#endif  // CANHELPER_HH_