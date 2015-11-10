from pagerank import PageRank

graph = {}
graph["A"] = set(["B", "C", "E"])
graph["B"] = set(["C", "E"])
graph["C"] = set(["D"])
graph["D"] = set([])
graph["E"] = set([])

pr = PageRank(graph, .25, "test")


print "Itterations"
pr.runPageRankI(100)
print "\nConverge"
pr.runPageRankE(.000001)
