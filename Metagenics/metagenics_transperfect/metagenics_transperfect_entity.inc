<?php

require_once TPT_ROOT . '/' . ('transperfect_settings.inc');
require_once TPT_ROOT . '/' . ('transperfect_common.inc');
require_once TPT_ROOT . '/' . ('gl_ws/gl_ws_common.inc');

function entity_build_filters() {

  $filters = array();
/*
  $filters['modified'] = array(
      'title' => 'Show',
      'field' => 'changed',
      'form-type' => 'radios',
      'options' => array('&nbsp;&nbsp;' . t('Modified Content') . '&nbsp;&nbsp;&nbsp;&nbsp;',
          '&nbsp;&nbsp;' . t('Everything') . '&nbsp;&nbsp;'),
  );

  $revisioning_module_exists = module_exists("revisioning");
  if ($revisioning_module_exists) {
    $filters['status'] = array(
        'title' => TPT_LABEL_STATUS,
        'options' => array(
            'status-1' => t('Latest Published Revision'),
            'status-0' => t('Latest Modified Revision'),
            'promote-1' => t('Promoted'),
            'promote-0' => t('Not Promoted'),
            'sticky-1' => t('Sticky'),
            'sticky-0' => t('Not Sticky'),
        ),
        'form-type' => 'select',
    );
  }
  else {
    $filters['status'] = array(
        'title' => TPT_LABEL_STATUS,
        'options' => array(
            '[any]' => t('Any'),
            'status-1' => t('Published'),
            'status-0' => t('Not Published'),
            'promote-1' => t('Promoted'),
            'promote-0' => t('Not Promoted'),
            'sticky-1' => t('Sticky'),
            'sticky-0' => t('Not Sticky'),
        ),
        'form-type' => 'select',
    );
  }
*/
  $languages = get_mapped_drupal_locales(FALSE);
  foreach ($languages as $key => $lang) {
    $lang_filter[$key] = $lang;
  }

  $filters['language_name'] = array(
      'title' => TPT_LABEL_SOURCE_LANG,
      'field' => 'language',
      'options' => $lang_filter,
      'form-type' => 'select',
  );

  $filters['node_parent'] = array(
      'title' => t('Parent Node'),
      'field' => 'node_parent',
      'form-type' => 'checkbox',
  );
/*
  $filters['target_language'] = array(
      'title' => TPT_LABEL_TARGET_LANG,
      'field' => 'language',
      'options' => $lang_filter,
      'form-type' => 'select',
  );
*/
/*
  $n_arr = array('[any]' => t('Any'));
  $t_arr = get_translatable_node_types_and_names();
  $node_types_filter = $n_arr + $t_arr;

  $filters['type'] = array(
      'title' => TPT_LABEL_CONTENT_TYPE,
      'field' => 'type',
      'options' => $node_types_filter,
      'form-type' => 'select',
  );
*/

   $filters['unstranslated'] = array(
                'form-type' => 'checkbox',
                'title' => 'Show unstranslated only',
                'field' => 'unstranslated',
            );

  $filters['title'] = array(
      'title' => 'Title',
      'field' => 'title',
      'form-type' => 'textfield',
  );

    $filters['sku'] = array(
      'title' => 'Sku',
      'field' => 'sku',
      'form-type' => 'textfield',
  );
/*
  $filters['modified-after'] = array(
      'title' => 'Modified After',
      'field' => 'changed',
      'form-type' => 'textfield',
  );
*/
  return $filters;

}

function entity_transpefect_build_filter_query(SelectQuery $query) {
  /*
  $revisioning_module_exists = module_exists("revisioning");
  $filters = build_filters();
  $count = 0;
  foreach (isset($_SESSION['transperfect_dashboard_filter']) ? $_SESSION['transperfect_dashboard_filter'] : array() as $filter) {
    list($key, $value) = $filter;
    if ($key == 'modified-after') {
      $query->condition($filters[$key]['field'], $value, '>=');
    }
    elseif ($key != 'modified' && $key != 'title' && $key != 'target_language' && $key != 'status' && $key != 'node_parent') {
      $query->condition($filters[$key]['field'], $value);
    }
    elseif ($key == 'title') {
      $query->condition($filters[$key]['field'], '%' . $value . '%', 'LIKE');
    }
    elseif ($key == 'status' && !$revisioning_module_exists) {
      list($key, $value) = explode('-', $value, 2);
      $query->condition($key, $value, '=');
    }
    elseif ($key == 'node_parent') {
      if ($value == 1) {
        $query->where('tnid = nid OR tnid = 0');
      }
    }
    $count++;
  }
  */
}

function entity_send_for_translations($eids, $pd4, $submission_name, $due_date, $project_code, $source_locale, $target_locale_arr, $submission_details) {

  //$node_check = variable_get('transperfect_implementation_type', 0);
  $submitter = $submission_details['submitter'];
  $globalLink_arr = array();

  foreach ($eids as $eid) {

    $rows = get_sent_tpt_rows_by_eid($eid);
    $target_arr = $target_locale_arr;

    foreach ($rows as $row) {
      if (array_search($row->target, $target_locale_arr)) {
        unset($target_arr[$row->target]);
      }
    }

    if (empty($target_arr)) {
      continue;
    }

    $entity_arr = entity_load('commerce_product', array($eid));
    $entity = reset($entity_arr);

    foreach ($target_arr as $key => $target_locale) {
      //$target_arr[$key] = strtolower(str_replace('_', '-', $target_locale));
    }

    $tnid = NULL;
    $tvid = NULL;
//    if ($node->tnid != 0 && $node->tnid != $node->nid) {
//      $tnode = node_load($node->tnid);
//      $tnid = $tnode->nid;
//      $tvid = $tnode->vid;
//    }

    $title = $tnid != NULL ? $tnode->title : $entity->field_product_display_title['und'][0]['safe_value'];
    $name = '.xml';
    $xml = get_xml_entity($entity, $target_locale, $tnid, $tvid, $name);

    if (!$xml) {
      form_set_error('', check_plain('Content is empty and has not been sent: ' . $title));
      continue;
    }

    $globalLink = new GlobalLink();
    $globalLink->eid = $tnid != NULL ? $tnid : $eid;
    //$globalLink->vid = $tvid != NULL ? $tvid : $node->vid;
    $globalLink->title = $title;
    $globalLink->type = 'commerce_product';
    $globalLink->sourceLocale = $source_locale;
    $globalLink->targetLocale = $target_arr;
    $globalLink->sourceXML = $xml;
    $globalLink->sourceFileName = $name;
    $globalLink->submissionName = $submission_name;
    $globalLink->dueDate = $due_date;
    $globalLink->submissionInstructions = $submission_details['instructions'] . "\nSubmitter: " . $submitter;
    $globalLink_arr[] = $globalLink;
  }

  if (!empty($globalLink_arr)) {
    sendDocumentsForTranslationToPD($globalLink_arr, $pd4, $project_code, $submitter);
  }

  return $globalLink_arr;

}


function update_entity_ticket_id($arr) {
  foreach ($arr as $globalLink) {

    $eid = $globalLink->eid;
    $target_locale_arr = $globalLink->targetLocale;
    foreach ($target_locale_arr as $target_locale) {
      $row = get_transperfect_entity_row($eid, $globalLink->type, $globalLink->sourceLocale, $target_locale);
      if ($row) {

                db_update('transperfect_core_entity')
                  ->fields(array('title' => $globalLink->title, 'document_ticket' => $globalLink->documentTicket,
                      'submission' => $globalLink->submissionName, 'submission_ticket' => $globalLink->submissionTicket,
                      'status' => TPT_STATUS_SENT_FOR_TRANSLATIONS, 'timestamp' => REQUEST_TIME, 'last_modified' => REQUEST_TIME))
                  ->condition('rid', $row->rid, '=')
                  ->execute();
      }
      else {
       db_insert('transperfect_core_entity')
                  ->fields(array('object_id' => $eid, 'object_type' => $globalLink->type, 'title' => $globalLink->title,
                      'source' => $globalLink->sourceLocale, 'target' => $target_locale, 'document_ticket' => $globalLink->documentTicket,
                      'submission' => $globalLink->submissionName, 'submission_ticket' => $globalLink->submissionTicket,
                      'status' => TPT_STATUS_SENT_FOR_TRANSLATIONS, 'timestamp' => REQUEST_TIME, 'last_modified' => REQUEST_TIME))
                  ->execute();
      }
    }
  }

}



function get_xml_entity($entity, $target_locale, $tnid = NULL, $tvid = NULL, &$name = '', $for_display = FALSE) {

  if (is_null($entity)) {
    return TPT_STATUS_SOURCE_DELETED;
  }
  elseif (!$entity) {
    return TPT_STATUS_SOURCE_DELETED;
  }

  if ($entity && is_object($entity)) {
    if ($entity->language != 'en') {
      $name = 'Entity_' . $entity->product_id . $name;
    }
    else {
      $name = format_file_name($entity->field_product_display_title['und'][0]['value']) . $name;
    }
    $xml = generate_xml_document_entity($entity, $target_locale, $tnid, $tvid, $for_display);
    return $xml;
  }

  return TPT_STATUS_SOURCE_DELETED;

}



function generate_xml_document_entity($entity, $target_locale, $tnid = NULL, $tvid = NULL, $for_display = FALSE) {

  $is_hook_enabled = variable_get('transperfect_implementation_type', 0);
  $drupal_target_locale = get_drupal_locale_code($target_locale);

  try {
    $dom = new DOMDocument('1.0', 'UTF-8');
    $dom->formatOutput = TRUE;

    $root = $dom->createElement("content");
    $eid = $dom->createAttribute('product_id');

    if ($tnid != NULL) {
      $eid->value = $tnid;
    }
    else {
      $eid->value = $entity->product_id;
    }

    $root->appendChild($eid);
    $dom->appendChild($root);
/*
    if ($for_display) {
      insert_child_element($dom, $root, "title", $eid->title);
    }

    elseif (is_field_configured_for_translation('node', $node->type, 'title', $node->type) && $is_hook_enabled == 0) {
      insert_child_element($dom, $root, "title", $node->title);
    }
    elseif ($is_hook_enabled == 1) {
      if (tpt_is_field_translatable($node, 'title', $drupal_target_locale)) {
        insert_child_element($dom, $root, "title", $node->title);
      }
    }
*/


    if($entity->type == 'prod_medfood'){
      $field_arr = field_info_instances('commerce_product', 'prod_medfood');
    }
    else{
      $field_arr = field_info_instances('commerce_product', 'product');
    }

    $keys = array_keys($field_arr);
    foreach ($keys as $field) {

      $field_def = field_read_field($field);
      $items = field_get_items('commerce_product', $entity, $field);

      if ($items) {
        $parent_fc = '';

            foreach ($items as $delta => $item) {

              if (isset($item['value']) && is_string($item['value'])) {
                $f_label = field_info_instance('commerce_product', $field, $bundle);
                $f_value = $item['value'];
                $f_format = (isset($item['format']) && !is_null($item['format'])) ? $item['format'] : '';
                insert_child_element_entity($dom, $root, "field", $f_value, array('entity_type' => 'commerce_product', 'entity_id' => $entity->product_id, 'field_name' => $field, 'delta' => $delta, 'format' => $f_format));
              }
              /*
              elseif ($field_def['type'] == 'link_field' && isset($item['title']) && is_string($item['title'])) {
                $f_label = field_info_instance($entity_type, $field, $bundle);
                $f_value = $item['title'];
                $f_format = (isset($item['format']) && !is_null($item['format'])) ? $item['format'] : '';
                insert_child_element($dom, $root, "field", $f_value, array('entity_type' => $entity_type, 'content_type' => $content_type, 'parent_fc' => $parent_fc, 'bundle' => $bundle, 'entity_id' => $entity_id, 'field_name' => $field, 'label' => $f_label['label'], 'delta' => $delta, 'format' => $f_format));
              }
              */
          }
      }
    }
  /*
    if (module_exists("metatag")) {
      if (isset($node->metatags)) {
        if ($for_display) {
          $metatags = $node->metatags;
          foreach ($metatags as $name => $value) {
            if (isset($value['value'])) {
              insert_child_element($dom, $root, "metatag", $value['value'], array('entity_type' => 'node', 'name' => $name, 'label' => 'Metatag - ' . ucwords($name)));
            }
          }
        }
        elseif (is_field_configured_for_translation('node', $node->type, 'metatags', $node->type)) {
          $metatags = $node->metatags;
          foreach ($metatags as $name => $value) {
            if (isset($value['value'])) {
              insert_child_element($dom, $root, "metatag", $value['value'], array('entity_type' => 'node', 'name' => $name, 'label' => 'Metatag - ' . ucwords($name)));
            }
          }
        }
      }
    }
    */
  }
  catch (Exception $ex) {
    watchdog('transperfect', 'Exception in creating XML - File[%exf], Line[%exl], Code[%exc], Message[%exm]', array('%exf' => $ex->getFile(), '%exl' => $ex->getLine(), '%exc' => $ex->getCode(), '%exm' => $ex->getMessage()), WATCHDOG_ERROR);
    throw $ex;
  }

  $root_element = $dom->getElementsByTagName('content')->item(0);

  if (!$root_element->hasChildNodes()) {
    //return FALSE;
  }

  return $dom->saveXML();
}


function insert_child_element_entity($dom, $root, $elem_name, $elem_value, $attributes = NULL) {

  if ($elem_name && $elem_value) {
    $item = $dom->createElement($elem_name);
    if (isset($attributes) && is_array($attributes)) {
      foreach ($attributes as $key => $value) {
        $item->setAttribute($key, $value);
      }
    }
    $text = $dom->createTextNode($elem_value);
    $item->appendChild($text);
    $root->appendChild($item);
  }

}


function get_transperfect_entity_row($object_id, $object_type, $source, $target) {
  $result = db_select('transperfect_core_entity', 'tce')
          ->fields('tce')
          ->condition('object_id', $object_id, '=')
          ->condition('object_type', $object_type, '=')
          ->condition('source', $source, '=')
          ->condition('target', $target, '=')
          ->execute();
  foreach ($result as $row) {
    return $row;
  }

  return FALSE;
}

function get_entity_title(&$globalLink) {
  $result = db_select('transperfect_core_entity', 'tce')
          ->fields('tce')
          ->condition('document_ticket', $globalLink->documentTicket, '=')
          ->condition('submission_ticket', $globalLink->submissionTicket, '=')
          ->execute();
  foreach ($result as $item) {
    $oid = $item->object_id;
    $entity_arr = entity_load('commerce_product', array($oid));
    $entity = reset($entity_arr);

    $globalLink->eid = $entity->product_id;
    $globalLink->tptRowId = $item->rid;
    $globalLink->title = $item->title;
    if ($item->status == TPT_STATUS_SENT_FOR_TRANSLATIONS)
      $globalLink->status = TPT_STATUS_COMPLETED;
    elseif ($item->status == TPT_STATUS_ERROR)
      $globalLink->status = TPT_STATUS_ERROR;
    if (!$entity || is_null($entity) || !is_object($entity)) {
      return FALSE;
    }
    else {
      $title = l(format_display_string($entity->field_product_display_title['und'][0]['value']), 'admin/commerce/products/' . $item->object_id);
      return $title;
    }
  }
  return FALSE;
}

function get_translated_entity_content($pd4, &$globalLink_arr) {
  try {
    $count = 0;
    foreach ($globalLink_arr as $globalLink) {
      if (!$globalLink->sourceDeleted) {
        $globalLink->targetXML = downloadTargetResource($pd4, $globalLink->targetTicket);
        if (isset($globalLink->targetXML)) {
          $count++;
          update_entity($globalLink);
          if ($globalLink->status != TPT_STATUS_ERROR) {
            sendDownloadConfirmation($globalLink->targetTicket, $pd4);
            update_tpt_entity_status($globalLink);
          }
          else {
            $count--;
          }
        }
      }
    }
  }
  catch (SoapFault $e) {
    watchdog('transperfect', 'SOAP Exception in Sending Download Confirmation - File[%exf], Line[%exl], Code[%exc], Message[%exm]', array('%exf' => $e->getFile(), '%exl' => $e->getLine(), '%exc' => $e->faultcode, '%exm' => $e->faultstring), WATCHDOG_ERROR);
    form_set_error('', check_plain('Web Services Error: ' . $e->faultcode . ' - ' . $e->faultstring));
  }
  catch (Exception $ex) {
    watchdog('transperfect', 'Exception in Sending Download Confirmation - File[%exf], Line[%exl], Code[%exc], Message[%exm]', array('%exf' => $ex->getFile(), '%exl' => $ex->getLine(), '%exc' => $ex->getCode(), '%exm' => $ex->getMessage()), WATCHDOG_ERROR);
    form_set_error('', check_plain('Error: ' . $ex->getMessage()));
  }

  return $count;
}

function update_entity(&$globalLink, $t_arr = NULL) {
  $success = FALSE;
  try {

    $revisioning_module_exists = module_exists("revisioning");

    if ($t_arr != NULL) {
      $translated_arr = $t_arr;
    }
    else {
      $translated_arr = get_translated_entity_array($globalLink->targetXML);
    }

    $target_id = $translated_arr['product_id'];
    $target_vid = $translated_arr['vid'];
    $target_title = '';

    if (isset($translated_arr['title'])) {
      $target_title = $translated_arr['title'];
    }

    $globalLink->nid = $target_nid;
    $globalLink->vid = $target_vid;

    // $current_node = node_load($target_nid);
    $entity_arr = entity_load('commerce_product', array($target_id));
    $entity = reset($entity_arr);

    if (!$entity || is_null($entity) || !is_object($entity)) {
      $globalLink->status = TPT_STATUS_SOURCE_DELETED;
      return;
    }

    if(!isset($entity->translations->data[$globalLink->targetLocale])){
        $handler = entity_translation_get_handler('commerce_product', $entity, TRUE);

        $entity_source_lang = $entity->field_product_region['und'][0]['value'] == 'usen' ? 'en-us' : 'en-ca';

        $values = array();
        $translation = array(
          'translate' => 0,
          'status'    => 1,
          'language'  => $globalLink->targetLocale, // language to set
          'source'    => $entity_source_lang, // source language
        );
        // Copy field values
        foreach (field_info_instances('commerce_product', $entity->type) as $instance) {
          $field_name = $instance['field_name'];
          $field = field_info_field($field_name);
          if ($field['translatable'] == 1) {

            if(isset($translated_arr[$field_name])){
              $value = $translated_arr[$field_name]['und'][0]->translatedContent;
            }
            else{
              $value = $entity->{$field_name}[$entity->language];
            }

            $values[$field_name][$globalLink->targetLocale][0]['value'] = $value;

            //$entity->{$field_name}[$globalLink->targetLocale][0]['value'] = $value;
          }
        }

        $handler->setTranslation($translation, $values);
        //$handler->saveTranslations();

        field_attach_presave('commerce_product', $entity);
        field_attach_update('commerce_product', $entity);
        entity_save('commerce_product', $entity);
    }
    else{

        foreach (field_info_instances('commerce_product') as $instance) {
          $field_name = $instance['field_name'];
          $field = field_info_field($field_name);

          if ($field['translatable']) {
            $value = isset($translated_arr[$field_name]) ? $translated_arr[$field_name] : $entity->{$field_name}[$entity->language];

            $entity->{$field_name}[$globalLink->targetLocale][0]['value'] = $value;
          }
        }

        if(empty($entity->title_field[$globalLink->targetLocale])){
          $entity->title_field[$globalLink->targetLocale][0]['value'] = $entity->title_original;
        }

         if($entity->title == null){
          $entity->title = $entity->title_original;
        }
        entity_save('commerce_product', $entity);
    }
/*
    foreach($entity as $key => $value){
      if($key != 'title' && $key != 'vid' && $key != 'product_id' && !empty($translated_arr[$key])){
        $value[$globalLink->targetLocale][0]['value'] = $translated_arr[$key];
      }
    }



    entity_save($globalLink->type, $entity);
*/
  }

  catch (Exception $ex) {
    $globalLink->status = TPT_STATUS_ERROR;
    watchdog('transperfect', 'Exception in Receive Translations - Update Node - File[%exf], Line[%exl], Code[%exc], Message[%exm]', array('%exf' => $ex->getFile(), '%exl' => $ex->getLine(), '%exc' => $ex->getCode(), '%exm' => $ex->getMessage()), WATCHDOG_ERROR);
  }

}

function get_translated_entity_array($xml) {
  if (is_null($xml) || !is_string($xml) || $xml == '') {
    return array();
  }

  $dom = new DomDocument;
  $dom->preserveWhiteSpace = FALSE;
  $dom->loadXML($xml);

  $arr = array();

  $contents = $dom->getElementsByTagName('content');
  foreach ($contents as $content) {
    if (!is_null($content->attributes)) {
      foreach ($content->attributes as $attrName => $attrNode) {
        if ('rid' == $attrName) {
          $arr['rid'] = $attrNode->value;
        }
        if ('nid' == $attrName) {
          $arr['nid'] = $attrNode->value;
        }
        elseif ('vid' == $attrName) {
          $arr['vid'] = $attrNode->value;
        }
        elseif ('product_id' == $attrName) {
          $arr['product_id'] = $attrNode->value;
        }
      }
    }
  }

  $titles = $dom->getElementsByTagName('title');
  foreach ($titles as $title) {
    $arr['title'] = $title->nodeValue;
  }

  $fields = $dom->getElementsByTagName('field');

  foreach ($fields as $field) {

    $fieldObject = new GLField();
    $fieldObject->type = 'field';
    $fieldObject->translatedContent = $field->nodeValue;

    if (!is_null($field->attributes)) {
      foreach ($field->attributes as $attrName => $attrNode) {
        switch ($attrName) {
          case 'entity_type':
            $fieldObject->entityType = $attrNode->value;
            continue 2;
          case 'content_type':
            $fieldObject->contentType = $attrNode->value;
            continue 2;
          case 'parent_fc':
            $fieldObject->parentFCName = $attrNode->value;
            continue 2;
          case 'bundle':
            $fieldObject->bundle = $attrNode->value;
            continue 2;
          case 'entity_id':
            $fieldObject->entityId = $attrNode->value;
            continue 2;
          case 'field_name':
            $fieldObject->fieldName = $attrNode->value;
            continue 2;
          case 'label':
            $fieldObject->fieldLabel = $attrNode->value;
            continue 2;
          case 'delta':
            $fieldObject->delta = $attrNode->value;
            continue 2;
          case 'format':
            $fieldObject->format = $attrNode->value;
            continue 2;
        }
      }
      if (is_null($fieldObject->entityId)) {
        $fieldObject->entityId = '0';
      }
      if (is_null($fieldObject->bundle)) {
        $fieldObject->bundle = $fieldObject->fieldName;
      }
      if (is_null($fieldObject->delta)) {
        $fieldObject->delta = '0';
      }

      $arr[$fieldObject->fieldName][LANGUAGE_NONE][$fieldObject->delta] = $fieldObject;

    }
  }

/*
  $metatags = $dom->getElementsByTagName('metatag');

  foreach ($metatags as $metatag) {

    $metatagObject = new GLField();
    $metatagObject->type = 'metatag';
    $metatagObject->translatedContent = $metatag->nodeValue;

    if (!is_null($metatag->attributes)) {
      foreach ($metatag->attributes as $attrName => $attrNode) {
        switch ($attrName) {
          case 'entity_type':
            $metatagObject->entityType = $attrNode->value;
            continue 2;
          case 'content_type':
            $fieldObject->contentType = $attrNode->value;
            continue 2;
          case 'bundle':
            $fieldObject->bundle = $attrNode->value;
            continue 2;
          case 'entity_id':
            $metatagObject->entityId = $attrNode->value;
            continue 2;
          case 'name':
            $metatagObject->fieldName = $attrNode->value;
            continue 2;
          case 'label':
            $metatagObject->fieldLabel = $attrNode->value;
            continue 2;
        }
      }
      if (is_null($metatagObject->entityId)) {
        $metatagObject->entityId = '0';
      }
      if (is_null($metatagObject->bundle)) {
        $metatagObject->bundle = $metatagObject->fieldName;
      }
      $arr['metatag'][$metatagObject->bundle][$metatagObject->entityId] = $metatagObject;
    }
  }

  */

  return $arr;
}


function update_tpt_entity_status(&$globalLink) {

  $eid = $globalLink->eid;

  $sourceLocaleParts = explode('-', $globalLink->sourceLocale);
  $sourceLocale = $sourceLocaleParts[0].'_'.strtoupper($sourceLocaleParts[1]);

  $targetLocaleParts = explode('-', $globalLink->targetLocale);
  $targetLocale = $targetLocaleParts[0].'_'.strtoupper($targetLocaleParts[1]);

  if ($globalLink->status != TPT_STATUS_SOURCE_DELETED && $globalLink->status != TPT_STATUS_ERROR) {

      $row = get_transperfect_entity_row($eid, 'commerce_product', $sourceLocale, $targetLocale);
      if ($row) {

                db_update('transperfect_core_entity')
                  ->fields(array('title' => $globalLink->title, 'status' => TPT_STATUS_PENDING_TRANSLATIONS, 'timestamp' => REQUEST_TIME))
                  ->condition('rid', $row->rid, '=')
                  ->execute();
      }
  }
  elseif ($globalLink->status == TPT_STATUS_SOURCE_DELETED) {
    $row = get_transperfect_entity_row($eid, 'commerce_product', $sourceLocale, $targetLocale);
    if ($row) {

                db_update('transperfect_core_entity')
                  ->fields(array('title' => $globalLink->title, 'status' => TPT_STATUS_SOURCE_DELETED, 'timestamp' => REQUEST_TIME))
                  ->condition('rid', $row->rid, '=')
                  ->execute();
      }
  }
  elseif ($globalLink->status == TPT_STATUS_ERROR) {
    $row = get_transperfect_entity_row($eid, 'commerce_product', $sourceLocale, $targetLocale);
   if ($row) {

                db_update('transperfect_core_entity')
                  ->fields(array('title' => $globalLink->title, 'status' => TPT_STATUS_ERROR, 'timestamp' => REQUEST_TIME))
                  ->condition('rid', $row->rid, '=')
                  ->execute();
      }
  }
}


function update_deleted_entity_records($pd4, $globalLink) {
  try {
    $globalLink->status = TPT_STATUS_SOURCE_DELETED;
    sendDownloadConfirmation($globalLink->targetTicket, $pd4);
    update_tpt_entity_status($globalLink);
  }
  catch (SoapFault $e) {
    watchdog('transperfect', 'SOAP Exception in Sending Download Confirmation - File[%exf], Line[%exl], Code[%exc], Message[%exm]', array('%exf' => $e->getFile(), '%exl' => $e->getLine(), '%exc' => $e->faultcode, '%exm' => $e->faultstring), WATCHDOG_ERROR);
    form_set_error('', check_plain('Web Services Error: ' . $e->faultcode . ' - ' . $e->faultstring));
    return FALSE;
  }
  catch (Exception $ex) {
    watchdog('transperfect', 'Exception in update_deleted_records - File[%exf], Line[%exl], Code[%exc], Message[%exm]', array('%exf' => $ex->getFile(), '%exl' => $ex->getLine(), '%exc' => $ex->getCode(), '%exm' => $ex->getMessage()), WATCHDOG_ERROR);
    form_set_error('', check_plain('Error: ' . $ex->getMessage()));
    return FALSE;
  }

  return TRUE;
}

function get_active_submission_rows_by_eid($eids) {
  $query = db_select('transperfect_core_entity', 'tce');
  $query->condition('status', array(TPT_STATUS_SENT_FOR_TRANSLATIONS, TPT_STATUS_ERROR), 'IN');
  $query->condition('object_id', $eids, 'IN');
  $query->fields('tce');
  $results = $query->execute();
  $arr = array();
  foreach ($results as $row) {
    if (array_key_exists($row->object_id, $arr)) {
      array_push($arr[$row->object_id], $row);
    }
    else {
      $arr[$row->object_id] = array($row);
    }
  }
  $final_arr = array();
  foreach ($arr as $object_id => $object_id_arr) {
    $sub_arr = array();
    foreach ($object_id_arr as $r) {
      if (array_key_exists($r->submission, $sub_arr)) {
        array_push($sub_arr[$r->submission], $r->target);
      }
      else {
        $sub_arr[$r->submission] = array($r->vid => $r->target);
      }
    }
    if (count($sub_arr) > 0) {
      $final_arr[$object_id] = $sub_arr;
    }
  }
  if (count($final_arr) > 0) {
    return $final_arr;
  }
  return FALSE;
}

