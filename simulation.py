import networkx as nx
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Import graph and evolution from .csv files
numCols = 20
df = pd.read_csv("./files/graph.csv", names=list(range(numCols)))
evolution = pd.read_csv("./files/evolution.csv", names=[i for i in range(len(df))]).T

# Create networkx graph
G = nx.Graph()
G.add_nodes_from([i for i in range(len(df))])

for i in range(len(df)):
    for j in range(numCols):
        if(not pd.isna(df[j][i])):
            G.add_edge(i, int(df[j][i]))

# Define and compute color map
def color_map(evolution):
    node_colors = pd.DataFrame(index=range(evolution.shape[0]), columns=evolution.columns)
    for t in range(evolution.shape[1]):
        for i in range(evolution.shape[0]):
            if evolution[t][i] == 0: node_colors[t][i] = "#4169E1"
            elif evolution[t][i] == 1: node_colors[t][i] = "#BF2C35"
            elif evolution[t][i] == 2: node_colors[t][i] = "#F07857"
            elif evolution[t][i] == 3: node_colors[t][i] = "#BE398D"
            elif evolution[t][i] == -1: node_colors[t][i] = "#008000"
            elif evolution[t][i] == -2: node_colors[t][i] = "#006600"
            elif evolution[t][i] == -3: node_colors[t][i] = "#004D00"
            elif evolution[t][i] == -4: node_colors[t][i] = "#003500"
            else: node_colors[t][i] = "#001E00"
    return node_colors
node_colors = color_map(evolution)

# Define and compute animation
def animate_nodes(G, node_colors, pos=None, *args, **kwargs):

    # define graph layout if None given
    if pos is None:
        pos = nx.spring_layout(G)

    # draw graph
    nodes = nx.draw_networkx_nodes(G, pos, node_size = 50, *args, **kwargs)
    edges = nx.draw_networkx_edges(G, pos, *args, **kwargs)
    plt.axis('off')

    def update(t):
        nodes.set_facecolor(node_colors[t])
        return nodes,

    fig = plt.gcf()
    animation = FuncAnimation(fig, update, interval=50, frames=len(node_colors), blit=True)
    return animation
animation = animate_nodes(G, node_colors)
animation.save('./files/animation.gif', writer='Pillow', savefig_kwargs={'facecolor':'white'}, fps=5)
                
# SIR analysis
S = []
I = []
R = []
for t in range(evolution.shape[1]):
    S.append(len(evolution[t][evolution[t] == 0]))
    I.append(len(evolution[t][evolution[t] > 0]))
    R.append(len(evolution[t][evolution[t] < 0]))