__author__ = 'Roman Solovyev [IPPM]'

import random
from collections import defaultdict
from itertools import product, chain, islice, repeat
import copy

class scheme_alt(object):
    def __init__(self):
        self.__inputs__ = []
        self.__outputs__ = []
        self.__elements__ = {}
        self.processed = {}
        self.dependencies = defaultdict(set)
        self.sorted_labels = ()

    def __str__(self):
        return '\n'.join(
            ['inputs', str(self.__inputs__), 'outputs', str(self.__outputs__), 'elements', str(self.__elements__)])

    def inputs(self):
        return len(self.__inputs__)

    def outputs(self):
        return len(self.__outputs__)

    def elements(self):
        return len(self.__elements__)

    def input_labels(self):
        return list(self.__inputs__)

    def output_labels(self):
        return list(self.__outputs__)

    def element_labels(self):
        return list(self.__elements__.keys())

    def all_labels(self):
        return self.input_labels() + self.element_labels()

    def rename_labels(self, rename_dict):
        def rename(label):
            return rename_dict[label] if label in rename_dict else label

        self.__inputs__ = list(map(rename, self.__inputs__))
        self.__outputs__ = list(map(rename, self.__outputs__))

        renamed_elements = dict()
        for element_label in self.__elements__:
            new_label = rename(element_label)
            renamed_elements[new_label] = self.__elements__[element_label]
        self.__elements__ = renamed_elements

        for element_label in self.__elements__:
            operation, operands = self.__elements__[element_label]
            self.__elements__[element_label] = (operation, list(map(rename, operands)))

    def display_truth_table(self):
        for vector in product((0,1), repeat = self.inputs()):
            print(vector, '|', self.process(vector))

    def label_levels(self):
        return {label: self.level(label) for label in chain(self.__inputs__, self.__elements__.keys())}

    def get_sorted_labels(self):
        if not self.sorted_labels:
            levels = self.label_levels()
            self.sorted_labels = sorted(self.element_labels(), key=lambda t: levels[t])
            return self.sorted_labels
        else:
            return self.sorted_labels

    def level(self, label):
        if label in self.__inputs__:
            return 0
        else:
            operation, operands = self.__elements__[label]
            if operation in ('VCC', 'GND'):
                return 0
            else:
                return max(map(self.level, operands)) + 1


def generate_random_ckt(num_inputs, num_outputs, max_width, circ_type='comb'):
    """
    :param num_inputs: Required number of inputs for ckt.
    :param num_outputs: Required number of outputs for ckt.
    :param max_width: maximum width of ckt.
    :return: generated ckt
    """

    overall_index = 0

    if num_inputs < 3:
        print('Too small number of inputs')

    if num_outputs < 3:
        print('Too small number of outputs')

    height = num_outputs
    width = max_width
    if width < 2:
        width = 2

    numInLevel = dict()
    for i in range(width-1):
        numInLevel[i] = random.randint(round(height/2), height)
    numInLevel[width-1] = num_outputs

    current_nodes = []
    ckt = scheme_alt()
    for i in range(num_inputs):
        ckt.__inputs__.append(str(overall_index))
        current_nodes.append(str(overall_index))
        overall_index += 1

    elements = ["BUFF", "NOT", "AND", "OR", "NAND", "NOR", "XOR", "NXOR"]
    if circ_type == 'seq':
        elements.append("DFF")
    for i in range(width):
        h = numInLevel[i]
        level_nodes = []
        for j in range(h):
            o_node_name = str(overall_index)
            overall_index += 1
            type = random.choice(elements)
            if type == 'NOT' or type == 'BUFF':
                node = random.choice(current_nodes)
                ckt.__elements__[o_node_name] = (type, [node])
            else:
                node1 = random.choice(current_nodes)
                tmpnodes = current_nodes[:]  # fastest way to copy
                tmpnodes.remove(node1)
                node2 = random.choice(tmpnodes)
                ckt.__elements__[o_node_name] = (type, [node1, node2])
            level_nodes.append(o_node_name)
        if i == width-1:
            ckt.__outputs__ = level_nodes
        current_nodes = current_nodes + level_nodes

    return ckt


def print_circuit(ckt, file_name):
    out = open(file_name, "w")
    for i in ckt.__inputs__:
        out.write("INPUT(" + i + ")\n")
    out.write("\n")
    for i in ckt.__outputs__:
        out.write("OUTPUT(" + i + ")\n")
    out.write("\n")
    for el in ckt.__elements__:
        out.write(el + ' = ' + ckt.__elements__[el][0] + "(")
        out.write(ckt.__elements__[el][1][0])
        for i in range(1, len(ckt.__elements__[el][1])):
            out.write(", " + ckt.__elements__[el][1][i])
        out.write(")\n")
    out.close()


def print_failures(ckt, file_name):
    out = open(file_name, "w")
    total = 1
    for i in ckt.__inputs__:
        for l in ['SA0', 'SA1', 'NEG']:
            out.write(str(total) + " " + i + " " + l + "\n")
            total += 1
    for o in ckt.__outputs__:
        for l in ['SA0', 'SA1', 'NEG']:
            out.write(str(total) + " " + o + " " + l + "\n")
            total += 1

    for el in ckt.__elements__:
        if el in ckt.__outputs__ or el in ckt.__inputs__:
            continue
        for l in ['SA0', 'SA1', 'NEG']:
            out.write(str(total) + " " + el + " " + l + "\n")
            total += 1
        type = ckt.__elements__[el][0]
        if type == 'BUFF':
            out.write(str(total) + " " + el + " " + "RDOB_NOT\n")
            total += 1
        elif type == 'NOT':
            out.write(str(total) + " " + el + " " + "RDOB_BUFF\n")
            total += 1
        else:
            for l in ["AND", "OR", "NAND", "NOR", "XOR", "NXOR"]:
                if l == type:
                    continue
                out.write(str(total) + " " + el + " " + "RDOB_" + l + "\n")
                total += 1

    out.close()


if __name__ == '__main__':
    ckt = generate_random_ckt(10, 10, 100)
    print_circuit(ckt, 'Case_05/design_05.isc')
    print_failures(ckt, 'Case_05/fault_description.txt')
    print(ckt)