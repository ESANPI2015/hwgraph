#include "hwg.h"
#include "hwg_yaml.h"

#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

static hwg_parse_error get_event(yaml_parser_t *parser, yaml_event_t *event,
                          yaml_event_type_t type);
static hwg_parse_error parse_nodes(yaml_parser_t *parser, hw_graph_t *g);
static hwg_parse_error parse_node(yaml_parser_t *parser, hw_graph_t *g);
static hwg_parse_error parse_node_id(yaml_parser_t *parser, unsigned int *id);
static hwg_parse_error parse_node_type(yaml_parser_t *parser,
                                hw_node_type_t *type);
static hwg_parse_error parse_node_ports(yaml_parser_t *parser, hw_port_t *ports,
                                  unsigned int *numPorts);
static hwg_parse_error parse_edges(yaml_parser_t *parser, hw_graph_t *g);


static hwg_parse_error get_event(yaml_parser_t *parser, yaml_event_t *event,
                          yaml_event_type_t type) {
  if(!yaml_parser_parse(parser, event)) {
    fprintf(stderr, "HWG_GET_EVENT: YAML parser encountered an error.\n");
    return HWG_PARSE_ERR_UNKNOWN;
  }
  if(event->type != type) {
    fprintf(stderr, "HWG_GET_EVENT: Expected %d got %d.\n",
            type, event->type);

    fprintf(stderr, "YAML_SCALAR_EVENT: %d\n", YAML_SCALAR_EVENT);
    fprintf(stderr, "YAML_SEQUENCE_START_EVENT: %d\n", YAML_SEQUENCE_START_EVENT);
    fprintf(stderr, "YAML_SEQUENCE_END_EVENT: %d\n", YAML_SEQUENCE_END_EVENT);
    fprintf(stderr, "YAML_MAPPING_START_EVENT: %d\n", YAML_MAPPING_START_EVENT);
    fprintf(stderr, "YAML_MAPPING_END_EVENT: %d\n", YAML_MAPPING_END_EVENT);

    return HWG_PARSE_ERR_UNKNOWN;
  }
  return HWG_PARSE_ERR_NONE;
}

static hwg_parse_error get_int(yaml_parser_t *parser, unsigned int *result) {
  hwg_parse_error err = HWG_PARSE_ERR_NONE;
  yaml_event_t event;

  if ((err = get_event(parser, &event, YAML_SCALAR_EVENT)) != HWG_PARSE_ERR_NONE)
    return err;

  *result = (unsigned int) atol((const char*)event.data.scalar.value);
  yaml_event_delete(&event);

  return err;
}

/*static hwg_parse_error get_double(yaml_parser_t *parser, double *result) {*/
  /*hwg_parse_error err = HWG_PARSE_ERR_NONE;*/
  /*yaml_event_t event;*/

  /*if ((err = get_event(parser, &event, YAML_SCALAR_EVENT)) != HWG_PARSE_ERR_NONE)*/
    /*return err;*/

  /**result = atof((const char*)event.data.scalar.value);*/
  /*yaml_event_delete(&event);*/

  /*return err;*/
/*}*/

static hwg_parse_error get_string(yaml_parser_t *parser, char s[HWG_MAX_STRING_LENGTH]) {
  hwg_parse_error err = HWG_PARSE_ERR_NONE;
  yaml_event_t event;

  if ((err = get_event(parser, &event, YAML_SCALAR_EVENT)) != HWG_PARSE_ERR_NONE)
    return err;

  strncpy(s, (const char*)event.data.scalar.value, HWG_MAX_STRING_LENGTH);
  yaml_event_delete(&event);

  return err;
}

static hwg_parse_error _skip_node_helper(yaml_parser_t *parser, yaml_event_t *event,
                                  bool is_nested) {
  hwg_parse_error err = HWG_PARSE_ERR_NONE;
  yaml_event_type_t type;
  yaml_event_t next_event;
  bool done = false;
  type = event->type;

  switch(type) {
      case YAML_SEQUENCE_START_EVENT:
        do {
          if(!yaml_parser_parse(parser, &next_event)) {
            fprintf(stderr, "HWG_SKIP_NODE_HELPER: Error while parsing\n");
            return HWG_PARSE_ERR_UNKNOWN;
          }
          err = _skip_node_helper(parser, &next_event, true);
          if(next_event.type == YAML_SEQUENCE_END_EVENT) {
            done = true;
          }
          yaml_event_delete(&next_event);
        } while(!done);
        break;
      case YAML_MAPPING_START_EVENT:
        do {
          if(!yaml_parser_parse(parser, &next_event)) {
            fprintf(stderr, "HWG_SKIP_NODE_HELPER: Error while parsing\n");
            return HWG_PARSE_ERR_UNKNOWN;
          }
          err = _skip_node_helper(parser, &next_event, true);
          if(next_event.type == YAML_MAPPING_END_EVENT) {
            done = true;
          }
          yaml_event_delete(&next_event);
        } while(!done);
        break;
      case YAML_SEQUENCE_END_EVENT:
      case YAML_MAPPING_END_EVENT:
        break;
      case YAML_ALIAS_EVENT:
      case YAML_SCALAR_EVENT:
        break;
      case YAML_NO_EVENT:
        fprintf(stderr, "HWG_SKIP_NODE_HELPER: What is a YAML NO_EVENT? Let's skip the next node.\n");
        return HWG_PARSE_ERR_UNKNOWN;
      default:
        fprintf(stderr, "HWG_SKIP_NODE_HELPER: unhandled event: %d.\n", event->type);
        return HWG_PARSE_ERR_UNKNOWN;
  }
  return err;
}

static hwg_parse_error skip_node(yaml_parser_t *parser, yaml_event_t *event) {
  return _skip_node_helper(parser, event, false);
}

static hwg_parse_error skip_next_node(yaml_parser_t *parser, yaml_event_t *event) {
  hwg_parse_error err;
  yaml_event_t next_event;

  if(!yaml_parser_parse(parser, &next_event)) {
    fprintf(stderr, "HWG_SKIP_NEXT_NODE: Error while parsing\n");
    return HWG_PARSE_ERR_UNKNOWN;
  }
  err = skip_node(parser, &next_event);
  yaml_event_delete(&next_event);

  return err;
}

static hwg_parse_error parse_nodes(yaml_parser_t *parser, hw_graph_t *g) {
  hwg_parse_error err = HWG_PARSE_ERR_NONE;
  yaml_event_t event;

  err = get_event(parser, &event, YAML_SEQUENCE_START_EVENT); /*Parse until SEQ START*/
  yaml_event_delete(&event);

  err = get_event(parser, &event, YAML_MAPPING_START_EVENT); /*Parse until MAPPING START*/
  do {
        switch(event.type) {
            case YAML_MAPPING_START_EVENT:
              err = parse_node(parser, g);
              break;
            default:
              fprintf(stderr, "HWG_PARSE_NODES: unexpected sequence \"%d\"\n.", event.type);
              break;
        }
        yaml_event_delete(&event);
        if (!yaml_parser_parse(parser, &event))
            err = HWG_PARSE_ERR_UNKNOWN;
  } while (err == HWG_PARSE_ERR_NONE && event.type != YAML_SEQUENCE_END_EVENT);

  if(err != HWG_PARSE_ERR_NONE) {
    fprintf(stderr, "HWG_PARSE_NODES: Got %u\n", err);
    return err;
  }
  yaml_event_delete(&event); /*Delete SEQ END*/

  return err;
}

static hwg_parse_error parse_node(yaml_parser_t *parser, hw_graph_t *g) {
  hwg_parse_error err;
  hw_graph_error gerr;
  yaml_event_t event;
  bool gotId = false;
  bool gotType = false;
  bool gotName = false;
  bool gotPorts = false;
  hw_node_t node;
  hw_graph_t subgraph;
  hw_node_subgraph_t subNode;

  err = get_event(parser, &event, YAML_SCALAR_EVENT); /*Go to first SCALAR*/
  do {
        if(strcmp((const char*)event.data.scalar.value, "id") == 0) {
          if(gotId) {
            fprintf(stderr, "HWG_PARSE_NODE: Multiple \"id\" sections.\n");
          }
          err = parse_node_id(parser, &node.id);
          gotId = true;
        } else if(strcmp((const char*)event.data.scalar.value, "type") == 0) {
          if(gotType) {
            fprintf(stderr, "HWG_PARSE_NODE: Multiple \"type\" sections.\n");
          }
          err = parse_node_type(parser, &node.type);
          gotType = true;
        } else if(strcmp((const char*)event.data.scalar.value, "ports") == 0) {
          if(gotPorts) {
            fprintf(stderr, "HWG_PARSE_NODE: Multiple \"ports\" sections.\n");
          }
          err = parse_node_ports(parser, node.ports, &node.numPorts);
          gotPorts = true;
        } else if(strcmp((const char*)event.data.scalar.value, "name") == 0) {
          if(gotName) {
            fprintf(stderr, "HWG_PARSE_NODE: Multiple \"name\" sections.\n");
          }
          err = get_string(parser, node.name);
          gotName = true;
        } else {
          skip_node(parser, &event);
          skip_next_node(parser, &event);
        }
        yaml_event_delete(&event);
        if (!yaml_parser_parse(parser, &event))
            err = HWG_PARSE_ERR_UNKNOWN;
  } while (err == HWG_PARSE_ERR_NONE && event.type != YAML_MAPPING_END_EVENT);

  if (err != HWG_PARSE_ERR_NONE)
  {
      fprintf(stderr, "HWG_PARSE_NODE: Parser error\n");
      return err;
  }
  yaml_event_delete(&event);

  if (!gotId)
  {
      fprintf(stderr, "HWG_PARSE_NODE: No id found\n");
      return HWG_PARSE_ERR_MISSING_ID;
  }
  if (!gotType)
  {
      fprintf(stderr, "HWG_PARSE_NODE: No type found\n");
      return HWG_PARSE_ERR_MISSING_TYPE;
  }
  if (!gotPorts || node.numPorts < 1)
  {
      fprintf(stderr, "HWG_PARSE_NODE: No port found\n");
      return HWG_PARSE_ERR_MISSING_PORTS;
  }

  if (node.type == HW_NODE_TYPE_SUBGRAPH)
  {
      /*Special handling for SUBGRAPH nodes!*/
      memcpy(&subNode.base, &node, sizeof(hw_node_t));
      gerr = hw_graph_init(&subgraph, node.id, node.name, g->subName);
      if (gerr != HW_GRAPH_ERR_NONE)
      {
          fprintf(stderr, "HWG_PARSE_NODE: Graph init failed (%u)\n", (unsigned int)gerr);
          return HWG_PARSE_ERR_UNKNOWN;
      }
      err = hw_graph_from_yaml_file(node.name, &subgraph);
      if (err != HWG_PARSE_ERR_NONE)
      {
          hw_graph_destroy(&subgraph);
          return err;
      }
      gerr = hw_node_assign_subgraph(&subNode, &subgraph);
      if (gerr != HW_GRAPH_ERR_NONE)
      {
          fprintf(stderr, "HWG_PARSE_NODE: Graph assign failed (%u)\n", (unsigned int)gerr);
          hw_graph_destroy(&subgraph);
          return HWG_PARSE_ERR_UNKNOWN;
      }
      gerr = hw_graph_clone_subgraph_node(g, &subNode);
      if (gerr != HW_GRAPH_ERR_NONE)
      {
          fprintf(stderr, "HWG_PARSE_NODE: Could not clone subgraph node (%u)\n", (unsigned int)gerr);
          hw_graph_destroy(&subgraph);
          return HWG_PARSE_ERR_UNKNOWN;
      }
  } else {
      /*Clone node*/
      if ((gerr = hw_graph_clone_node(g, &node)) != HW_GRAPH_ERR_NONE)
      {
          fprintf(stderr, "HWG_PARSE_NODE: Could not clone node (%u)\n", (unsigned int)gerr);
          return HWG_PARSE_ERR_UNKNOWN;
      }
  }

  return err;
}

static hwg_parse_error parse_node_id(yaml_parser_t *parser, unsigned int *id) {
  return get_int(parser, id);
}

static hwg_parse_error parse_node_type(yaml_parser_t *parser,
                                hw_node_type_t *type) {
  hwg_parse_error err = HWG_PARSE_ERR_NONE;
  char type_str[HWG_MAX_STRING_LENGTH];
  unsigned int i;

  err = get_string(parser, type_str);
  for (i = 0; i < HW_NODE_TYPES; ++i)
  {
      if (strcmp(hw_node_type_str[i], type_str) == 0)
      {
          *type = i;
          break;
      }
  }
  if (i >= HW_NODE_TYPES)
  {
      fprintf(stderr, "HWG_PARSE_NODE_TYPE: Error parsing node type \"%s\".\n", type_str);
      err = HWG_PARSE_ERR_WRONG_TYPE;
  }
  return err;
}

static hwg_parse_error parse_port_type(yaml_parser_t *parser, hw_port_type_t *type)
{
  hwg_parse_error err = HWG_PARSE_ERR_NONE;
  char type_str[HWG_MAX_STRING_LENGTH];
  unsigned int i;

  err = get_string(parser, type_str);
  for (i = 0; i < HW_PORT_TYPES; ++i)
  {
      if (strcmp(hw_port_type_str[i], type_str) == 0)
      {
          *type = i;
          break;
      }
  }
  if (i >= HW_NODE_TYPES)
  {
      fprintf(stderr, "HWG_PARSE_PORT_TYPE: Error parsing port type \"%s\".\n", type_str);
      err = HWG_PARSE_ERR_WRONG_TYPE;
  }
  return err;
}

static hwg_parse_error parse_node_ports(yaml_parser_t *parser,
                                  hw_port_t *ports, unsigned int *numPorts) {
  hwg_parse_error err;
  yaml_event_t event;
  bool gotId = false;
  bool gotType = false;
  bool gotName = false;
  size_t cnt = 0;

  err = get_event(parser, &event, YAML_SEQUENCE_START_EVENT);
  yaml_event_delete(&event);
  err = get_event(parser, &event, YAML_MAPPING_START_EVENT);
  do {
      gotId = false;
      gotType = false;
      gotName = false;
      if (event.type == YAML_MAPPING_START_EVENT)
      {
          err = get_event(parser, &event, YAML_SCALAR_EVENT);
          do {
              if (strcmp((const char*)event.data.scalar.value, "id") == 0)
              {
                  if (gotId)
                  {
                      fprintf(stderr, "HWG_PARSE_NODE_PORTS: Multiple \"id\" sections.\n");
                  }
                  err = get_int(parser, &ports[cnt].id);
                  gotId = true;
              }
              else if (strcmp((const char*)event.data.scalar.value, "name") == 0)
              {
                  if (gotName)
                  {
                      fprintf(stderr, "HWG_PARSE_NODE_PORTS: Multiple \"name\" sections.\n");
                  }
                  err = get_string(parser, ports[cnt].name);
                  gotName = true;
              }
              else if (strcmp((const char*)event.data.scalar.value, "type") == 0)
              {
                  if (gotType)
                  {
                      fprintf(stderr, "HWG_PARSE_NODE_PORTS: Multiple \"type\" sections.\n");
                  }
                  err = parse_port_type(parser, &ports[cnt].type);
                  gotType = true;
              } else {
                skip_next_node(parser, &event);
              }
              yaml_event_delete(&event);
              if (!yaml_parser_parse(parser, &event))
                  err = HWG_PARSE_ERR_UNKNOWN;
          } while (err == HWG_PARSE_ERR_NONE && event.type != YAML_MAPPING_END_EVENT);
          if (!gotId)
              err = HWG_PARSE_ERR_MISSING_ID;
          if (!gotType)
              err = HWG_PARSE_ERR_MISSING_TYPE;
          ++cnt;
      }
      yaml_event_delete(&event);
      if (!yaml_parser_parse(parser, &event))
          err = HWG_PARSE_ERR_UNKNOWN;
  } while (err == HWG_PARSE_ERR_NONE && event.type != YAML_SEQUENCE_END_EVENT && cnt < HWG_MAX_PORTS);

  if (err != HWG_PARSE_ERR_NONE)
  {
      fprintf(stderr, "HWG_PARSE_NODE_PORTS: Error parsing port %u\n", (unsigned int)cnt);
      return err;
  }
  yaml_event_delete(&event);
  *numPorts = cnt;

  return err;
}

static hwg_parse_error parse_edges(yaml_parser_t *parser, hw_graph_t *g)
{
  hwg_parse_error err = HWG_PARSE_ERR_NONE;
  hw_graph_error gerr;
  yaml_event_t event;
  bool gotId = false;
  bool gotName = false;
  unsigned int gotNodes = 0;
  unsigned int gotPorts = 0;
  unsigned int nodeId;
  hw_edge_t edge;

  /*fprintf(stderr, "parse edges...");*/
  err = get_event(parser, &event, YAML_SEQUENCE_START_EVENT);
  yaml_event_delete(&event);
  err = get_event(parser, &event, YAML_MAPPING_START_EVENT);
  do {
      gotId = false;
      gotName = false;
      gotNodes = 0;
      gotPorts = 0;
      if (event.type == YAML_MAPPING_START_EVENT)
      {
          err = get_event(parser, &event, YAML_SCALAR_EVENT);
          do {
              /*Parse edge contents*/
              if(strcmp((const char*)event.data.scalar.value, "node") == 0)
              {
                  if (gotNodes >= 2)
                  {
                      fprintf(stderr, "HWG_PARSE_EDGES: Only two nodes can be specified by an edge yet.\n");
                      return HWG_PARSE_ERR_TOO_MANY_NODES;
                  }
                  err = get_int(parser, &nodeId);
                  /*NOTE: We store the id in a ptr. This is a little bit ugly, but will be converted soon ...*/
                  edge.nodes[gotNodes] = hw_graph_get_node(g, nodeId);
                  gotNodes++;
              }
              else if(strcmp((const char*)event.data.scalar.value, "port") == 0)
              {
                  if (gotPorts >= 2)
                  {
                      fprintf(stderr, "HWG_PARSE_EDGES: Only two nodes can be specified by an edge yet.\n");
                      return HWG_PARSE_ERR_TOO_MANY_NODES;
                  }
                  err = get_int(parser, &edge.ports[gotPorts]);
                  gotPorts++;
              }
              else if(strcmp((const char*)event.data.scalar.value, "id") == 0)
              {
                  if (gotId)
                  {
                      fprintf(stderr, "HWG_PARSE_EDGES: Multiple \"id\" sections.\n");
                  }
                  err = get_int(parser, &edge.id);
                  gotId = true;
              }
              else if(strcmp((const char*)event.data.scalar.value, "name") == 0)
              {
                  if (gotName)
                  {
                      fprintf(stderr, "HWG_PARSE_EDGES: Multiple \"name\" sections.\n");
                  }
                  err = get_string(parser, edge.name);
                  gotName = true;
              }
              else
              {
                  skip_next_node(parser, &event);
              }

              /*Get next event*/
              yaml_event_delete(&event);
              if (!yaml_parser_parse(parser, &event))
                  err = HWG_PARSE_ERR_UNKNOWN;
          } while (err == HWG_PARSE_ERR_NONE && event.type != YAML_MAPPING_END_EVENT);

          /*Got new valid edge: Set entries and push to list*/
          if ((gotNodes == 2) && (gotPorts == 2) && gotId)
          {
              gerr = hw_graph_clone_edge(g, &edge);
              if (gerr != HW_GRAPH_ERR_NONE)
              {
                  fprintf(stderr, "HWG_PARSE_EDGES: Clone failed (%u)\n", (unsigned int)gerr);
                  err = HWG_PARSE_ERR_UNKNOWN;
              }
          } else {
              fprintf(stderr, "HWG_PARSE_EDGES: Wrong edge\n");
              err = HWG_PARSE_ERR_MISSING_ID;
          }
      }

      /*Get next event*/
      yaml_event_delete(&event);
      if (!yaml_parser_parse(parser, &event))
      {
          fprintf(stderr, "Parser error\n");
          err = HWG_PARSE_ERR_UNKNOWN;
      }
  } while (err == HWG_PARSE_ERR_NONE && event.type != YAML_SEQUENCE_END_EVENT);

  if (err != HWG_PARSE_ERR_NONE)
  {
      fprintf(stderr, "HWG_PARSE_EDGES: Error parsing edge %u\n", edge.id);
      return err;
  }
  yaml_event_delete(&event);

  return err;
}


/*static hwg_parse_error connect_nodes(hw_graph_t *g, priority_list_t *edge_list)*/
/*{*/
  /*hwg_parse_error err;*/
  /*priority_list_iterator_t it;*/
  /*hw_edge_t *edge;*/

  /*for(edge = priority_list_first(edge_list, &it);*/
      /*edge;*/
      /*edge = priority_list_next(&it))*/
  /*{*/
        /*err = hw_graph_create_edge(g, edge->id, edge->name,*/
                                   /*(unsigned int)edge->nodes[0], edge->ports[0],*/
                                   /*(unsigned int)edge->nodes[1], edge-ports[1]);*/
        /*if(err != HWG_GRAPH_ERR_NONE) {*/
              /*fprintf(stderr, "HWG_CONNECT_NODES: Creating edge failed (%u).\n", err);*/
        /*}*/
        /*free(edge);*/
  /*}*/

  /*priority_list_clear(edge_list);*/

  /*return err;*/
/*}*/

hwg_parse_error hw_graph_from_parser(yaml_parser_t *parser, hw_graph_t *g)
{
  hwg_parse_error err = HWG_PARSE_ERR_NONE;
  yaml_event_t event;
  bool gotId = false;
  bool gotName = false;
  bool gotSubgraphName = false;

  if(!yaml_parser_parse(parser, &event)) {
    fprintf(stderr, "HW_GRAPH_FROM_PARSER: YAML parser encountered an error.\n");
    return HWG_PARSE_ERR_UNKNOWN;
  }
  if(event.type == YAML_STREAM_START_EVENT) {
    yaml_event_delete(&event);
    if(!yaml_parser_parse(parser, &event)) {
        fprintf(stderr, "HW_GRAPH_FROM_PARSER: YAML parser encountered an error after STREAM_START.\n");
        return HWG_PARSE_ERR_UNKNOWN;
    }
    if(event.type == YAML_DOCUMENT_START_EVENT) {
      yaml_event_delete(&event);
      if ((err = get_event(parser, &event, YAML_MAPPING_START_EVENT)) != HWG_PARSE_ERR_NONE) {
          fprintf(stderr, "HW_GRAPH_FROM_PARSER: YAML parser encountered an error finding MAPPING_START.\n");
          return err;
      }
      yaml_event_delete(&event);
      if ((err = get_event(parser, &event, YAML_SCALAR_EVENT)) != HWG_PARSE_ERR_NONE) {
          fprintf(stderr, "HW_GRAPH_FROM_PARSER: YAML parser encountered an error finding SCALAR.\n");
          return err;
      }
      do {
          /*Handle event*/
          if (event.type == YAML_SCALAR_EVENT)
          {
                if(strcmp("nodes", (const char*)event.data.scalar.value) == 0) {
                    err = parse_nodes(parser, g);
                } else if(strcmp("edges",(const char*)event.data.scalar.value)==0) {
                    /* TODO: Postpone edge parsing. Move to a second pass of the parsing process*/
                    err = parse_edges(parser, g);
                }
                else if(strcmp((const char*)event.data.scalar.value, "id") == 0)
                {
                    if (gotId)
                    {
                        fprintf(stderr, "HWG_PARSE_EDGES: Multiple \"id\" sections.\n");
                    }
                    err = get_int(parser, &g->id);
                    gotId = true;
                }
                else if(strcmp((const char*)event.data.scalar.value, "name") == 0)
                {
                    if (gotName)
                    {
                        fprintf(stderr, "HWG_PARSE_EDGES: Multiple \"name\" sections.\n");
                    }
                    err = get_string(parser, g->name);
                    gotName = true;
                } 
                else if(strcmp((const char*)event.data.scalar.value, "subName") == 0)
                {
                    if(gotSubgraphName) {
                        fprintf(stderr, "HW_GRAPH_FROM_PARSER: Multiple \"subName\" sections.\n");
                    }
                    err = get_string(parser, g->subName);
                    gotSubgraphName = true;
                } else {
                    fprintf(stderr, "HW_GRAPH_FROM_PARSER: Scalar event %s unexpected\n", event.data.scalar.value);
                }
          }
          /*Get next event*/
          yaml_event_delete(&event);
          if (!yaml_parser_parse(parser, &event))
              err = HWG_PARSE_ERR_UNKNOWN;
      } while (err == HWG_PARSE_ERR_NONE && event.type != YAML_MAPPING_END_EVENT);
    }
  }

  return err;
}

hwg_parse_error hw_graph_from_yaml_file(const char *filename, hw_graph_t *g)
{
    hwg_parse_error err = HWG_PARSE_ERR_NONE;
  yaml_parser_t parser;
  FILE *fp = NULL;
  /*char full_path[HWG_MAX_STRING_LENGTH];*/

  /*getcwd(full_path, HWG_MAX_STRING_LENGTH);*/
  /*strncat(full_path, filename, HWG_MAX_STRING_LENGTH);*/

  yaml_parser_initialize(&parser);
  fp = fopen(filename, "r");
  if(!fp) {
    fprintf(stderr, "HW_GRAPH_FROM_YAML_FILE: Could not open file \"%s\".\n", filename);
    return HWG_PARSE_ERR_IO;
  }
  yaml_parser_set_input_file(&parser, fp);

  err = hw_graph_from_parser(&parser,g);

  yaml_parser_delete(&parser);
  fclose(fp);
  return err;
}


hwg_parse_error hw_graph_from_yaml_string(const unsigned char *buffer, const size_t size, hw_graph_t *g)
{
    hwg_parse_error err = HWG_PARSE_ERR_NONE;
  yaml_parser_t parser;
  yaml_parser_initialize(&parser);

  yaml_parser_set_input_string(&parser, buffer, size);

  err = hw_graph_from_parser(&parser,g);

  yaml_parser_delete(&parser);
  return err;
}
