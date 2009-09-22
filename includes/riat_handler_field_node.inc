<?php
// $Id: riat_handler_field_node.inc,v 1.1 2009/06/18 03:54:43 eclipsegc Exp $

class riat_handler_field_node extends views_handler_field_node {
  /**
   * Render whatever the data is as a link to the node.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    $node = node_load($data);
    if (!empty($this->options['link_to_node'])) {
      return l($node->title, "node/" . $node->nid, array('html' => TRUE));
    }
    else {
      return $node->title;
    }
  }
}
