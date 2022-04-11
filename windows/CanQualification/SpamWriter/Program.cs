// See https://aka.ms/new-console-template for more information
using System.Diagnostics;

UInt16 track = 0;
AutoResetEvent _receptionEvent = new AutoResetEvent(false);
int sentMessages = 0;
Stopwatch sw = new Stopwatch();

await Task.Delay(1000);

Console.CursorVisible = false;
_ = SendLoop();
sw.Start();

while (true)
{
    Console.SetCursorPosition(0, 0);
    Console.WriteLine(sentMessages);
    Console.Write((((double)sentMessages / (double)sw.ElapsedMilliseconds) * 1000).ToString("F"));
    Console.WriteLine(" / s                ");
    await Task.Delay(1);
}

async Task SendLoop()
{

}
