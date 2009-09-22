<?php
// $Id: riat_menu.inc,v 1.3 2009/07/27 19:39:04 eclipsegc Exp $

function riat_menu_options() {
  $types = node_get_types();
  $form = array();
  $form['types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Hide Content Types'),
    '#tree' => TRUE,
  );
  foreach ($types as $type) {
    $form['types'][$type->type] = array(
      '#type' => 'checkbox',
      '#title' => $type->name,
      '#default_value' => variable_get('riat_hide_'. $type->type, 0),
      '#tree' => TRUE,
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );
  return $form;
}

function riat_menu_options_submit($form, &$form_state) {
  $values = (object) $form_state['values'];
  foreach ($values->types as $key => $value) {
    variable_set('riat_hide_'. $key, $value);
  }
  menu_rebuild();
}

function riat_menu_chid_admin($form_state, $chid) {
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
  $defaults = db_query("SELECT context FROM {riat_menu} WHERE chid = %d", $chid->chid);
  while($values = db_fetch_array($defaults)) {
    $default_values[] = $values['context'];
  }
  $form = array();
  $form['riat_chid'] = array(
    '#type' => 'value',
    '#value' => $chid,
  );
  $form[$chid->chid] = array(
    '#type' => 'checkboxes',
    '#title' => ucfirst($chid->type) .' menus',
    '#options' => array(
      'link' => t('Links'),
      'tab' => t('Menu Tabs'),
      'block' => t('Block'),
    ),
    '#default_value' => $default_values ? $default_values : array(),
    '#tree' => TRUE,
  );
  $form['submit'] = array(
    '#value' => t('Submit'),
    '#type' => 'submit',
  );
  $form['reset'] = array(
    '#value' => t('Reset'),
    '#type' => 'submit',
  );
  return $form;
}

function riat_menu_chid_admin_submit($form, &$form_state) {
  $chid = $form_state['values']['riat_chid']->chid;
  $full_chid = riat_chid_load($chid);
  $rid = riat_load($full_chid->rid);  
  foreach ($form_state['values'][$chid] as $key => $option) {
    if ($option) {
      riat_menu_save($chid, $option);
    }
    else {
      riat_menu_save($chid, $key, 'delete');
    }
  }
  menu_rebuild();
  $form_state['redirect'] = 'admin/build/relationships/'. $rid->context .'/chain/'. $rid->rid;
}

function riat_menu_save($chid, $context, $op = 'save') {
  $result = db_result(db_query("SELECT chid FROM {riat_menu} WHERE chid = %d AND context = '%s'", $chid, $context));
  if ($result && $op == 'delete') {
    db_query("DELETE FROM {riat_menu} WHERE chid = %d AND context = '%s'", $chid, $context);
  }
  elseif (!$result && $op == 'save') {
    db_query("INSERT INTO {riat_menu} (chid, context) VALUES (%d, '%s')", $chid, $context);
  }
}

function riat_menu_get_context($context) {
  $query = db_query("SELECT chid FROM {riat_menu} WHERE context = '%s'", $context);
  while ($item = db_fetch_array($query)) {
    $return[] = $item;
  }
  return $return;
}
