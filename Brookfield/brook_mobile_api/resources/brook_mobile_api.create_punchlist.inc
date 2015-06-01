<?php
/**
 * @file
 * Functionality for services creating a punchlist node.
 */

/**
 * Function to handle creating punchlist node from POST data.
 */
function _brook_mobile_api_create_punchlist($vals) {
  if (!empty($vals)) {
    try {
      global $user;
      $vals = array_filter($vals);

      $fields = field_info_instances('node', 'orientation_punchlist');

      foreach ($fields as $name => $field) {
        if ($field['required'] && empty($vals[$name])) {
          $not_set[] = $name;
        }
      }

      if (!empty($not_set)) {
        $missing_fields = implode(', ', $not_set);
        services_error('Missing required values: ' . $missing_fields, 400, 1);
      }

      $node = entity_create('node', array('type' => 'orientation_punchlist'));

      $nwrapper = entity_metadata_wrapper('node', $node);
      $nwrapper->author->set($user->uid);

      foreach ($vals as $key => $val) {
        if ($key == 'field_punchlist_customer_sig_mob') {
          $filename = 'customer_sig_' . $vals['field_lot_reference'] . '_' . $vals['field_customer_reference'] . '.png';
          $file = array(
            'file' => $val,
            'filename' => $filename,
            'filepath' => 'public://' . $filename,
          );
          module_load_include('inc', 'service', 'file_resource');
          $file_data = _file_resource_create($file);
          if (!empty($file_data['fid'])) {
            $file = file_load($file_data['fid']);
            $nwrapper->{$key}->file->set($file);
          }
        }
        else {
          $nwrapper->{$key}->set($val);
        }
      }

      node_save($node);

      return array('nid' => $node->nid);
    }
    catch (Exception $e) {
      return services_error($e->getMessage(), $e->getCode());
    }
  }
}
