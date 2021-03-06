import operator
import itertools
import sys
import signal
from datetime import datetime


class PageRank:
    def __init__(self, graph, d, file_name):
        self.graph = graph
        self.d = d
        self.numVerts = float(len(graph))
        self.file_name = file_name
        signal.signal(signal.SIGINT, self.signal_handler)
        self.ittrNum = 0
        self.ranks = {}
        self.epsilon = 0
        self.interrupt = False


    def runPageRankE(self, e):


        # start with all weights being equal
        for n in self.graph:
            self.ranks[n] = 1 / self.numVerts
        # While sum of new rank of nodes - old rank of nodes > e
        while True:
            self.ittrNum = self.ittrNum + 1
            newRanks = {}
            # Calc PageRank for each node
            for n in self.graph:
                newRanks[n] = self.__calcNodeRank(n)

            self.epsilon = self.__calcDifference(newRanks, self.ranks)
            print "Iteration: ", self.ittrNum, " -- Epsilon: ", self.epsilon

            if self.epsilon < e or self.interrupt:
                return self.__foundPageRank(newRanks, self.ittrNum, self.interrupt)
            # update Ranks
            self.ranks = newRanks
            end_time = datetime.now()

    def runPageRankI(self, numIttr):
        ittrNum = 0

        # start with all weights being equal
        self.ranks = {}
        for n in self.graph:
            self.ranks[n] = 1 / self.numVerts

        start_time = datetime.now()
        end_time = datetime.now()
        newRanks = {}
        while ittrNum < numIttr:
            start_time = end_time
            #print "Iteration: ", ittrNum
            # Calc PageRank for each node
            for n in self.graph:
                newRanks[n] = self.__calcNodeRank(n)

            # update Ranks
            #if ittrNum + 1 >= numIttr:
            #    print "Epsilon: ", self.__calcDifference(newRanks, self.ranks)
            self.ranks = newRanks
            ittrNum = ittrNum + 1
            end_time = datetime.now()
            #print "Time: ", end_time - start_time

        return self.__foundPageRank(self.ranks, ittrNum)

    def __foundPageRank(self, ranks, ittrNum, interrupt=False):
        print "Page rank found in: ", ittrNum, "iterations"
        length = len(ranks)
        ranks = reversed(sorted(ranks.items(), key=operator.itemgetter(1)))
        with open(self.file_name.split('.')[0] + '-result.txt', 'w+') as f:
            for key, val in itertools.islice(ranks, 0, length):
                f.write(key + "\t" + str(val) + "\n")

            f.write("Itterations" + "\t" + str(ittrNum) + "\n")
            if interrupt:
                f.write("Epsilon\t" + str(self.epsilon) + "\n");

        return ranks

    def __calcNodeRank(self, nodeId):
        # pR(i) = (1 - d) / numNodes + d * sum(pR-1(j) / outdegree(j))
        inNodes = self.__findInNodes(nodeId)
        inSum = 0
        for n in inNodes:
            inSum += self.ranks[n] / float(len(self.graph[n]))
        return (1 - self.d) / self.numVerts + self.d * inSum

    def __findInNodes(self, nodeId):
        # Returns all nodes that point to [nodeId]
        nodes = []
        for n in self.graph:
            if nodeId in self.graph[n]:
                nodes.append(n)
        return nodes

    # dict1 = new
    # dict2 = old
    def __calcDifference(self, dict1, dict2):
        sum = 0
        for n in dict1:
            sum += abs(dict1[n] - dict2[n])
        return sum

    def signal_handler(self, signal, frame):
        print "Pressed ctrl+c"
        self.interrupt = True
