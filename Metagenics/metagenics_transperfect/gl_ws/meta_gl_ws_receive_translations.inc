<?php

require_once TPT_ROOT . '/' . ('gl_ws/gl_ws_common.inc');
require_once TPT_ROOT . '/' . ('gl_ws/client/SessionService2.php');
require_once TPT_ROOT . '/' . ('gl_ws/client/ProjectService2.php');
require_once TPT_ROOT . '/' . ('gl_ws/client/TargetService2.php');

function metagenics_getReadyTranslationsDetailsFromPD($pdObject, $type = 'entity') {
  $username = $pdObject->username;
  $password = $pdObject->password;
  $url = $pdObject->url;
  $projectShortCode = $pdObject->projectShortCode;
  $proj_arr = array();
  if ($projectShortCode != '') {
    $arr = explode(',', $projectShortCode);
    foreach ($arr as $value) {
      $proj_arr[$value] = $value;
    }
  }

  $sessionService = new SessionService2(GL_WSDL_PATH . 'SessionService2.wsdl', array(
              'location' => $url . "/services/SessionService2"
          ));
  $projectService = new ProjectService2(GL_WSDL_PATH . 'ProjectService2.wsdl', array(
              'location' => $url . "/services/ProjectService2"
          ));
  $targetService = new TargetService2(GL_WSDL_PATH . 'TargetService2.wsdl', array(
              'location' => $url . "/services/TargetService2"
          ));

  $globalLink_arr = array();

  $token = @Login($sessionService, $username, $password);
  $projects = GetUserProjects($projectService, $token);
  $proj_ticket_arr = array();
  foreach ($projects as $project) {
    $short_code = $project->projectInfo->shortCode;
    foreach ($proj_arr as $conf_proj) {
      if ($conf_proj == $short_code) {
        $proj_ticket_arr[] = $project->ticket;
      }
    }
  }

  if (count($proj_ticket_arr) > 0) {
    $targets = @metagenics_getCompletedTargetsByProjects($targetService, $proj_ticket_arr, $token, $pdObject);
    $count = 1;
    if (is_array($targets)) {
      foreach ($targets as $target) {
        if (!is_null($target->ticket) && $target->ticket != '') {

          $sourceFile = $target->document->documentInfo->name;
          if ($type == 'entity') {
            if (!startsWith($sourceFile, "Entity")) {
              continue;
            }
          }

          $globalLink = new GlobalLink();
          $globalLink->submissionTicket = $target->document->documentGroup->submission->ticket;
          $globalLink->submissionName = $target->document->documentGroup->submission->submissionInfo->name;
          $globalLink->documentTicket = $target->document->ticket;
          $globalLink->sourceFileName = $sourceFile;
          $globalLink->targetTicket = $target->ticket;
          $globalLink->sourceLocale = strtolower($target->sourceLanguage->locale);
          $globalLink->targetLocale = strtolower($target->targetLanguage->locale);
          $globalLink_arr[$count] = $globalLink;
          $count++;
        }
      }
    }
    elseif (!is_null($targets)) {
      $target = $targets;
      if (!is_null($target->ticket) && $target->ticket != '') {

        $sourceFile = $target->document->documentInfo->name;
        if ($type == 'entity') {
            if (!startsWith($sourceFile, "Entity")) {
              continue;
            }
          }

        $globalLink = new GlobalLink();
        $globalLink->submissionTicket = $target->document->documentGroup->submission->ticket;
        $globalLink->submissionName = $target->document->documentGroup->submission->submissionInfo->name;
        $globalLink->documentTicket = $target->document->ticket;
        $globalLink->sourceFileName = $sourceFile;
        $globalLink->targetTicket = $target->ticket;
          $globalLink->sourceLocale = strtolower($target->sourceLanguage->locale);
          $globalLink->targetLocale = strtolower($target->targetLanguage->locale);

        $globalLink_arr[$count] = $globalLink;

      }
    }
  }
  return $globalLink_arr;
}

function metagenics_getCompletedTargetsByProjects($targetService, $projectTickets, $token, $pdObject) {
  $getCompletedTargetsByProjectsRequest = new getCompletedTargetsByProjects();

  $getCompletedTargetsByProjectsRequest->projectTickets = $projectTickets;
  $getCompletedTargetsByProjectsRequest->maxResults = $pdObject->maxTargetCount;
  $getCompletedTargetsByProjectsRequest->userId = $token;

  $getCompletedTargetsByProjectsResponse = $targetService->getCompletedTargetsByProjects($getCompletedTargetsByProjectsRequest);

  return $getCompletedTargetsByProjectsResponse->return;
}

function metagenics_getCompletedTargetsByDoc($pdObject, $ticket) {
  $username = $pdObject->username;
  $password = $pdObject->password;
  $url = $pdObject->url;

  $sessionService = new SessionService2(GL_WSDL_PATH . 'SessionService2.wsdl', array(
              'location' => $url . "/services/SessionService2"
          ));

  $targetService = new TargetService2(GL_WSDL_PATH . 'TargetService2.wsdl', array(
              'location' => $url . "/services/TargetService2"
          ));

  $token = Login($sessionService, $username, $password);

  $getCompletedTargetsByDocumentsRequest = new getCompletedTargetsByDocuments();

  $getCompletedTargetsByDocumentsRequest->documentTickets = array($ticket);
  $getCompletedTargetsByDocumentsRequest->maxResults = 99;
  $getCompletedTargetsByDocumentsRequest->userId = $token;

  $getCompletedTargetsByDocumentsResponse = $targetService->getCompletedTargetsByDocuments($getCompletedTargetsByDocumentsRequest);

  return $getCompletedTargetsByDocumentsResponse->return;
}

function metagenics_downloadTargetResource($pdObject, $targetTicket) {
  $username = $pdObject->username;
  $password = $pdObject->password;
  $url = $pdObject->url;

  $sessionService = new SessionService2(GL_WSDL_PATH . 'SessionService2.wsdl', array(
              'location' => $url . "/services/SessionService2"
          ));
  $targetService = new TargetService2(GL_WSDL_PATH . 'TargetService2.wsdl', array(
              'location' => $url . "/services/TargetService2"
          ));
  $token = @Login($sessionService, $username, $password);
  $downloadTargetResourceRequest = new downloadTargetResource();

  $downloadTargetResourceRequest->targetId = $targetTicket;
  $downloadTargetResourceRequest->userId = $token;

  $downloadTargetResourceResponse = $targetService->downloadTargetResource($downloadTargetResourceRequest);
  $repositoryItem = $downloadTargetResourceResponse->return;
  $targetContent = $repositoryItem->data->_;

  return $targetContent;
}

function metagenics_sendDownloadConfirmation($targetTicket, $pdObject) {
  $username = $pdObject->username;
  $password = $pdObject->password;
  $url = $pdObject->url;

  $sessionService = new SessionService2(GL_WSDL_PATH . 'SessionService2.wsdl', array(
              'location' => $url . "/services/SessionService2"
          ));
  $targetService = new TargetService2(GL_WSDL_PATH . 'TargetService2.wsdl', array(
              'location' => $url . "/services/TargetService2"
          ));
  $token = Login($sessionService, $username, $password);
  $sendDownloadConfirmationRequest = new sendDownloadConfirmation();

  $sendDownloadConfirmationRequest->targetId = $targetTicket;
  $sendDownloadConfirmationRequest->userId = $token;

  $sendDownloadConfirmationResponse = $targetService->sendDownloadConfirmation($sendDownloadConfirmationRequest);

  return $sendDownloadConfirmationResponse->return;
}
