import gym
from gym import spaces

import socket
import json
import sys
import threading
import time


action_dict = {"power": 0.3, "steer": 0.1}


def dispose_client_tcp(tcp_client_1, tcp_client_address):
    while True:
        msg = tcp_client_1.recv(4*1024)
        if msg:
            msg = msg.decode('utf-8')
            msgdata = json.loads(msg)
            # print(msgdata)

            action_dict['power'] = 1
            action_str = json.dumps(action_dict)
            tcp_client_1.send(action_str.encode())
        else:
            tcp_client_1.close()
            break


# if __name__ == '__main__':
#     host  = '127.0.0.1'
#     port = 9900
#     server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
#     server.bind((host,port))
#     server.listen(10)

#     while True:
#         tcp_client_1,tcp_client_address = server.accept()
#         thread = threading.Thread(target=dispose_client_tcp,args=(tcp_client_1,tcp_client_address))
#         thread.setDaemon(True)
#         thread.start()

#     server.close()


class UdpServer:
    def __init__(self):
        self.host = '127.0.0.1'
        self.port = 10000
        self.server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.server.bind((self.host, self.port))

    def recv(self):
        try:
            msg, self.client_addr = self.server.recvfrom(4*1024)
            # print(msg,self.client_addr)
        except:
            pass
        if msg:
            msg = msg.decode('utf-8')
            msgdata = json.loads(msg)
            return msgdata
        else:
            return ""

    def send(self, str):
        # print(str)
        try:
            self.server.sendto(str.encode(), self.client_addr)
        except:
            pass


class CarSim(gym.Env):
    def __init__(self):
        super().__init__()
        self.reword = 0
        self.action_space = spaces.Box(low=-1.0, high=1.0, shape=(1, 1))
        self.observation_space = spaces.Box(low=-1.0, high=1.0, shape=(3, 1))
        self.state = None
        self.tcpServer = UdpServer()
        self.score = 0
        self.done = False
        self.step_n = 0

    # def step(self,action):
    #     msg = self.tcpServer.recv()
    #     #print(msg)
    #     #self.tcpServer.send('{"power":1,"steer":0.1}')
    #     if msg:
    #         tmpdata = msg['linetrace']
    #         self.done = msg['done']
    #         if not self.done:
    #             self.observation_space = tmpdata
    #             thisscore = 0
    #             for x in tmpdata:
    #                 thisscore+=x
    #                 if x<0.06:
    #                     self.reword -= 1111111
    #             if thisscore<self.score:
    #                 self.reword -=1
    #             else:
    #                 self.reword = thisscore
    #                 self.reword +=1
    #             self.reword+=0.1
    #     if not self.done:
    #         action_dict['power'] = action[0][0].tolist()
    #         action_dict['steer'] = action[0][1].tolist()
    #         action_str = json.dumps(action_dict)
    #         self.tcpServer.send(action_str)
    #         print(action_str)
    #         return self.observation_space,self.reword,False,None
    #     else:
    #         #self.tcpServer.closeClient()
    #         #t = threading.Timer(2,self.tcpServer.accept)
    #         self.done = False
    #         return self.observation_space,self.reword,True,None

    def car_in_road_reward(self, state):
        #print(state)
        step_reword = 1
        for i in range(0, 5):
            if state[i] > 0.4:
                step_reword += state[i]-0.4
            else:
                step_reword -= (0.4 - state[i])*10

        # if state[5] > 0:
        #     step_reword += state[5]*0.01
        # else:
        #     step_reword += state[5]*0.05
        return step_reword * 0.01

    def step(self, action):

        self.step_n+=1
        self.reword = 0
        action_dict['power'] = 0.35
        action_dict['steer'] = action[0]/2
        action_str = json.dumps(action_dict)
        print(action_str)
        self.tcpServer.send(action_str)

        msg = self.tcpServer.recv()
            
        # print(msg)
        # self.tcpServer.send('{"power":1,"steer":0.1}')
        if msg:
            v = msg['speed']
            tmpdata = msg['linetrace']
            self.done = msg['done']
            if self.step_n>9999999:
                self.done = True
                self.step_n = 0
            if not self.done:
                #offsetaay = tmpdata.append(v)
                offsetaay = tmpdata[::2]
                self.observation_space = offsetaay
                self.reword = self.car_in_road_reward(tmpdata)
                #print(self.reword,action[0]/2)
        if not self.done:

            return self.observation_space, self.reword, False, None
        else:
            # self.tcpServer.closeClient()
            # t = threading.Timer(2,self.tcpServer.accept)
            self.done = False
            pauseflag = False 
            while not pauseflag:
                msg = self.tcpServer.recv()
                if msg:
                    pauseflag = msg['start']
                    
            return self.observation_space, self.reword, True, None

    def reset(self):
        return [1.0, 1.0, 1.0]

    def close(self):
        return super().close()
