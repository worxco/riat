<?php
// $Id: riat_parent_views.views_default.inc,v 1.3 2009/08/17 21:09:21 eclipsegc Exp $

/**
 * Implementation of hook_views_default_views().
 */
function riat_parent_views_views_default_views() {
  /*
   * Template view for parent/child relationships.
   */
  $query = db_query("SELECT chid, rid, view_name FROM {riat_parent_views}");
  while ($result = db_fetch_object($query)) {
    $chid = riat_chid_load($result->chid);
    if ($chid->pchid) {
      $pchid = riat_chid_load($chid->pchid);
      $parent = $pchid->type;
    }
    else {
      $parent = db_result(db_query("SELECT type FROM {riat_chain_definition} WHERE rid = %d", $result->rid));
    }
    $child = $chid->type;

    $view = new view;
    $view->name = $result->view_name;
    $view->description = 'Displays children '. $child .' nodes of the currently selected '. $parent .' node for chid '. $result->chid .' which is within rid '. $result->rid .'.';
    $view->tag = 'riat';
    $view->view_php = '';
    $view->base_table = 'node';
    $view->is_cacheable = FALSE;
    $view->api_version = 2;
    $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
    $handler = $view->new_display('default', 'Defaults', 'default');
    $handler->override_option('sorts', array(
      'created' => array(
        'order' => 'DESC',
        'granularity' => 'second',
        'id' => 'created',
        'table' => 'node',
        'field' => 'created',
        'relationship' => 'none',
      ),
    ));
    $handler->override_option('arguments', array(
      'aid' => array(
        'default_action' => 'default',
        'style_plugin' => 'default_summary',
        'style_options' => array(),
        'wildcard' => 'all',
        'wildcard_substitution' => 'All',
        'title' => '',
        'default_argument_type' => 'node',
        'default_argument' => '',
        'validate_type' => 'numeric',
        'validate_fail' => 'not found',
        'break_phrase' => 0,
        'not' => 0,
        'id' => 'aid',
        'table' => 'riat_node_relationship',
        'field' => 'aid',
        'validate_user_argument_type' => 'uid',
        'validate_user_roles' => array(
          '2' => 0,
        ),
        'relationship' => 'none',
        'default_options_div_prefix' => '',
        'default_argument_user' => 0,
        'default_argument_fixed' => '',
        'default_argument_php' => '',
        'validate_argument_node_type' => array(
          'page' => 0,
          'story' => 0,
        ),
        'validate_argument_node_access' => 0,
        'validate_argument_nid_type' => 'nid',
        'validate_argument_vocabulary' => array(),
        'validate_argument_type' => 'tid',
        'validate_argument_transform' => 0,
        'validate_user_restrict_roles' => 0,
        'validate_argument_php' => '',
      ),
    ));
    $handler->override_option('filters', array(
      'relationship' => array(
        'operator' => 'in',
        'value' => array(
          'child_node' => 'child_node',
        ),
        'group' => '0',
        'exposed' => FALSE,
        'expose' => array(
          'operator' => FALSE,
          'label' => '',
        ),
        'id' => 'relationship',
        'table' => 'riat_node_relationship',
        'field' => 'relationship',
        'relationship' => 'none',
      ),
      'type' => array(
        'operator' => 'in',
        'value' => array(
          $child => $child,
        ),
        'group' => '0',
        'exposed' => FALSE,
        'expose' => array(
          'operator' => FALSE,
          'label' => '',
        ),
        'id' => 'type',
        'table' => 'node',
        'field' => 'type',
        'relationship' => 'none',
      ),
    ));
    $handler->override_option('access', array(
      'type' => 'none',
    ));
    $handler->override_option('use_pager', '1');
    $handler->override_option('row_plugin', 'node');
    $handler->override_option('row_options', array(
      'relationship' => 'none',
      'build_mode' => 'teaser',
      'links' => 1,
      'comments' => 0,
    ));
    $views[$view->name] = $view;
  }

  return $views;
}