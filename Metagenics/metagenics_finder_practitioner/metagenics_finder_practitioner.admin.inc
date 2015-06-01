<?php


/**
 * System settings form for Metagenics User
 */
function metagenics_finder_practitioner_system_settings_form($form, $form_state) {
  $form = array();
 
  $form['meta_fap_score_criteria'] = array(
    '#type' => 'fieldset',
    '#title' => t('FAP Scoring'),
  );

  $form['meta_fap_score_criteria']['flt_score'] = array(
    '#type' => 'textfield',
    '#title' => t('FLT Role Score'),
    '#description' => t('Practitioner score will be boosted by this amount'),
    '#default_value' => variable_get('flt_score'),
  );

  $form['meta_fap_score_criteria']['store_score'] = array(
    '#type' => 'textfield',
    '#title' => t('Online Store Score'),
    '#description' => t('Practitioner score will be boosted by this amount'),
    '#default_value' => variable_get('store_score'),
  );

  $form['meta_fap_shop_now_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable "Shop Now" buttons in FAP Results'),
    '#default_value' => variable_get('meta_fap_shop_now_enabled'),
  );

  return system_settings_form($form);
}

