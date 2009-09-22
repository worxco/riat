<?php
// $Id: riat_parent.autocomplete.inc,v 1.2 2009/07/27 19:39:04 eclipsegc Exp $

function riat_parent_child_delete_reparent_autocomplete($node, $string = '') {
  $chid = riat_chid_load($node->riat['chid']);
  $parent = (object) riat_parent_get($node);
  $query = db_query("
    SELECT n.nid, n.title
    FROM {node} n
    INNER JOIN {riat_node_relationship} rnr ON n.nid = rnr.nid
    WHERE LOWER(n.title) LIKE LOWER('%s%%')
    AND rnr.chid = %d
    AND rnr.rid = %d
    AND n.nid != %d
    ", $string, $chid->chid, $node->riat['rid'], $node->nid);
  while ($result = db_fetch_object($query)) {
    $matches[$result->title ." [nid:". $result->nid ."]"] = check_plain($result->title ." [nid:". $result->nid ."]");
  }

  print drupal_to_js($matches);
  exit();
}

function riat_parent_child_parent_autocomplete($node, $string = '') {
  $chid = riat_chid_load($node->riat['chid']);
  $parent = (object) riat_parent_get($node);
  $query = db_query("
    SELECT n.nid, n.title
    FROM {node} n
    INNER JOIN {riat_node_relationship} rnr ON n.nid = rnr.nid
    WHERE LOWER(n.title) LIKE LOWER('%s%%')
    AND rnr.chid = %d
    AND rnr.rid = %d
    AND n.nid != %d
    ", $string, $chid->pchid, $node->riat['rid'], $parent->id);
  while ($result = db_fetch_object($query)) {
    $matches[$result->title ." [nid:". $result->nid ."]"] = check_plain($result->title ." [nid:". $result->nid ."]");
  }

  print drupal_to_js($matches);
  exit();
}