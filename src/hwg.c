#include "hwg.h"
#include <string.h>

/*Assign an ID of possibly a different entity to a node*/
hw_graph_error hw_node_assign_id(hw_node_t *node, const unsigned int id)
{
    if (node->assignedObjects >= HWG_MAX_OBJECTS)
        return HW_GRAPH_ERR_OOR;

    node->assignedObjectIds[node->assignedObjects] = id;
    node->assignedObjects++;
    return HW_GRAPH_ERR_NONE;
}

/*Assigns a subgraph to a node, creating a port for each unconnected port in subgraph*/
hw_graph_error hw_node_assign_subgraph(hw_node_subgraph_t *node, hw_graph_t *subgraph)
{
    hw_graph_error err = HW_GRAPH_ERR_NONE;
    hw_base_node_t *current;
    priority_list_iterator_t it;
    unsigned int i;

    if (node->subgraph)
        return HW_GRAPH_ERR_UNKNOWN;

    node->subgraph = subgraph;

    /*Cycle through ALL nodes of subgraph*/
    for (current = priority_list_first(subgraph->nodes, &it);
            current;
            current = priority_list_next(&it))
    {
        /*Find an unconnected port*/
        for (i = 0; i < current->numPorts; ++i)
            if (!current->ports[i].edge)
                /*If some has been found: Add a port with same id, type and name to the hosting SUBGRAPH node*/
                if ((err = hw_node_add_port(&node->base, current->ports[i].id, current->ports[i].type, current->ports[i].name)) != HW_GRAPH_ERR_NONE)
                    return err;
    }

    return err;
}

hw_graph_error hw_node_add_port(hw_base_node_t *node, const unsigned int id, const hw_port_type_t type, const char *name)
{
    if (hw_node_get_port(node, id))
        return HW_GRAPH_ERR_DUPLICATE_PORT;

    if (node->numPorts >= HWG_MAX_PORTS)
        return HW_GRAPH_ERR_OOR;

    node->ports[node->numPorts].id = id;
    node->ports[node->numPorts].type = type;
    strncpy(node->ports[node->numPorts].name, name, HWG_MAX_STRING_LENGTH);
    node->ports[node->numPorts].edge = NULL;
    node->numPorts++;

    return HW_GRAPH_ERR_NONE;
}

hw_port_t *hw_node_get_port(hw_base_node_t *node, const unsigned id)
{
    unsigned int i;

    for (i = 0; i < HWG_MAX_PORTS; ++i)
        if (node->ports[i].id == id) return &node->ports[i];

    return NULL;
}

hw_base_node_t *hw_graph_get_node(hw_graph_t *graph, const unsigned id)
{
    hw_base_node_t *node;
    priority_list_iterator_t it;

    for (node = priority_list_first(graph->nodes, &it);
            node;
            node = priority_list_next(&it))
        if (node->id == id) return node;

    return NULL;
}

hw_edge_t *hw_graph_get_edge(hw_graph_t *graph, const unsigned id)
{
    hw_edge_t *edge;
    priority_list_iterator_t it;

    for (edge = priority_list_first(graph->edges, &it);
            edge;
            edge = priority_list_next(&it))
        if (edge->id == id) return edge;

    return NULL;
}

hw_graph_error hw_graph_create_node(hw_graph_t *graph, const unsigned int id, const hw_node_type_t type, const char *name)
{
    hw_graph_error err = HW_GRAPH_ERR_NONE;
    hw_base_node_t *newNode;
    priority_list_iterator_t it;

    if (hw_graph_get_node(graph, id))
        return HW_GRAPH_ERR_DUPLICATE_NODE;

    switch (type) {
        case HW_NODE_TYPE_SUBGRAPH:
            newNode = calloc(1, sizeof(hw_node_subgraph_t));
        default:
            newNode = calloc(1, sizeof(hw_node_t));
    }

    if (!newNode)
        return HW_GRAPH_ERR_NOMEM;
    newNode->type = type;
    strncpy(newNode->name, name, HWG_MAX_STRING_LENGTH);

    priority_list_insert(graph->nodes, newNode, newNode->costs, &it);

    return err;
}

hw_graph_error hw_graph_create_edge(hw_graph_t *graph, const unsigned int id, const char *name, const unsigned int nodeId1, const unsigned int portId1, const unsigned int nodeId2, const unsigned int portId2)
{
    hw_graph_error err = HW_GRAPH_ERR_NONE;
    hw_edge_t *newEdge;
    hw_base_node_t *node1, *node2;
    priority_list_iterator_t it;

    if (hw_graph_get_edge(graph, id))
        return HW_GRAPH_ERR_DUPLICATE_EDGE;
    node1 = hw_graph_get_node(graph, nodeId1);
    node2 = hw_graph_get_node(graph, nodeId2);
    if (!node1 || !node2)
        return HW_GRAPH_ERR_NOT_FOUND;
    if (!hw_node_get_port(node1, portId1) || !hw_node_get_port(node2, portId2))
        return HW_GRAPH_ERR_NOT_FOUND;

    newEdge = calloc(1, sizeof(hw_edge_t));
    if (!newEdge)
        return HW_GRAPH_ERR_NOMEM;

    newEdge->id = id;
    strncpy(newEdge->name, name, HWG_MAX_STRING_LENGTH);
    newEdge->nodes[0] = node1;
    newEdge->nodes[1] = node2;
    newEdge->ports[0] = portId1;
    newEdge->ports[1] = portId2;

    priority_list_insert(graph->edges, newEdge, newEdge->costs, &it);

    return err;
}

hw_graph_error hw_graph_clone(hw_graph_t *graph, const hw_graph_t *src)
{
    hw_graph_error err = HW_GRAPH_ERR_NONE;
    hw_base_node_t *node;
    hw_edge_t *edge;
    priority_list_iterator_t it;
    priority_list_t *nodes, *edges;

    nodes = graph->nodes;
    edges = graph->edges;

    if (!nodes || !edges)
        return HW_GRAPH_ERR_NOT_INITIALIZED;

    memcpy(graph, src, sizeof(hw_graph_t));

    graph->nodes = nodes;
    graph->edges = edges;

    for (node = priority_list_first(src->nodes, &it);
         node && (err == HW_GRAPH_ERR_NONE);
         node = priority_list_next(&it))
            err = hw_graph_clone_node(graph, node);

    for (edge = priority_list_first(src->edges, &it);
         edge && (err == HW_GRAPH_ERR_NONE);
         edge = priority_list_next(&it))
            err = hw_graph_clone_edge(graph, edge);

    return err;
}

hw_graph_error hw_graph_clone_node(hw_graph_t *graph, const hw_base_node_t *node)
{
    hw_graph_error err = HW_GRAPH_ERR_NONE;
    hw_node_t *newNode;
    priority_list_iterator_t it;
    unsigned int i;

    if (hw_graph_get_node(graph, node->id))
        return HW_GRAPH_ERR_DUPLICATE_NODE;

    if (node->type == HW_NODE_TYPE_SUBGRAPH)
        return hw_graph_clone_subgraph_node(graph, (hw_node_subgraph_t *)node);

    newNode = calloc(1, sizeof(hw_node_t));
    if (!newNode)
        return HW_GRAPH_ERR_NOMEM;
    memcpy(newNode, node, sizeof(hw_node_t));

    /*Resolve edge pointers in ports of node*/
    for (i = 0; i < newNode->base.numPorts; ++i)
        newNode->base.ports[i].edge = hw_graph_get_edge(graph, node->ports[i].edge->id);
    priority_list_insert(graph->nodes, newNode, newNode->base.costs, &it);

    return err;
}

hw_graph_error hw_graph_clone_subgraph_node(hw_graph_t *graph, const hw_node_subgraph_t *node)
{
    hw_graph_error err = HW_GRAPH_ERR_NONE;
    hw_node_subgraph_t *newNode;
    hw_graph_t *newSubgraph;
    priority_list_iterator_t it;
    unsigned int i;

    if (hw_graph_get_node(graph, node->base.id))
        return HW_GRAPH_ERR_DUPLICATE_NODE;

    if (node->base.type != HW_NODE_TYPE_SUBGRAPH)
        return HW_GRAPH_ERR_WRONG_TYPE;

    newNode = calloc(1, sizeof(hw_node_subgraph_t));
    if (!newNode)
        return HW_GRAPH_ERR_NOMEM;
    memcpy(newNode, node, sizeof(hw_node_subgraph_t));
    newSubgraph = calloc(1, sizeof(hw_graph_t));
    if (!newSubgraph) {
        free(newNode);
        return HW_GRAPH_ERR_NOMEM;
    }
    err = hw_graph_init(newSubgraph, node->base.id, node->base.name, graph->subName);
    if (err != HW_GRAPH_ERR_NONE) {
        free(newNode);
        hw_graph_destroy(newSubgraph);
        return err;
    }
    if (node->subgraph) {
        err = hw_graph_clone(newSubgraph, node->subgraph);
        if (err != HW_GRAPH_ERR_NONE) {
            free(newNode);
            hw_graph_destroy(newSubgraph);
            return err;
        }
    }
    newNode->subgraph = newSubgraph;
    /*Resolve edge pointers in ports of node*/
    for (i = 0; i < newNode->base.numPorts; ++i)
        newNode->base.ports[i].edge = hw_graph_get_edge(graph, node->base.ports[i].edge->id);
    priority_list_insert(graph->nodes, newNode, newNode->base.costs, &it);

    return err;
}

hw_graph_error hw_graph_clone_edge(hw_graph_t *graph, const hw_edge_t *edge)
{
    hw_graph_error err = HW_GRAPH_ERR_NONE;
    hw_edge_t *newEdge;
    priority_list_iterator_t it;

    if (hw_graph_get_edge(graph, edge->id))
        return HW_GRAPH_ERR_DUPLICATE_EDGE;

    newEdge = calloc(1, sizeof(hw_edge_t));
    if (!newEdge)
        return HW_GRAPH_ERR_NOMEM;
    memcpy(newEdge, edge, sizeof(hw_edge_t));

    /*Resolve pointers to nodes*/
    newEdge->nodes[0] = hw_graph_get_node(graph, edge->nodes[0]->id);
    newEdge->nodes[1] = hw_graph_get_node(graph, edge->nodes[1]->id);
    priority_list_insert(graph->edges, newEdge, newEdge->costs, &it);

    return err;
}

hw_graph_error hw_graph_init(hw_graph_t *graph, const unsigned int id, const char *name, const char *subName)
{
    graph->id = id;
    strncpy(graph->name, name, HWG_MAX_STRING_LENGTH);
    strncpy(graph->subName, subName, HWG_MAX_STRING_LENGTH);
    graph->maxCosts = 0;
    graph->costs = 0;

    priority_list_init(&graph->nodes);
    if (!graph->nodes)
        return HW_GRAPH_ERR_NOMEM;
    priority_list_init(&graph->edges);
    if (!graph->edges) {
        priority_list_deinit(graph->nodes);
        return HW_GRAPH_ERR_NOMEM;
    }

    return HW_GRAPH_ERR_NONE;
}

void hw_graph_destroy(hw_graph_t *graph)
{
    hw_graph_error err = HW_GRAPH_ERR_NONE;
    hw_base_node_t *node;
    hw_edge_t *edge;
    priority_list_iterator_t it;

    /*Free everything :)*/
    if (graph->nodes) {
        for (node = priority_list_first(graph->nodes, &it);
             node && (err == HW_GRAPH_ERR_NONE);
             node = priority_list_next(&it))
        {
            if (node->type == HW_NODE_TYPE_SUBGRAPH)
                hw_graph_destroy(((hw_node_subgraph_t *)node)->subgraph);
            else
                free(node);
        }
        priority_list_deinit(graph->nodes);
        graph->nodes = NULL;
    }

    if (graph->edges) {
        for (edge = priority_list_first(graph->edges, &it);
             edge && (err == HW_GRAPH_ERR_NONE);
             edge = priority_list_next(&it))
            free(edge);
        priority_list_deinit(graph->edges);
        graph->edges = NULL;
    }
}
