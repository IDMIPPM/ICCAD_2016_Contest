__author__ = 'Roman Solovyev [IPPM]'

import os
import re
import subprocess
import shutil
import time
import copy
import sys
import networkx as nx
from cada033 import *

def test_1():
    G = nx.DiGraph()
    G.add_edge(1, 2)
    print(G.nodes())
    print(G.edges())
    exit()


def print_graph_structure(G):
    print(G.nodes())
    print(G.edges())


# Читаем файл схемы и формируем граф
def read_graph_from_isc_file(circ_file):
    G = nx.DiGraph()
    with open(circ_file) as f:
        for line in f:
            line = line.strip()
            if line == '':
                continue
            if line[0] == '#':
                continue
            if line[:6] == "INPUT(":
                m1 = re.match(r"INPUT\((\d+)\)", line)
                if not m1:
                    print('Unexpected line format (1): {}'.format(line))
                    exit()
                node = int(m1.group(1))
                G.add_node(node)
            elif line[:7] == "OUTPUT(":
                m1 = re.match(r"OUTPUT\((\d+)\)", line)
                if not m1:
                    print('Unexpected line format (2): {}'.format(line))
                    exit()
                node = int(m1.group(1))
                G.add_node(node)
            else:
                # 3227 = DFF(0, 461, 922, 4149)
                m1 = re.match(r"(\d+)\s*=\s*(DFF)\((.*),\s*(\d+)\)", line)
                m2 = re.match(r"(\d+)\s*=\s*(BUFF|NOT)\((\d+)\)", line)
                m3 = re.match(r"(\d+)\s*=\s*(AND|NAND|OR|NOR|XOR|NXOR)\((\d+),\s*(\d+)\)", line)

                if m1:
                    o_node = int(m1.group(1))
                    i_node = int(m1.group(4))
                    type = m1.group(2)
                    G.add_node(o_node)
                    G.add_node(i_node)
                    G.add_edge(i_node, o_node, {'type': type})
                elif m2:
                    o_node = int(m2.group(1))
                    i_node = int(m2.group(3))
                    type = m2.group(2)
                    G.add_node(o_node)
                    G.add_node(i_node)
                    G.add_edge(i_node, o_node, {'type': type})
                elif m3:
                    o_node = int(m3.group(1))
                    i_node1 = int(m3.group(3))
                    i_node2 = int(m3.group(4))
                    type = m3.group(2)
                    G.add_node(o_node)
                    G.add_node(i_node1)
                    G.add_node(i_node2)
                    G.add_edge(i_node1, o_node, {'type': type})
                    G.add_edge(i_node2, o_node, {'type': type})
                else:
                    print('Unexpected line format (N): {}'.format(line))
                    exit()

    f.close()
    return G


# Читаем файлик с найденными парами и добавляем ребра в граф
def get_groups_graph(ipairs):
    G = nx.Graph()
    with open(ipairs) as f:
        for line in f:
            line = line.strip()
            arr = line.split(" ")
            node1 = int(arr[0])
            node2 = int(arr[1])
            G.add_edge(node1, node2)
    return G


def show_graph(G):
    import pygraphviz
    import matplotlib.pyplot as plt
    from networkx.drawing.nx_agraph import graphviz_layout

    os.environ["PATH"] += ";C:\\Program Files (x86)\\Graphviz2.38\\bin"
    '''
    dot − filter for drawing directed graphs
    neato − filter for drawing undirected graphs
    twopi − filter for radial layouts of graphs
    circo − filter for circular layout of graphs
    fdp − filter for drawing undirected graphs
    sfdp − filter for drawing large undirected graphs
    patchwork − filter for squarified tree maps
    osage − filter for array-based layouts
    neato, dot, twopi, circo, fdp, nop, wc, acyclic, gvpr, gvcolor, ccomps, sccmap, tred, sfdp
    '''
    pos = graphviz_layout(G, prog='neato', args="")
    plt.figure(figsize=(8, 8))
    nx.draw(G, pos, node_size=10, alpha=0.5, node_color="red", with_labels=True)
    plt.axis('equal')
    # plt.savefig('circular_tree.png')
    plt.show()


def experiment_1(design, faults, ipairs):
    G = read_graph_from_isc_file(design)
    # show_graph(G)
    print_graph_structure(G)
    print(nx.shortest_path(G, 1388, 2407))
    fault_list = read_fault_list(faults)
    res = dict()
    for fl in fault_list:
        res[int(fl[0])] = (int(fl[1]), fl[2])
    Groups = get_groups_graph(ipairs)

    # Ищем максимальный изолированный подграф
    max_sub_graph = None
    max_nodes = 0
    for g in nx.connected_component_subgraphs(Groups):
        len1 = len(g.nodes())
        if len1 > max_nodes: # and len1 != 168 and len1 != 30:
            max_sub_graph = g
            max_nodes = len1

    # Ищем среднее растояние между фолтами в максимальной подгруппе
    path_exists = 0
    total = 0
    avg_distance_in_max_group = 0
    nodes_list = max_sub_graph.nodes()
    for i in range(len(nodes_list)):
        for j in range(i+1, len(nodes_list)):
            n1 = nodes_list[i]
            n2 = nodes_list[j]
            node1 = res[n1][0]
            node2 = res[n2][0]
            print(n1, n2, node1, node2)
            if nx.has_path(G, node1, node2):
                sh_path = nx.shortest_path_length(G, node1, node2)
            elif nx.has_path(G, node2, node1):
                sh_path = nx.shortest_path_length(G, node2, node1)
            else:
                # print('No path between {} and {}'.format(node1, node2))
                sh_path = -1
            if sh_path != -1:
                avg_distance_in_max_group += sh_path
                path_exists += 1

            print('Min path between {} and {}: {}'.format(node1, node2, sh_path))
            total += 1

    print('Max group size: {}'.format(len(nodes_list)))
    print('Path exists {} from {}'.format(path_exists, total))
    print('Average path length if exists: {}'.format(avg_distance_in_max_group/path_exists))


if __name__ == '__main__':
    print('Graph librrary: NetworkX ver {}'.format(nx.__version__))
    design = "Case_03/design_03.isc"
    faults = "Case_03/fault_description.txt"
    ipairs = 'identical_fault_pairs.txt'
    experiment_1(design, faults, ipairs)

