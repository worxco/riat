<?php
// $Id: riat_parent.batch.inc,v 1.2 2009/07/26 19:12:45 eclipsegc Exp $

function _riat_parent_cascading_delete_batch($descendant) {
  node_delete($descendant->did);
}

function _riat_parent_cascading_delete_batch_finished($success, $results, $operations) {
  if (!$success) {
    if (count($results['bad'])) {
      drupal_set_message(t('There was a problem deleting nodes: !bad-list', array('!bad-list' => theme('item_list', $results['bad']))), 'error');
    }
    else {
      drupal_set_message(t('There was a problem deleting the nodes.'), 'error');
    }
  }
  if (count($results['good'])) {
    drupal_set_message(t('Successfully deleted: !good-list', array('!good-list' => theme('item_list', $results['good']))));
  }
  watchdog('riat_parent', 'Completed node deletion.');
}

function _riat_parent_cascading_fix_ancestors_batch($rid, $chid, $nid, $aid, $op = 'insert') {
  switch ($op) {
    case 'insert':
      db_query("INSERT into {riat_relationships_descendants} (rid, chid, nid, did) VALUES (%d, %d, %d, %d)", $rid, $chid, $aid, $nid);
      break;
    case 'delete':
      db_query("DELETE FROM {riat_relationships_descendants} WHERE nid = %d and did = %d", $aid, $nid);
      break;
  }
}

function _riat_parent_cascading_reparent_batch_finished($success, $results, $operations) {
  if (!$success) {
    if (count($results['bad'])) {
      drupal_set_message(t('There was a problem reparenting nodes: !bad-list', array('!bad-list' => theme('item_list', $results['bad']))), 'error');
    }
    else {
      drupal_set_message(t('There was a problem reparenting the nodes.'), 'error');
    }
  }
  if (count($results['good'])) {
    drupal_set_message(t('Successfully reparented: !good-list', array('!good-list' => theme('item_list', $results['good']))));
  }
  watchdog('riat_parent', 'Completed node reparenting.');
}