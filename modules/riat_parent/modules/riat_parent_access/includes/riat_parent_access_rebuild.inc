<?php
// $Id: riat_parent_access_rebuild.inc,v 1.1 2009/07/29 22:20:36 eclipsegc Exp $

function riat_parent_access_rebuild($batch_mode = FALSE) {
  db_query("DELETE FROM {node_access}");
  // Only recalculate if the site is using a node_access module.
  if (count(module_implements('node_grants'))) {
    if ($batch_mode) {
      $batch = array(
        'title' => t('Rebuilding content access permissions'),
        'operations' => array(),
        'finished' => '_node_access_rebuild_batch_finished',
        'file' => drupal_get_path('module', 'riat_parent_access') .'/includes/riat_parent_access_rebuild.inc',
      );
      // Select all non-riat nodes.
      $query = db_query("SELECT n.nid FROM {node} n LEFT JOIN {riat_node_relationship} rnr ON n.nid = rnr.nid WHERE rnr.nid IS NULL");
      while ($result = db_fetch_object($query)) {
        $batch['operations'][] = array('riat_parent_access_acquire_grants', array($result->nid));
      }
      $query = db_query("SELECT nid FROM {riat_parent_access_nid_settings} ORDER BY depth");
      while ($result = db_fetch_object($query)) {
        $batch['operations'][] = array('riat_parent_access_acquire_grants', array($result->nid));
      }
      batch_set($batch);
    }
    else {
      // If not in 'safe mode', increase the maximum execution time.
      if (!ini_get('safe_mode')) {
        set_time_limit(240);
      }
      $result = db_query("SELECT n.nid FROM {node} n LEFT JOIN {riat_node_relationship} rnr ON n.nid = rnr.nid WHERE rnr.nid IS NULL");
      while ($node = db_fetch_object($result)) {
        riat_parent_access_acquire_grants($nid);
      }
      $result = db_query("SELECT nid FROM {riat_parent_access_nid_settings} ORDER BY depth");
      while ($node = db_fetch_object($result)) {
        riat_parent_access_acquire_grants($nid);
      }
    }
  }
  else {
    // Not using any node_access modules. Add the default grant.
    db_query("INSERT INTO {node_access} VALUES (0, 0, 'all', 1, 0, 0)");
  }

  if (!isset($batch)) {
    drupal_set_message(t('Content permissions have been rebuilt.'));
    node_access_needs_rebuild(FALSE);
    cache_clear_all();
  }
}

function riat_parent_access_acquire_grants($nid, $reset = FALSE) {
  $node = node_load($nid, NULL, TRUE);
  // To preserve database integrity, only aquire grants if the node
  // loads successfully.
  if (!empty($node)) {
    if ($reset && $node->chid_access_settings_defaults == 'optional_inh' && $node->chid_access_settings == 'independent') {
      $node->chid_access_settings = 'inherit';
      node_save($node);
    }
    else {
      node_access_acquire_grants($node);
    }
  }
}