<?php
//$Id: riat_parent_access.autocomplete.inc,v 1.2 2009/08/03 13:32:13 eclipsegc Exp $

function riat_parent_access_user_autocomplete($node = NULL, $string = '') {
  $rid = $node->rid ? $node->rid : $node->riat['rid'];
  $chid = $node->chid ? $node->chid : $node->riat['chid'];
  if ($chid) {
    $chid_settings = db_result(db_query("SELECT setting FROM {riat_parent_access_chid_settings} WHERE chid = %d", $chid));
  }
  if (is_null($node)) {
    $query = db_query("SELECT name, uid FROM {users} WHERE name LIKE LOWER('%s%%')", $string);
  }
  elseif ($rid && $node->rid_access_settings == 'private') {
    $query = db_query("SELECT name, uid FROM {users} WHERE name LIKE LOWER('%s%%')", $string);
  }
  elseif ($chid && $node->chid_access_settings == 'independent') {
    if ($chid_settings == 'optional_inh') {
      $access = riat_parent_access_get_access($node->riat_parent['id']);
      $gid = $access->gid;
      $query = db_query("
        SELECT u.name, u.uid
        FROM {users} u
        LEFT JOIN {riat_parent_access_users} rpau
        ON rpau.uid = u.uid
        WHERE u.name LIKE LOWER('%s%%')
        AND rpau.gid = %d
        ", $string, $gid);
    }
    elseif ($chid_settings == 'optional_ind') {
      $query = db_query("SELECT name, uid FROM {users} WHERE name LIKE LOWER('%s%%')", $string);
    }
  }
  while ($result = db_fetch_object($query)) {
    $matches[$result->name ." [uid:". $result->uid ."]"] = check_plain($result->name ." [uid:". $result->uid ."]");
  }

  print drupal_to_js($matches);
  exit();
}

function riat_parent_access_user_autocomplete_save() {
  $user = $_GET['user'];
  $uid = substr($user, (strpos($user, 'uid:') + 4), -1);
  $hash = $_GET['hash'];
  drupal_set_message('<pre>'. print_r($user, TRUE) .'</pre>');
  drupal_set_message('<pre>'. print_r($uid, TRUE) .'</pre>');
  if ($uid && $hash && db_query("INSERT INTO {riat_parent_access_staging_access} (uid, hash) VALUES (%d, '%s')", $uid, $hash)) {
    return TRUE;
  }
}

function riat_parent_access_users() {
  $hash = $_SESSION['riat_hash'];
  $query = db_query("SELECT u.name, u.uid FROM {users} u LEFT JOIN {riat_parent_access_staging_access} rpasa ON rpasa.uid = u.uid WHERE rpasa.hash = '%s'", $hash);
  $header = array('Username', '');
  while ($result = db_fetch_object($query)) {
    $rows[] = array(l($result->name, 'user/'. $result->uid), l('remove', 'riat/remove_user/'. $result->uid .'/'. $hash, array('attributes' => array('class' => 'riat_user_delete'))));
  }
  if ($rows) {
    print theme('table', $header, $rows);
  }
  exit();
}