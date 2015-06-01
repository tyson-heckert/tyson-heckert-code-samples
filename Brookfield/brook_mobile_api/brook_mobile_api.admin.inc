<?php
/**
 * @file
 * Admin configuration page for brookfield mobile api module.
 */

/**
 * Admin configuration page.
 */
function brook_mobile_api_admin_settings($form, &$form_state) {
  $form = array();

  $form['punchlist_wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Punchlist Update Settings'),
  );

  $form['punchlist_wrapper']['brook_mobile_api_last_punchlist_update'] = array(
    '#type' => 'date_popup',
    '#date_format' => 'm/d/Y',
    '#title' => t('Punchlist fields last updated date'),
    '#attributes' => array('class' => array('date-picker')),
    '#default_value' => variable_get('brook_mobile_api_last_punchlist_update', ''),
  );

  $form['punchlist_wrapper']['brook_mobile_api_punchlist_sync_fields'] = array(
    '#title' => t('Punchlist sync fields'),
    '#description' => t('List the fields that will be synced with the ipad app. Use machine names one per line.'),
    '#type' => 'textarea',
    '#default_value' => variable_get('brook_mobile_api_punchlist_sync_fields', ''),
  );

  $form['taxonomy_wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy Update Settings'),
  );

  $form['taxonomy_wrapper']['brook_mobile_api_last_update_taxonomy_terms'] = array(
    '#type' => 'date_popup',
    '#title' => t('Taxonomy terms last updated date'),
    '#date_format' => 'm/d/Y',
    '#default_value' => variable_get('brook_mobile_api_last_update_taxonomy_terms', ''),
  );

  return system_settings_form($form);
}
