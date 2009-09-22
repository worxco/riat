<?php
// $Id: riat.pages.inc,v 1.1 2009/06/18 03:54:43 eclipsegc Exp $

/**
 * Core's node_add() function wrongly checks to see if we have the right to create this node type.
 * That is the menu system's job, this is a clone of that function modified slightly to allow
 * what we're attempting to do.
 */
function riat_node_add($type) {
  module_load_include('inc', 'node', 'node.pages');
  global $user;

  $types = node_get_types();
  $type = isset($type) ? str_replace('-', '_', $type) : NULL;
  // If a node type has been specified, validate its existence.
  if (isset($types[$type])) {
    // Initialize settings:
    $node = array('uid' => $user->uid, 'name' => (isset($user->name) ? $user->name : ''), 'type' => $type, 'language' => '');

    drupal_set_title(t('Create @name', array('@name' => $types[$type]->name)));
    $output = drupal_get_form($type .'_node_form', $node);
  }

  return $output;
}
