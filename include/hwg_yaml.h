#ifndef _HWG_YAML_H
#define _HWG_YAML_H

#include <yaml.h>

typedef enum {
    HWG_PARSE_STATE_UNDEF,
    HWG_PARSE_STATE_TOP,
    HWG_PARSE_STATE_NODES,
    HWG_PARSE_STATE_EDGES,
    HWG_PARSE_STATE_NODE,
    HWG_PARSE_STATE_NODE_PORTS,
    HWG_PARSE_STATE_EDGE
} hwg_parse_state;

typedef enum { 
    HWG_PARSE_ERR_NONE = 0,
    HWG_PARSE_ERR_MISSING_ID,
    HWG_PARSE_ERR_MISSING_TYPE,
    HWG_PARSE_ERR_MISSING_PORTS,
    HWG_PARSE_ERR_WRONG_TYPE,
    HWG_PARSE_ERR_TOO_MANY_NODES,
    HWG_PARSE_ERR_IO,
    HWG_PARSE_ERR_UNKNOWN
} hwg_parse_error;

hwg_parse_error hw_graph_from_parser(yaml_parser_t *parser, hw_graph_t *g);
hwg_parse_error hw_graph_from_yaml_file(const char *filename, hw_graph_t *g);
hwg_parse_error hw_graph_from_yaml_string(const unsigned char *buffer, const size_t size, hw_graph_t *g);

hwg_parse_error hw_graph_to_emitter(yaml_emitter_t *emitter, const hw_graph_t *g);
hwg_parse_error hw_graph_to_yaml_file(const char *filename, const hw_graph_t *g);
hwg_parse_error hw_graph_to_yaml_string(unsigned char *buffer, size_t buffer_size,
                                 const hw_graph_t *g, size_t *bytes_written);

#endif
