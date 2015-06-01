<?php

require_once TPT_ROOT . '/' . ('transperfect_core_constants.inc');

//update to use a table and row that is unique to entities and not just nodes
function get_sent_tpt_rows_by_eid($eid) {
  $result = db_select('transperfect_core', 'tc')
          ->fields('tc')
          ->condition('nid', $eid, '=')
          ->condition('status', array(TPT_STATUS_SENT_FOR_TRANSLATIONS, TPT_STATUS_ERROR), 'IN')
          ->execute();
  $rows = array();
  foreach ($result as $row) {
    $rows[] = $row;
  }
  return $rows;
}
