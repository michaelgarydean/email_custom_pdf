<?php

/**
 * @file
 * Module file for the email_club_registration module.
 */

/**
 * Implements hook_permission().
 */
function email_club_registration_permission() {
  $permissions['Modify settings for sending CSU club contracts'] = array(
    'title' => t('Modify settings to email CSU club contracts'),
    'description' => t('Allows a user to modify the email message options used when sending out CSU club contracts')
  );
  return $permissions;
}

/**
 * Implements hook_menu().
 */
function email_club_registration_menu() {
  $items['admin/config/content/csu-clubs'] = array(
    'title' => 'CSU Club Settings',
    'description' => 'Settings related to CSU Clubs',
    'type' => MENU_NORMAL_ITEM,
    'access arguments' => array('administer email club registration settings'),
    'page callback' => 'system_admin_menu_block_page',
    'file' => 'system.admin.inc',
    'file path' => drupal_get_path('module', 'system'),
  );
  $items['admin/config/content/csu-clubs/general'] = array(
    'title' => 'General Club Settings',
    'description' => 'General CSU Club Settings',
    'type' => MENU_NORMAL_ITEM,
    'page callback' => 'drupal_get_form',
    'access arguments' => array('administer email club registration settings'),
    'page arguments' => array('email_club_registration_general_club_settings_form'),
  );
  $items['admin/config/content/csu-clubs/email'] = array(
    'title' => 'Club E-mail Settings',
    'type' => MENU_NORMAL_ITEM,
    'page callback' => 'drupal_get_form',
    'access arguments' => array('administer email club registration settings'),
    'page arguments' => array('email_club_registration_email_club_settings_form'),
  );
  return $items;
}

/**
 * General Club Settings Form.
 *
 * @ingroup forms
 */
function email_club_registration_general_club_settings_form($form, &$form_state) {
  $form['approval_cancellation_date'] = array(
    '#type' => 'date',
    '#title' => t('Club Cancellation Date'),
    '#description' => t('The date on which Clubs begin to be cancelled automatically. Note: the year is ignored. The month and day only are used'),
    '#default_value' => variable_get('email_club_registration_club_cancellation_date', array()),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Submit handler for the email_club_registration_general_club_settings_form.
 */
function email_club_registration_general_club_settings_form_submit($form, &$form_state) {
  variable_set('email_club_registration_club_cancellation_date',
    $form_state['values']['approval_cancellation_date']);
}

/**
 * Implements hook_cron_queue_info().
 */
function email_club_registration_cron_queue_info() {
  $queues['email_club_registration_cancel_club'] = array(
    'worker callback' => 'email_club_registration_cancel_club_worker',
  );
  return $queues;
}

/**
 * Implements hook_cron().
 */
function email_club_registration_cron() {
  $last_run = variable_get('email_club_registration_cron_club_delete_last_run', array());
  $today = getdate();
  $target_day = variable_get('email_club_registration_club_cancellation_date', array());
  if (empty($target_day)) {
    return;
  }
  if ($target_day['month'] == $today['mon'] && $target_day['day'] == $today['mday']) {
    // Check last run.
    if (!empty($last_run)) {
      if ($last_run['year'] >= $today['year'] &&
        $last_run['month'] == $target_day['month'] &&
        $last_run['day'] == $target_day['day']) {
        // Our last run was this year or in the future, skip.
        return;
      }
    }
    $query = db_select('node', 'n');
    $query->fields('n')
      ->condition('n.type', 'club_registration');
    $results = $query->execute();
    $queue = DrupalQueue::get('email_club_registration_cancel_club');
    foreach ($results as $club_node) {
      watchdog('email_club_registration', t('Queued club node !nid for automatic cancellation',
          array('!nid' => $club_node->nid)));
      $queue->createItem($club_node);
    }
    variable_set('email_club_registration_cron_club_delete_last_run', $target_day);
  }
}

/**
 * Worker callback for cancelling clubs.
 */
function email_club_registration_cancel_club_worker($data) {
  $node = node_load($data->nid);
  $node->field_club_approved_[LANGUAGE_NONE][0]['value'] = 0;
  node_save($node);
}

/**
 * Sends an email to the user.
 *
 * This function was implemented to test outgoing emails from Drupal.
 */
function email_club_registration_email_action() {
  global $user;

  $from = 'mike@koumbit.org';
  $to = 'mike@koumbit.org';
  $language = user_preferred_language($user);
  
  $params = array(
    'headers'        => array('Content-Type' => 'text/html'),
    'username'       => $user->name,
    'subject'        => 'Concordia Student Union - Club Contract',
    'body'           => 'Test body'
  );

  // Call the hook_mail function using the parameters provided to drupal_mail().
  drupal_mail('email_club_registration', 'attach_pdf_of_node', $to, $language, $params, $from);
}

/**
 * Implements hook_mail().
 */
function email_club_registration_mail($key, &$message, $params) {
  // Set headers etc for the email.
  $message['subject'] = $params['subject'];
  $message['body'][] = $params['body'];

  switch ($key) {
    // Generate a PDF of the current node and add it as an attachment.
    case 'attach_pdf_of_node':

      // Load the print_pdf module for the print_pdf_generate_path in order to generate the pdf.
      module_load_include('inc', 'print_pdf', 'print_pdf.pages');
      
      // Generate HTML from a template file.
      $html = '';

      // Generate the attachment details for the pdf.
      $attachment = array(
        'filecontent'   => print_pdf_generate_html($html, null),
        'filemime'      => 'application/pdf',
        'filename'      => 'club_registration_form.pdf'
      );
      break;
  }

  // If there is an attachment that was passed or generated, add it to the message array.
  if (isset($attachment)) {
    $message['params']['attachments'][] = $attachment;
  }
}
