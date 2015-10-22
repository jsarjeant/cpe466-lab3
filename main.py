from Graph import Graph
from pagerank import PageRank
import sys
from datetime import datetime


def print_time(start_time):
    endTime = datetime.now()
    print "Time: ", endTime - start_time, "\n"
    return endTime


fileNames = []
print_adjacency_list = False
directed = False
for arg in sys.argv:
    if ".txt" in arg or ".csv" in arg or ".gml" in arg:
        fileNames.append(arg)

    if "-p" in arg:
        print_adjacency_list = True

    if "-d" in arg:
        directed = True

    print arg, " == ", "soc-LiveJournal1.txt : ", arg == "soc-LiveJournal1.txt"

for fileName in fileNames:
    total_time = time = datetime.now()
    print "----------------"
    print "\nCreating graph from", fileName, "..."
    graph = Graph(fileName, print_adjacency_list, directed)
    read_time = time = print_time(time)
    print "Generating Page Rank"
    pr = PageRank(graph.return_graph(), .85, fileName)
    '''print "Itterations"
    pr.runPageRankI(20)
    time = print_time(time)'''
    print "\nConverge"
    pr.runPageRankE(.000001)
    processing_time = time = print_time(time)
    with open(fileName.split('.')[0] + '-result.txt', 'a') as f:
        f.write("Read Time" + '\t' + str(read_time - total_time) + '\n')
        f.write("Processing Time" + '\t' + str(processing_time - total_time) + '\n')

    print "\nTotal Time: ", time - total_time