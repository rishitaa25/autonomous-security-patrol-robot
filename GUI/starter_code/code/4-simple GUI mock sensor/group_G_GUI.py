# Description: Display using a polar plot the distance measurements collected by a CyBot 180 degree 
# sensor scan at 4 degree increments from 0 to 180 degrees. Sensor data read from a text file.

# Original Code example modified by: Phillip Jones (10/02/2021), (05/15/2023)
# Original polar plot code example from matplot: https://matplotlib.org/stable/gallery/pie_and_polar_charts/polar_demo.html

# Useful matplotlib tutorial: https://matplotlib.org/stable/tutorials/introductory/pyplot.html
# Useful best practices Quick Start: https://matplotlib.org/stable/tutorials/introductory/quick_start.html
# General Python Reference/Tutorials: https://www.w3schools.com/python/ 

# Quick YouTube Overviews (See above links as primary resources for additional details): 
# - Quick Polar Plot (subplot) Overview: https://www.youtube.com/watch?v=pb-pZtvkGPM
# - Quick subplots Overview : https://www.youtube.com/watch?v=Tqph7_qMujk

#Import/Include useful math and plotting functions
import numpy as np
import matplotlib.pyplot as plt
import os  # import function for finding absolute path to this python script
import serial
import time # Time library   
# Socket library:  https://realpython.com/python-sockets/  
# See: Background, Socket API Overview, and TCP Sockets  
import socket
import tkinter as tk # Tkinter GUI library
# Thread library: https://www.geeksforgeeks.org/how-to-use-thread-in-tkinter-python/
import threading

# A little python magic to make it more convient for you to ajust where you want the data file to live
# Link for more info: https://towardsthecloud.com/get-relative-path-python 
absolute_path = os.path.dirname(__file__) # Absoult path to this python script
relative_path = "./"   # Path to sensor data file relative to this python script (./ means data file is in the same directory as this python script
full_path = os.path.join(absolute_path, relative_path) # Full path to sensor data file
filename = 'sensor-scan.txt' # Name of sensor data file

##### START Define Functions  #########

# Main: Mostly used for setting up, and starting the GUI
def main():

        global window  # Made global so quit function (send_quit) can access
        window = tk.Tk() # Create a Tk GUI Window

        # Last command label  
        global Last_command_Label  # Made global so that Client function (socket_thread) can modify
        Last_command_Label = tk.Label(text="Last Command Sent: ") # Creat a Label
        Last_command_Label.pack() # Pack the label into the window for display

        # Quit command Button
        quit_command_Button = tk.Button(text ="Press to Quit", command = send_quit)
        quit_command_Button.pack()  # Pack the button into the window for display

        # Cybot Scan command Button
        scan_command_Button = tk.Button(text ="Press to Scan", command = send_scan)
        scan_command_Button.pack() # Pack the button into the window for display


        # Create a Thread that will run a fuction assocated with a user defined "target" function.
        # In this case, the target function is the Client socket code
        my_thread = threading.Thread(target=socket_thread) # Creat the thread
        my_thread.start() # Start the thread

        # Start event loop so the GUI can detect events such as button clicks, key presses, etc.
        window.mainloop()


# Quit Button action.  Tells the client to send a Quit request to the Cybot, and exit the GUI
def send_quit():
        global gui_send_message # Command that the GUI has requested be sent to the Cybot
        global window  # Main GUI window
        
        gui_send_message = "quit\n"   # Update the message for the Client to send
        time.sleep(1)
        window.destroy() # Exit GUI


# Scan Button action.  Tells the client to send a scan request to the Cybot
def send_scan():
        global gui_send_message # Command that the GUI has requested sent to the Cybot
        
        gui_send_message = "M\n"   # Update the message for the Client to send


# Client socket code (Run by a thread created in main)
def socket_thread():
        # Define Globals
        global Last_command_Label # GUI label for displaying the last command sent to the Cybot
        global gui_send_message   # Command that the GUI has requested be sent to the Cybot

        # A little python magic to make it more convient for you to adjust where you want the data file to live
        # Link for more info: https://towardsthecloud.com/get-relative-path-python 
        absolute_path = os.path.dirname(__file__) # Absoult path to this python script
        relative_path = "./"   # Path to sensor data file relative to this python script (./ means data file is in the same directory as this python script)
        full_path = os.path.join(absolute_path, relative_path) # Full path to sensor data file
        filename = 'sensor-scan.txt' # Name of file you want to store sensor data from your sensor scan command

        # Choose to create either a UART or TCP port socket to communicate with Cybot (Not both!!)
        # UART BEGIN
        #cybot = serial.Serial('COM100', 115200)  # UART (Make sure you are using the correct COM port and Baud rate!!)
        # UART END

        # TCP Socket BEGIN (See Echo Client example): https://realpython.com/python-sockets/#echo-client-and-server
        HOST = "192.168.1.1"  # The server's hostname or IP address
        PORT = 288        # The port used by the server
        cybot_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Create a socket object
        cybot_socket.connect((HOST, PORT))   # Connect to the socket  (Note: Server must first be running)

        cybot = cybot_socket.makefile("rbw", buffering=0)  # makefile creates a file object out of a socket:  https://pythontic.com/modules/socket/makefile
        # TCP Socket END

        # Send some text: Either 1) Choose "Hello" or 2) have the user enter text to send
        send_message = "Hello\n"                            # 1) Hard code message to "Hello", or
        # send_message = input("Enter a message:") + '\n'   # 2) Have user enter text
        gui_send_message = "wait\n"  # Initialize GUI command message to wait

        cybot.write(send_message.encode()) # Convert String to bytes (i.e., encode), and send data to the server

        print("Sent to server: " + send_message)

        # Send messages to server until user sends "quit"
        while send_message != 'quit\n':

                # Update the GUI to display command being sent to the CyBot
                command_display = "Last Command Sent:\t" + send_message
                Last_command_Label.config(text=command_display)

                # Check if a sensor scan command has been sent
                if (send_message == "M\n") or (send_message == "m\n"):

                        print("Requested Sensor scan from Cybot:\n")
                        rx_message = bytearray(1) # Initialize a byte array

                        # Create or overwrite existing sensor scan data file
                        file_object = open(full_path + filename, 'w') # Open the file for writing to capture live CyBot scan data

                        # Collect sensor data lines from CyBot until "END" is received
                        while (rx_message.decode() != "END\n"):
                                rx_message = cybot.readline()          # Wait for sensor response, readline expects message to end with "\n"
                                file_object.write(rx_message.decode()) # Write a line of sensor data to the file
                                print(rx_message.decode())             # Convert message from bytes to String (i.e., decode), then print

                        file_object.close() # Important to close file once you are done with it!!

                        # Re-open the file we just wrote to read and plot the data
                        file_object = open(full_path + filename, 'r') # Open the file: file_object is just a variable for the file "handler" returned by open()
                        file_header = file_object.readline() # Read and store the header row (i.e., 1st row) of the file into file_header
                        file_data = file_object.readlines()  # Read the rest of the lines of the file into file_data
                        file_object.close() # Important to close file once you are done with it!!

                        # Initialize lists before appending
                        angle_degrees = []
                        distance = []

                        # For each line of the file split into columns, and assign each column to a variable
                        for line in file_data:
                                data = line.split()    # Split line into columns (by default delineates columns by whitespace)
                                if len(data) >= 2:     # Make sure there are at least 2 columns - claude suggestion
                                        try:
                                                angle_degrees.append(float(data[0]))  # Column 0 holds the angle at which distance was measured
                                                distance.append(float(data[1]))       # Column 1 holds the distance that was measured at a given angle
                                        except ValueError:
                                                print(f"Skipping non-numeric line: {line.strip()}")  # Skips "END" or any bad line - claude suggestion

                        # Convert python sequence (list of strings) into a numpy array
                        angle_degrees = np.array(angle_degrees) # Avoid "TypeError: can't multiply sequence by non-int of type float"
                                                                 # Link for more info: https://www.freecodecamp.org/news/typeerror-cant-multiply-sequence-by-non-int-of-type-float-solved-python-error/

                        angle_radians = (np.pi / 180) * angle_degrees # Convert degrees into radians

                        # Create a polar plot
                        fig, ax = plt.subplots(subplot_kw={'projection': 'polar'}) # One subplot of type polar
                        ax.plot(angle_radians, distance, color='r', linewidth=4.0)  # Plot distance verse angle (in radians), using red, line width 4.0
                        ax.set_xlabel('Distance (m)', fontsize=14.0)  # Label x axis
                        ax.set_ylabel('Angle (degrees)', fontsize=14.0) # Label y axis
                        ax.xaxis.set_label_coords(0.5, 0.15) # Modify location of x axis label (Typically do not need or want this)
                        ax.tick_params(axis='both', which='major', labelsize=14) # set font size of tick labels
                        ax.set_rmax(2.5)                      # Saturate distance at 2.5 meters
                        ax.set_rticks([0.5, 1, 1.5, 2, 2.5]) # Set plot "distance" tick marks at .5, 1, 1.5, 2, and 2.5 meters
                        ax.set_rlabel_position(-22.5)         # Adjust location of the radial labels
                        ax.set_thetamax(180)                  # Saturate angle to 180 degrees
                        ax.set_xticks(np.arange(0, np.pi + .1, np.pi / 4)) # Set plot "angle" tick marks to pi/4 radians (i.e., displayed at 45 degree) increments
                                                                             # Note: added .1 to pi to go just beyond pi (i.e., 180 degrees) so that 180 degrees is displayed
                        ax.grid(True)                         # Show grid lines

                        # Create title for plot (font size = 14pt, y & pad controls title vertical location)
                        ax.set_title("Polar Plot of CyBot Sensor Scan from 0 to 180 Degrees", size=14, y=1.0, pad=-24)
                        plt.show(block=False)  # block=False prevents freezing the GUI - claude suggestion, good for keeping the code going after plotting
                        plt.pause(0.1)         # Small pause so the plot renders properly 

                else:
                        print("Waiting for server reply\n")
                        rx_message = cybot.readline()      # Wait for a message, readline expects message to end with "\n"
                        print("Got a message from server: " + rx_message.decode() + "\n") # Convert message from bytes to String (i.e., decode)


                # Choose either: 1) Idle wait, or 2) Request a periodic status update from the Cybot
                # 1) Idle wait: for gui_send_message to be updated by the GUI
                while gui_send_message == "wait\n":
                        time.sleep(.1)  # Sleep for .1 seconds
                send_message = gui_send_message

                # 2) Request a periodic Status update from the Cybot:
                # every .1 seconds if GUI has not requested to send a new command
                #time.sleep(.1)
                #if(gui_send_message == "wait\n"):   # GUI has not requested a new command
                #        send_message = "status\n"   # Request a status update from the Cybot
                #else:
                #        send_message = gui_send_message  # GUI has requested a new command

                gui_send_message = "wait\n"  # Reset gui command message request to wait

                cybot.write(send_message.encode()) # Convert String to bytes (i.e., encode), and send data to the server

        print("Client exiting, and closing file descriptor, and/or network socket\n")
        time.sleep(2) # Sleep for 2 seconds
        cybot.close() # Close file object associated with the socket or UART
        cybot_socket.close()  # Close the socket (NOTE: comment out if using UART interface, only use for network socket option)

##### END Define Functions  #########


### Run main ###
main()# # Description: Display using a polar plot the distance measurements collected by a CyBot 180 degree 
