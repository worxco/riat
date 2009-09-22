<?php
// $Id: riat_handler_filter_relationship.inc,v 1.1 2009/06/18 03:54:43 eclipsegc Exp $

class riat_handler_filter_relationship extends views_handler_filter_in_operator {
  function get_value_options() {
    $this->value_title = t('Node relationship type');
    $relationships = riat_get_all('node');
    foreach ($relationships as $key => $relationship) {
      $options[$key] = $relationship['title'];
    }
    $this->value_options = $options;
  }
}
