<?php

require_once TPT_ROOT . '/' . ('transperfect_core_constants.inc');
require_once TPT_ROOT . '/' . ('transperfect_settings.inc');
require_once TPT_ROOT . '/' . ('transperfect_common.inc');

require_once drupal_get_path('module', 'metagenics_transperfect').'/gl_ws/meta_gl_ws_receive_translations.inc';

function metagenics_transperfect_dashboard_receive_page($type) {
  $_SESSION['transpefect_dashboard_receive_type'] = $type;
  $array = array();
  $array[] = drupal_get_form('metagenics_transperfect_node_receive_filter_form');
  $array[] = drupal_get_form('metagenics_transperfect_node_receive_pager_form');
  $array[] = drupal_get_form('metagenics_transperfect_dashboard_receive');
  return $array;
}

function metagenics_transperfect_node_receive_filter_form() {
  $form = array();
/*
  $submission_options = get_distinct_active_submission_names();
  unset($submission_options['']);
  $submission_options['[any]'] = 'Any';
  $default_submission = '[any]';
  if (!empty($_SESSION['transperfect_node_receive_filter_submission']) &&
          !in_array($_SESSION['transperfect_node_receive_filter_submission'], $submission_options)) {
    $_SESSION['transperfect_node_receive_filter_submission'] = '';
    $_SESSION['transperfect_globalLink_arr'] = array();
    unset($_SESSION['transperfect_globalLink_arr_last_refreshed']);
  }

  $filter = FALSE;

  if (!empty($_SESSION['transperfect_node_receive_filter_submission'])) {
    $default_submission = $_SESSION['transperfect_node_receive_filter_submission'];
    $filter = TRUE;
  }

  $n_arr = array('[any]' => t(' Any '));
  $t_arr = get_mapped_tpt_locales_with_drupal_desc(FALSE);
  $languages = $n_arr + $t_arr;

  $status_arr = array('[any]' => t(' Any '),
      TPT_STATUS_COMPLETED => TPT_STATUS_COMPLETED,
      TPT_STATUS_ERROR => TPT_STATUS_ERROR,
      TPT_STATUS_SOURCE_DELETED => TPT_STATUS_SOURCE_DELETED);

  $default = '[any]';
  if (!empty($_SESSION['transperfect_node_receive_filter_language'])) {
    $default = $_SESSION['transperfect_node_receive_filter_language'];
    $filter = TRUE;
  }

  $default_status = '[any]';
  if (!empty($_SESSION['transperfect_node_receive_filter_status'])) {
    $default_status = $_SESSION['transperfect_node_receive_filter_status'];
    $filter = TRUE;
  }

  $form['node_receive_filter'] = array(
      '#type' => 'fieldset',
      '#title' => t('FILTER RECORDS'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
  );

  $form['node_receive_filter']['filters'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('container-inline')),
  );

  $form['node_receive_filter']['filters'][] = array('#markup' => "<table>");

  $form['node_receive_filter']['filters']['language'] = array(
      '#type' => 'select', '#title' => TPT_LABEL_TARGET_LANG . ':&nbsp;&nbsp;',
      '#options' => $languages,
      '#default_value' => $default,
      '#field_suffix' => '&nbsp;&nbsp;&nbsp;&nbsp;',
      '#prefix' => "<tr><td style='text-align: left;width: 15%;vertical-align: middle;border: none;'>",
      '#suffix' => '</tr>',
      '#field_suffix' => '</td>',
      '#field_prefix' => '</td><td>',
  );

  $form['node_receive_filter']['filters']['status'] = array(
      '#type' => 'select', '#title' => TPT_LABEL_STATUS . ':&nbsp;&nbsp;',
      '#options' => $status_arr,
      '#default_value' => $default_status,
      '#field_suffix' => '&nbsp;&nbsp;&nbsp;&nbsp;',
      '#prefix' => "<tr><td style='text-align: left;width: 15%;vertical-align: middle;border: none;'>",
      '#suffix' => '</tr>',
      '#field_suffix' => '</td>',
      '#field_prefix' => '</td><td>',
  );

  $form['node_receive_filter']['filters']['submission'] = array(
      '#type' => 'select', '#title' => TPT_LABEL_SUBMISSION . ':&nbsp;&nbsp;',
      '#options' => $submission_options,
      '#default_value' => $default_submission,
      '#field_suffix' => '&nbsp;&nbsp;&nbsp;&nbsp;',
      '#prefix' => "<tr><td style='text-align: left;width: 15%;vertical-align: middle;border: none;'>",
      '#suffix' => '</tr>',
      '#field_suffix' => '</td>',
      '#field_prefix' => '</td><td>',
  );

  $form['node_receive_filter']['filters'][] = array('#markup' => "</table>");

  $form['node_receive_filter']['filters']['filter'] = array(
      '#type' => 'submit', '#value' => TPT_BTN_DASHBOARD_FILTER);

  if ($filter) {

    $form['filter-search'] = array(
        '#type' => 'fieldset',
        '#title' => t('FILTER CRITERIA'),
    );

    $form['filter-search']['filter-markup'] = array(
        '#type' => 'container',
    );
    $form['filter-search']['filter-markup'][] = array('#markup' => "<table>");
    $form['filter-search']['filter-markup'][] = array('#markup' => "<tr><td style='text-align: left;width: 15%;vertical-align: middle;border: none;'><b>" . TPT_LABEL_TARGET_LANG . "</b>:&nbsp;</td><td style='text-align: left;width: 85%;vertical-align: middle;border: none;'>" . $languages[$default] . "</td></tr>");
    $form['filter-search']['filter-markup'][] = array('#markup' => "<tr><td style='text-align: left;width: 15%;vertical-align: middle;border: none;'><b>" . TPT_LABEL_STATUS . "</b>:&nbsp;</td><td style='text-align: left;width: 85%;vertical-align: middle;border: none;'>" . $status_arr[$default_status] . "</td></tr>");
    $form['filter-search']['filter-markup'][] = array('#markup' => "<tr><td style='text-align: left;width: 15%;vertical-align: middle;border: none;'><b>" . TPT_LABEL_SUBMISSION . "</b>:&nbsp;</td><td style='text-align: left;width: 85%;vertical-align: middle;border: none;'>" . $submission_options[$default_submission] . "</td></tr>");
    $form['filter-search']['filter-markup'][] = array('#markup' => "</table>");
    $form['filter-search']['status']['actions'] = array(
        '#type' => 'actions',
        '#attributes' => array('class' => array('container-inline')),
    );

    $form['filter-search']['status']['actions']['reset'] = array(
        '#type' => 'submit',
        '#value' => TPT_BTN_DASHBOARD_RESET,
    );
  }

  */
  return $form;
}

function metagenics_transperfect_node_receive_filter_form_submit($form, &$form_state) {
  /*
  $op = isset($form_state['values']['op']) ? $form_state['values']['op'] : '';
  if ($op == TPT_BTN_DASHBOARD_FILTER) {
    $_SESSION['transperfect_node_receive_filter_language'] = $form_state['values']['language'];
    $_SESSION['transperfect_node_receive_filter_status'] = $form_state['values']['status'];
    $_SESSION['transperfect_node_receive_filter_submission'] = $form_state['values']['submission'];
    $_SESSION['transperfect_globalLink_arr'] = array();
    unset($_SESSION['transperfect_globalLink_arr_last_refreshed']);
  }
  elseif ($op == TPT_BTN_DASHBOARD_RESET) {
    $_SESSION['transperfect_node_receive_filter_language'] = '';
    $_SESSION['transperfect_node_receive_filter_status'] = '';
    $_SESSION['transperfect_node_receive_filter_submission'] = '';
    $_SESSION['transperfect_globalLink_arr'] = array();
    unset($_SESSION['transperfect_globalLink_arr_last_refreshed']);
  }
  */
}

function metagenics_transperfect_node_receive_pager_form() {
  $form = array();
  $form['page_counter']['markup'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('tpt-center')),
  );
  $form['page_counter']['markup']['p_count'] = array(
      '#type' => 'textfield',
      '#size' => 5,
      '#default_value' => isset($_SESSION['transperfect_receive_page_count']) ? $_SESSION['transperfect_receive_page_count'][0] : TPT_PAGER_LIMIT,
      '#field_prefix' => t('Show') . '&nbsp;&nbsp;',
      '#field_suffix' => '&nbsp;&nbsp;' . t('records') . '&nbsp;&nbsp;&nbsp;&nbsp;',
      '#prefix' => '<div class="container-inline">',
  );
  $form['page_counter']['markup']['action'] = array(
      '#type' => 'submit',
      '#value' => TPT_BTN_DASHBOARD_GO,
      '#suffix' => '</div>',
      '#limit_validation_errors' => array(),
      '#submit' => array('transperfect_node_receive_pager_form_submit'),
      '#access' => TRUE,
  );
  return $form;
}

function metagenics_transperfect_node_receive_pager_form_submit($form, &$form_state) {
  $op = $form_state['values']['op'];
  if ($op == TPT_BTN_DASHBOARD_GO) {
    $page_count = $form_state['input']['p_count'];
    if (!is_numeric($page_count) || $page_count < 1) {
      form_set_error('', t('Invalid Page Count.'));
    }
    else {
      $_SESSION['transperfect_receive_page_count'] = array($page_count);
      return;
    }
  }
}

function metagenics_transperfect_dashboard_receive() {
  $form = array();

  if (!empty($_SESSION['transpefect_dashboard_receive_type'])) {
    $type = $_SESSION['transpefect_dashboard_receive_type'];
    $globalLink_arr = array();

    if (empty($_SESSION['metagenics_transperfect_globalLink_arr'])) {
      try {
        $pd4 = get_project_director_details();
        if (validate_project_director_details($pd4)) {
          $_SESSION['transperfect_globalLink_arr_last_refreshed'] = date("D M j G:i:s T Y", REQUEST_TIME);
          $globalLink_arr = metagenics_getReadyTranslationsDetailsFromPD($pd4, $type);

          if (sizeof($globalLink_arr) > 0) {
            $_SESSION['metagenics_transperfect_globalLink_arr'] = serialize($globalLink_arr);
          }
        }
      }
      catch (SoapFault $e) {
        watchdog('transperfect', 'SOAP Exception in Receive Translations - Get Ready -  File[%exf], Line[%exl], Code[%exc], Message[%exm]', array('%exf' => $e->getFile(), '%exl' => $e->getLine(), '%exc' => $e->faultcode, '%exm' => $e->faultstring), WATCHDOG_ERROR);
        form_set_error('', check_plain('Web Services Error: ' . $e->faultcode . ' - ' . $e->faultstring));
      }
      catch (Exception $ex) {
        watchdog('transperfect', 'Exception in Receive Translations - Get Ready - File[%exf], Line[%exl], Code[%exc], Message[%exm]', array('%exf' => $ex->getFile(), '%exl' => $ex->getLine(), '%exc' => $ex->getCode(), '%exm' => $ex->getMessage()), WATCHDOG_ERROR);
        form_set_error('', check_plain('Error: ' . $ex->getMessage()));
      }
    }
    else {
      $globalLink_arr = unserialize($_SESSION['metagenics_transperfect_globalLink_arr']);
    }

    $header = array('submissionName' => TPT_LABEL_SUBMISSION,
        'sourceLocale' => TPT_LABEL_SOURCE_LANG,
        'targetLocale' => TPT_LABEL_TARGET_LANG,
        'title' => TPT_LABEL_TITLE,
        'status' => TPT_LABEL_STATUS);

    if (is_preview_enabled()) {
      $header['preview'] = '';
    }

    $rows = array();
    $count = 1;

    $filter_lang = '[any]';
    if (!empty($_SESSION['transperfect_node_receive_filter_language'])) {
      $filter_lang = $_SESSION['transperfect_node_receive_filter_language'];
    }

    if (isset($globalLink_arr) && is_array($globalLink_arr)) {
      foreach ($globalLink_arr as $key => &$globalLink) {
        if ($filter_lang != '[any]' && $globalLink->targetLocale != $filter_lang) {
          unset($globalLink_arr[$key]);
        }
      }
    }

    if (isset($globalLink_arr) && is_array($globalLink_arr)) {
      $filter_status = '[any]';
      if (!empty($_SESSION['transperfect_node_receive_filter_status'])) {
        $filter_status = $_SESSION['transperfect_node_receive_filter_status'];
      }

      $rid_arr = array();
      foreach ($globalLink_arr as $key => &$globalLink) {
        $file_name = $globalLink->sourceFileName;
        if (startsWith($file_name, "Menu")) {
          continue;
        }

        $source_xml = '';
        $status = TPT_STATUS_COMPLETED;
        $title = get_entity_title($globalLink);
        if (!$title) {
          $title = $globalLink->title;
          $status = TPT_STATUS_SOURCE_DELETED;
          $source_xml = TPT_STATUS_SOURCE_DELETED;
          $globalLink->status = TPT_STATUS_SOURCE_DELETED;
          $globalLink->sourceDeleted = TRUE;
        }
        if ($globalLink->status == TPT_STATUS_ERROR) {
          $status = TPT_STATUS_ERROR;
        }

        if ($filter_status != '[any]' && $status != $filter_status) {
          unset($globalLink_arr[$key]);
          continue;
        }

        $filter_submission = '[any]';
        if (!empty($_SESSION['transperfect_node_receive_filter_submission'])) {
          $filter_submission = $_SESSION['transperfect_node_receive_filter_submission'];
        }

        if ($filter_submission != '[any]' && $globalLink->submissionName != $filter_submission) {
          unset($globalLink_arr[$key]);
          continue;
        }

        $tpt_row_id = get_tpt_row_id_from_submission($globalLink->submissionTicket, $globalLink->documentTicket, $globalLink->targetLocale);
        if ($tpt_row_id) {
          $rid_arr['tpt_content_div_' . $key] = $tpt_row_id;
        }

        $source_name = $globalLink->sourceLocale;
        $target_name = $globalLink->targetLocale;

        $rows[$key] = array(
            'submissionName' => $globalLink->submissionName,
            'sourceLocale' => $source_name,
            'targetLocale' => $target_name,
            'title' => $title,
            'status' => $status,
        );

        if (is_preview_enabled()) {
          $rows[$key]['preview'] = $source_xml != TPT_STATUS_SOURCE_DELETED ? l(' Preview Translations ', 'javascript:void(0)', array('fragment' => '', 'external' => TRUE, 'attributes' => array('title' => 'Click to Preview', 'id' => 'tpt_content_link_' . $key))) : '';
        }

        $count++;
      }

      if (sizeof($globalLink_arr) > 0) {
        $_SESSION['metagenics_transperfect_globalLink_arr'] = serialize($globalLink_arr);
      }
    }

    $page_count = isset($_SESSION['transperfect_receive_page_count']) ? $_SESSION['transperfect_receive_page_count'][0] : TPT_PAGER_LIMIT;

    $form['table'] = array(
        '#type' => 'tableselect',
        '#header' => $header,
        '#options' => pager_array_splice($rows, $page_count),
        '#empty' => t('No items available'),
    );

    $form['pager'] = array('#markup' => theme('pager'));

    if (is_preview_enabled()) {
      $lcount = 1;
      $div_arr = array();

      foreach ($rid_arr as $key => $rid) {
        if (isset($rid)) {
          $lcount = str_replace('tpt_content_div_', '', $key);
          $form['tpt_content_div_' . $lcount] = array('#type' => 'markup', '#markup' => '', '#prefix' => '<div id="tpt_content_div_' . $lcount . '" style="display: none;">', '#suffix' => '</div>');
          $div_arr['tpt_content_link_' . $lcount] = 'tpt_content_div_' . $lcount;
        }
      }

      drupal_add_js(array(
          'transperfect' => array(
              'popup' => $div_arr,
              'previewpath' => base_path() . transperfect_get_root_menu('transperfect-translations/preview'),
              'rids' => $rid_arr,
              )), 'setting');
    }

    if ($count > 1) {
      $form['submit_update_selected'] = array('#type' => 'submit', '#value' => TPT_BTN_DASHBOARD_UPDATE_SELECTED);
      $form['submit_update_all'] = array('#type' => 'submit', '#value' => TPT_BTN_DASHBOARD_UPDATE_ALL);
      $form['submit_clear_deleted_hidden'] = array('#type' => 'submit', '#value' => TPT_BTN_DASHBOARD_CLEAR_DELETED_RECORDS, '#attributes' => array('style' => 'display:none;', 'disabled' => 'true'));
      $form['submit_clear_deleted'] = array('#type' => 'submit', '#value' => TPT_BTN_DASHBOARD_CLEAR_DELETED_RECORDS);
    }

    $last_refreshed = "";
    if (isset($_SESSION['transperfect_globalLink_arr_last_refreshed'])) {
      $last_refreshed = $_SESSION['transperfect_globalLink_arr_last_refreshed'];
    }
    $form['submit_refresh_hidden'] = array('#type' => 'submit', '#value' => TPT_BTN_DASHBOARD_REFRESH, '#prefix' => '<BR/>', '#attributes' => array('style' => 'display:none;', 'disabled' => 'true'));
    $form['submit_refresh'] = array('#type' => 'submit', '#value' => TPT_BTN_DASHBOARD_REFRESH);
    $form['markup'] = array(
        '#type' => 'markup',
        '#markup' => 'Last refreshed : ' . $last_refreshed);
  }

  drupal_add_js('jQuery(document).ready(function(){jQuery("#edit-submit-refresh").click(function(){jQuery("#edit-submit-refresh").hide();jQuery("#edit-submit-refresh-hidden").css("display","inline");});});', 'inline');
  drupal_add_js('jQuery(document).ready(function(){jQuery("#edit-submit-clear-deleted").click(function(){jQuery("#edit-submit-clear-deleted").hide();jQuery("#edit-submit-clear-deleted-hidden").css("display","inline");});});', 'inline');
  drupal_add_library('system', 'ui.dialog');
  $module_path = drupal_get_path('module', 'transperfect');
  drupal_add_css($module_path . '/css/transperfect.css');
  drupal_add_js($module_path . '/js/transperfect_receive.js');
  return $form;
}

function metagenics_transperfect_dashboard_receive_validate($form, &$form_state) {
  $op = isset($form_state['values']['op']) ? $form_state['values']['op'] : '';

  $pd4 = get_project_director_details();
  validate_project_director_details($pd4);

  if ($op == TPT_BTN_DASHBOARD_UPDATE_SELECTED) {
    $rids = array_filter($form_state['values']['table']);
    if (count($rids) == 0) {
      form_set_error('', t('No items selected.'));
    }
  }
}

function metagenics_transperfect_dashboard_receive_submit($form, &$form_state) {

  $type = $_SESSION['transpefect_dashboard_receive_type'];
  $op = isset($form_state['values']['op']) ? $form_state['values']['op'] : '';
  $pd4 = get_project_director_details();
  if ($op == TPT_BTN_DASHBOARD_UPDATE_ALL) {
    try {
      $globalLink_arr = unserialize($_SESSION['metagenics_transperfect_globalLink_arr']);
      $count = get_translated_entity_content($pd4, $globalLink_arr);
      $error = FALSE;
      $delete_records_selected = FALSE;
      foreach ($globalLink_arr as $updated) {
        if ($updated->sourceDeleted == TRUE) {
          $delete_records_selected = TRUE;
        }
        if ($updated->status == TPT_STATUS_ERROR) {
          update_tpt_entity_status($updated);
          $error = TRUE;
        }
      }
      $status = 'status';
      if ($error) {
        drupal_set_message(t('There was some error while updating the records. Please check logs for more details.'), 'warning', NULL);
        $status = 'warning';
      }
      drupal_set_message(check_plain($count . ' record(s) updated successfully.'), $status, NULL);
      if ($delete_records_selected) {
        drupal_set_message(t('Source Deleted record(s) cannot be updated.'), 'warning', NULL);
      }
    }
    catch (Exception $ex) {
      watchdog('transperfect', 'Exception in Receive Translations - Update All - File[%exf], Line[%exl], Code[%exc], Message[%exm]', array('%exf' => $ex->getFile(), '%exl' => $ex->getLine(), '%exc' => $ex->getCode(), '%exm' => $ex->getMessage()), WATCHDOG_ERROR);
      form_set_error('', check_plain('Error: ' . $ex->getMessage()));
    }
    $_SESSION['metagenics_transperfect_globalLink_arr'] = array();
    unset($_SESSION['transperfect_globalLink_arr_last_refreshed']);
  }
  elseif ($op == TPT_BTN_DASHBOARD_CLEAR_DELETED_RECORDS) {
    try {
      $globalLink_arr = unserialize($_SESSION['metagenics_transperfect_globalLink_arr']);
      $success = FALSE;
      $count = 0;
      foreach ($globalLink_arr as $globalLink) {
        if ($globalLink->sourceDeleted == TRUE) {
          $success = update_deleted_entity_records($pd4, $globalLink);
          $count++;
        }
      }
      if ($count == 0) {
        drupal_set_message(t('No source record(s) deleted.'), 'warning', NULL);
      }

      if ($success) {
        drupal_set_message(check_plain($count . ' Source Deleted record(s) cleared successfully.'), 'status', NULL);
      }
    }
    catch (Exception $ex) {
      watchdog('transperfect', 'Exception in Receive Translations - Update Deleted Records - File[%exf], Line[%exl], Code[%exc], Message[%exm]', array('%exf' => $ex->getFile(), '%exl' => $ex->getLine(), '%exc' => $ex->getCode(), '%exm' => $ex->getMessage()), WATCHDOG_ERROR);
      form_set_error('', check_plain('Error: ' . $ex->getMessage()));
    }
    $_SESSION['metagenics_transperfect_globalLink_arr'] = array();
    unset($_SESSION['transperfect_globalLink_arr_last_refreshed']);
  }
  elseif ($op == TPT_BTN_DASHBOARD_UPDATE_SELECTED) {
    try {
      $delete_records_selected = FALSE;
      $globalLink_arr = unserialize($_SESSION['metagenics_transperfect_globalLink_arr']);
      $updated_globalLink_arr = array();
      $rids = array_filter($form_state['values']['table']);
      foreach ($rids as $rid) {
        array_push($updated_globalLink_arr, $globalLink_arr[$rid]);
      }
      $count = get_translated_entity_content($pd4, $updated_globalLink_arr);
      $error = FALSE;
      foreach ($updated_globalLink_arr as $updated) {
        if ($updated->sourceDeleted == TRUE) {
          $delete_records_selected = TRUE;
        }
        if ($updated->status == TPT_STATUS_ERROR) {
          $error = TRUE;
          update_tpt_entity_status($updated);
        }
      }
      $status = 'status';
      if ($error) {
        drupal_set_message(t('There was some error while updating the records. Please check logs for more details.'), 'warning', NULL);
        $status = 'warning';
      }
      drupal_set_message(check_plain($count . ' record(s) updated successfully.'), $status, NULL);
      if ($delete_records_selected) {
        drupal_set_message(t('Source Deleted record(s) cannot be updated.'), 'warning', NULL);
      }
    }
    catch (Exception $ex) {
      watchdog('transperfect', 'Exception in Receive Translations - Update Selected - File[%exf], Line[%exl], Code[%exc], Message[%exm]', array('%exf' => $ex->getFile(), '%exl' => $ex->getLine(), '%exc' => $ex->getCode(), '%exm' => $ex->getMessage()), WATCHDOG_ERROR);
      form_set_error('', check_plain('Error: ' . $ex->getMessage()));
    }
    $_SESSION['metagenics_transperfect_globalLink_arr'] = array();
    unset($_SESSION['transperfect_globalLink_arr_last_refreshed']);
  }
  elseif ($op == TPT_BTN_DASHBOARD_REFRESH) {
    sleep(2);
    $pd4 = get_project_director_details();
    try {
      $globalLink_arr = metagenics_getReadyTranslationsDetailsFromPD($pd4, $type);
    }
    catch (SoapFault $e) {
      watchdog('transperfect', 'SOAP Exception in Receive Translations - Refresh -  File[%exf], Line[%exl], Code[%exc], Message[%exm]', array('%exf' => $e->getFile(), '%exl' => $e->getLine(), '%exc' => $e->faultcode, '%exm' => $e->faultstring), WATCHDOG_ERROR);
      form_set_error('', check_plain('Web Services Error: ' . $e->faultcode . ' - ' . $e->faultstring));
    }
    catch (Exception $ex) {
      watchdog('transperfect', 'Exception in Receive Translations - Refresh - File[%exf], Line[%exl], Code[%exc], Message[%exm]', array('%exf' => $ex->getFile(), '%exl' => $ex->getLine(), '%exc' => $ex->getCode(), '%exm' => $ex->getMessage()), WATCHDOG_ERROR);
      form_set_error('', check_plain('Error: ' . $ex->getMessage()));
    }
    if (sizeof($globalLink_arr) > 0) {
      $_SESSION['metagenics_transperfect_globalLink_arr'] = serialize($globalLink_arr);
      $_SESSION['transperfect_globalLink_arr_last_refreshed'] = date("D M j G:i:s T Y", REQUEST_TIME);
    }
    else {
      $_SESSION['metagenics_transperfect_globalLink_arr'] = array();
      unset($_SESSION['transperfect_globalLink_arr_last_refreshed']);
    }
  }
}

function metagenics_receive_translated_contents_automatically($cron_type) {
  drupal_set_time_limit(0); //set it so your cron wont time out if it takes a long time to process
  watchdog('transperfect', "Cron Started by %type", array('%type' => $cron_type), WATCHDOG_NOTICE);
  try {
    $pd4 = get_project_director_details();
    $globalLink_node_arr = getReadyTranslationsDetailsFromPD($pd4, 'node');
    $count = get_translated_content($pd4, $globalLink_node_arr);
    foreach ($globalLink_node_arr as $updated) {
      if ($updated->status == TPT_STATUS_ERROR) {
        update_tpt_status($updated);
      }
    }
    $_SESSION['metagenics_transperfect_globalLink_arr'] = array();
    sleep(1); // time delay for PD

    $globalLink_block_arr = getReadyTranslationsDetailsFromPD($pd4, 'block');
    $count += get_translated_blocks($pd4, $globalLink_block_arr);
    $_SESSION['transperfect_block_globalLink_arr'] = array();
    sleep(1); // time delay for PD

    $globalLink_interface_arr = getReadyTranslationsDetailsFromPD($pd4, 'interface');
    $count += get_translated_interfaces($pd4, $globalLink_interface_arr);
    $_SESSION['transperfect_interface_globalLink_arr'] = array();
    sleep(1); // time delay for PD

    $globalLink_menu_arr = getReadyTranslationsDetailsFromPD($pd4, 'menu');
    $count += get_translated_menus($pd4, $globalLink_menu_arr);
    $_SESSION['transperfect_menu_globalLink_arr'] = array();
    sleep(1); // time delay for PD

    $globalLink_taxonomy_arr = getReadyTranslationsDetailsFromPD($pd4, 'taxonomy');
    $count += get_translated_taxonomies($pd4, $globalLink_taxonomy_arr);
    $_SESSION['transperfect_taxonomy_globalLink_arr'] = array();
    sleep(1); // time delay for PD

    $globalLink_webform_arr = getReadyTranslationsDetailsFromPD($pd4, 'webform');
    $count += get_translated_webforms($pd4, $globalLink_webform_arr);
    $_SESSION['transperfect_webform_globalLink_arr'] = array();
  }
  catch (SoapFault $e) {
    watchdog('transperfect', 'SOAP Exception in Receive Translations Cron - File[%exf], Line[%exl], Code[%exc], Message[%exm]', array('%exf' => $e->getFile(), '%exl' => $e->getLine(), '%exc' => $e->faultcode, '%exm' => $e->faultstring), WATCHDOG_ERROR);
  }
  catch (Exception $ex) {
    watchdog('transperfect', 'Exception in Receive Translations Cron - File[%exf], Line[%exl], Code[%exc], Message[%exm]', array('%exf' => $ex->getFile(), '%exl' => $ex->getLine(), '%exc' => $ex->getCode(), '%exm' => $ex->getMessage()), WATCHDOG_ERROR);
  }

  watchdog('transperfect', "Cron Completed - %c Records Updated.", array('%c' => $count), WATCHDOG_NOTICE);
}

function metagenics_preview_translated_content() {
  $row_id = $_POST['rid'];
  $arr = get_translations_for_row_id($row_id);
  drupal_json_output($arr);
}

function metagenics_get_translations_for_row_id($row_id) {
  $row = get_transperfect_row($row_id);

  $globalLink = new GlobalLink();
  $globalLink->tptRowId = $row_id;
  $globalLink->nid = $row->nid;
  $globalLink->vid = $row->vid;
  $globalLink->type = $row->type;
  $globalLink->title = $row->title;
  $globalLink->sourceLocale = $row->source;
  $globalLink->targetLocale = $row->target;
  $globalLink->documentTicket = $row->document_ticket;
  $globalLink->submissionTicket = $row->submission_ticket;
  $globalLink->submission = $row->submission;
  $globalLink->status = $row->status;

  $source_arr = array();
  $translation = array();

  $error = FALSE;
  try {
    $pd4 = get_project_director_details();
    $targets = getCompletedTargetsByDoc($pd4, $globalLink->documentTicket);

    if (is_array($targets)) {
      foreach ($targets as $target) {
        if (str_replace('-', '_', $target->targetLanguage->locale) == $globalLink->targetLocale) {
          if (!is_null($target->ticket) && $target->ticket != '') {
            $globalLink->targetTicket = $target->ticket;
            $globalLink->targetXML = downloadTargetResource($pd4, $globalLink->targetTicket);
            if (!is_null($globalLink->targetXML) && $globalLink->targetXML != '') {
              $translation = get_translated_array($globalLink->targetXML);
              $nid = $translation['nid'];
              $vid = $translation['vid'];
              $node = node_load($nid, $vid);
              if ($node && is_object($node) && !is_null($node)) {
                $drupal_locale_code = get_drupal_locale_code($globalLink->sourceLocale);
                if ($node->tnid != 0 && $node->language != $drupal_locale_code) {
                  $translations = translation_node_get_translations($node->tnid);
                  if (isset($translations) && isset($translations[$drupal_locale_code])) {
                    $src_tnode = $translations[$drupal_locale_code];
                    $node = node_load($src_tnode->nid);
                  }
                }
                $name = '';
                $source_xml = get_xml($node, $globalLink->targetLocale, NULL, NULL, $name, TRUE);
                $source_arr = array();
                if ($source_xml != TPT_STATUS_SOURCE_DELETED) {
                  $source_arr = get_translated_array($source_xml);
                }
              }
              else {
                $source_xml = TPT_STATUS_SOURCE_DELETED;
                $globalLink->sourceDeleted = TRUE;
              }
            }
          }
        }
      }
    }
    elseif (!is_null($targets)) {
      $target = $targets;
      if (str_replace('-', '_', $target->targetLanguage->locale) == $globalLink->targetLocale) {
        if (!is_null($target->ticket) && $target->ticket != '') {
          $globalLink->targetTicket = $target->ticket;
          $globalLink->targetXML = downloadTargetResource($pd4, $globalLink->targetTicket);
          if (!is_null($globalLink->targetXML) && $globalLink->targetXML != '') {
            $translation = get_translated_array($globalLink->targetXML);
            $nid = $translation['nid'];
            $vid = $translation['vid'];
            $node = node_load($nid, $vid);
            if ($node && is_object($node) && !is_null($node)) {
              $drupal_locale_code = get_drupal_locale_code($globalLink->sourceLocale);
              if ($node->tnid != 0 && $node->language != $drupal_locale_code) {
                $translations = translation_node_get_translations($node->tnid);
                if (isset($translations) && isset($translations[$drupal_locale_code])) {
                  $src_tnode = $translations[$drupal_locale_code];
                  $node = node_load($src_tnode->nid);
                }
              }
              $name = '';
              $source_xml = get_xml($node, $globalLink->targetLocale, NULL, NULL, $name, TRUE);
              $source_arr = array();
              if ($source_xml != TPT_STATUS_SOURCE_DELETED) {
                $source_arr = get_translated_array($source_xml);
              }
            }
            else {
              $source_xml = TPT_STATUS_SOURCE_DELETED;
              $globalLink->sourceDeleted = TRUE;
            }
          }
        }
      }
    }
  }
  catch (SoapFault $e) {
    $error = $e->faultstring;
  }
  catch (Exception $ex) {
    $error = $ex->getMessage();
  }

  $arr = array();
  $arr['target_ticket'] = $globalLink->targetTicket;
  if ($globalLink->sourceDeleted) {
    $arr = array(TPT_STATUS_SOURCE_DELETED);
  }
  elseif ($error) {
    $arr = array('error' => $error);
  }
  else {
    $arr['source'] = $source_arr;
    $arr['target'] = $translation;
  }

  return $arr;
}