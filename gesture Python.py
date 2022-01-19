import cv2
import numpy as np
import math
cap = cv2.VideoCapture(0)
from os import sys
import paho.mqtt.client as mqttClient
import time



while(1):
        
    try:
        ret, frame = cap.read()
        frame=cv2.flip(frame,1)
        kernel = np.ones((3,3),np.uint8)
        
        roi=frame[100:300, 100:300]        
        cv2.rectangle(frame,(100,100),(300,300),(0,255,0),0)    
        hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)
        
        
         
        lower_skin = np.array([0,20,70], dtype=np.uint8)
        upper_skin = np.array([245,245,245], dtype=np.uint8)
        mask = cv2.inRange(hsv, lower_skin, upper_skin)
        
        mask = cv2.dilate(mask,kernel,iterations = 4)
        mask = cv2.GaussianBlur(mask,(5,5),100)       
        
        contours,hierarchy= cv2.findContours(mask,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
        cnt = max(contours, key = lambda x: cv2.contourArea(x))        
        epsilon = 0.0005*cv2.arcLength(cnt,True)
        approx= cv2.approxPolyDP(cnt,epsilon,True)      
        hull = cv2.convexHull(cnt)
        
        areahull = cv2.contourArea(hull)
        areacnt = cv2.contourArea(cnt)
      
        arearatio=((areahull-areacnt)/areacnt)*100
        hull = cv2.convexHull(approx, returnPoints=False)
        defects = cv2.convexityDefects(approx, hull)
        l=0
        
        for i in range(defects.shape[0]):
            s,e,f,d= defects[i,0]
            start = tuple(approx[s][0])
            end = tuple(approx[e][0])
            far = tuple(approx[f][0])
            a = math.sqrt((end[0] - start[0])**2 + (end[1] - start[1])**2)
            b = math.sqrt((far[0] - start[0])**2 + (far[1] - start[1])**2)
            c = math.sqrt((end[0] - far[0])**2 + (end[1] - far[1])**2)
            
            angle = math.acos((b**2 + c**2 - a**2)/(2*b*c))*57
            
        
            if angle <= 90 and angle>30:
                l += 1
                cv2.circle(roi, far, 3, [255,0,0], -1)
            cv2.line(roi,start, end, [0,255,0], 2)
            
            
        l+=1
        
        font = cv2.FONT_HERSHEY_SIMPLEX
        if l==1:
            if areacnt<2000:
                cv2.putText(frame,'Put hand in the box',(0,50), font, 2, (0,0,255), 3, cv2.LINE_AA)
                
            else:
                if arearatio<12:
                    cv2.putText(frame,'Stop',(0,50), font, 2, (0,0,255), 3, cv2.LINE_AA)
                else:
                    cv2.putText(frame,'Avant',(0,50), font, 2, (0,0,255), 3, cv2.LINE_AA)

        elif l==2:
            cv2.putText(frame,'Arriere',(0,50), font, 2, (0,0,255), 3, cv2.LINE_AA)

        elif l==3:
            cv2.putText(frame,'Gauche',(0,50), font, 2, (0,0,255), 3, cv2.LINE_AA)
                    
        elif l==4:
            cv2.putText(frame,'Droite',(0,50), font, 2, (0,0,255), 3, cv2.LINE_AA)
     
        cv2.imshow('mask',mask)
        cv2.imshow('frame',frame)
    except:
        pass
        
    
    esc = cv2.waitKey(5) & 0xFF
    if esc == 27:
        break
cv2.destroyAllWindows()
cap.release()         

sys.exit(0)
