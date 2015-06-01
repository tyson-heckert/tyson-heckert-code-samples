<?php

/**
 * @file
 * Admin page form and submit function file for metagenics_commerce_recurring
 * module.
 */

/**
 * Menu callback for recurring order settings form.
 *
 * @param array $form
 *   Form array
 *
 * @param array $form_state
 *   Form state array
 *
 * @return array
 *   The complete form array
 */
function metagenics_commerce_recurring_admin_settings_form($form, &$form_state) {
  $form = array();

  $form['metagenics_commerce_recurring_admin_email_disable'] = array(
    '#type' => 'checkbox',
    '#title' => 'Disable recurring order daily status emails',
    '#default_value' => variable_get('metagenics_commerce_recurring_admin_email_disable', NULL),
  );

  $form['metagenics_commerce_recurring_admin_email_to'] = array(
    '#type' => 'textfield',
    '#title' => 'Recurring order daily report recipients',
    '#description' => 'Enter email address separated by comma. These people will recieve the daily report on recurring orders',
    '#default_value' => variable_get('metagenics_commerce_recurring_admin_email_to', NULL),
  );

  $form['metagenics_commerce_recurring_admin_email_subject'] = array(
    '#type' => 'textfield',
    '#title' => 'Recurring order daily report email subject',
    '#default_value' => variable_get('metagenics_commerce_recurring_admin_email_subject', NULL),
  );

  $form['metagenics_commerce_recurring_admin_email_body'] = array(
    '#type' => 'textarea',
    '#title' => 'Recurring order daily report email body',
    '#description' => 'Recommended tokens are [metagenics:recurring_order_daily_report_success] and [metagenics:recurring_order_daily_report_fail]',
    '#default_value' => variable_get('metagenics_commerce_recurring_admin_email_body', NULL),
  );

  $form['token_list'] = array(
    '#theme' => 'token_tree',
    '#dialog' => TRUE,
  );

  // Returning a system settings form is nice because the submit handler will
  // save all these form fields as variables for you.
  return system_settings_form($form);
}


/**
 * Custom Form for the ARO queue table.
 *
 * This allows users to search for orders that should go out on a specific date,
 * it will add those ARO templates to the queue, then will allow users to select
 * which ones they want to process.
 */
function metagenics_commerce_recurring_aro_queue_form($form, &$form_state) {
  $form = array();

  // Confirmation page check.
  if (empty($form_state['storage']['confirm'])) {

    // Define headers for tableselect.
    $header = array(
      'qid' => t('Queue ID'),
      'template_nid' => t('Template ID'),
      'processing_status' => t('Processing status'),
      'template_next_order_date' => t('Next Ship date in Template'),
      'pending_order_date' => t('Pending Ship Date'),
      'order_date' => t('Actual Ship date'),
      'frequency' => t('Frequency'),
    );

    $query = db_select('metagenics_commerce_recurring_process_queue', 'm')->extend('PagerDefault');
    $query->fields('m');

    // Add conditions for our filters.
    // Really interesting way to do an OR query here.
    if (!empty($_GET['status_filter'])) {
      $db_or = db_or();
      foreach ($_GET['status_filter'] as $status_filter) {
        $db_or->condition('processing_status', $status_filter, '=');
      }
      $query->condition($db_or);
    }

    if (!empty($_GET['template_id'])) {
      $query->condition('template_nid', $_GET['template_id'], '=');
    }

    if (!empty($_GET['next_ship_date_start'])) {
      $query->condition('template_next_order_date', strtotime($_GET['next_ship_date_start']), '>=');
    }

    if (!empty($_GET['next_ship_date_end'])) {
      $query->condition('template_next_order_date', strtotime($_GET['next_ship_date_end']), '<=');
    }

    if (!empty($_GET['pending_ship_date_start'])) {
      $query->condition('pending_order_date', strtotime($_GET['pending_ship_date_start']), '>=');
    }

    if (!empty($_GET['pending_ship_date_end'])) {
      $query->condition('pending_order_date', strtotime($_GET['pending_ship_date_end']), '<=');
    }

    if (!empty($_GET['frequency'])) {
      $db_or = db_or();
      foreach ($_GET['frequency'] as $frequency) {
        $db_or->condition('frequency', $frequency, '=');
      }
      $query->condition($db_or);
    }

    $query->limit(50);
    $query->extend('TableSort')->orderByHeader($header);
    $result = $query->execute();

    $options = array();
    foreach ($result as $record) {

      // Here order date is the date the order was actually created.
      $order_date = $record->order_date == 0 ? '' : date('m-d-Y h:i A', $record->order_date);
      $order_data = unserialize($record->data);

      $order_info = '';
      if (!empty($order_data['order_id'])) {
        $order_info = 'Order ID: ' . l($order_data['order_id'], 'admin/commerce/orders/' . $order_data['order_id']);
      }
      if (!empty($order_data['errors'])) {
        $order_info = 'Failed: ' . l($order_data['order']->order_id, 'admin/commerce/orders/' . $order_data['order']->order_id) . ' <pre>' . print_r($order_data['errors'], TRUE) . '</pre>';
      }
      $aro_edit_link = l($record->template_nid, 'admin/metagenics/aro-action-links/' . $record->template_nid . '/aro-edit-page');
      // Options list for our tableselect.
      $options[$record->qid] = array(
        'qid' => $record->qid,
        'template_nid' => $aro_edit_link,
        'processing_status' => $record->processing_status,
        'template_next_order_date' => date('m-d-Y', $record->template_next_order_date),
        'pending_order_date' => date('m-d-Y', $record->pending_order_date),
        'order_date' => $order_date . ' ' . $order_info,
        'frequency' => $record->frequency,
      );

      $form_state['options'] = $options;
    }

    $form['#attached']['css'][] = drupal_get_path('module', 'metagenics_commerce_recurring') . '/css/metagenics_commerce_recurring.css';

    $aro_processing_status = variable_get('metagenics_commerce_recurring_orders_enabled', ARO_OFF);
    if ($aro_processing_status == ARO_OFF) {
      $aro_processing_info  = '<strong>ARO Processing is OFF (0).</strong>';
    }
    if ($aro_processing_status == ARO_QUEUE_ONLY) {
      $aro_processing_info  = '<strong>ARO Processing is in TEST Mode (1).</strong> This only adds items to the Meta ARO Queue, but the orders will not be processed.';
    }

    if ($aro_processing_status == ARO_QUEUE_AND_PROCESS) {
      $aro_processing_info = '<strong>ARO Processing is ON (2).</strong> You should change this if this is a dev server and you are not actively testing!';
    }

    $form['aro_processing_status'] = array(
      '#markup' => $aro_processing_info . '<br/> To change these settings you will need to modify <strong>$conf[\'metagenics_commerce_recurring_orders_enabled\']</strong> in settings.php',
    );

    $status_list = array(
      'Pending' => 'Pending',
      'In Process Queue' => 'In Process Queue',
      'Processing' => 'Processing',
      'Completed' => 'Completed',
      'Failed' => 'Failed',
    );

    $frequency_options = array(
      1 => '30 Days',
      2 => '60 Days',
      3 => '90 Days',
    );

    $form['filter_queue'] = array(
      '#type' => 'fieldset',
      '#title' => t('Filter queued orders'),
      '#collapsible' => TRUE,
    );

    $form['filter_queue']['status_filter'] = array(
      '#type' => 'select',
      '#title' => 'Processing status',
      '#options' => $status_list,
      '#multiple' => TRUE,
      '#default_value' => $_GET['status_filter'],
    );

    $form['filter_queue']['frequency'] = array(
      '#type' => 'select',
      '#title' => 'Frequency',
      '#options' => $frequency_options,
      '#multiple' => TRUE,
      '#default_value' => $_GET['frequency'],
    );

    $form['filter_queue']['template_id'] = array(
      '#type' => 'textfield',
      '#title' => 'Template ID',
      '#default_value' => $_GET['template_id'],
    );

    $form['filter_queue']['date_filters'] = array(
      '#type' => 'fieldset',
    );

    $form['filter_queue']['date_filters']['next_ship_date'] = array(
      '#type' => 'fieldset',
      '#title' => t('Next ship date'),
    );

    $form['filter_queue']['date_filters']['next_ship_date']['next_ship_date_start'] = array(
      '#type' => 'date_popup',
      '#date_format' => 'm/d/Y',
      '#title' => t('Start'),
      '#default_value' => $_GET['next_ship_date_start'],
    );

    $form['filter_queue']['date_filters']['next_ship_date']['next_ship_date_end'] = array(
      '#type' => 'date_popup',
      '#date_format' => 'm/d/Y',
      '#title' => t('End'),
      '#default_value' => $_GET['next_ship_date_end'],
    );

    $form['filter_queue']['date_filters']['pending_ship_date'] = array(
      '#type' => 'fieldset',
      '#title' => t('Pending ship date'),
    );

    $form['filter_queue']['date_filters']['pending_ship_date']['pending_ship_date_start'] = array(
      '#type' => 'date_popup',
      '#date_format' => 'm/d/Y',
      '#title' => t('Start'),
      '#default_value' => $_GET['pending_ship_date_start'],
    );

    $form['filter_queue']['date_filters']['pending_ship_date']['pending_ship_date_end'] = array(
      '#type' => 'date_popup',
      '#date_format' => 'm/d/Y',
      '#title' => t('End'),
      '#default_value' => $_GET['pending_ship_date_end'],
    );

    $form['filter_queue']['filter_submit'] = array(
      '#type' => 'submit',
      '#value' => 'Filter queue',
      '#submit' => array('metagenics_commerce_recurring_aro_queue_process_orders_form_filter_submit'),
    );

    $form['filter_queue']['filter_reset'] = array(
      '#type' => 'submit',
      '#value' => 'Reset',
      '#submit' => array('metagenics_commerce_recurring_aro_queue_process_orders_form_filter_reset'),
    );

    // Define fieldset for date add to queue.
    $form['add_to_queue'] = array(
      '#type' => 'fieldset',
      '#title' => t('Manually add ARO orders to queue'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['add_to_queue']['by_ship_date'] = array(
      '#type' => 'fieldset',
      '#title' => t('Add to queue by ARO Next Ship Date'),
      '#collapsible' => TRUE,
      '#description' => t('THIS IS NOT A FILTER! The date you select below will add any ARO templates that have this date as the Next Ship Date. This is usefull for adding groups of ARO templates to the queue at once.'),
    );
    // Add to queue ship date textfield, should probably convert to datepicker
    // at some point.
    $form['add_to_queue']['by_ship_date']['add_to_queue_ship_date'] = array(
      '#type' => 'textfield',
      '#title' => t('Add by next Ship date'),
      '#description' => t('Enter a date in the format mm/dd/yyyy and all orders with that ship date will be added to the processing queue. Any orders with a matching next ship date will be added to the queue.'),
    );

    // Add to queue by date custom submit/validation button.
    $form['add_to_queue']['by_ship_date']['add_to_queue_submit'] = array(
      '#type' => 'submit',
      '#value' => 'Submit',
      '#submit' => array('metagenics_commerce_recurring_add_to_queue_submit'),
      '#validate' => array('metagenics_commerce_recurring_add_to_queue_validate'),
    );

    $form['add_to_queue']['by_aro_id'] = array(
      '#type' => 'fieldset',
      '#title' => t('Add to queue by ARO Template ID'),
      '#collapsible' => TRUE,
      '#description' => t('Enter a list of ARO template IDs (on per line) and the date they should be processed.'),
    );
    // Add to queue by ID textarea allows one or many template node ID's to be
    // submitted and manually added to our custom queue table.
    $form['add_to_queue']['by_aro_id']['add_to_queue_id_text'] = array(
      '#type' => 'textarea',
      '#title' => t('Add by template ID'),
      '#description' => t("Enter a template ID or list of template ID's one per line."),
    );

    // Add to queue by ID custom process date field allows the orders added to
    // queue to be processed on any date.
    $form['add_to_queue']['by_aro_id']['add_to_queue_id_ship_date'] = array(
      '#type' => 'date_popup',
      '#date_format' => 'm/d/Y',
      '#title' => t('Ship date'),
      '#description' => t('Enter a date for these orders to ship on.'),
    );

    // Add to queueue by ID custom submit/validation button.
    $form['add_to_queue']['by_aro_id']['add_to_queue_id_submit'] = array(
      '#type' => 'submit',
      '#value' => 'Add by ID',
      '#submit' => array('metagenics_commerce_recurring_add_to_queue_id_submit'),
      '#validate' => array('metagenics_commerce_recurring_add_to_queue_id_validate'),
    );

    // Our tableselect.
    $form['queue_table'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
      '#empty' => t('No orders are queued.'),
      '#js_select' => TRUE,
    );

    $form['pager'] = array('#markup' => theme('pager'));

    // Custom submit to force process of orders chosen with the tableselect.
    $form['process_orders_submit'] = array(
      '#type' => 'submit',
      '#value' => 'Process Selected Orders Now',
      '#submit' => array('metagenics_commerce_recurring_aro_queue_process_orders_form_submit'),
    );

    // Custom submit to force delete of orders chosen with the tableselecct.
    $form['remove_from_queue_submit'] = array(
      '#type' => 'submit',
      '#value' => 'Remove selected items from queue',
      '#submit' => array('metagenics_commerce_recurring_aro_remove_queue_form_submit'),
    );

    $form['export_queue_records'] = array(
      '#type' => 'submit',
      '#value' => 'Export',
      '#submit' => array('metagenics_commerce_recurring_queue_export'),
      '#prefix' => '<div class="export-button-wrapper">',
      '#suffix' => '</div>',
    );

    return $form;
  }
  // Confirmation step page.
  else {
    $form = array();

    $items = array_filter($form_state['values']['queue_table']);
    foreach ($items as $key) {
      $list[] = $key . ': ' . $form_state['options'][$key]['template_nid'];
    }

    $form['confirm_values'] = array(
      '#markup' => theme_item_list(
        array(
          'items' => $list,
          'title' => t('Values:'),
          'type' => 'ul',
          'attributes' => array(),
        )
      ),
    );

    $form['#submit'] = $form_state['submit_handlers'];

    $question = $form_state['triggering_element']['#value'] . '?';

    return confirm_form($form, $question, 'admin/commerce/orders/aro-queue');
  }
}

/**
 * Submit function for metagenics_commerce_recurring_aro_queue_form().
 *
 * Orders manually pushed this way never make it to the drupal queue and are
 * processed directly by us, only Pending and Failed orders can be done this way
 * since they are not going to be in the drupal queue.
 */
function metagenics_commerce_recurring_aro_queue_process_orders_form_submit($form, &$form_state) {

  // Confirm submit.
  if (empty($form_state['storage']['confirm'])) {
    $form_state['storage']['confirm'] = TRUE;
    $form_state['storage']['values'] = $form_state['values'];
    $form_state['rebuild'] = TRUE;
  }
  else {
    if (!empty($form_state['storage']['values']['queue_table'])) {

      foreach ($form_state['storage']['values']['queue_table'] as $key => $qid) {
        if ($qid !== 0) {

          // Query for full data for the selected queue records.
          $sql = 'SELECT * FROM metagenics_commerce_recurring_process_queue WHERE qid = :qid';
          $results = db_query($sql, array(':qid' => $qid));

          if ($results) {
            foreach ($results as $result) {

              // Pending and failed orders can be submitted.
              if ($result->processing_status == 'Pending' || $result->processing_status == 'Failed') {

                // Add our processing to the batch operations.
                $operations[] = array('metagenics_commerce_recurring_queue_process_worker', array($result));
                drupal_set_message('Processed queue item ' . $qid);
              }
              else {
                drupal_set_message('Only Pending or Failed orders can be manually pushed. Queue item ' . $qid . ' is ' . $result->processing_status, 'error');
              }
            }
          }
        }
      }
    }
  }

  // Set the batch and allow it to process our manually pushed orders.
  if (!empty($operations)) {
    $batch = array(
      'operations' => $operations,
      'title' => t('Processing recurring order queue'),
    );

    batch_set($batch);
  }
}

/**
 * Custom submit function to delete orders from the queue.
 */
function metagenics_commerce_recurring_aro_remove_queue_form_submit($form, &$form_state) {
  // Confirm submit.
  if (empty($form_state['storage']['confirm'])) {
    $form_state['storage']['confirm'] = TRUE;
    $form_state['storage']['values'] = $form_state['values'];
    $form_state['rebuild'] = TRUE;
  }
  else {
    foreach ($form_state['storage']['values']['queue_table'] as $key => $qid) {
      if ($qid !== 0) {
        try {
          $success = db_delete('metagenics_commerce_recurring_process_queue')
          ->condition('qid', $qid)
          ->execute();

          if ($success) {
            drupal_set_message(t('QID @qid was removed from the process queue', array('@qid' => $qid)));
          }
        }
        catch (Exception $e) {
          drupal_set_message(t('QID @qid was not in the queue and does not need to be removed.', array('@qid' => $qid)));
        }

      }
    }
  }
}

/**
 * Custom submit for adding the aro orders to the queue by date.
 */
function metagenics_commerce_recurring_add_to_queue_submit($form, &$form_state) {

  if (!empty($form_state['values']['add_to_queue_ship_date'])) {
    $ship_date = $form_state['values']['add_to_queue_ship_date'];
    // Get the orders that should ship on the ship date submitted.
    $templates = metagenics_commerce_recurring_template_process_list(date('Y-m-d', strtotime($ship_date)) . ' 00:00:00');

    if ($templates) {
      foreach ($templates as $template) {
        metagenics_commerce_recurring_queue_add_orders($template, $ship_date);
      }
    }
    else {
      drupal_set_message("There were no orders that are set to be proccessed on " . $ship_date, 'warning');
    }
  }
}

/**
 * Custom submit for adding the aro orders to the queue by ID.
 */
function metagenics_commerce_recurring_add_to_queue_id_submit($form, &$form_state) {

  $template_ids = metagenics_commerce_recurring_clean_input_ids($form_state['values']['add_to_queue_id_text']);
  $templates = metagenics_commerce_recurring_template_process_list(NULL, 'with fields', $template_ids);
  $ship_date = $form_state['values']['add_to_queue_id_ship_date'];
  if ($templates) {

    foreach ($templates as $template) {
      metagenics_commerce_recurring_queue_add_orders($template, $ship_date);
    }
  }
  else {
    drupal_set_message('There were no templates IDs that matched your input. Please try again.', 'warning');
  }
}

/**
 * Custom form submit for filters on the queue page.
 *
 * @param array $form
 *   The form array
 *
 * @param array $form_state
 *   The form state array
 */
function metagenics_commerce_recurring_aro_queue_process_orders_form_filter_submit($form, &$form_state) {

  // Clear all get parameters and reset from the new form entries.
  metagenics_commerce_recurring_aro_queue_process_orders_form_filter_reset($form, $form_state);

  if (!empty($form_state['values']['status_filter'])) {
    $_GET['status_filter'] = $form_state['values']['status_filter'];
  }

  if (!empty($form_state['values']['frequency'])) {
    $_GET['frequency'] = $form_state['values']['frequency'];
  }

  if (!empty($form_state['values']['template_id'])) {
    $_GET['template_id'] = $form_state['values']['template_id'];
  }

  if (!empty($form_state['values']['next_ship_date_start'])) {
    $_GET['next_ship_date_start'] = $form_state['values']['next_ship_date_start'];
  }

  if (!empty($form_state['values']['next_ship_date_end'])) {
    $_GET['next_ship_date_end'] = $form_state['values']['next_ship_date_end'];
  }

  if (!empty($form_state['values']['pending_ship_date_end'])) {
    $_GET['pending_ship_date_end'] = $form_state['values']['pending_ship_date_end'];
  }

  if (!empty($form_state['values']['pending_ship_date_start'])) {
    $_GET['pending_ship_date_start'] = $form_state['values']['pending_ship_date_start'];
  }
}


/**
 * Custom form submit for the filter reset button.
 *
 * @param array $form
 *   The form array
 *
 * @param array $form_state
 *   The form state array
 */
function metagenics_commerce_recurring_aro_queue_process_orders_form_filter_reset($form, &$form_state) {
  if (!empty($_GET['status_filter'])) {
    unset($_GET['status_filter']);
  }

  if (!empty($_GET['frequency'])) {
    unset($_GET['frequency']);
  }

  if (!empty($_GET['template_id'])) {
    unset($_GET['template_id']);
  }

  if (!empty($_GET['next_ship_date_start'])) {
    unset($_GET['next_ship_date_start']);
  }

  if (!empty($_GET['pending_ship_date_start'])) {
    unset($_GET['pending_ship_date_start']);
  }

  if (!empty($_GET['next_ship_date_end'])) {
    unset($_GET['next_ship_date_end']);
  }

  if (!empty($_GET['pending_ship_date_end'])) {
    unset($_GET['pending_ship_date_end']);
  }
}

/**
 * Custom validate for adding the aro orders to the queue by ID.
 */
function metagenics_commerce_recurring_add_to_queue_id_validate($form, &$form_state) {

  // Make sure id field is not empty.
  if (empty($form_state['values']['add_to_queue_id_text']) || $form_state['values']['add_to_queue_id_text'] == ' ') {
    form_error($form['add_to_queue']['by_aro_id']['add_to_queue_id_text'], t('ID field cannot be empty.'));
  }
  // Make sure entered values are numeric.
  if (!empty($form_state['values']['add_to_queue_id_text']) || $form_state['values']['add_to_queue_id_text'] !== ' ') {

    $ids = metagenics_commerce_recurring_clean_input_ids($form_state['values']['add_to_queue_id_text']);

    $errors = array();
    foreach ($ids as $id) {
      if (!is_numeric(trim($id))) {
        $errors[] = $id;
      }
    }
    if (!empty($errors)) {
      $id_errors = implode(', ', $errors);
      form_error($form['add_to_queue']['by_aro_id']['add_to_queue_id_text'], $id_errors . ' are not numeric. Please update the list and resubmit.');
    }
  }

  // Make sure date field is not empty.
  $ship_date = $form_state['values']['add_to_queue_id_ship_date'];
  if ($ship_date == NULL) {
    form_error($form['add_to_queue']['by_aro_id']['add_to_queue_id_ship_date'], t('Date field cannot be empty'));
  }

}

/**
 * Custom form validation for the ARO queue admin form.
 */
function metagenics_commerce_recurring_add_to_queue_validate($form, &$form_state) {

  $ship_date = $form_state['values']['add_to_queue_ship_date'];

  if (empty($ship_date) || $ship_date == ' ') {
    form_error($form['add_to_queue']['by_ship_date']['add_to_queue_ship_date'], t('Date field cannot be blank!'));
  }

  $ship_date_parts = explode('/', $ship_date);

  // If there is no value in the year date part, error!.
  if (empty($ship_date_parts[2])) {
    form_error($form['add_to_queue']['by_ship_date']['add_to_queue_ship_date'], t('There was an error with your date entry format. Please check the format and try again.'));
  }

  // If the user doesn't enter numbers, error!
  foreach ($ship_date_parts as $date_part) {
    if (preg_match('/^[0-9]+$/', $date_part)) {
      $nan_error = FALSE;
    }
    else {
      form_error($form['add_to_queue']['by_ship_date']['add_to_queue_ship_date'], t("Date field can only contain numbers and /'s"));
    }
  }

  // Check that the character count on each part is correct.
  if (strlen($ship_date_parts[0]) !== 2 ||
    strlen($ship_date_parts[1]) !== 2 ||
    strlen($ship_date_parts[2]) !== 4 ||
    strlen($ship_date) !== 10) {

    form_error($form['add_to_queue']['by_ship_date']['add_to_queue_ship_date'], t('There was an error with your date entry format. Please check the format and try again.'));
  }

}

/**
 * A dynamic link callback to link users from admin data to other pages.
 *
 *  We are using this mainly for the aro queue table so we can dynmaically load
 *  in order and user data on a per-case basis rather than loading it all in one
 *  view.
 *
 * @param int $aro_template_id
 *   The aro template nid that we use to load everything else in with.
 * @param string $action
 *   The action that you want to use.
 */
function _metagenics_commerce_recurring_action_links($aro_template_id, $action) {

  switch ($action) {
    case 'aro-edit-page':
      $aro_template = node_load($aro_template_id);
      $user_id = $aro_template->uid;
      drupal_goto('account/' . $user_id . '/orders/recurring-orders/' . $aro_template_id . '/edit');
      break;
  }
}

/**
 * Cleans extra spaces off of the input ids.
 *
 * @param string $ids
 *   a string of ids. This will be trimmed, exploded, and formatted.
 *
 * @return array
 *   An array with only integer values, stripped of extra lines or spaces
 */
function metagenics_commerce_recurring_clean_input_ids($ids) {

  $trimmed = trim($ids);

  $ids = explode("\r\n", $trimmed);

  return $ids;

}

/**
 * Submit function for the export button.
 *
 * The query for the tableselect is rerun but without pager or limit to capture
 * all records. Then exported as csv using built in php and drupal functions.
 *
 * @param array $form
 *   The form array
 *
 * @param array $form_state
 *   The form state array
 *
 */
function metagenics_commerce_recurring_queue_export($form, &$form_state) {

  // Define headers
  $header = array(
    'qid' => t('Queue ID'),
    'template_nid' => t('Template ID'),
    'processing_status' => t('Processing status'),
    'template_next_order_date' => t('Next Ship date in Template'),
    'pending_order_date' => t('Pending Ship Date'),
    'order_date' => t('Actual Ship date'),
    'frequency' => t('Frequency'),
  );

  // First line of csv is headers
  $output = implode(",", $header) . "\n";

  // Run query with all filters and no pager/limits
  $query = db_select('metagenics_commerce_recurring_process_queue', 'm');
  $query->fields('m');

  // Add conditions for our filters.
  // Really interesting way to do an OR query here.
  if (!empty($_GET['status_filter'])) {
    $db_or = db_or();
    foreach ($_GET['status_filter'] as $status_filter) {
      $db_or->condition('processing_status', $status_filter, '=');
    }
    $query->condition($db_or);
  }

  if (!empty($_GET['template_id'])) {
    $query->condition('template_nid', $_GET['template_id'], '=');
  }

  if (!empty($_GET['next_ship_date_start'])) {
    $query->condition('template_next_order_date', strtotime($_GET['next_ship_date_start']), '>=');
  }

  if (!empty($_GET['next_ship_date_end'])) {
    $query->condition('template_next_order_date', strtotime($_GET['next_ship_date_end']), '<=');
  }

  if (!empty($_GET['pending_ship_date_start'])) {
    $query->condition('pending_order_date', strtotime($_GET['pending_ship_date_start']), '>=');
  }

  if (!empty($_GET['pending_ship_date_end'])) {
    $query->condition('pending_order_date', strtotime($_GET['pending_ship_date_end']), '<=');
  }

  if (!empty($_GET['frequency'])) {
    $db_or = db_or();
    foreach ($_GET['frequency'] as $frequency) {
      $db_or->condition('frequency', $frequency, '=');
    }
    $query->condition($db_or);
  }

  $result = $query->execute();

  $options = array();
  foreach ($result as $record) {

    // Here order date is the date the order was actually created.
    $order_date = $record->order_date == 0 ? '' : date('m-d-Y h:i A', $record->order_date);
    $order_data = unserialize($record->data);

    $order_info = '';
    if (!empty($order_data['order_id'])) {
      $order_info = 'Order ID: ' . $order_data['order_id'];
    }
    if (!empty($order_data['errors'])) {
      $order_info = 'Failed: ' . $order_data['order']->order_id;
    }

    // data array.
    $data = array(
      'qid' => $record->qid,
      'template_nid' => $record->template_nid,
      'processing_status' => $record->processing_status,
      'template_next_order_date' => date('m-d-Y', $record->template_next_order_date),
      'pending_order_date' => date('m-d-Y', $record->pending_order_date),
      'order_date' => $order_date . ' ' . $order_info,
      'frequency' => $record->frequency,
    );

    // output the array as a line in the csv.
    $output .= implode(",", $data) . "\n";
  }

  // These headers will cause the output to become a file
  drupal_add_http_header('Content-Type', 'text/csv; utf-8');
  drupal_add_http_header('Content-Disposition', 'attachment;filename=RecurringOrderQueueExport.csv');

  ob_clean();
  print $output;
  exit;
}
