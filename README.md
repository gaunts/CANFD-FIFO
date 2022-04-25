# CANFD-FIFO

This repository reproduces a message loss due to full FIFO, using two IXXAT USB-to-CAN FD compact.

## Some context
On a machine with STM32 boards connected to a Linux machine, we sometimes experience message loss, probably due to FIFO problems. The driver on STM32 boards is able to detect FIFO errors. As a higher-level developer, I tried to reproduce using Windows or Linux as senders, and propose a solution to the embeded team.  

I have been able to reproduce, but can't find a solution to how FIFO errors should be handled. I tried changing the FIFO size programatically to no avail.


## Runing the test
### Setup
The setup used to use this program:
![Test-setup-image](/images/fifo-setup.png)

I am trying to detect FIFO related errors and statuses to delay or retry message sendings.
To reproduce such errors, the SpamWriter / SpamReader programs work as follows with X representing how much messages we can try sending in one go.  
When X = 1, no error ever occur. When X gets higher, errors happen earlier, with errors happening as soon as X = 2 :

![Test-sequence-image](/images/fifo-sequence.png)  

You can change the value of X through the variable fifoSize in SpamWriter.cs:
````c#
int fifoSize = 1;
````

### Execution
#### On Linux:  
- Install the ECI driver. I have version 1.13.3877.0
- Install make and g++
- Then, in the Linux folder:
````bash
$ make
$ ./spamreader.out
````

#### On Windows:
- Install the VCI driver. I have version 4.1.149.0.
- Launch SpamWriter from Visual studio. I use VS 2022.


SpamReader must be launched first. The Windows version of SpamReader works the same way and causes the same issues.

Both programs display the number of Query / Response succesfully made in total and per second, as well as the last tracking number value sent / received.
The Windows program also displays any Status message in the output window of Visual studio.
The Linux program displays the last non-data message received.

## Encountered problem

If fifoSize > 1, the sender will display the following messages:

````
7802811 : Status 0
	Reader.FillCount = 2
	Reader.Capacity = 152
	Writer.FreeCount = 152
7805058 : Status ErrLimit
	Reader.FillCount = 0
	Reader.Capacity = 152
	Writer.FreeCount = 152
````
As soon as these statuses are received, the program stops because at least one of the messages is lost. We observe that `lastReceivedTrackingNumber < lastSentTrackingNumber`

As we can see, Reader.FillCount is far from the max Capacity, and Writer.FreeCount is high enough to be sending messages.  
The FIFO sizes are both set to a high value in CanManager.cs. I have found no information about accepted values :
````C#
_fdChannel.Initialize(110, 110, 0, CanFilterModes.Pass, false);
````
ICanMessageWriter.SendMessage always returns `true` on Windows and Linux

## Questions

- How can I configure the FIFO size using the Linux driver (ECI116.h) ?
- How do I know which message is concerned, and how do I handle it, knowing the SendMessage method always returns `true`? (Wait for the FIFO to empty, retry message sending..)
