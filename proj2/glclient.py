#!./glclient.py

import socket
import sys


#UDP port will be specified in the command line arguements, defualts to 4242
UDP_PORT = 4242


def main():
    #sys.argv[] for connection

    #UDP_PORT = sys.argv[1:] (TAKES TWO COMMAND ARGS)
    if(sys.argv[1].lower() != "localhost"):
        UDP_IP = sys.argv[1]
    else:
        UDP_IP = socket.gethostbyname(socket.gethostname())
    UDP_PORT = int(sys.argv[2])
    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server = (UDP_IP, UDP_PORT)
    client.connect(server)
    
    #Currently send and recieve
    #For put expect "ok" with nothing else
    while True:
        message = input(">")

        messageType = message.split(" ")

        if messageType[0].lower() == "exit":
            break

        client.send(message.encode())
        payload, client_address  = client.recvfrom(4096)
        returnmessage = payload.decode()
        splitReturn = returnmessage.split(" ", maxsplit=1)

        if splitReturn[0].lower() == "ok":
            if messageType[0] == "list" or messageType[0] == "take":
                steps = splitReturn[1].split(";")
                for item in steps:
                    print(item)
            elif messageType[0] == "get":
                print(splitReturn[1])
        else:
            print(returnmessage)
        
if __name__=="__main__":
    main()

