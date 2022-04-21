using System.Diagnostics;

int track = 0;
AutoResetEvent _receptionEvent = new AutoResetEvent(false);
int sentMessages = 0;
Stopwatch sw = new Stopwatch();
CanManager.CanManager.Connect(1);

Console.CursorVisible = false;
sw.Start();

_ = DisplayLoop();

while (true)
{
    var msg = CanManager.CanManager.WaitForMessageReception();
    track = msg[0];
    CanManager.CanManager.SendMessage(track);
    sentMessages++;
}

async Task DisplayLoop()
{
    await Task.Run(async () =>
    {
        while (true)
        {
            if (sentMessages == 0)
            {
                sw.Restart();
            }
            Console.SetCursorPosition(0, 0);
            Console.WriteLine(sentMessages);
            Console.Write((((double)sentMessages / (double)sw.ElapsedMilliseconds) * 1000).ToString("F"));
            Console.WriteLine(" / s                ");
            Console.WriteLine(track + "     ");
            await Task.Delay(1);
        }
    });
}
