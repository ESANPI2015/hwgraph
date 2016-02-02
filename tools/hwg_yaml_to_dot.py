#!/usr/bin/env python

import sys
import os
import yaml

if len(sys.argv) < 2:
    print "no argument given"
    exit(0)

if not os.path.isfile(sys.argv[1]):
    print "argument is not a correct file path"
    exit(0)

filename = sys.argv[1]
ymlGraph = yaml.load(open(filename))

s = '''graph "%s" {
  size="100,100";
  ranksep="1";
  nodesep="1";
''' %filename

lineWidth = 2.0
for node in ymlGraph["nodes"]:
    nodeId = int(node["id"])
    s += "  node%05d" %nodeId
    s += " [penwidth=%f, shape=Mrecord, label=\"[%d]|{" %(lineWidth, nodeId)

    idx = 0
    for nInput in node["ports"]:
        if idx == 0:
            s += "{"
        else:
            s += "| "
        first = False
        s += " <%d> %s %s" %(int(nInput["id"]), nInput["type"], nInput["name"])
        idx += 1
    s += "} | %s}\"" %node["name"].replace(">", "\>")

    if node["type"] == "FPGA":
        s += ", fillcolor=\"#bbeebb\", style=filled"
    elif node["type"] == "SUBGRAPH":
        s += ", fillcolor=\"#bbccff\", style=filled"
    else:
        s += ", fillcolor=\"#ffff70\", style=filled"

    s += "];\n"

s += "\n"

for edge in ymlGraph["edges"]:
    edgeId = int(edge["id"])
    edgeName = edge["name"]
    nodeIds = []
    portIds = []
    idx = 0
    for node in edge["nodes"]:
        nodeIds.append(int(node["id"]))
        portIds.append(int(node["port"]))
        idx += 1

    s += "node%05d:%d -- node%05d:%d [penwidth=%f, label=%s];\n" %(nodeIds[0], portIds[0], nodeIds[1], portIds[1], lineWidth, edgeName)

s += "}\n"

with open(filename+".dot", "w") as f:
    f.write(s)
