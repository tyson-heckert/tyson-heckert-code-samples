<?php

/**
 * @file
 * ARO Editor workflow file for metagenics_commerce_recurring module.
 */

/**
 * Implements hook_form_FORM_ID_alter().
 */
function metagenics_commerce_form_recurring_order_template_node_form_alter(&$form, &$form_state) {
  if (!empty($form_state['build_info']['args'][1]) && $form_state['build_info']['args'][1] === 'patient_recurring_order_form') {
    $account = $form_state['build_info']['args'][2];
    $node = $form_state['node'];

    $last_ship_date = $form['field_last_order_date']['und'][0]['value']['#default_value'];
    $next_ship_date_display = date('m/d/Y', strtotime($form['field_aro_next_ship_date']['und'][0]['#default_value']['value']));

    if (!empty($form_state['values'])) {
      $frequency = $form_state['values']['field_frequency']['und']['0']['value'];
      $next_ship_date  = metagenics_commerce_recurring_calculate_next_ship_date($last_ship_date, $frequency);
    }
    else {
      $next_ship_date  = $form['field_aro_next_ship_date']['und'][0]['#default_value']['value'];
    }

    $next_ship_date_display = date('m/d/Y', strtotime($next_ship_date));
    // Repurpose some form elements to preserve fieldset.
    $form['title'] = array(
      '#prefix' => '<div class="form-item-title order-info-item">',
      '#suffix' => '</div>',
      '#markup' => '<label>' . t('Order ID:') . '</label> ' . $node->nid,
      '#weight' => $form['title']['#weight'],
    );

    $form['field_last_order_date'] = array(
      '#prefix' => '<div class="field-name-field-last-order-date order-info-item">',
      '#suffix' => '</div>',
      '#markup' => '<label>' . t('Last Order Date:') . '</label> ' . $form['field_last_order_date'][LANGUAGE_NONE][0]['value']['#default_value'],
      '#weight' => $form['field_last_order_date']['#weight'],
    );

    $form['fieldset_next_order_date'] = array(
      '#type' => 'fieldset',
      '#title' => '',
      '#weight' => 4,
      '#prefix' => '<div id="next-order-date-wrapper">',
      '#suffix' => '</div>',
    );
    $form['fieldset_next_order_date']['next_order_date_display'] = array(
      '#prefix' => '<div class="field-name-field-next-ship-date order-info-item">',
      '#suffix' => '</div>',
      '#markup' => '<label>' . t('Next Order Date:') . '</label> <span class="next-ship-date-display">' . $next_ship_date_display . '</span><div class="open-next-date"><a class="button" href="#">Change Order Date</a></div>',
      '#weight' => 2,
    );

    // Programatically added markup field.
    $form['shipping_note'] = array(
      '#prefix' => '<div class="aro-edit-shipping-note">',
      '#suffix' => '</div>',
      '#markup' => t("<strong>Note:</strong> Future orders will automatically ship on the same day of the month at the frequency you've chosen."),
      '#weight' => 5,
    );

    // Programatically added markup field.
    $form['field_aro_next_ship_date'][LANGUAGE_NONE][0]['#suffix'] = '<div class="aro-edit-shipping-note">' . t("Choose the date on which you would like your next order to ship.") . '</div>';

    $form['field_aro_next_ship_date'][LANGUAGE_NONE][0]['#element_validate'][] = 'metagenics_commerce_recurring_next_ship_date_validate';
    $form['field_aro_next_ship_date']['und'][0]['#default_value']['value'] = $next_ship_date;
    $form['field_aro_next_ship_date']['und'][0]['#date_items']['value'] = $next_ship_date;
    $form_state['input']['field_aro_next_ship_date']['und'][0]['value'] = $next_ship_date;
    $form['fieldset_next_order_date']['field_aro_next_ship_date'] = $form['field_aro_next_ship_date'];
    // Unset the original field so we don't get two.
    unset($form['field_aro_next_ship_date']);

    $form['field_frequency']['und']['#ajax']['callback'] = 'metagenics_commerce_recurring_recalc_date_ajax';
    $form['field_frequency']['und']['#ajax']['wrapper'] = 'next-order-date-wrapper';

    // Programatically add our fields to the field group.
    $form['#group_children']['next_order_date_display'] = 'group_order_info';
    $form['#group_children']['fieldset_next_order_date'] = 'group_order_info';
    $form['#group_children']['shipping_note'] = 'group_order_info';

    // Remove fields the patient doesn't need to see.
    $form['field_order_log']['#access'] = FALSE;
    $form['field_shipping_information'][LANGUAGE_NONE]['profiles'][0]['remove']['#access'] = FALSE;
    $form['field_billing_information'][LANGUAGE_NONE]['profiles'][0]['remove']['#access'] = FALSE;
    $form['field_order_line_item']['#access'] = FALSE;
    $form['field_ro_discounts']['#access'] = FALSE;
    $form['field_aro_original_order']['#access'] = FALSE;
    $form['field_ro_discounts']['#access'] = FALSE;
    $form['field_aro_deleted']['#access'] = FALSE;
    $form['field_aro_deleted_date']['#access'] = FALSE;
    $form['field_aro_suspended_date']['#access'] = FALSE;
    $form['field_aro_reinstated_date']['#access'] = FALSE;
    $form['field_order_log']['#access'] = FALSE;

    if($node->field_aro_deleted[LANGUAGE_NONE][0]['value'] == 1){
      $form['#disabled'] = TRUE;
    }

    // Create a new Line Items fieldset.
    $form['line_items_wrapper'] = array(
      '#type' => 'fieldset',
      '#title' => t('Products'),
      '#weight' => $form['field_order_line_item']['#weight'],
      '#prefix' => '<div id="line-items-wrapper">',
      '#suffix' => '</div>',
    );

    $form['line_items_wrapper']['line_items'] = array(
      '#type' => 'tableform',
      '#tf_header' => array(
        t('Item'),
        t('Item Price'),
        t('Qty'),
        t('Total'),
        NULL,
      ),
      '#tf_rows' => array(),
      '#tree' => TRUE,
      '#sticky' => FALSE,
    );

    $node_wrapper = entity_metadata_wrapper('node', $node);
    if (empty($form_state['line_items']) && !$form_state['executed']) {
      $form_state['line_items'] = $node_wrapper->field_order_line_item->value();
    }

    foreach ($form_state['line_items'] as $key => $fc_line_item) {
      $fc_line_item_wrapper = entity_metadata_wrapper('field_collection_item', $fc_line_item);
      $product = reset($fc_line_item_wrapper->field_product->value());
      $product_wrapper = entity_metadata_wrapper('commerce_product', $product);

      // Create a pseudo line item so we can calculate sell price properly.
      // @see commerce_product_calculate_sell_price().
      $line_item = commerce_product_line_item_new($product);
      $line_item->quantity = $fc_line_item_wrapper->field_product_quantity->value();
      $line_item->field_line_item_frequency[LANGUAGE_NONE][0]['value'] = $node_wrapper->field_frequency->value();
      drupal_alter('commerce_product_calculate_sell_price_line_item', $line_item);
      rules_invoke_event('commerce_product_calculate_sell_price', $line_item);
      $line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);

      $image = $product_wrapper->field_image->value();
      $form['line_items_wrapper']['line_items']['#tf_rows'][] = array(
        'item_info' => array(
          '#data' => '<img src="' . image_style_url('commerce_line_item_thumbnail', $image['uri']) . '" /><div class="display-title">' . $product_wrapper->field_product_display_title->value() . '</div><div class="title">' . $product_wrapper->title_field->value() . '</div>',
          '#cell_attributes' => array(
            'class' => array('item-information'),
          ),
        ),
        'item_price' => array(
          '#data' => metagenics_custom_product_get_formatted_prices($line_item_wrapper, FALSE, FALSE),
          '#cell_attributes' => array(
            'class' => array('item-price'),
          ),
        ),
        $key => array(
          '#type' => 'container',
          '#tree' => TRUE,
          '#cell_attributes' => array(
            'class' => array('quantity'),
          ),
          'fc_item' => array(
            '#type' => 'value',
            '#value' => $fc_line_item_wrapper->value(),
          ),
          'product_id' => array(
            '#type' => 'value',
            '#value' => $product->product_id,
          ),
          'quantity' => array(
            '#type' => 'textfield',
            '#size' => 4,
            '#maxlength' => 4,
            '#default_value' => $line_item->quantity,
          ),
        ),
        'item_total' => array(
          '#data' => metagenics_commerce_recurring_line_item_total($line_item_wrapper),
          '#cell_attributes' => array(
            'class' => array('item-total'),
          ),
        ),
        'delete_' . $key => array(
          '#type' => 'submit',
          '#value' => t('Remove'),
          '#name' => 'delete-line-item-' . $key,
          '#limit_validation_errors' => array(),
          '#submit' => array('metagenics_commerce_recurring_remove_line_item'),
          '#ajax' => array(
            'callback' => 'metagenics_commerce_recurring_line_item_ajax',
            'wrapper' => 'line-items-wrapper',
          ),
        ),
      );
    }

    $form['line_items_wrapper']['line_items_new'] = array(
      '#type' => 'container',
      '#attributes' => array(
        '#id' => 'line-items-new',
      ),
      '#tree' => TRUE,
    );
    $form['line_items_wrapper']['line_items_new']['prefix'] = array(
      '#markup' => '<span class="line-item-new-prefix">' . t('Add Product:') . '</span>',
    );

    $form['line_items_wrapper']['line_items_new']['product_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Sku or Product Name'),
      '#autocomplete_path' => 'metagenics_commerce_recurring/ajax/product',
      '#element_validate' => array('_metagenics_commerce_recurring_autocomplete_validate'),
      '#default_value' => '-- Select Product --',
      '#attributes' => array(
        'class' => array('line-items-new-product-id'),
        'placeholder' => '-- Select Product --',
      ),
    );

    $form['line_items_wrapper']['line_items_new']['quantity'] = array(
      '#type' => 'textfield',
      '#title' => t('Qty'),
      '#size' => 4,
      '#maxlength' => 4,
      '#default_value' => 1,
    );

    $form['line_items_wrapper']['line_items_new']['add_new'] = array(
      '#type' => 'submit',
      '#value' => t('Add'),
      '#limit_validation_errors' => array(
        array('nid'),
        array('line_items_new'),
        array('line_items'),
      ),
      '#validate' => array('metagenics_commerce_recurring_add_new_line_item_validate'),
      '#submit' => array('metagenics_commerce_recurring_add_new_line_item'),
      '#ajax' => array(
        'callback' => 'metagenics_commerce_recurring_line_item_ajax',
        'wrapper' => 'line-items-wrapper',
      ),
    );

    // Modify shipping and billing.
    $addressfields = array('field_shipping_information', 'field_billing_information');

    foreach ($addressfields as $addressfield) {
      $field = field_info_field($addressfield);
      $profiles = commerce_customer_profile_load_multiple(array(), array(
        'type' => $field['settings']['profile_type'],
        'uid' => $node->uid, 'status' => TRUE,
      ));

      if ($profiles) {
        // Prepare the options.
        $options = array();
        foreach ($profiles as $id => $profile) {
          $field_values = field_get_items('commerce_customer_profile', $profile, 'commerce_customer_address');
          $options[$id] = $field_values[0]['thoroughfare'];
        }
        drupal_alter('commerce_addressbook_labels', $options, $profiles);

        // Prepare the default value.
        $default_value = 'none';
        if ($node_wrapper->{$addressfield}->profile_id->value()) {
          $default_value = $node_wrapper->{$addressfield}->profile_id->value();
        }

        $form[$addressfield]['#type'] = 'fieldset';
        $form[$addressfield]['#title'] = $form[$addressfield][LANGUAGE_NONE]['profiles'][0]['#title'];
        $form[$addressfield][LANGUAGE_NONE]['profiles'][0]['#type'] = 'container';

        $form[$addressfield]['#prefix'] = '<div id="' . strtr($addressfield, '_', '-') . '-ajax-wrapper">';
        $form[$addressfield]['#suffix'] = '</div>';
        $form[$addressfield]['addressbook_entries'] = array(
          '#type' => 'value',
          '#value' => $profiles,
        );

        $form[$addressfield]['addressbook'] = array(
          '#type' => 'select',
          '#title' => t('Addresses on File'),
          '#description' => t('You may select a pre-existing address on file.'),
          '#options' => $options,
          '#empty_option' => t('-- Choose --'),
          '#empty_value' => 'none',
          '#ajax' => array(
            'callback' => 'commerce_addressbook_checkout_form_callback',
            'wrapper' => strtr($addressfield, '_', '-') . '-ajax-wrapper',
          ),
          '#element_validate' => array('metagenics_commerce_recurring_customer_profile_validate'),
          '#weight' => -100,
          '#default_value' => $default_value,
        );
      }
    }

    // This is a little weird, but we're trying to mimic commerce's handling of
    // copying an address from one field to another, so the default value of
    // the "field" is being stored in $node->data.
    // @see commerce_customer_profile_pane_checkout_form()
    $node_data = !empty($node->data) ? unserialize($node->data) : array();

    $source_profile_type = commerce_customer_profile_type_load('shipping');
    $target_profile_type = commerce_customer_profile_type_load('billing');
    $form['field_billing_information']['commerce_recurring_profile_copy'] = array(
      '#type' => 'checkbox',
      '#title' => t('My %target is the same as my %source.', array('%target' => $target_profile_type['name'], '%source' => $source_profile_type['name'])),
      '#element_validate' => array('metagenics_commerce_recurring_customer_profile_copy_validate'),
      '#default_value' => isset($node_data['profile_copy']['field_billing_information']['status']) ? $node_data['profile_copy']['field_billing_information']['status'] : 0,
      '#weight' => -30,
      '#ajax' => array(
        'callback' => 'metagenics_commerce_recurring_customer_profile_copy_refresh',
        'wrapper' => 'field-billing-information-ajax-wrapper',
      ),
      '#attached' => array(
        'css' => array(drupal_get_path('module', 'commerce_customer') . '/theme/commerce_customer.theme.css'),
      ),
      '#prefix' => '<div class="commerce-customer-profile-copy">',
      '#suffix' => '</div>',
    );

    // If the order data has reference to fields that were copied over, hide
    // them so we don't confuse the user by still allowing them to edit values.
    if (!empty($node_data['profile_copy']['field_billing_information']['status']) && isset($node_data['profile_copy']['field_billing_information']['elements'])) {
      foreach ($node_data['profile_copy']['field_billing_information']['elements'] as $field_name => $field) {
        foreach ($field as $langcode => $items) {
          if (is_array($items)) {
            foreach ($items as $delta => $item) {
              if (!empty($form['field_billing_information'][$langcode][$delta])) {
                $form['field_billing_information'][$langcode][$delta]['#access'] = FALSE;
              }
              elseif (!empty($form['field_billing_information'][$langcode])) {
                $form['field_billing_information'][$langcode]['#access'] = FALSE;
              }
            }
          }
          else {
            $form['field_billing_information'][$langcode]['#access'] = FALSE;
          }
        }
      }
    }

    $form['field_billing_information']['cc_options'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'id' => 'credit-card-options',
      ),
    );

    $form['field_billing_information']['cc_options']['card_images'] = array(
      '#markup' => '<div class="commerce-wepay-icons">' . implode(' ', commerce_wepay_cc_auth_icons()) . '</div>',
    );

    $form['field_billing_information']['cc_options']['card_option'] = array(
      '#type' => 'radios',
      '#options' => array(
        0 => t('Current credit card information'),
        1 => t('Enter new credit card information'),
      ),
      '#default_value' => 0,
      '#ajax' => array(
        'callback' => 'metagenics_commerce_recurring_cc_options_ajax',
        'wrapper' => 'credit-card-options',
      ),
      '#suffix' => '<div class="hsa-warning">' . t('HSA/FSA Cards cannot be accepted') . '</div>',
    );

    if (!empty($form_state['input']['field_billing_information']['cc_options']['card_option'])) {
      // Use the commerce provided credit card form for consistency.
      module_load_include('inc', 'commerce_payment', 'includes/commerce_payment.credit_card');
      $credit_card_settings = array(
        'type' => _commerce_wepay_cc_auth_all_card_types(),
        'number' => '',
        'code' => '',
      );
      $form['field_billing_information']['cc_options']['new_card_info'] = commerce_payment_credit_card_form($credit_card_settings);
    }
    else {
      $wepay_card_id = $node_wrapper->author->field_wepay_credit_card_id->value();
      $form['field_billing_information']['cc_options']['existing_card_info']['#markup'] = metagenics_commerce_recurring_wepay_card_information($wepay_card_id);
    }

    $form['field_billing_information']['cc_disclaimer'] = array(
      '#type' => 'container',
      '#states' => array(
        'visible' => array(
          ':input[name="field_billing_information[cc_options][card_option]"]' => array('value' => '1'),
        ),
      ),
      '#attributes' => array(
        'class' => array('credit-card-disclaimer', 'display-warning'),
      ),
      'disclaimer' => array(
        '#markup' => t('<strong>Warning:</strong> Changing your credit card information here will update your credit card information for all recurring orders on this account.'),
      ),
    );

    $form['field_billing_information']['billed_shipped_notice'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('billed-shipped-notice'),
      ),
      'Notice' => array(
        '#markup' => 'Your Credit Card will not be charged until order has been shipped.',
      ),
    );

    // Modify Form actions.
    array_unshift($form['actions']['submit']['#submit'], 'metagenics_commerce_recurring_order_form_submit');
    $form['actions']['submit']['#submit'][] = 'metagenics_commerce_recurring_order_form_redirect';
    $form['actions']['preview']['#access'] = FALSE;
    $form['actions']['delete']['#access'] = FALSE;
    $form['actions']['preview_changes']['#access'] = FALSE;
    $form['actions']['cancel'] = array(
      '#theme' => 'link',
      '#text' => t('Cancel'),
      '#path' => 'account/' . $account->uid . '/orders/recurring-orders',
      '#options' => array(
        'attributes' => array(
          'class' => array('button'),
        ),
        'html' => FALSE,
      ),
      '#weight' => 10,
    );

    $form['#validate'][] = 'metagenics_commerce_recurring_add_new_line_item_validate';
  }

  $form['#validate'][] = 'metagenics_commerce_recurring_order_form_validate';

  // Remove additional settings so we don't have issues with inconsistencies.
  $form['additional_settings']['#access'] = FALSE;

  // After revoking access to the additional settings, we set these values
  // programmatically.
  $form['options']['status']['#value'] = 1;
  $form['language']['#value'] = 'und';

  // Add our custom js.
  $form['#attached']['js'][] = drupal_get_path('module', 'metagenics_commerce_recurring') . '/js/metagenics_commerce_recurring.js';

  if (!empty($form_state['values']['field_billing_information']['commerce_recurring_profile_copy'])) {
    //$form['field_billing_information']['addressbook']['#access'] = FALSE;
    //$form['field_billing_information']['addressbook_entries']['#access'] = FALSE;
    //$form['field_billing_information']['und']['#access'] = FALSE;
  }

  $hidden_state = array(
    'visible' => array(
      ':input[name="field_billing_information[commerce_recurring_profile_copy]"]' => array('checked' => FALSE),
    ),
  );

  $form['field_billing_information']['und']['profiles'][0]['commerce_customer_address']['#states'] = $hidden_state;
  $form['field_billing_information']['und']['profiles'][0]['field_contact_phone_number']['#states'] = $hidden_state;
  $form['field_billing_information']['addressbook'] = $hidden_state;
  $form['field_billing_information']['addressbook_entries'] = $hidden_state;
  $form['field_aro_gen_orders_reference']['#access'] = FALSE;
  $form['field_aro_origin_order_ref']['#access'] = FALSE;
  drupal_process_states($form);

}

/**
 * AJAX callback to return cc options.
 */
function metagenics_commerce_recurring_cc_options_ajax(&$form, &$form_state) {
  return $form['field_billing_information']['cc_options'];
}

/**
 * AJAX callback to recalculate next ship date.
 */
function metagenics_commerce_recurring_recalc_date_ajax(&$form, &$form_state) {

  return $form['fieldset_next_order_date'];
}

/**
 * Element validation for next order date.
 */
function metagenics_commerce_recurring_next_ship_date_validate($element, &$form_state, $form) {
  // Ensure next order date is in the future.
  if (!empty($form_state['values']['field_aro_next_ship_date'])) {
    $next_date = $form_state['values']['field_aro_next_ship_date'][LANGUAGE_NONE][0]['value'];
    $next_timestamp = strtotime($next_date);
    $min_timestamp = strtotime('today');
    if ($next_timestamp < $min_timestamp) {
      form_set_error($element['#field_name'], t('Next Order Date must be in the future.'));
    }
    else {
      $day = strtolower(date('l', $next_timestamp));
      // if ($day === 'sunday') {
      //   form_set_error($element['#field_name'], t('Next Order Date must not be on a Sunday.'));
      // }
    }
  }
}

/**
 * Validation callback for customer profile form.
 */
function metagenics_commerce_recurring_customer_profile_validate($element, &$form_state, $form) {
  if (in_array('addressbook', $form_state['triggering_element']['#parents']) && $form_state['triggering_element']['#id'] == $element['#id']) {
    $field_name = $element['#parents'][0];
    $node_wrapper = entity_metadata_wrapper('node', $form_state['node']);
    if (is_numeric($element['#value'])) {
      $profile = commerce_customer_profile_load($element['#value']);
      // Validate that the profile being selected is owned by this user.
      if ($profile->uid != $node_wrapper->author->uid->value()) {
        drupal_set_message(t('You must own the profile you are choosing.'), 'error');
        return;
      }
      // If we detect a change in the element's value, and the customer profile
      // reference isn't already set to the specified value...
      if ($node_wrapper->{$field_name}->raw() != $element['#value']) {
        // Update the order based on the value and rebuild the form.
        if ($element['#value'] == 0) {
          $node_wrapper->{$field_name} = NULL;
        }
        else {
          $node_wrapper->{$field_name} = $element['#value'];
        }
      }
    }
    else {
      $node_wrapper->{$field_name} = NULL;
    }
    unset($form_state['input'][$field_name]);
    // dsm($form);
    $element_key = $form[$field_name][LANGUAGE_NONE]['profiles'][0]['commerce_customer_address'][LANGUAGE_NONE][0]['element_key']['#value'];
    unset($form_state['addressfield'][$element_key]);
  }
}

/**
 * Custom Validation for the product autocomplete field.
 */
function _metagenics_commerce_recurring_autocomplete_validate($element, &$form_state, $form) {
  // If a value was entered into the autocomplete...
  $value = '';
  if (!empty($element['#value'])) {
    // Take "label (entity id)', match the id from parenthesis.
    if (preg_match("/.+\((\d+)\)/", $element['#value'], $matches)) {
      $value = $matches[1];
    }
    elseif ($element['#value'] !== t('-- Select Product --')) {
      form_error($element, t("The product name you've entered does not exist or is no longer available, please try again."));
    }
  }
  // Update the value of this element so the field can validate the product IDs.
  form_set_value($element, $value, $form_state);
}

/**
 * Custom validation for the entire ARO template form...I think.
 */
function metagenics_commerce_recurring_order_form_validate(&$form, &$form_state) {
  $values = $form_state['values'];

  if (!empty($values['field_billing_information']['cc_options']['new_card_info']['credit_card'])) {
    $card_info = $values['field_billing_information']['cc_options']['new_card_info']['credit_card'];
    $node_wrapper = entity_metadata_wrapper('node', $form_state['node']);
    $account_wrapper = $node_wrapper->author;

    module_load_include('inc', 'commerce_payment', 'includes/commerce_payment.credit_card');
    // Validate the credit card fields.
    $settings = array(
      'form_parents' => array(
        'field_billing_information',
        'cc_options',
        'new_card_info',
        'credit_card',
      ),
    );

    // Check if settings are overriding this check.
    $perform_card_validation_check = TRUE;

    $payment_method = commerce_payment_method_instance_load('commerce_wepay_cc_auth|commerce_payment_commerce_wepay_cc_auth');
    if ($payment_method['settings']['server'] == 'staging' && $payment_method['settings']['bypass_commerce_card_validation'] == 1) {
      $perform_card_validation_check = FALSE;
    }

    // This will run CC data through Drupal Commerce's validation check first.
    if ($perform_card_validation_check) {
      if (!commerce_payment_credit_card_validate($card_info, $settings)) {
        return;
      }
    }

    // Call the WePay API to create a checkout for this order.
    // Include in WePay API.
    require_once drupal_get_path('module', 'commerce_wepay_cc_auth') . '/wepay.php';

    $client_id = $payment_method['settings']['client_id'];
    $client_secret = $payment_method['settings']['client_secret'];
    $access_token = $payment_method['settings']['access_token'];

    if ($payment_method['settings']['server'] == 'production') {
      WePay::useProduction($client_id, $client_secret);
    }
    else {
      WePay::useStaging($client_id, $client_secret);
    }

    try {
      $wepay = new WePay(NULL);

      // Get the users billing address.
      $profile_id = $values['field_billing_information'][LANGUAGE_NONE][0]['profile_id'];
      $profile = commerce_customer_profile_load($profile_id);

      if (!$profile) {
        form_set_error('field_billing_information', t('You must complete billing information to authorize a new credit card.'));
        return;
      }

      $create_values = array(
        'client_id' => $client_id,
        'user_name' => $profile->commerce_customer_address[LANGUAGE_NONE][0]['first_name'] . ' ' . $profile->commerce_customer_address[LANGUAGE_NONE][0]['last_name'],
        'email' => $account_wrapper->mail->value(),
        'cc_number' => $card_info['number'],
        'cvv' => $card_info['code'],
        'expiration_month' => $card_info['exp_month'],
        'expiration_year' => $card_info['exp_year'],
        'address' => array(
          'address1' => $profile->commerce_customer_address[LANGUAGE_NONE][0]['thoroughfare'] . ', ' . $profile->commerce_customer_address[LANGUAGE_NONE][0]['premise'],
          'city' => $profile->commerce_customer_address[LANGUAGE_NONE][0]['locality'],
          'state' => $profile->commerce_customer_address[LANGUAGE_NONE][0]['administrative_area'],
          'zip' => $profile->commerce_customer_address[LANGUAGE_NONE][0]['postal_code'],
          'country' => 'US',
        ),
      );

      $create = $wepay->request('credit_card/create', $create_values);

      $authorize_values = array(
        'client_id' => $client_id,
        'client_secret' => $client_secret,
        'credit_card_id' => $create->credit_card_id,
      );

      $authorize = $wepay->request('credit_card/authorize', $authorize_values);

      $account_wrapper->field_wepay_credit_card_id->set($authorize->credit_card_id);
      $account_wrapper->save();
    }
    catch (WePayException $e) {
      watchdog('Metagenics - WePay CC Authorize', $e->getMessage(), array(), WATCHDOG_ERROR);

      $prefix = 'field_billing_information][cc_options][new_card_info][credit_card][';
      if (strpos($e->getMessage(), 'CVV')) {
        form_set_error($prefix . 'code', $e->getMessage());
      }
      elseif (strpos($e->getMessage(), 'expiration')) {
        form_set_error($prefix . 'exp_year', $e->getMessage());
        form_set_error($prefix . 'exp_month', $e->getMessage());
        $_SESSION['messages']['error'] = array_unique($_SESSION['messages']['error']);
      }
      else {
        form_set_error($prefix . 'number', $e->getMessage());
      }
    }
  }
}

/**
 * Submit callback for recurring order form.
 */
function metagenics_commerce_recurring_order_form_submit(&$form, &$form_state) {
  $values = &$form_state['values'];
  if (!empty($values['line_items'])) {
    $values['field_order_line_item'][LANGUAGE_NONE] = array();
    foreach ($values['line_items'] as $line_item) {
      if (is_array($line_item) && !empty($line_item['quantity'])) {
        $fc_item = $line_item['fc_item'];
        $fc_item->field_product[LANGUAGE_NONE]['0']['product_id'] = $line_item['product_id'];
        $fc_item->field_product_quantity[LANGUAGE_NONE]['0']['value'] = $line_item['quantity'];
        $values['field_order_line_item'][LANGUAGE_NONE][] = array(
          'field_product' => array(
            LANGUAGE_NONE => array(
              '0' => array(
                'product_id' => $line_item['product_id'],
              ),
            ),
          ),

          'field_product_quantity' => array(
            LANGUAGE_NONE => array(
              '0' => array(
                'value' => $line_item['quantity'],
              ),
            ),
          ),

          'entity' => $fc_item,
        );
      }
    }
  }
  if (!empty($values['line_items_new']['product_id']) && !empty($values['line_items_new']['quantity'])) {
    $fc_item = meta_field_collection_item_create('field_order_line_item', 'node', $form_state['values']['nid']);
    $fc_item->field_product[LANGUAGE_NONE][0]['product_id'] = $values['line_items_new']['product_id'];
    $fc_item->field_product_quantity[LANGUAGE_NONE][0]['value'] = $values['line_items_new']['quantity'];
    $values['field_order_line_item'][LANGUAGE_NONE][] = array(
      'field_product' => array(
        LANGUAGE_NONE => array(
          '0' => array(
            'product_id' => $values['line_items_new']['product_id'],
          ),
        ),
      ),

      'field_product_quantity' => array(
        LANGUAGE_NONE => array(
          '0' => array(
            'value' => $values['line_items_new']['quantity'],
          ),
        ),
      ),

      'entity' => $fc_item,
    );
  }
}

/**
 * Sets redirect after recurring order form.
 */
function metagenics_commerce_recurring_order_form_redirect(&$form, &$form_state) {
  $form_state['redirect'] = 'account/' . $form_state['values']['uid'] . '/orders/recurring-orders';
}

/**
 * Validation callback for the add/save buttons.
 */
function metagenics_commerce_recurring_add_new_line_item_validate(&$form, &$form_state){
  $new = $form_state['values']['line_items_new'];

  // Make sure there are some products in the order or going to be added to the
  // order.
  if (empty($form_state['values']['line_items']) && empty($form_state['values']['line_items_new']['product_id']) && empty($form_state['line_items'])) {
    form_set_error('line_items_wrapper', t('An order cannot be empty.'));
  }

  // Make sure quantities are valid.
  if (!empty($new['product_id']) && !empty($new['quantity']) && !is_numeric($new['quantity'])) {
    form_set_error('line_items_new][quantity', t('Please enter a numeric value for the quantity.'));
  }

  // Make sure quantities are not zero when adding new products.
  if (!empty($new['product_id']) && empty($new['quantity'])) {
    form_set_error('line_items_new][quantity', t('Please enter a quantity greater than zero.'));
  }

  if (!empty($form_state['values']['line_items'])) {
    $line_items = $form_state['values']['line_items'];
    $quanity_check = FALSE;

    // Check each item in the order and make sure it has a valid quanitty.
    foreach ($line_items as $key => $value) {
      if (is_array($value) && !empty($value['quantity']) && !is_numeric($value['quantity'])) {
        form_set_error('line_items][' . $key, t('Please enter a numeric value for the quantity.'));
      }

      // If even one item in the order has a non zero quanity we can let the
      // quanity check pass.
      if (is_array($value) && !empty($value['quantity']) && is_numeric($value['quantity'])) {
        $quanity_check = TRUE;
      }
    }

    // If we're trying to add a product it also counts as passing the quanitty
    // check.
    if (!empty($new['product_id']) && !empty($new['quantity']) && is_numeric($new['quantity'])) {
      $quanity_check = TRUE;
    }

    // If quantity check it still false at this point it means the user is trying
    // to remove or save an empty order without trying to add a new item, this
    // cannot be allowed.
    if (!$quanity_check) {
      form_set_error('line_items', t('An order cannot be empty.'));
      foreach ($line_items as $key => $value) {
        form_set_error('line_items][' . $key . '][quantity');
      }
    }
  }
}

/**
 * Helper function to add a new line item.
 */
function metagenics_commerce_recurring_add_new_line_item(&$form, &$form_state) {
  if(!empty($form_state['values']['line_items_new']['product_id'])){
    $fc_item = meta_field_collection_item_create('field_order_line_item', 'node', $form_state['values']['nid']);
    $fc_item->field_product[LANGUAGE_NONE][0]['product_id'] = $form_state['values']['line_items_new']['product_id'];
    $fc_item->field_product_quantity[LANGUAGE_NONE][0]['value'] = $form_state['values']['line_items_new']['quantity'];
    $form_state['line_items'][] = $fc_item;

    // Remove the input values so the form is "fresh".
    unset($form_state['input']['line_items_new']);

    $form_state['rebuild'] = TRUE;
  }
}

/**
 * Removes line item from form.
 */
function metagenics_commerce_recurring_remove_line_item(&$form, &$form_state) {
  if (strpos($form_state['triggering_element']['#name'], 'delete-line-item-') === 0) {
    $key = substr($form_state['triggering_element']['#name'], 17);
    unset($form_state['line_items'][$key]);

    //$form_state['line_items'] = array_values($form_state['line_items']);
  }

  $form_state['rebuild'] = TRUE;
}

/**
 * AJAX callback for line items.
 */
function metagenics_commerce_recurring_line_item_ajax(&$form, &$form_state) {
  return $form['line_items_wrapper'];
}

/**
 * Calculate the line item total.
 */
function metagenics_commerce_recurring_line_item_total($wrapper) {
  $output = array();

  $price = $wrapper->commerce_unit_price->value();
  $total = $price['amount'] * $wrapper->quantity->value();

  $base_price = 0;
  foreach ($price['data']['components'] as $component) {
    if ($component['name'] === 'base_price') {
      $base_price += $component['price']['amount'];
    }
  }
  $base_price = $base_price * $wrapper->quantity->value();

  $formatted_total = commerce_currency_format($total, $price['currency_code']);
  $output['total_price'] = array(
    '#prefix' => '<div class="custom_checkout_base_total">',
    '#suffix' => '</div>',
    'price' => array(
      '#markup' => $formatted_total,
    ),
  );

  if ($total !== $base_price) {
    $formatted_savings = commerce_currency_format($base_price - $total, $price['currency_code']);
    $output['total_savings'] = array(
      '#prefix' => '<div class="custom_checkout_price_difference_price">',
      '#suffix' => '</div>',
      'price' => array(
        '#prefix' => '<em style="color: #8BA13E;">' . t('Savings:') . ' ',
        '#suffix' => '</em>',
        '#markup' => $formatted_savings,
      ),
    );
  }

  return $output;
}

/**
 * Autocomplete callback for ARO products.
 */
function metagenics_commerce_recurring_ajax_autocomplete_product($string = '') {
  global $language;

  $matches = array();
  if ($string) {
    $query = db_select('commerce_product', 'cp');
    $query->innerJoin('field_data_field_product_display_title', 'fdfpdt', "fdfpdt.entity_type = 'commerce_product' AND fdfpdt.entity_id = cp.product_id");
    $query->innerJoin('field_data_title_field', 'fdtf', "fdtf.entity_type = 'commerce_product' AND fdtf.entity_id = cp.product_id");
    $query->fields('fdfpdt', array('field_product_display_title_value'))
    ->fields('fdtf', array('title_field_value', 'entity_id'))
    ->fields('cp', array('sku'))
    ->condition('fdtf.language', $language->language)
    ->condition('cp.status', 1)
    ->orderBy('field_product_display_title_value', 'ASC')
    ->range(0, 10);

    $or = db_or()
    ->condition('field_product_display_title_value', strtr($string, ' ', '%') . '%', 'LIKE')
    ->condition('sku', db_like($string) . '%', 'LIKE');

    $query->condition($or);

    $result = $query->execute();
    foreach ($result as $product) {
      $matches[$product->field_product_display_title_value . ': ' . $product->title_field_value . ' (' . $product->entity_id . ')'] = $product->sku . ' - ' . $product->field_product_display_title_value . ': ' . $product->title_field_value;
    }
  }

  drupal_json_output($matches);
}

/**
 * Get the credit card info from WePay.
 *
 * Show on the Checkout Payment and Review pages.
 */
function metagenics_commerce_recurring_wepay_card_information($wepay_card_id) {
  $content = '<div class="payment-details">';

  // Call the WePay API to get the credit card details for the WePay CC ID.
  // Include in WePay API.
  require_once drupal_get_path('module', 'commerce_wepay_cc_auth') . '/wepay.php';

  $payment_method = commerce_payment_method_instance_load('commerce_wepay_cc_auth|commerce_payment_commerce_wepay_cc_auth');
  $client_id = $payment_method['settings']['client_id'];
  $client_secret = $payment_method['settings']['client_secret'];

  if ($payment_method['settings']['server'] == 'production') {
    WePay::useProduction($client_id, $client_secret);
  }
  else {
    WePay::useStaging($client_id, $client_secret);
  }

  try {
    $wepay = new WePay(NULL);

    $values = array(
      'client_id' => $client_id,
      'client_secret' => $client_secret,
      'credit_card_id' => $wepay_card_id,
    );

    $credit_card_details = $wepay->request('credit_card', $values);

    // Set Credit Card state in $order
    // $order->credit_card_state = $credit_card_details->state;

    // Substitute an image for the credit card name is the string returned from
    // the WePay API.
    $wepay_credit_card_name = replace_credit_card_name($credit_card_details->credit_card_name);

    $content .= '<div class="payment-details-name">' . $credit_card_details->user_name . '</div>';
    $content .= '<div class="payment-details-card-info">' . $wepay_credit_card_name . '</div>';

    switch ($credit_card_details->state) {
      case 'expired':
        $message = t('The credit card on file is expired. Please enter new credit card information.');
        break;

      case 'deleted':
        $message = t('The credit card on file has been deleted. Please enter new credit card information.');
        break;

      case 'invalid':
        $message = t('The credit card on file is invalid. Please enter new credit card information.');
        break;
    }

    if (!empty($message)) {
      drupal_set_message($message, 'warning');
    }
  }
  catch (WePayException $e) {
    watchdog('Metagenics - WePay CC Details', $e->getMessage(), array(), WATCHDOG_ERROR);
    $content .= t('Payment information is not available.');
    metagenics_logging_log_add_record('wepay_cc_failed', 'Payment information is not available.', $e->getMessage());
  }
  $content .= '</div>';

  return $content;
}

/**
 * Element validate callback: Pertaining to the "copy profile" checkbox.
 */
function metagenics_commerce_recurring_customer_profile_copy_validate($element, &$form_state, $form) {
  $triggering_element = end($form_state['triggering_element']['#array_parents']);
  $field = reset($element['#array_parents']);
  $node_data = !empty($form_state['node']->data) ? unserialize($form_state['node']->data) : array();

  // Checkbox: Off - Only invoked for the corresponding trigger element.
  if ($triggering_element == 'commerce_recurring_profile_copy' && $form_state['triggering_element']['#id'] == $element['#id'] && empty($element['#value'])) {
    // $form_state['order']->data['profile_copy'][$field]['status'] = FALSE;
    // unset($form_state['order']->data['profile_copy'][$field]['elements']);
    // commerce_order_save($form_state['order']);
  }

  // Checkbox: On - Only invoked for the corresponding trigger element, or the
  // "continue" checkout form button.
  elseif ((($triggering_element == 'commerce_recurring_profile_copy' && $form_state['triggering_element']['#id'] == $element['#id']) || $triggering_element == 'continue') && !empty($element['#value'])) {
    $type = 'billing';  // Removes 'customer_profile_'
    $source_id = 'field_shipping_information';
    $info = array('commerce_customer_profile', $type, $field);

    // Try getting the source profile from the form_state values, if it is
    // present on the form..
    if (isset($form_state['values'][$source_id])) {
      metagenics_commerce_recurring_customer_profile_copy_fields($info, $form_state['input'][$field][LANGUAGE_NONE]['profiles'][0], $form_state['input'][$source_id][LANGUAGE_NONE]['profiles'][0], $form_state);
      metagenics_commerce_recurring_customer_profile_copy_fields($info, $form_state['values'][$field][LANGUAGE_NONE]['profiles'][0], $form_state['values'][$source_id][LANGUAGE_NONE]['profiles'][0], $form_state);
    }

    // Otherwise, attempt to get source profile from the order object.
    else {
      // Check for source profile via order wrapper.
      $wrapper = entity_metadata_wrapper('node', $form_state['node']);
      $profile = NULL;

      $profile = $wrapper->{$source_id}->value();
      if (!empty($node_data['profiles'][$source_id])) {
        $profile = commerce_customer_profile_load($node_data['profiles'][$source_id]);
      }

      if (!empty($profile)) {
        metagenics_commerce_recurring_customer_profile_copy_fields($info, $form_state['input'][$field], $profile, $form_state);
        metagenics_commerce_recurring_customer_profile_copy_fields($info, $form_state['values'][$field], $profile, $form_state);
      }

      $node_data['profile_copy'][$field]['status'] = TRUE;
      $form_state['node']->data = serialize($node_data);
    }

    // Unset any cached addressfield data for this customer profile.
    if (!empty($form_state['addressfield'])) {
      foreach ($form_state['addressfield'] as $key => $value) {
        if (strpos($key, 'commerce_customer_profile|' . $type) === 0) {
          unset($form_state['addressfield'][$key]);
        }
      }
    }
  }
}

/**
 * Copy field values from a source profile to a target array.
 *
 * @param array $info
 *   An array containing info for the entity type, bundle, and pane ID.
 * @param array $target
 *   An array (typically $form_state) in which values will be copied to.
 * @param mixed $source
 *   Can be either an array or object of values.
 * @param array $form_state
 *   The form state array from the form.
 */
function metagenics_commerce_recurring_customer_profile_copy_fields($info, &$target, $source, &$form_state) {
  list($entity_type, $bundle, $field) = $info;

  // Loop over all the field instances that could be attached to this entity.
  foreach (field_info_instances($entity_type, $bundle) as $field_name => $instance) {
    $field = NULL;

    // Extract the field value from the source object or array.
    if (is_object($source) && isset($source->{$field_name})) {
      $field = $source->{$field_name};
    }
    elseif (is_array($source) && isset($source[$field_name])) {
      $field = $source[$field_name];
    }

    // Loop over the source field value and copy its items to the target.
    if (is_array($field)) {
      foreach ($field as $langcode => $items) {
        if (is_array($items)) {
          $target[$field_name][$langcode] = array();

          foreach ($items as $delta => $item) {
            $target[$field_name][$langcode][$delta] = $item;
            $node_data = !empty($form_state['node']->data) ? unserialize($form_state['node']->data) : array();
            $node_data['profile_copy'][$field]['elements'][$field_name][$langcode][$delta] = TRUE;
            $form_state['node']->data = serialize($node_data);
          }
        }
        else {
          $target[$field_name][$langcode] = $items;
          $node_data = !empty($form_state['node']->data) ? unserialize($form_state['node']->data) : array();
          $node_data['profile_copy'][$field]['elements'][$field_name][$langcode] = TRUE;
          $form_state['node']->data = serialize($node_data);
        }
      }
    }
  }
}

/**
 * Ajax callback: Returns ajax command to refresh customer profile pane.
 */
function metagenics_commerce_recurring_customer_profile_copy_refresh($form, &$form_state) {
  $node_data = unserialize($form_state['node']->data);
  return $form['field_billing_information'];
}
