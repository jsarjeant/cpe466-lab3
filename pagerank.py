import operator
import itertools

class PageRank:
   
   def __init__(self, graph, d):
      self.graph = graph
      self.d = d
      self.numVerts = float(len(graph))

   def runPageRankE(self, e):
      ittrNum = 0

      # start with all weights being equal
      self.ranks = {}
      for n in self.graph:
         self.ranks[n] = 1 / self.numVerts
      # While sum of new rank of nodes - old rank of nodes > e
      while True:
         ittrNum = ittrNum + 1
         newRanks = {}
         # Calc PageRank for each node
         for n in self.graph:
            newRanks[n] = self.__calcNodeRank(n)
         if self.__calcDifference(newRanks, self.ranks) < e:
            return self.__foundPageRank(newRanks, ittrNum)
         # update Ranks
         self.ranks = newRanks

   def runPageRankI(self, numIttr):
      ittrNum = 0

      # start with all weights being equal
      self.ranks = {}
      for n in self.graph:
         self.ranks[n] = 1 / self.numVerts

      while ittrNum < numIttr:
         newRanks = {}
         # Calc PageRank for each node
         for n in self.graph:
            newRanks[n] = self.__calcNodeRank(n)
 
         # update Ranks
         self.ranks = newRanks
         ittrNum = ittrNum + 1
       
      return self.__foundPageRank(self.ranks, ittrNum)

   def __foundPageRank(self, ranks, ittrNum):
      print "----------------"
      print "Page rank found in: ", ittrNum, "iterations"
      ranks = reversed(sorted(ranks.items(), key=operator.itemgetter(1)))
      for val in itertools.islice(ranks, 0, 10):
         print val
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
