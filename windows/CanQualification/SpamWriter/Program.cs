// See https://aka.ms/new-console-template for more information
using Ixxat.Vci4.Bal.Can;
using System.Diagnostics;

int track = 0;
int lastReceivedTrack = 0;
List<int> receivedTracks = new List<int>();
int received = 0;
AutoResetEvent receiveAutoReset = new AutoResetEvent(false);

int sentMessages = 0;
Stopwatch sw = new Stopwatch();
CanManager.CanManager.Connect(0);

await Task.Delay(1000);

Console.CursorVisible = false;
_ = SendLoop();
_ = ReceiveLoop();
sw.Start();

while (true)
{
    if(sentMessages == 0)
    {
        sw.Restart();
    }
    Console.SetCursorPosition(0, 0);
    Console.WriteLine(sentMessages);
    Console.Write((((double)sentMessages / (double)sw.ElapsedMilliseconds) * 1000).ToString("F"));
    Console.WriteLine(" / s                ");
    Console.WriteLine(track - 1 + "         ");
    Console.WriteLine(lastReceivedTrack + "          ");
    await Task.Delay(1);
}

async Task ReceiveLoop()
{
    await Task.Run(() =>
    {
        while (true)
        {
            ICanMessage2 msg = CanManager.CanManager.WaitForMessageReception();
            received++;
            lastReceivedTrack = msg[0];
            receivedTracks.Add(lastReceivedTrack);
            receiveAutoReset.Set();
        }
    });
}

async Task SendLoop()
{
    int fifosize = 3;
    await Task.Run(() =>
    {
        while (true)
        {
            for (int i = 0; i < fifosize; i++)
            {
                if (CanManager.CanManager.SendMessage(track++))
                {
                    if (track > 255)
                        track = 0;
                }
                else
                {
                    i--;
                }
                //Debug.WriteLine(track);
            }

            int lastTrack = track == 0 ? 255 : track - 1;
            while (lastReceivedTrack != lastTrack)
            {
                receiveAutoReset.WaitOne();
            }
            sentMessages += fifosize;
            received -= fifosize;
        }
    });
}
