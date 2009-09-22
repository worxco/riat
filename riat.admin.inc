<?php
// $Id: riat.admin.inc,v 1.2 2009/07/29 22:20:36 eclipsegc Exp $

function riat_relationship_chain_admin_form($context) {
  $output = '';
  $header = array('Title', 'Description', '', '');
  $query = db_query("SELECT rid, title, description FROM {riat_chain_definition} WHERE context = '%s'", $context);
  while ($row = db_fetch_object($query)) {
    $rows[] = array($row->title, $row->description, l('alter', 'admin/build/relationships/'. $context .'/chain/'. $row->rid), l('edit', 'admin/build/relationships/'. $context .'/'. $row->rid .'/edit'), l('delete', 'admin/build/relationships/'. $context .'/'. $row->rid .'/delete'));
  }
  $output .= theme('table', $header, $rows);
  $rows = array();
  $types = node_get_types();
  $header = array('Base Type', '');
  foreach ($types as $type) {
    $rows[] = array($type->name, l('add new chain', "admin/build/relationships/$context/$type->type"));
  }
  $output .= theme('table', $header, $rows);
  return $output;
}

function riat_relationship_chain_define_form($form_state, $context, $object_type, $object = 'node') {
  $form['title'] = array(
    '#title' => t('Relationship Chain Name'),
    '#type' => 'textfield',
    '#required' => TRUE,
  );
  $form['description'] = array(
    '#title' => t('Relationships Chain Description'),
    '#type' => 'textarea',
  );
  $form['context'] = array(
    '#type' => 'value',
    '#value' => $context,
  );
  $form['object'] = array(
    '#type' => 'value',
    '#value' => $object,
  );
  if ($object == 'node' && $type = node_get_types('type', $object_type)) {
    $form['type'] = array(
      '#type' => 'value',
      '#value' => $type,
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

function riat_relationship_chain_define_form_submit(&$form, &$form_state) {
  $values = (object) $form_state['values'];
  $chain = new stdClass();
  $chain->type = $values->type->type ? $values->type->type : $values->type;
  $chain->title = $values->title;
  $chain->description = $values->description;
  $chain->context = $values->context;
  $chain->object = $values->object;
  if (!$values->rid) {
    drupal_write_record('riat_chain_definition', $chain);
    $form_state['chain'] = $chain;
  }
  else {
    $chain->rid = $values->rid;
    drupal_write_record('riat_chain_definition', $chain, 'rid');
    $form_state['chain'] = $chain;
  }
  if ($form_state['values']['form_id'] == 'riat_relationship_chain_edit_form') {
    $form_state['redirect'] = 'admin/build/relationships/'. $chain->context;
  }
  else {
    $form_state['redirect'] = 'admin/build/relationships/'. $chain->context .'/chain/'. $chain->rid;
  }
  menu_rebuild();
}

function riat_relationship_chain_build_form($form_state, $chain) {
  $breadcrumb = array(
    l(t('Home'), NULL),
    l(t('Administer'), 'admin'),
    l(t('Site Building'), 'admin/build'),
    l(t('Relationships'), 'admin/build/relationships'),
    l(t('Parents'), 'admin/build/relationships/'. $chain->context),
  );
  drupal_set_breadcrumb($breadcrumb);
  drupal_add_css(drupal_get_path('module', 'riat') .'/riat_admin.css');
  drupal_add_js(drupal_get_path('module', 'riat') .'/riat_admin.js');
  drupal_set_title($chain->title);
  $type = node_get_types('type', $chain->type);
  $relationships = riat_get_all(array('attachable' => TRUE));
  $start = array(array('parent'));
  $options = array();
  foreach ($relationships as $key => $relationship) {
    $options[] = array(
      'title' => 'Add '. $relationship['title'],
      'href' => $_GET['q'] ."/$key",
    );
  }
  $form['start_element'] = array(
    '#type' => 'markup',
    '#value' => '<div class="branch">'."<strong>Chain Starter:</strong> $type->name $chain->object type | ".'<span class="relationship_options"><a>Options</a><div class="options">'. theme('links', $options, array()) ."</div></span></div>",
  );
  riat_relationship_chain_build($form, 0, $chain->title);
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Sumbit'),
    '#prefix' => '<div style="clear:both;">'
  );
  return $form;
}

function riat_relationship_chain_build(&$form, $pchid = 0, $name = NULL) {
  if (!is_null($name)) {
    $items = riat_retrieve_chain($pchid, $name);
  }
  else {
    $items = riat_retrieve_chain($pchid);
  }
  if ($items) {
    $form[$pchid .'_open'] = array(
      '#type' => 'markup',
      '#value' => '<ul>',
    );
    $relationships = riat_get_all(array('attachable' => TRUE));
    foreach ($items as $item) {
      $options = array();
      $options[] = array(
        'title' => 'Edit',
        'href' => 'admin/build/relationships/chid/'. $item['chid'] .'/edit',
      );
      foreach ($relationships as $key => $relationship) {
        $options[] = array(
          'title' => 'Add '. $relationship['title'],
          'href' => $_GET['q'] ."/$key/". $item['chid'],
        );
      }
      $links = module_invoke_all('riat_chain_menu', $item);
      foreach($links as $link) {
        $options[] = $link;
      }
      $options[] = array(
        'title' => 'Menu/Links Settings',
        'href' => 'admin/build/relationships/chid/'. $item['chid'] .'/menu',
      );
      $options[] = array(
        'title' => 'Delete',
        'href' => 'admin/build/relationships/chid/'. $item['chid'] .'/delete',
      );
      $key = implode('_', $item);
      $type = node_get_types('type', $item['type']);
      $item = (object) $item;
      $form[$key] = array(
        '#type' => 'markup',
        '#value' => '<li><div class="leaf">'. $type->name .' node type '. $item->relationship .' | <span class="relationship_options"><a>Options</a><div class="options">'. theme('links', $options, array()) ."</div></span></div>",
      );
      riat_relationship_chain_build($form, $item->chid);
      $form[$key .'_close'] = array(
        '#type' => 'markup',
        '#value' => '</li>',
      );
    }
    $form[$pchid .'_close'] = array(
      '#type' => 'markup',
      '#value' => '</ul>',
    );
  }
}

function riat_relationship_chain_delete_form($form_state, $chain) {
  $breadcrumb = array(
    l(t('Home'), NULL),
    l(t('Administer'), 'admin'),
    l(t('Site Building'), 'admin/build'),
    l(t('Relationships'), 'admin/build/relationships'),
    l(t('Parents'), 'admin/build/relationships/'. $chain->context),
  );
  drupal_set_breadcrumb($breadcrumb);
  $form['rid'] = array(
    '#type' => 'value',
    '#value' => $chain->rid,
  );
  $form['context'] = array(
    '#type' => 'value',
    '#value' => $chain->context,
  );
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#prefix' => t('Are you sure you want to delete this relationship chain?'),
  );
  return $form;
}

function riat_relationship_chain_delete_form_submit($form, &$form_state) {
  db_query("DELETE FROM {riat_chain_definition} WHERE rid = %d", $form_state['values']['rid']);
  $query = db_query("SELECT chid FROM {riat_chain} WHERE rid = %d", $form_state['values']['rid']);
  while ($result = db_fetch_object($query)) {
    $form_state['chids'][] = $result->chid;
  }
  db_query("DELETE FROM {riat_chain} WHERE rid = %d", $form_state['values']['rid']);
  $form_state['redirect'] = 'admin/build/relationships/'. $form_state['values']['context'];
  menu_rebuild();
}

function riat_relationship_chain_edit_form($form_state, $chain) {
  $breadcrumb = array(
    l(t('Home'), NULL),
    l(t('Administer'), 'admin'),
    l(t('Site Building'), 'admin/build'),
    l(t('Relationships'), 'admin/build/relationships'),
    l(t('Parents'), 'admin/build/relationships/'. $chain->context),
  );
  drupal_set_breadcrumb($breadcrumb);
  $form['title'] = array(
    '#title' => t('Relationship Chain Name'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => $chain->title,
  );
  $form['description'] = array(
    '#title' => t('Relationships Chain Description'),
    '#type' => 'textarea',
    '#default_value' => $chain->description,
  );
  $form['rid'] = array(
    '#type' => 'value',
    '#value' => $chain->rid,
  );
  $form['context'] = array(
    '#type' => 'value',
    '#value' => $chain->context,
  );
  $form['object'] = array(
    '#type' => 'value',
    '#value' => $chain->object,
  );
  $form['type'] = array(
    '#type' => 'value',
    '#value' => $chain->type,
  );
  $form['rid'] = array(
    '#type' => 'value',
    '#value' => $chain->rid,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['#submit'][] = 'riat_relationship_chain_define_form_submit';
  return $form;
}

function riat_relationship_add_relationship_form($form_state, $chain, $relationship, $pchid = 0) {
  $breadcrumb = array(
    l(t('Home'), NULL),
    l(t('Administer'), 'admin'),
    l(t('Site Building'), 'admin/build'),
    l(t('Relationships'), 'admin/build/relationships'),
    l(t('Parents'), 'admin/build/relationships/'. $chain->context),
    l(t($chain->title), 'admin/build/relationships/'. $chain->context .'/chain/'. $chain->rid),
  );
  drupal_set_breadcrumb($breadcrumb);
  drupal_set_title($chain->title);
  $types = node_get_types();
  $options = array();
  foreach ($types as $type) {
    $options['node--'. $type->type] = $type->name;
  }
  $query = db_query("SELECT rid, title FROM {riat_chain_definition} WHERE context = '%s' AND rid != %d", $chain->context, $chain->rid);
  while ($row = db_fetch_object($query)) {
    $options['tree--'. $row->rid] = $row->title;
  }
  $form['choices'] = array(
    '#type' => 'select',
    '#title' => t('Choose an item to relate'),
    '#options' => $options,
  );
  $form['multiple'] = array(
    '#type' => 'radios',
    '#title' => t('Type of relationship'),
    '#options' => array(
      'one' => t('One to One'),
      'many' => t('One to Many'),
    ),
    '#default_value' => 'many',
  );
  $form['chain'] = array(
    '#type' => 'value',
    '#value' => $chain,
  );
  $form['pchid'] = array(
    '#type' => 'value',
    '#value' => $pchid,
  );
  $form['relationship'] = array(
    '#type' => 'value',
    '#value' => $relationship,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

function riat_relationship_add_relationship_form_validate($form, &$form_state) {
  $object_type = explode('--', $form_state['values']['choices']);
  $form_state['values']['choices'] = $object_type[1];
  if (is_numeric($form_state['values']['choices']) && $form_state['values']['choices'] == arg(5)) {
    form_set_error('choices', t('You may not select the chain you\'re currently in as a child'));
  }
  $form_state['values']['object'] = $object_type[0];
  if ($form_state['values']['multiple'] == 'many') {
    $form_state['values']['multiple'] = 1;
  }
  else {
    $form_state['values']['multiple'] = 0;
  }
}

function riat_relationship_add_relationship_form_submit($form, &$form_state) {
  $values = (object) $form_state['values'];
  $chain = new stdClass();
  $chain->pchid = $values->pchid;
  $chain->rid = $values->chain->rid;
  $chain->type = $values->choices;
  $chain->object = $values->object;
  $chain->relationship = $values->relationship;
  $chain->multiple = $values->multiple;
  drupal_write_record('riat_chain', $chain);
  $form_state['values']['chid'] = $chain;
  $form_state['redirect'] = 'admin/build/relationships/'. $values->chain->context .'/chain/'. $values->chain->rid;
  menu_rebuild();
}

function riat_relationship_chid_edit($form_state, $chid) {
  $rid = riat_load($chid->rid);
  $breadcrumb = array(
    l(t('Home'), NULL),
    l(t('Administer'), 'admin'),
    l(t('Site Building'), 'admin/build'),
    l(t('Relationships'), 'admin/build/relationships'),
    l(t('Parents'), 'admin/build/relationships/'. $rid->context),
    l(t($rid->title), 'admin/build/relationships/'. $rid->context .'/chain/'. $rid->rid),
  );
  drupal_set_breadcrumb($breadcrumb);
  if ($chid->pchid) {
    $pchid = riat_chid_load($chid->pchid);
    $relation = $pchid->type;
  }
  else {
    $relation = $rid->type;
  }
  $results[0] = $rid->type;
  $query = db_query("SELECT chid, type FROM {riat_chain} WHERE rid = %d", $chid->rid);
  while ($result = db_fetch_object($query)) {
    if ($result->chid != $chid->chid) {
      $results[$result->chid] = $result->type;
    }
  }
  $multiple = $chid->multiple ? 'many' : 'one';
  $form = array();
  $form['rid'] = array(
    '#type' => 'value',
    '#value' => $rid,
  );
  $form['details'] = array(
    '#type' => 'markup',
    '#value' => '<ul class="chid_details">
    <li><strong>Relationship Chain:</strong> '. $rid->title .'</li>
    <li><strong>Object Type:</strong> '. $chid->object .'</li>
    <li><strong>Object Parameter:</strong> '. $chid->type .'</li>
    <li><strong>Relationship:</strong> '. $chid->relationship .'</li>
    <li><strong>Relation:</strong> '. $relation .'</li>
    </ul>',
  );
  $form['parent'] = array(
    '#type' => 'select',
    '#title' => t('Parent'),
    '#options' => $results,
    '#default_value' => $chid->pchid,
  );
  $form['chid'] = array(
    '#type' => 'value',
    '#value' => $chid->chid,
  );
  $form['multiple'] = array(
    '#type' => 'radios',
    '#title' => t('Type of relationship'),
    '#options' => array(
      'one' => t('One to One'),
      'many' => t('One to Many'),
    ),
    '#default_value' => $multiple,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

function riat_relationship_chid_edit_validate($form, &$form_state) {
  if ($form_state['values']['multiple'] == 'many') {
    $form_state['values']['multiple'] = 1;
  }
  else {
    $form_state['values']['multiple'] = 0;
  }
}

function riat_relationship_chid_edit_submit($form, &$form_state) {
  $values = (object) $form_state['values'];
  $rid = $values->rid;
  switch ($values->op) {
    case 'Save':
      $object = new stdClass();
      $object->chid = $values->chid;
      $object->pchid = $values->parent;
      $object->multiple = $values->multiple;
      drupal_write_record('riat_chain', $object, 'chid');
      break;
  }
  $form_state['redirect'] = 'admin/build/relationships/'. $rid->context .'/chain/'. $rid->rid;
}

function riat_relationship_chid_delete($form_state, $chid) {
  $children = riat_pchid_load($chid);
  $item = riat_chid_load($chid);
  $rid = riat_load($item->rid);
  $breadcrumb = array(
    l(t('Home'), NULL),
    l(t('Administer'), 'admin'),
    l(t('Site Building'), 'admin/build'),
    l(t('Relationships'), 'admin/build/relationships'),
    l(t('Parents'), 'admin/build/relationships/'. $rid->context),
    l(t($rid->title), 'admin/build/relationships/'. $rid->context .'/chain/'. $rid->rid),
  );
  drupal_set_breadcrumb($breadcrumb);
  $form = array();
  $form['item'] = array(
    '#type' => 'value',
    '#value' => $item,
  );
  $form['chid'] = array(
    '#type' => 'value',
    '#value' => $chid,
  );
  if ($children) {
    $form['children'] = array(
      '#type' => 'value',
      '#value' => $children,
    );
    $form['description'] = array(
      '#type' => 'markup',
      '#value' => t('Would you like to delete all descendent references? or simply move them up a level?'),
      '#prefix' => '<div id="delete_description">',
      '#suffix' => '</div>',
    );
    $form['select'] = array(
      '#type' => 'radios',
      '#title' => t('Choose one'),
      '#options' => array(
        'delete' => 'Delete',
        'move' => 'Move',
      ),
      '#required' => TRUE,
    );
  }
  else {
    $form['description'] = array(
      '#type' => 'markup',
      '#value' => t('Are you sure you want to delete this item?'),
      '#prefix' => '<div id="delete_description">',
      '#suffix' => '</div>',
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Delete',
  );
  return $form;
}

function riat_relationship_chid_delete_submit($form, &$form_state) {
  $values = (object) $form_state['values'];
  $op = $values->select ? $values->select : 'delete';
  $rid = riat_load($values->item->rid);
  riat_chid_delete($values->chid, $op);
  $form_state['redirect'] = 'admin/build/relationships/'. $rid->context .'/chain/'. $rid->rid;
  menu_rebuild();
}

function riat_retrieve_chain($pchid = 0, $name = NULL) {
  if ($pchid == 0 && is_null($name)) {
    return;
  }
  elseif (!$name) {
    $rid = db_result(db_query("SELECT rid FROM {riat_chain} WHERE pchid = %d", $pchid));
    $name = db_result(db_query("SELECT title FROM {riat_chain_definition} WHERE rid = %d", $rid));
  }
  $query = db_query("SELECT rc.chid, rc.rid, rc.type, rc.object, rc.relationship, rc.weight, rc.multiple FROM {riat_chain} rc LEFT JOIN {riat_chain_definition} rcd ON rc.rid = rcd.rid WHERE rcd.title = '%s' AND rc.pchid = %d ORDER BY rc.weight", $name, $pchid);
  while ($result = db_fetch_array($query)) {
    $results[] = $result;
  }
  return $results;
}