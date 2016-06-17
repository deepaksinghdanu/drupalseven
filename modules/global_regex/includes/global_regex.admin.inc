<?php

/**
 * @file
 * Include file for Global Regex module.
 *
 * Contains functionality related to admin forms.
 */

/**
 * Form callback for adding global regex.
 */
function global_regex_add_form($form, &$fom_state, $regex_type = NULL) {
  $locked = $regex_type['machine_name'] != NULL ? 1 : 0;

  $form['name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => isset($regex_type['name']) ? $regex_type['name'] : '',
    '#description' => t('The human readable name of this regex type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => $regex_type['machine_name'] != NULL ? $regex_type['machine_name'] : '',
    '#maxlength' => 32,
    '#disabled' => $locked,
    '#machine_name' => array(
      'exists' => 'global_regex_type_load',
    ),
    '#description' => t('The human readable name of this regex type.'),
  );
  $form['regex'] = array(
    '#title' => t('Regex'),
    '#type' => 'textfield',
    '#default_value' => isset($regex_type['regex']) ? $regex_type['regex'] : '',
    '#required' => TRUE,
    '#size' => 30,
  );
  $form['regex_type'] = array(
    '#type' => 'value',
    '#value' => $regex_type,
  );
  $form['error_message'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom error message'),
    '#description' => t("Specify an error message that should be displayed when user input doesn't pass validation"),
    '#required' => TRUE,
    '#size' => 60,
    '#maxlength' => 255,
    '#default_value' => isset($regex_type['error_message']) ? $regex_type['error_message'] : '',
  );

  $output = '<p>' . t('The following tokens are available for error message.') . '</p>';
  $token_types = global_regex_token_types();
  if (!empty($token_types)) {
    $items = array();
    foreach ($token_types as $key => $value) {
      $items[] = $key . ' == ' . $value;
    }
    $output .= theme('item_list', array(
      'items' => $items,
    ));
  }

  $form['token_help'] = array(
    '#type' => 'fieldset',
    '#title' => t('Replacement patterns'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#value' => $output,
    '#id' => 'error-message-token-help',
    '#prefix' => '<div>',
    '#suffix' => '</div>',
  );

  $form['status'] = array(
    '#title' => t('Active'),
    '#type' => 'checkbox',
    '#default_value' => isset($regex_type['status']) ? $regex_type['status'] : 0,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save regex type'),
    '#weight' => 40,
  );
  $form['actions']['cancel'] = array(
    '#type' => 'markup',
    '#markup' => l(t('Cancel'), 'admin/config/system/global-regex'),
    '#weight' => 42,
  );
  if ($regex_type['machine_name'] != NULL) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete regex type'),
      '#weight' => 41,
    );
  }
  return $form;
}

/**
 * Provide token token types for error message.
 */
function global_regex_token_types() {
  return array(
    '[entity-type]' => t('Machine name of entity type'),
    '[bundle]' => t('Machine name of bundle'),
    '[field-name]' => t('User readable name of current field'),
    '[value]' => t('Current value to be validated on'),
  );
}

/**
 * Form validation handler for global_regex_add_form().
 *
 * @see global_regex_add_form_submit()
 */
function global_regex_add_form_validate($form, &$form_state) {
  if (empty($form_state['values']['regex_type'])) {
    // '0' is invalid, since elsewhere we check it using empty().
    if ($form_state['values']['type'] == '0') {
      form_set_error('type', t("Invalid machine-readable name. Enter a name other than 0."));
    }
    if (global_regex_type_exists($form_state['values']['type'])) {
      form_set_error('type', t('The machine-readable name %name is already taken.', array('%name' => $form_state['values']['type'])));
    }
  }
}

/**
 * Function for checking duplicate machine readable name of global regex type.
 */
function global_regex_type_exists($machine_name) {
  $query = db_select('global_regex')
      ->condition('machine_name', $machine_name)
      ->fields('global_regex', array('id'));
  $result = $query->execute()->fetchAssoc();
  return empty($result['id']) ? FALSE : TRUE;
}

/**
 * Form submission handler for global_regex_add_form().
 *
 * @see global_regex_add_form_validate()
 */
function global_regex_add_form_submit($form, &$form_state) {
  $data = array(
    'id' => isset($form_state['values']['regex_type']['id']) ? $form_state['values']['regex_type']['id'] : '',
    'machine_name' => $form_state['values']['type'],
    'name' => trim($form_state['values']['name']),
    'regex' => $form_state['values']['regex'],
    'status' => $form_state['values']['status'],
    'error_message' => $form_state['values']['error_message'],
  );
  if (empty($form_state['values']['regex_type'])) {
    $data = drupal_write_record('global_regex', $data);
    drupal_set_message(t('Global regex type has been created.'), 'status');
  }
  else {
    $op = isset($form_state['values']['op']) ? $form_state['values']['op'] : '';
    if ($op == t('Delete regex type')) {
      $form_state['redirect'] = 'global-regex/' . $form_state['values']['type'] . '/delete';
      return;
    }
    $data = drupal_write_record('global_regex', $data, 'id');
    drupal_set_message(t('Global regex type has been updated.'), 'status');
  }
  $form_state['redirect'] = 'admin/config/system/global-regex';
}

/**
 * Callback function for deleting single global regex type.
 */
function global_regex_delete_confirm($form, &$form_state, $machine_name) {
  $form['type'] = array('#type' => 'value', '#value' => $machine_name);
  $message = t('Are you sure you want to delete the global regex type %type?', array('%type' => $machine_name));
  $caption = t('This action cannot be undone.');
  return confirm_form($form, $message, 'admin/config/system/global-regex', $caption, t('Delete'));
}

/**
 * Process global regex type delete confirm submissions.
 *
 * @see global_regex_delete_confirm()
 */
function global_regex_delete_confirm_submit($form, &$form_state) {
  db_delete('global_regex')
      ->condition('machine_name', $form_state['values']['type'])
      ->execute();
  db_delete('global_regex_fields')
      ->condition('regex_machine_name', $form_state['values']['type'])
      ->execute();
  $form_state['redirect'] = 'admin/config/system/global-regex';
}

/**
 * Callback function for listing of global regex.
 */
function global_regex_types() {
  $build = array();
  $header = array(
    array('data' => t('ID'), 'field' => 'id', 'sort' => 'asc'),
    array('data' => t('Name'), 'field' => 'name', 'sort' => 'asc'),
    array('data' => t('Status'), 'field' => 'status', 'sort' => 'asc'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
  $global_regex_types = db_select('global_regex')
          ->fields('global_regex')
          ->extend('PagerDefault')
          ->extend('TableSort')
          ->orderByHeader($header)
          ->limit(10)
          ->execute()->fetchAll();
  $build['add_link'] = array(
    '#markup' => l(t('Add Global Regex'), 'add/global-regex'),
  );
  if (!empty($global_regex_types)) {
    $rows = array();

    foreach ($global_regex_types as $type) {
      $rows[] = array(
        $type->id,
        check_plain($type->name),
        $type->status,
        l(t('Edit'), 'global-regex/' . $type->machine_name . '/edit'),
        l(t('Delete'), 'global-regex/' . $type->machine_name . '/delete'),
      );
    }
    $build['table'] = array(
      '#markup' => theme('table', array('header' => $header, 'rows' => $rows)),
    );
    $build['pager'] = array(
      '#markup' => theme('pager'),
    );
  }
  else {
    $build['no_result'] = array(
      '#markup' => '<div>' . t('No results found.') . '</div>',
    );
  }
  return $build;
}
