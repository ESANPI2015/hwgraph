#ifndef _HWG_H
#define _HWG_H

#include "priority_list.h"

#define HWG_MAX_STRING_LENGTH 256
#define HWG_MAX_PORTS 256

typedef enum {
    HW_NODE_TYPE_NOT_SPEC = 0,
    HW_NODE_TYPE_CONVENTIONAL,
    HW_NODE_TYPE_FPGA,
    HW_NODE_TYPE_SUBGRAPH,
    HW_NODE_TYPES
} hw_node_type_t;

typedef enum {
    HW_PORT_TYPE_NOT_SPEC = 0,
    HW_PORT_TYPE_SOURCE,
    HW_PORT_TYPE_SINK,
    HW_PORT_TYPE_NDLCOM,
    HW_PORT_TYPES
} hw_port_type_t;

extern const char *hw_node_type_str[HW_NODE_TYPES];
extern const char *hw_port_type_str[HW_PORT_TYPES];

struct hw_edge_t;

typedef struct {
    unsigned int id;
    hw_port_type_t type;
    char name[HWG_MAX_STRING_LENGTH];

    struct hw_edge_t *edge;
} hw_port_t;

typedef struct {
    unsigned int id;
    hw_node_type_t type;
    char name[HWG_MAX_STRING_LENGTH];

    unsigned int numPorts;
    hw_port_t ports[HWG_MAX_PORTS];
} hw_node_t;

typedef struct hw_edge_t {
    unsigned int id;
    char name[HWG_MAX_STRING_LENGTH];

    hw_node_t *nodes[2];
    unsigned int ports[2];
} hw_edge_t;

typedef struct {
    unsigned int id;
    char name[HWG_MAX_STRING_LENGTH];

    priority_list_t *nodes;
    priority_list_t *edges;
} hw_graph_t;

typedef enum {
    HW_GRAPH_ERR_NONE = 0,
    HW_GRAPH_ERR_OOR,
    HW_GRAPH_ERR_DUPLICATE_NODE,
    HW_GRAPH_ERR_DUPLICATE_EDGE,
    HW_GRAPH_ERR_DUPLICATE_PORT,
    HW_GRAPH_ERR_NOMEM,
    HW_GRAPH_ERR_NOT_FOUND,
    HW_GRAPH_ERR_NOT_INITIALIZED,
    HW_GRAPH_ERR_WRONG_TYPE,
    HW_GRAPH_ERR_PORT_ALREADY_ASSIGNED,
    HW_GRAPH_ERR_UNKNOWN
} hw_graph_error;

typedef struct {
    hw_node_t base;
    hw_graph_t *subgraph;
} hw_node_subgraph_t;

hw_graph_error hw_node_assign_subgraph(hw_node_subgraph_t *node, hw_graph_t *subgraph);
hw_graph_error hw_node_add_port(hw_node_t *node, const unsigned int id, const hw_port_type_t type, const char *name);
hw_port_t *hw_node_get_port(hw_node_t *node, const unsigned id);

hw_node_t *hw_graph_get_node(hw_graph_t *graph, const unsigned id);
hw_edge_t *hw_graph_get_edge(hw_graph_t *graph, const unsigned id);

hw_graph_error hw_graph_create_node(hw_graph_t *graph, const unsigned int id, const hw_node_type_t type, const char *name);
hw_graph_error hw_graph_create_edge(hw_graph_t *graph, const unsigned int id, const char *name, const unsigned int nodeId1, const unsigned int portId1, const unsigned int nodeId2, const unsigned int portId2); 

hw_graph_error hw_graph_clone(hw_graph_t *graph, const hw_graph_t *src);
hw_graph_error hw_graph_clone_node(hw_graph_t *graph, const hw_node_t *node);
hw_graph_error hw_graph_clone_subgraph_node(hw_graph_t *graph, const hw_node_subgraph_t *node);
hw_graph_error hw_graph_clone_edge(hw_graph_t *graph, const hw_edge_t *edge);

hw_graph_error hw_graph_init(hw_graph_t *graph, const unsigned int id, const char *name);
void hw_graph_destroy(hw_graph_t *graph);

#endif
