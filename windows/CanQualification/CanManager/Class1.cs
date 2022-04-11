using Ixxat.Vci4;
using Ixxat.Vci4.Bal;
using Ixxat.Vci4.Bal.Can;

namespace CanManager
{
    public static class CanManager
    {
        public static void Connect()
        {
            using (IVciDeviceManager deviceManager = VciServer.Instance().DeviceManager)
            {
                var deviceList = deviceManager.GetDeviceList();
                IBalObject? balObject = null;
                foreach (IVciDevice device in deviceList)
                {
                    balObject = device.OpenBusAccessLayer();
                    device.Dispose();
                }

                var fdControl = (ICanControl2)balObject.OpenSocket(0, typeof(ICanControl2));
                var bitRate = new CanBitrate2(CanBitrateMode.Raw, 2, 63, 16, 16, 0);
                var extendedBitRate = new CanBitrate2(CanBitrateMode.Raw, 2, 5, 4, 4, 0);
                fdControl.InitLine(CanOperatingModes.Standard,
                    CanExtendedOperatingModes.FastDataRate | CanExtendedOperatingModes.ExtendedDataLength, CanFilterModes.Pass, 0, CanFilterModes.Pass, 0, bitRate, extendedBitRate);
                fdControl.StartLine();

                var _fdChannel = (ICanChannel2)balObject.OpenSocket(0, typeof(ICanChannel2));
                _fdChannel.Initialize(3, 3, 0, CanFilterModes.Pass, false);
                _fdChannel.Activate();
                var _writer = _fdChannel.GetMessageWriter();
                var reader = _fdChannel.GetMessageReader();
                reader.AssignEvent(_readAutoResetEvent);
                var status = _fdChannel.ChannelStatus;
            }
        }

        public static void SendMessage()
        {

        }

        public static void WaitForMessageReception()
        {

        }
    }
}