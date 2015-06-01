<?php

/**
 * @file
 * Views api call for brookfield gallery.
 */


/**
 * Implements hook_views_data().
 */
function brookfield_gallery_views_data() {
  $data = array();

  $data['node']['gallery_display_date'] = array(
    'field' => array(
      'title' => t('Gallery Image Display Date'),
      'help' => t('Display either original date taken or post date.'),
      'handler' => 'brookfield_gallery_views_gallery_display_date',
    ),
  );

  return $data;
}
