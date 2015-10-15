from Graph import Graph
from pagerank import PageRank
import sys

fileName = "karateDir.csv"
for arg in sys.argv:
    if ".txt" in arg or ".csv" in arg or ".gml" in arg:
        fileName = arg

graph = Graph(fileName)
pr = PageRank(graph.return_graph(), .85)

print "Itterations"
pr.runPageRankI(100)
print "\nConverge"
pr.runPageRankE(.000001)