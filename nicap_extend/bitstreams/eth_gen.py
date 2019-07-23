#Python send ethernet file

from scapy.all import *

import numpy as np

xbin = np.fromfile('mode1.bin',dtype='uint8')
bitlen = xbin.shape[0]
bitdatas=[]
for i in range (bitlen):
    bitdatas.append(xbin[i])
full_frames = bitlen//1024
if (full_frames*1024 == bitlen):
    last_frame = 0
else: 
    last_frame = bitlen-full_frames*1024
bitdata = []
for i in range(bitlen//4):
    for j in range (4):
        bitdata.append(bitdatas[i*4+3-j])
        
pktdata = []
for i in range (full_frames):
    if (i == 0):
        pktdata.append([0,17,170,187,204,221,238,255,0,0,0,0,2,79,192,2,0,0])
    elif ((last_frame == 0) and (i == full_frames-1)):
        pktdata.append([0,17,170,187,204,221,238,255,0,0,0,0,2,79,192,1,0,0])
    else:    
        pktdata.append([0,17,170,187,204,221,238,255,0,0,0,0,2,79,192,0,0,0])

if (last_frame):
    pktdata.append([0,17,170,187,204,221,238,255,0,0,0,0,2,79,192,1,0,0])
''' last 2 values in the above packet header 0's are padding for layer-2 format'''
    
for i in range (full_frames):
    for j in range (1024):
        pktdata[i].append(bitdata[i*1024+j])
        #pktdata[i].append(xbin[1024*i+j])
for i in range (last_frame):    
    pktdata[bitlen//1024].append(bitdata[full_frames*1024+i])


if (last_frame >0):
    num_frames = full_frames + 1
else:
    num_frames = full_frames
    


for i in range (num_frames):
    length = 1024 + 18
    if (i == full_frames):
        length = last_frame + 18

    ph = Ether(dst="45:0a:35:01:02:03",src="40:B0:34:13:38:A5",type=length)
    ph1 = ph/bytes(pktdata[i])
    sendp(ph1,iface="enx00e1100017c5")
    
#print(xbin[:1024])    
#print(lines)
#lines = struct.unpack('c',fc)

#print(lines)
#print(len(lines))

    #if (i%1024 == 0):
    #    index += 1
        
        
#print(bitframe)