<?php

/**
 * @file
 * Contains the gallery image date field handler.
 */

/**
 * Field handler to display date taken if available.
 */
class brookfield_gallery_views_gallery_display_date extends views_handler_field {
  function query() {
     // do nothing -- to override the parent query.
  }

  function render($values) {
    if (!empty($values->field_field_image_original_date)) {
      return drupal_render($values->field_field_image_original_date);
    }
    else {
      $element = array(
        '#markup' => format_date($values->node_created, 'short'),
      );

      return drupal_render($element);
    }
  }
}
