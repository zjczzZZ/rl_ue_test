
# import socket
# import json
# import sys

# import threading

# action_dict = {"power":0.3,"steer":0.1}

# def dispose_client_tcp(tcp_client_1,tcp_client_address):
#     while True:
#         msg = tcp_client_1.recv(4*1024)
#         if msg:
#             msg = msg.decode('utf-8')
#             msgdata = json.loads(msg)
#             print(msgdata)

#             action_dict['power'] = 1
#             action_str = json.dumps(action_dict)
#             tcp_client_1.send(action_str.encode())
#         else:
#             tcp_client_1.close()
#             break


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


# if __name__ == "__main__":

import env
import torch.optim as optim
import torch.nn as nn
import torch
import gym
import sys
sys.path.append('your gym folder')


class PolicyNetwork(nn.Module):
    def __init__(self, input_dim, output_dim):
        super(PolicyNetwork, self).__init__()
        self.fc1 = nn.Linear(input_dim, 64)
        self.fc2 = nn.Linear(64, output_dim)

    def forward(self, state):
        x = torch.relu(self.fc1(state))
        action_probs = torch.softmax(self.fc2(x), dim=-1)
        return action_probs


env = env.CarSim()
input_dim = 6  # env.observation_space.shape[0]  # 状态空间维度
output_dim = 2  # env.action_space.n  # 动作空间大小

policy_net = PolicyNetwork(input_dim, output_dim)
optimizer = optim.Adam(policy_net.parameters(), lr=1e-2)


def evaluate_policy(policy_net, env, episodes=1000):
    total_rewards = 0
    for i in range(episodes):
        state = env.reset()
        done = False
        episode_reward = 0
        while not done:
            state_tensor = torch.FloatTensor(state).unsqueeze(0)
            with torch.no_grad():
                action_probs = policy_net(state_tensor)
            # action = torch.argmax(action_probs).item()
            # next_state, reward, done, _ = env.step(action)
            next_state, reward, done, _ = env.step(action_probs)
            episode_reward += reward
            state = next_state
        total_rewards += episode_reward

    average_reward = total_rewards / episodes
    return average_reward





# 定义模型
class Model(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.fc_statu = torch.nn.Sequential(
            torch.nn.Linear(6, 128),
            torch.nn.ReLU(),
        )

        self.fc_mu = torch.nn.Sequential(
            torch.nn.Linear(128, 2),
            torch.nn.Tanh(),
        )

        self.fc_std = torch.nn.Sequential(
            torch.nn.Linear(128, 2),
            torch.nn.Softplus(),
        )

    def forward(self, state):
        state = self.fc_statu(state)

        mu = self.fc_mu(state) * 2.0
        std = self.fc_std(state)

        return mu, std


model = Model()

model_td = torch.nn.Sequential(
    torch.nn.Linear(6, 128),
    torch.nn.ReLU(),
    torch.nn.Linear(128, 2),
)

model(torch.randn(1, 6)), model_td(torch.randn(1, 6))


# 使用上文定义的PolicyNetwork和初始化的env
episodes = 1000
average_reward = evaluate_policy(policy_net, env, episodes)
print(f"Average reward over {episodes} episodes: {average_reward}")