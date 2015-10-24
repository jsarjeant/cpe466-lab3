import sys
import signal

class Graph:
    def __init__(self, fileName, print_adjacency_list, directed):
        self.graph = {}
        self.directed = directed
        signal.signal(signal.SIGINT, self.signal_handler)
        self.__process_graph(fileName)
        if print_adjacency_list:
            self.__print_adjacency_list()

        pass

    # Helper for the construction of the graph.
    # Takes in a file_name, iterates over all lines in the file,
    # adding the edges into the adjacency graph
    def __process_graph(self, file_name):
        # Set which index to look into after the split on each on of the file
        nf_index = 2
        nt_index = 0
        split_char = ','
        if ".txt" in file_name:
            nt_index = 1
            split_char = None

        with open(file_name, 'r') as f:
            for line in f:
                if line[0] == '#':
                    continue

                nodes = line.split(split_char) if split_char else line.split()
                nf = nodes[nf_index].strip()
                nt = nodes[nt_index].strip()
                #TODO: Deal with edge weights
                self.graph.setdefault(nf, set([]))
                self.graph[nf].add(nt)

                if self.directed:
                    self.graph.setdefault(nt, set([]))
                    self.graph[nt].add(nf)

    def __print_adjacency_list(self):
        for k, v in self.graph.items():
            print k, " - ", list(v)

    def return_graph(self):
        return self.graph

    def get_node_ids(self):
        return self.graph.keys()

    def get_node(self, node_id):
        return self.graph[node_id]

    def signal_handler(self, signal, frame):
        print 'You pressed Ctrl+C!'
        sys.exit(0)
