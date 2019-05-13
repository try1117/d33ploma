import networkx as nx
import matplotlib.pyplot as plt
import random
from matplotlib.figure import Figure

g = nx.Graph()

g.add_edge(0, 1, color='blue')
g.add_edge(0, 6, color='blue')
g.add_edge(0, 11, color='blue')
g.add_edge(1, 2, color='blue')
g.add_edge(1, 1, color='blue')
g.add_edge(1, 1, color='blue')
g.add_edge(2, 3, color='blue')
g.add_edge(2, 16, color='blue')
g.add_edge(3, 4, color='blue')
g.add_edge(3, 5, color='blue')
g.add_edge(3, 7, color='red')
g.add_edge(3, 12, color='blue')
g.add_edge(3, 13, color='blue')
g.add_edge(3, 15, color='blue')
g.add_edge(3, 16, color='blue')
g.add_edge(3, 8, color='blue')
g.add_edge(4, 8, color='blue')
g.add_edge(5, 11, color='blue')
g.add_edge(5, 12, color='blue')
g.add_edge(6, 9, color='blue')
g.add_edge(6, 18, color='blue')
g.add_edge(8, 14, color='red')
g.add_edge(8, 15, color='blue')
g.add_edge(9, 10, color='blue')
g.add_edge(10, 16, color='blue')
g.add_edge(11, 12, color='blue')
g.add_edge(13, 19, color='blue')
g.add_edge(14, 17, color='red')
g.add_edge(16, 19, color='blue')
g.add_edge(18, 19, color='blue')

nonbr = [(u, v) for (u, v, d) in g.edges(data=True) if d['color'] == 'blue']
bridges = [(u, v) for (u, v, d) in g.edges(data=True) if d['color'] == 'red']

pos = nx.spring_layout(g) 

nx.draw_networkx_nodes(g, pos, node_size=10)
nx.draw_networkx_edges(g, pos, edgelist=nonbr, edge_color='blue')
nx.draw_networkx_edges(g, pos, edgelist=bridges, edge_color='red')


# fig = plt.figure()
# nx.draw_networkx_edges(g, ax=fig.add_subplot(111))#, node_size=10)#, with_labels=True)#, font_weight='bold')

plt.show()

quit(0)

g = nx.Graph()
pet = nx.petersen_graph()
# plt.subplot()
# lollipop = nx.lollipop_graph(10, 20)
# er = nx.erdos_renyi_graph(100, 0.15)
# ws = nx.watts_strogatz_graph(30, 3, 0.1)

n = 7
g_list = list(nx.nonisomorphic_trees(n))
cnt = nx.number_of_nonisomorphic_trees(n)
print(cnt)
sz = 1
while sz ** 2 < cnt:
    sz += 1

# fig = self.figures['distribution']
fig = plt.figure()
fig.add_subplot(111)
# canvas = FigureCanvas(figure)

for idx, g in enumerate(g_list):
    # ax =
    # ax.draw(g)
    nx.draw(g, ax=fig.add_subplot(sz, sz, idx + 1), node_size=10)#, with_labels=True)#, font_weight='bold')
# fig.
plt.show()

# print(len(g_list))

# seq = [random.randint(1, n // 10) for i in range(n - 1)]
# if sum(seq) % 2:
#     seq.append(1)
# cmodel = nx.generators.degree_seq.configuration_model
# my_graph = cmodel(seq)

# print(seq)

# nx.draw(G, with_labels=True)#, font_weight='bold')
# plt.show()

# print(lollipop.edges())
