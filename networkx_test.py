import networkx as nx
import matplotlib.pyplot as plt
import random
from matplotlib.figure import Figure

g = nx.Graph()

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
