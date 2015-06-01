<?php
/**
 * @file
 * Functionality for outputing global data in json.
 */

/**
 * Custom access callback for endpoint.
 */
function _brook_mobile_api_access() {
  $access = user_access('access content');
  return $access;
}

/**
 * Custom callback for global_data resource.
 *
 * @param string $global_data
 *   The type of global data you want to grab.
 *
 * @return array
 *   Returns an array of data.
 */
function brook_mobile_api_sync_global_data($global_data) {
  switch ($global_data) {
    case 'punchlist_questions':
      $data = brook_mobile_api_sync_punch_questions();
      break;
  }

  return $data;
}

/**
 * Data for punchlist Qs for brook_mobile_api_sync_global_data().
 *
 * @return array
 *   Returns an array of data
 */
function brook_mobile_api_sync_punch_questions() {
  try {
    $data = array();
    $last_sync_date = $_GET['last_sync_date'];
    $pq_last_update = variable_get('brook_mobile_api_last_punchlist_update', 0);
    if (strtotime(str_replace('-', '/', $last_sync_date)) <= strtotime($pq_last_update)) {
      // $pq_list = variable_get('brook_mobile_api_punchlist_sync_fields', array());
      // $pq_list_arr = explode("\r\n", $pq_list);
      $pq_list_arr = field_info_instances('node', 'orientation_punchlist');
      $fg_form = field_group_info_groups('node', 'orientation_punchlist');
      $field_groups = $fg_form['form'];
      $key = 0;

      // Set field data.
      foreach ($pq_list_arr as $field_name => $field) {
        if ($field_name == 'field_punchlist_customer_sig') {
          continue;
        }

        $field_def = field_info_field($field_name);

        $data[$key]['field_machine_name'] = $field_name;
        $data[$key]['title'] = $field['label'];

        // Get field group data.
        foreach ($field_groups as $group_name => $group) {
          if (in_array($field_name, $group->children)) {
            $data[$key]['field_group'] = $group_name;
            break;
          }
        }
        $data[$key]['question_description'] = strip_tags($field['description']);
        $data[$key]['field_id'] = $field['field_id'];
        $data[$key]['type'] = $field_def['type'];
        $allowed_vals = list_allowed_values($field_def);
        if (!empty($allowed_vals)) {
          foreach ($allowed_vals as $value) {
            if ($value === 'Agree' || $value === 'Yes') {
              $data[$key]['yesValue'] = $value;
            }
            else if ($value === 'Disagree' || $value === 'No') {
              $data[$key]['noValue'] = $value;
            }
          }
        }

        $key++;
      }

      $keys = array(
        'field_punchlist_enter_permission',
        'field_punchlist_welcome_date',
        'field_punchlist_movein_date',
        'field_punchlist_electric_meter',
        'field_punchlist_gas_meter',
        'field_punchlist_gas_meter',
        'field_verify_customer_phone',
        'field_verify_customer_email',
        'field_punchlist_attendees',
        'field_punchlist_submit_name',
        'field_punchlist_contact',
        'field_punchlist_contact_number',
      );

      $order = array_combine($keys, $keys);
      foreach ($data as $key => $info) {
        if (in_array($info['field_machine_name'], $keys)) {
          $order[$info['field_machine_name']] = $info;
          unset($data[$key]);
        }
      }

      $data = array_merge($data, array_values($order));
    }
    // We have to put this into an array to match how the rest of the drupal
    // services work and to make it consistent within the app when pulling data.

    return $data;
  }
  catch (Exception $e) {
    return services_error($e->getMessage(), $e->getCode());
  }
}
