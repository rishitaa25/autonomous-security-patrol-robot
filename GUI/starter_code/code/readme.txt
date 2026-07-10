Graphical User Interface (GUI) Display of Object and/or Test Field Information

In Lab 3, PuTTY is used to communicate between the user on the PC and the CyBot robot. 

PuTTY is limited to text. However, being able to graphically display object information in a meaningful way will be very helpful for future labs and the project.

A graphical display or graphical user interface (GUI) may assist the user in interpreting object information and navigating in the test field. Like the PuTTY app, a GUI app runs on the PC. A simple option is for the GUI app to read data from a file that is created/written by PuTTY. This would let the user visualize the data. A more interactive option is to have the GUI app replace the use of PuTTY and make a connection with the CyBot to interact directly with the robot.

A GUI may be created using any language (framework) of your choice. We suggest trying MATLAB and/or Python. 

When using the wi-fi connection between the CyBot and PC, TCP network sockets are used for communication.

MATLAB

For those interested in trying MATLAB, you will find information at the MathWorks website about TCP/IP network socket communication. For example, MATLAB uses sockets to get data from instrumentation to analyze and plot the data. Essentially, the CyBot is like a piece of measurement equipment. Note: You can also use MATLAB without sockets to plot data from a file. Use PuTTY to create a file of scan data sent from the CyBot. Then use MATLAB. This is not automated but potentially helpful as you explore other approaches.

Python

Students have also used Python to create GUIs. Dr. Jones, who teaches CPRE 288 in other semesters, has been developing some "starter code" modules to introduce students to socket and GUI programming in Python. He has developed four sets of code, as shown in the "Python starter code" folder. 

Use as much or as little of the starter code as you want. You do not need to use the starter code. Whether or not you use the code, it may still be helpful to browse through the comments in the code files that refer to supporting information.

Here is additional background to get started:

Start with folder #1 to create a polar plot from a file of sensor scan data. You can use PuTTY to create your own file of scan data sent from the CyBot. 

In folders 2-4, a Python socket program is used to send and receive messages and data between the CyBot and PC.

IMPORTANT NOTE: The role of the server in the starter code is to act as a pseudo CyBot to let you develop features for your client Python code and perform initial testing of your code without a physical CyBot.  After you are satisfied with the correctness and functionality of the features in your client, you will then directly connect the client to the CyBot (i.e., the CyBot then acts as the server).  The server and the CyBot are never used at the same time. 

Folders 2 and 3 provide Python code with client and server sockets. Folder 4 adds GUI code. 

Start with folder 2 and do the following:
a) Read through the code for the client and server. Think about and summarize what the code is doing.
b) As needed, read more about network sockets, e.g., https://realpython.com/python-sockets/.
c) Run the client and server and send messages between them.
d) Same as c), but instead of connecting the client with the server, remove the server, and connect the client to the actual CyBot. The Simple-CyBot-echo.c code is starter code for the CyBot (build and run it on the CyBot) that should work out of the box for communicating with the client code.

In folder 3, the server, acting as a pseudo CyBot, gets mock sensor data from a file. In folder 4, the client includes GUI code.

The code modules are organized so that you can develop and test your Python-based GUI without being connected to the CyBot in the lab. Then when in the lab, use a version of the code that connects to the CyBot.

The "Python CyBot sketch" is a high level illustration of the interactions between the various elements in the starter code folders.

There are many ways to analyze and display object information in a GUI. The sensor data polar or radial plots as shown in lab documents are one way. A graphical representation of the test field would be another. Use your imagination.