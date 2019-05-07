import networkx as nx
import matplotlib.pyplot as plt
import random
from matplotlib.figure import Figure

g = nx.Graph()
g.add_edge(0, 1, color='red')
g.add_edge(0, 2, color='red')
g.add_edge(0, 4, color='red')
g.add_edge(1, 9, color='red')
g.add_edge(1, 10, color='red')
g.add_edge(1, 26, color='red')
g.add_edge(2, 3, color='red')
g.add_edge(2, 11, color='red')
g.add_edge(3, 6, color='red')
g.add_edge(3, 8, color='red')
g.add_edge(3, 18, color='red')
g.add_edge(4, 5, color='red')
g.add_edge(4, 23, color='red')
g.add_edge(5, 17, color='red')
g.add_edge(6, 7, color='red')
g.add_edge(6, 16, color='red')
g.add_edge(7, 20, color='red')
g.add_edge(8, 12, color='blue')
g.add_edge(8, 15, color='blue')
g.add_edge(8, 22, color='red')
g.add_edge(10, 25, color='red')
g.add_edge(11, 14, color='red')
g.add_edge(12, 13, color='blue')
g.add_edge(13, 21, color='blue')
g.add_edge(14, 27, color='red')
g.add_edge(14, 28, color='red')
g.add_edge(15, 21, color='blue')
g.add_edge(17, 19, color='red')
g.add_edge(19, 24, color='red')
g.add_edge(19, 29, color='red')

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
