#include "hwg.h"
#include "hwg_yaml.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

static int emit_ulong(yaml_emitter_t *emitter, unsigned long x) {
  yaml_event_t event;
  int ok=1;
  char buf[HWG_MAX_STRING_LENGTH];
  sprintf(buf, "%lu", x);
  ok &= yaml_scalar_event_initialize(&event, NULL, NULL, (yaml_char_t*)buf,
                                     -1, 1, 0, YAML_PLAIN_SCALAR_STYLE);
  ok &= yaml_emitter_emit(emitter, &event);
  return ok;
}

static int emit_str(yaml_emitter_t *emitter, const char *s) {
  yaml_event_t event;
  int ok=1;
  ok &= yaml_scalar_event_initialize(&event, NULL, NULL, (yaml_char_t*)s,
                                     -1, 1, 0, YAML_PLAIN_SCALAR_STYLE);
  ok &= yaml_emitter_emit(emitter, &event);
  return ok;
}

/*static int emit_double(yaml_emitter_t *emitter, double x) {*/
  /*yaml_event_t event;*/
  /*int ok=1;*/
  /*char buf[HWG_MAX_STRING_LENGTH];*/
  /*sprintf(buf, "%g", x);*/
  /*ok &= yaml_scalar_event_initialize(&event, NULL, NULL, (yaml_char_t*)buf,*/
                                     /*-1, 1, 0, YAML_PLAIN_SCALAR_STYLE);*/
  /*ok &= yaml_emitter_emit(emitter, &event);*/
  /*return ok;*/
/*}*/

static int emit_node_list(yaml_emitter_t *emitter, priority_list_t *node_list) {
  yaml_event_t event;
  hw_node_t *current_node;
  priority_list_iterator_t it;
  size_t i;
  int ok = 1;
  for(current_node = priority_list_first(node_list, &it);
      current_node; 
      current_node = priority_list_next(&it)) {
    ok &= yaml_mapping_start_event_initialize(&event, NULL, NULL, 0,
                                        YAML_BLOCK_MAPPING_STYLE);
    ok &= yaml_emitter_emit(emitter, &event);
    
    ok &= emit_str(emitter, "id");
    ok &= emit_ulong(emitter, current_node->id);
    ok &= emit_str(emitter, "type");
    ok &= emit_str(emitter, hw_node_type_str[current_node->type]);
    ok &= emit_str(emitter, "name");
    ok &= emit_str(emitter, current_node->name);
    ok &= emit_str(emitter, "ports");
    ok &= yaml_sequence_start_event_initialize(&event, NULL, NULL, 0,
                                               YAML_BLOCK_SEQUENCE_STYLE);
    ok &= yaml_emitter_emit(emitter, &event);
    for(i = 0; i < current_node->numPorts; ++i) {
      ok &= yaml_mapping_start_event_initialize(&event, NULL, NULL, 0,
                                                YAML_FLOW_MAPPING_STYLE);
      ok &= yaml_emitter_emit(emitter, &event);
      ok &= emit_str(emitter, "id");
      ok &= emit_ulong(emitter, current_node->ports[i].id);
      ok &= emit_str(emitter, "type");
      ok &= emit_str(emitter, hw_port_type_str[current_node->ports[i].type]);
      ok &= emit_str(emitter, "name");
      ok &= emit_str(emitter, current_node->ports[i].name);
      ok &= yaml_mapping_end_event_initialize(&event);
      ok &= yaml_emitter_emit(emitter, &event);
    }
    ok &= yaml_sequence_end_event_initialize(&event);
    ok &= yaml_emitter_emit(emitter, &event);
    ok &= yaml_mapping_end_event_initialize(&event);
    ok &= yaml_emitter_emit(emitter, &event);
  }
  return ok;
}

static int emit_edge_list(yaml_emitter_t *emitter, priority_list_t *edge_list) {
  yaml_event_t event;
  hw_edge_t *current_edge;
  priority_list_iterator_t it;
  int ok = 1;
  unsigned int i;

  for(current_edge = priority_list_first(edge_list, &it);
      current_edge;
      current_edge = priority_list_next(&it)) {

      if (current_edge->nodes[0] && current_edge->nodes[1])
      {
          ok &= yaml_mapping_start_event_initialize(&event, NULL, NULL, 0,
                                                    YAML_FLOW_MAPPING_STYLE);
          ok &= yaml_emitter_emit(emitter, &event);

          ok &= emit_str(emitter, "id");
          ok &= emit_ulong(emitter, current_edge->id);
          ok &= emit_str(emitter, "name");
          ok &= emit_str(emitter, current_edge->name);
          for (i = 0; i < 2; ++i)
          {
              ok &= emit_str(emitter, "node");
              ok &= emit_ulong(emitter, current_edge->nodes[i]->id);
              ok &= emit_str(emitter, "port");
              ok &= emit_ulong(emitter, current_edge->ports[i]);
          }

          ok &= yaml_mapping_end_event_initialize(&event);
          ok &= yaml_emitter_emit(emitter, &event);
      }

  }
  return ok;
}

hwg_parse_error hw_graph_to_yaml_file(const char *filename, const hw_graph_t *g) {
  yaml_emitter_t emitter;
  yaml_event_t event;
  int ok = 1;
  FILE *fp;
  hwg_parse_error err;

  fp = fopen(filename, "w");
  if(!fp) {
    return HWG_PARSE_ERR_IO;
  }
  ok &= yaml_emitter_initialize(&emitter);
  yaml_emitter_set_output_file(&emitter, fp);

  ok &= yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING);
  ok &= yaml_emitter_emit(&emitter, &event);
  ok &= yaml_document_start_event_initialize(&event, NULL, NULL, 0, 1);
  ok &= yaml_emitter_emit(&emitter, &event);

  err = hw_graph_to_emitter(&emitter, g);

  ok &= yaml_document_end_event_initialize(&event, 1);
  ok &= yaml_emitter_emit(&emitter, &event);
  ok &= yaml_stream_end_event_initialize(&event);
  ok &= yaml_emitter_emit(&emitter, &event);
  if(!ok) {
    fprintf(stderr, "YAML Error: %s\n", emitter.problem);
    err = HWG_PARSE_ERR_UNKNOWN;
  }

  yaml_emitter_delete(&emitter);
  fclose(fp);
  return err;
}

hwg_parse_error hw_graph_to_yaml_string(unsigned char *buffer, size_t buffer_size,
                                 const hw_graph_t *g, size_t *bytes_written) {
  hwg_parse_error err;
  yaml_emitter_t emitter;
  yaml_event_t event;
  int ok = 1;
  ok &= yaml_emitter_initialize(&emitter);
  yaml_emitter_set_output_string(&emitter, buffer, buffer_size, bytes_written);

  ok &= yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING);
  ok &= yaml_emitter_emit(&emitter, &event);
  ok &= yaml_document_start_event_initialize(&event, NULL, NULL, 0, 1);
  ok &= yaml_emitter_emit(&emitter, &event);

  err = hw_graph_to_emitter(&emitter, g);

  ok &= yaml_document_end_event_initialize(&event, 1);
  ok &= yaml_emitter_emit(&emitter, &event);
  ok &= yaml_stream_end_event_initialize(&event);
  ok &= yaml_emitter_emit(&emitter, &event);
  if(!ok) {
    fprintf(stderr, "YAML Error: %s\n", emitter.problem);
    err = HWG_PARSE_ERR_UNKNOWN;
  }

  yaml_emitter_delete(&emitter);
  return err;
}

hwg_parse_error hw_graph_to_emitter(yaml_emitter_t *emitter, const hw_graph_t *g) {
    hwg_parse_error err = HWG_PARSE_ERR_NONE;
  yaml_event_t event;
  int ok = 1;
  ok &= yaml_mapping_start_event_initialize(&event, NULL, NULL, 0,
                                            YAML_BLOCK_MAPPING_STYLE);
  ok &= yaml_emitter_emit(emitter, &event);
  /* emit graph properties*/
  ok &= emit_str(emitter, "id");
  ok &= emit_ulong(emitter, g->id);
  ok &= emit_str(emitter, "name");
  ok &= emit_str(emitter, g->name);
  /* emit nodes */
  ok &= emit_str(emitter, "nodes");
  ok &= yaml_sequence_start_event_initialize(&event, NULL, NULL, 0,
                                             YAML_BLOCK_SEQUENCE_STYLE);
  ok &= yaml_emitter_emit(emitter, &event);
  ok &= emit_node_list(emitter, g->nodes);
  ok &= yaml_sequence_end_event_initialize(&event);
  ok &= yaml_emitter_emit(emitter, &event);
  /* emit edges */
  ok &= emit_str(emitter, "edges");
  ok &= yaml_sequence_start_event_initialize(&event, NULL, NULL, 0,
                                             YAML_BLOCK_SEQUENCE_STYLE);
  ok &= yaml_emitter_emit(emitter, &event);
  ok &= emit_edge_list(emitter, g->edges);
  ok &= yaml_sequence_end_event_initialize(&event);
  ok &= yaml_emitter_emit(emitter, &event);
  /* finalize */
  ok &= yaml_mapping_end_event_initialize(&event);
  ok &= yaml_emitter_emit(emitter, &event);
  return err;
}
