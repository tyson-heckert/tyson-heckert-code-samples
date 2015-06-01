<?php
/**
 * @file
 * Functionality for service request endpoints.
 */


/**
 * Resource access callback for service requests
 */
function _brook_mobile_api_service_request_access() {
  // @TODO SET THIS THE CORRECT PERMISSION!!!
  return TRUE;
}
/**
 * Callback for endpoint to create service requests.
 *
 * @param array $service_requests
 *   The array of service requests to create.
 *
 * @return array
 *   An array of the nids for the newly created SRs
 */
function _brook_mobile_api_create_service_requests($service_requests) {
  try {
    $total_requests = count($service_requests);
    foreach ($service_requests as $key => $request) {
      $parent_id = $request['parent_id'];
      $sr_nid = brookfield_service_requests_create_request($parent_id, $request, $key + 1, $total_requests, 'brook_app_punchlist');
      if ($sr_nid == 0) {
        services_error('Error generating service request ' . $key . ' for punchlist ' . $parent_id, 400);
      }
      else {
        $sr_nids[] = $sr_nid;
      }
    }

    return array('nids' => $sr_nids);
  }
  catch (Exception $e) {
    services_error($e->getMessage(), $e->getCode);
  }
}
