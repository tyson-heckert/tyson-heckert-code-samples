<?php
/**
 * @file
 * Defines the admin settings for the brookfield_social module.
 */

/**
 * Generates the system settings form.
 */
function brookfield_social_admin_settings_form($form, &$form_state) {
  $form = array();

  $form['dealcatcher'] = array(
    '#type' => 'fieldset',
    '#title' => t('Deal Catcher Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['dealcatcher']['brookfield_social_dealcatcher_count'] = array(
    '#type' => 'textfield',
    '#title' => t('# of deals'),
    '#default_value' => variable_get('brookfield_social_dealcatcher_count', ''),
  );

  $options = brookfield_social_get_dealcatcher_categories();

  $form['dealcatcher']['categories'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select which cateogries to pull from'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('If no categories are selected, all categories will be used.'),
  );

  $form['dealcatcher']['categories']['brookfield_social_dealcatcher_local'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Local'),
    '#options' => $options['local'],
    '#default_value' => variable_get('brookfield_social_dealcatcher_local', array()),
  );

  $form['dealcatcher']['categories']['brookfield_social_dealcatcher_goods'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Goods'),
    '#options' => $options['goods'],
    '#default_value' => variable_get('brookfield_social_dealcatcher_goods', array()),
  );

  return system_settings_form($form);
}
