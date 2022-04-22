using Ixxat.Vci4;
using Ixxat.Vci4.Bal;
using Ixxat.Vci4.Bal.Can;
using System.Diagnostics;

namespace CanManager
{
    public static class CanManager
    {
        private static AutoResetEvent _readAutoResetEvent = new AutoResetEvent(false);
        private static ICanMessageWriter _writer;
        private static ICanMessageReader _reader;

        public static void Connect(int deviceIndex = 0)
        {
            using (IVciDeviceManager deviceManager = VciServer.Instance().DeviceManager)
            {
                var deviceList = deviceManager.GetDeviceList();
                IBalObject? balObject = null;
                int i = 0;
                foreach (IVciDevice device in deviceList)
                {
                    if (i++ != deviceIndex)
                        continue;
                    balObject = device.OpenBusAccessLayer();
                    device.Dispose();
                }

                if (balObject == null)
                    throw new Exception("Can Manager could not create a BAL object");

                var fdControl = (ICanControl2)balObject.OpenSocket(0, typeof(ICanControl2));
                var bitRate = new CanBitrate2(CanBitrateMode.Raw, 2, 63, 16, 16, 0);
                var extendedBitRate = new CanBitrate2(CanBitrateMode.Raw, 2, 5, 4, 4, 0);
                fdControl.InitLine(CanOperatingModes.Standard,
                    CanExtendedOperatingModes.FastDataRate | CanExtendedOperatingModes.ExtendedDataLength, CanFilterModes.Pass, 0, CanFilterModes.Pass, 0, bitRate, extendedBitRate);
                fdControl.StartLine();

                var _fdChannel = (ICanChannel2)balObject.OpenSocket(0, typeof(ICanChannel2));
                _fdChannel.Initialize(110, 110, 0, CanFilterModes.Pass, false);
                _fdChannel.Activate();
                _writer = _fdChannel.GetMessageWriter();
                _reader = _fdChannel.GetMessageReader();
                _reader.AssignEvent(_readAutoResetEvent);

                //while (_reader.FillCount > 0)
                //    _reader.ReadMessage(out ICanMessage2 _);
            }
        }

        public static bool SendMessage(int track)
        {
            IMessageFactory factory = VciServer.Instance().MsgFactory;
            ICanMessage2 message = (ICanMessage2)factory.CreateMsg(typeof(ICanMessage2));

            message.TimeStamp = 0;
            message.Identifier = 1;
            message.FrameType = CanMsgFrameType.Data;
            message.SelfReceptionRequest = false;
            message.ExtendedFrameFormat = false;
            message.DataLength = 8;
            message[0] = BitConverter.GetBytes(track).First();
            // Filling bytes with know values, just for testing purposes.
            message[1] = 0x11;
            message[2] = 0x22;
            message[3] = 0x33;
            message[4] = 0x44;
            message[5] = 0x55;
            message[6] = 0x66;
            message[7] = 0x77;

            return _writer.SendMessage(message);
        }

        public static ICanMessage2 WaitForMessageReception()
        {
            if (_reader.FillCount == 0)
                _readAutoResetEvent.WaitOne();

            if (!_reader.ReadMessage(out ICanMessage2 msg))
            {
                return WaitForMessageReception();
            }
            else if (msg.FrameType != CanMsgFrameType.Data || (msg[2] != 0x22 && msg[2] != 0x20)) // 0x20 is used by an other test program. Please ignore.
            {
                Debug.WriteLine(msg.ToString());
                Debug.WriteLine('\t' + "Reader.FillCount = " + _reader.FillCount);
                Debug.WriteLine('\t' + "Reader.Capacity = " + _reader.Capacity);
                Debug.WriteLine('\t' + "Writer.FreeCount = " + _writer.FreeCount);
                return WaitForMessageReception();
            }
            else
            {
                return msg;
            }
        }
    }
}