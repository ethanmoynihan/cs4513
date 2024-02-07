import socket
import sys

UDP_IP = socket.gethostbyname(socket.gethostname())
#UDP port will be specified in the command line arguements, defualts to 4242
UDP_PORT = 4242
class Glinda():

    def finditem(self, item:dict, key, value):
        #Returns true if the key is in the item with the specified value
        if key in item:
            #If the key is in then return true
            if item[key] == value or "*" == value:
                return True
        return False
        
    def parseItem(self, itemstring):
        #parses item into a dictionary
        retitem = {}
        for item in itemstring.split("&"):
            if item == "*":
                retitem["*"] = ""
                return retitem
            temp = item.split("=")
            if len(temp) == 1:
                return None
            if sys.getsizeof(temp[1]) <=256:
                retitem[temp[0]] = temp[1]
            else:
                return None
        return retitem
    
    def itemReturn(self, item:dict):
        retString = ''
        keys = item.keys()
        for i, key in enumerate(list(keys)):
                retString+= f"{key}={item[key]}"
                if i != len(keys)-1:
                    retString += "&"
        return retString
    
    #Get an item from the glinda server
    def get(self, key, value):
        if key == "*" and len(self.storage) !=0:
            #Find the first value if any exist
            return "ok " + self.itemReturn(self.storage[0])
        for item in self.storage:
            if self.finditem(item, key, value):
                #if the item is found return it as a string
                return "ok " + self.itemReturn(item)
        
        #If there is no matching string
        retString = "ok no attribute string"
        return retString
    
    #Put an item in the glinda server
    def put(self, item:dict, sock):
        #if string is broken or some item mal formed do not place in dict and return err
        retString = "ok "
        for i, itir in enumerate(self.waitinglist):
            if itir["key"] in item:
                if item[itir["key"]] == itir["val"] or itir["val"] == "*":
                    #Found a matching key
                    takeRet = self.itemReturn(item)
                    self.waitinglist.pop(i)
                    sent = sock.sendto(takeRet.encode(), itir['client']) 
                    return retString
            elif itir["key"] == "*":
                #Tried take all but nothing was in the database
                return retString
        #if it is not in the waitinglist then append
        self.storage.append(item)
        return retString
    
    #list items in the glinda server
    def list(self, keys, values):
        retString = 'ok '
        #get all items in the list
        #if any key is * then list all
        for key in keys:
            if key == "*" and len(self.storage) !=0:
                for i, item in enumerate(self.storage):
                    retString += self.itemReturn(item)
                    if i != len(self.storage)-1:
                        retString += ";"
                return retString
        #get items that match the key and value in the list
        itemcount = 0
        for item in self.storage:
            finder = True
            for i, key in enumerate(keys):
                #Check each key to see if it is in the item
                finder = finder & self.finditem(item, key, values[i])
            if finder:
                #if there is more than one item append the ;
                if itemcount == 0:
                    itemcount +=1
                else:
                    retString += ";" 
                #if the item is found return it as a string
                retString += self.itemReturn(item)
               
        return retString

    #Take an item from the glinda server
    def take(self, keys, values, client, sock):
        retString = 'ok '
        itemcount = 0
        indices=[]
        storageCopy = self.storage
        #get all items in the list
        for key in keys:
            if key == "*" and len(self.storage) !=0:
                for i, item in enumerate(storageCopy):
                    #if there is more than one item append the ;
                    if itemcount == 0:
                        itemcount +=1
                    else:
                        retString += ";" 
                    retString += self.itemReturn(item)
                    indices.append(i)
                for item in reversed(indices):
                    self.storage.pop(item)
                sent = sock.sendto(retString.encode(), client) 
                return True
        
        for i, item in enumerate(self.storage):
            finder = True
            for k, key in enumerate(keys):
                #if it exists for all keys with matching values
                finder = finder & self.finditem(item, key, values[k])
            if finder:
                #if there is more than one item append the ;
                if itemcount == 0:
                    itemcount +=1
                else:
                    retString += ";" 
                #if the item is found return it as a string
                retString += self.itemReturn(item)
                indices.append(i)
        
        if itemcount == 0:
            #add item to take list
            self.waitinglist.append({"client":client, "keys":keys, "vals":values})
            return False
        for item in reversed(indices):
            self.storage.pop(item)
        sent = sock.sendto(retString.encode(), client) 
        return True

    def __init__(self):
        #using a list of dictionaries to store the data
        self.storage = []
        #Save return address info and what was requested, step through requests after put assignment
        self.waitinglist = []


def main():
    #sys.argv[] for connection
    #UDP_PORT = sys.argv[1]
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server = (UDP_IP, UDP_PORT)
    sock.bind(server)
    print("server enabled")
    glin = Glinda()
    while True:
        payload, client_address  = sock.recvfrom(4096)
        decodedLoad = payload.decode()
        print(decodedLoad)
        splitstring = []
        splitstring = decodedLoad.split(" ", maxsplit=1)

        parsed = glin.parseItem(splitstring[1])
        returnMessage = ''
        if parsed == None:
            returnMessage = 'err'
            sent = sock.sendto(returnMessage.encode(), client_address) 
        else:
            match splitstring[0].lower():
                case "put":
                    if "*" in parsed:
                        returnMessage = "err * is not valid identifier"
                        sent = sock.sendto(returnMessage.encode(), client_address) 
                        continue
                    #Check take list
                    returnMessage = glin.put(parsed, sock)
                    sent = sock.sendto(returnMessage.encode(), client_address)   

                case "get":
                    keys = parsed.keys()
                    keys = list(keys)
                    returnMessage = glin.get(keys[0], parsed[keys[0]])
                    sent = sock.sendto(returnMessage.encode(), client_address)    
                case "list":
                    keys = list(parsed.keys())
                    vals = list(parsed.values())
                    returnMessage = glin.list(keys, vals)
                    sent = sock.sendto(returnMessage.encode(), client_address)    
                case "take":
                    #Pass the client_address in for case where take opperation fails and move on
                    keys = list(parsed.keys())
                    vals = list(parsed.values())
                    returnMessage = glin.take(keys, vals, client_address, sock)
                case __:
                    returnMessage = "err unknown command"
                    sent = sock.sendto(returnMessage.encode(), client_address) 
        
        

if __name__=="__main__":
    main()

