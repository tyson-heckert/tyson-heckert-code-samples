<?php
/**
 * @file
 * Custom settings for salesforce integration.
 */

function brookfield_salesforce_system_settings_form($form, $form_state) {
  $form['brookfield_sf_earliest_record_pull'] = array(
    '#type' => 'textfield',
    '#title' => 'Earliest Created Date',
    '#default_value' => variable_get('brookfield_sf_earliest_record_pull', NULL),
    '#description' => t('Change the earliest date salesforce will look back to. Format: 2012-05-01T22:08:24Z'),
  );

  $form['salesforce_pull_settings_submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );

  return $form;
}

/**
 * Custom salesforce pull form to pull selected mappings without cron.
 */
function brookfield_salesforce_custom_pull_form($form, $form_state) {
  $form = array();

  $record_types = array();
  $record_last_sync_info = array();
  foreach (salesforce_mapping_get_mapped_objects() as $type) {
    $record_types[$type] = $type;
    $sf_last_sync = variable_get('salesforce_pull_last_sync_' . $type, NULL);

    $record_last_sync_info[$type] = date('m-d-Y\ T h:i:s A', $sf_last_sync);
  }
  $markup = '';
  foreach ($record_last_sync_info as $key => $value) {
    $markup .= '<strong>' . $key . ':</strong> ' . $value . '<br/>';
  }

  $form['last_pull_info'] = array(
    '#type' => 'item',
    '#description' => 'The info above show the last time the mapping was pulled by cron',
    '#title' => 'SF Last Pull Date',
    '#markup' => $markup,
  );

  $form['last_modified_date'] = array(
    '#type' => 'textfield',
    '#title' => 'Last Modified Date',
    '#description' => t('Only records modified after this date will be pulled. Format: 2012-05-01T22:08:24Z'),
    '#default_value' => gmdate('Y-m-d\TH:i:s\Z'),
  );

  $form['force_update'] = array(
    '#type' => 'checkbox',
    '#title' => 'Force update',
    '#description' => 'This will force all pull results to update their records regardless of their salesforce last updated date. If importing DCGs it will force a full pull of them all, not just recently updated ones. Usefull for first time imports before the cron queue can really get going.',
  );

  $form['empty_customer_reference'] = array(
    '#type' => 'checkbox',
    '#title' => t('Empty Customer Reference'),
    '#description' => t('This will pull only service requests with an empty customer reference in Drupal'),
    '#states' => array(
      'visible' => array(
        ':input[name="salesforce_pull_types[Service_Request__c]"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['complete_code_c'] = array(
    '#type' => 'checkbox',
    '#title' => t('Complete Code c'),
    '#description' => t('This will pull only service requests with a complete code equal to c'),
    '#states' => array(
      'visible' => array(
        ':input[name="salesforce_pull_types[Service_Request__c]"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['salesforce_pull_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Record types'),
    '#options' => $record_types,
  );

  $form['salesforce_pull_submit'] = array(
    '#type' => 'submit',
    '#value' => 'Pull all selected record types',
  );

  return $form;
}

function brookfield_salesforce_test_config_form($form, $form_state) {

  $form = array();

  $form['brookfield_sf_testconfig_markup'] = array(
    '#type' => 'item',
    '#title' => 'Salesforce Test Mode configuration',
    '#markup' => '<p>This form allows you to enable test mode and set up a single Salesforce Account to test with. If you enable test mode, the normal Salesforce sync functionality will be disabled and not push or pull updates will not work.</p>
    <p>These settings will effect the brookfield_salesforce_query_alter function by limiting the query to the account that you specfify below.</p>
    '
  );

  $form['brookfield_sf_enable_test_mode'] = array(
    '#type' => 'checkbox',
    '#title' => 'Enable SF Test Mode',
    '#default_value' => variable_get('brookfield_sf_enable_test_mode', 0),
  );

  $form['brookfield_sf_test_mode_field'] = array(
    '#type' => 'textfield',
    '#title' => 'Test SF Field Name',
    '#description' => 'The field name that you want to query agains while testing. <b>Example:</b> FAST ID (Customer__c) or Lot ID (Lot_c).',
    '#default_value' => variable_get('brookfield_sf_test_mode_field', NULL),
  );

  $form['brookfield_sf_test_mode_field_value'] = array(
    '#type' => 'textfield',
    '#title' => 'Test SF Field Value',
    '#description' => 'The corresponding value for the field that you are querying against for testing.',
    '#default_value' => variable_get('brookfield_sf_test_mode_field_value', NULL),
  );

  return system_settings_form($form);
}

/**
 * Custom Submit Handler for brookfield_salesforce_system_settings_form().
 */
function brookfield_salesforce_custom_pull_form_submit($form, &$form_state) {

  if (!empty($form_state['values']['force_update'])) {
    $_SESSION['brookfield']['salesforce_pull_force_update'] = 1;
  }

  $conds = array();
  $conds[] = array(
    'field' => 'LastModifiedDate',
    'value' => $form_state['values']['last_modified_date'],
    'operator' => '>',
  );

  $batch = array(
    'operations' => array(),
    'title' => t('Importing Records'),
    'finished' => 'brookfield_salesforce_pull_finished',
  );

  foreach ($form_state['values']['salesforce_pull_types'] as $key => $pull_type) {

    $test_mode_enabled = variable_get('brookfield_sf_enable_test_mode', 0);

    if($pull_type === 'DefectCodeGroup__c' && empty($test_mode_enabled)){
      $force_full = FALSE;

      if(!empty($form_state['values']['force_update'])){
        $force_full = TRUE;
      }

      brookfield_salesforce_dcg_import(TRUE, $force_full);
      return;
    }

    if ($pull_type === 'Service_Request__c' && empty($test_mode_enabled)) {
      if(!empty($form_state['values']['empty_customer_reference'])){

        $sql = 'SELECT smo.salesforce_id
          FROM {node} n
          LEFT JOIN {field_data_field_customer_reference} cr ON n.nid = cr.entity_id
          LEFT JOIN {salesforce_mapping_object} smo ON n.nid = smo.entity_id
          WHERE n.type = \'customer_service_request\'
          AND cr.field_customer_reference_target_id IS NULL';

        $result = db_query($sql);

        while ($record = $result->fetchAssoc()) {
          $sr_ids[] = $record['salesforce_id'];
        }

        $chunk_array = array_chunk($sr_ids, 5);
        $total = count($sr_ids);

        foreach ($chunk_array as $key => $id_array) {

          $current = $current + count($id_array);

          $conds = array();
          $conds[] = array(
            'field' => 'LastModifiedDate',
            'value' => $form_state['values']['last_modified_date'],
            'operator' => '>',
          );
          $conds[] = array(
            'field' => 'Id',
            'value' => "('" . implode("','", $id_array) . "')",
            'operator' => 'IN',
          );
          if (!empty($form_state['values']['complete_code_c'])) {
            $conds[] = array(
              'field' => 'Complete_Code__c',
              'value' => '\'c\'',
              'operator' => '=',
            );
          }
          $batch['operations'][] = array('brookfield_salesforce_service_request_query', array($pull_type, $conds, $current, $total));
        }

      }
      else{
        $users = brookfield_salesforce_get_users();
        $chunk_array = array_chunk($users, 5);

        $total = count($users);

        foreach ($chunk_array as $key => $user_array) {

          $current = $current + count($user_array);

          $conds = array();
          $conds[] = array(
            'field' => 'LastModifiedDate',
            'value' => $form_state['values']['last_modified_date'],
            'operator' => '>',
          );
          $conds[] = array(
            'field' => 'Customer__c',
            'value' => "('" . implode("','", $user_array) . "')",
            'operator' => 'IN',
          );
          if (!empty($form_state['values']['complete_code_c'])) {
            $conds[] = array(
              'field' => 'Complete_Code__c',
              'value' => '\'c\'',
              'operator' => '=',
            );
          }
          $batch['operations'][] = array('brookfield_salesforce_service_request_query', array($pull_type, $conds, $current, $total));
        }
      }

    }
    else if(!empty($pull_type)) {
      brookfield_salesforce_pull_records($pull_type, $conds, $batch);
    }

  }

  batch_set($batch);
}

function brookfield_salesforce_system_settings_form_submit($form, &$form_state) {
  variable_set('brookfield_sf_earliest_record_pull', $form_state['values']['brookfield_sf_earliest_record_pull']);
}
