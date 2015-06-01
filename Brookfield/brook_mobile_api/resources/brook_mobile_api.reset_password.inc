<?php
/**
 * @file
 * Defines functionality for the password reset resource.
 */

/**
 * Resource callback to reset password.
 */
function brook_mobile_api_resource_password_reset($email) {
  $account = user_load_by_mail($email);

  if (empty($account)) {
    return array('status' => 'invalidEmail');
  }
  else {
    // Mail one time login URL and instructions using current language.
    global $language;
    $mail = _user_mail_notify('password_reset', $account, $language);
    if (!empty($mail)) {
      watchdog('user', 'Password reset instructions mailed to %name at %email.', array('%name' => $account->name, '%email' => $account->mail));
    }
    else {
      watchdog('user', 'There was an error re-sending password reset instructions mailed to %name at %email', array('%name' => $account->name, '%email' => $account->mail));
    }

    // Everything went right.
    return array('status' => 'success');
  }
}
