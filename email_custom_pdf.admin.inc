<?php

/**
 * @file
 * Contains the administrative functions of the Email Custom PDF module.
 *
 * This file is included by the Email Custom PDF module, and includes the
 * settings form.
 *
 * @ingroup email_custom_pdf
 * 
 * @todo Complete the submit function.
 * @todo Test the validation functions and data sanitization.
 */

/**
 * Form constructor for the Email Custom PDF configuration settings form.
 *
 * @ingroup forms
 */
function email_custom_pdf_settings() {
  // Load the site's url for use with filename, if none is provided by the user.
  global $base_url;
  
  // The email settings section of the form.
  $form['email_settings'] = array(
    '#title'          => t('Email Settings'),
    '#type'           => 'fieldset',
    '#description'    => t('Configure email settings for sending out the PDF.'),
    '#collapsible'    => TRUE,
    '#collapsed'      => FALSE
  );
  
  /** 
   * The FROM address included in the outgoing email.
   * 
   * By default, if no email has been stored just grab the email set in Site Information, 
   * or as a last result the email of user with UID 1.
   */
  
  $default_from_address = variable_get('site_mail', user_load(1)->mail); 
  
  $form['email_settings']['email_custom_pdf_from_email'] = array(
    '#title'          => t('From Email Address'),
    '#type'           => 'textfield',
    '#description'    => t('The outgoing email address the message will be sent from.'),
    '#default_value'  => variable_get('email_custom_pdf_from_email', $default_from_address)
  );

  // The email message's subject field
  $form['email_settings']['email_custom_pdf_message_subject'] = array(
    '#title'          => t('Message Subject'),
    '#type'           => 'textfield',
    '#description'    => t('The subject field for the outgoing email.'),
    '#default_value'  => variable_get('email_custom_pdf_message_subject', variable_get('site_name', $base_url))
  );
  
  // The body text sent in the email.
  $form['email_settings']['email_custom_pdf_message_body'] = array(
    '#title'          => t('Message Text'),
    '#type'           => 'textarea',
    '#description'    => t('The body text of the email message'),
    
  );
  
  // The PDF generation settings section of the form.
  $form['pdf_settings'] = array(
    '#title'          => t('PDF Settings'),
    '#type'           => 'fieldset',
    '#description'    => t('Configure settings for generating the PDF.'),
    '#collapsible'    => TRUE,
    '#collapsed'      => FALSE
  );
  
  /**
   * The filename to use for the generated PDF.
   * 
   * If no filename has been set, try setting it to the site's name, as a last resort to the URL.
   */
  $form['pdf_settings']['email_custom_pdf_filename'] = array(
    '#title'          => t('PDF Filename'),
    '#type'           => 'textfield',
    '#description'    => t('The name given to the attached PDF. The file extension (.pdf) is not neccessary.'),
    '#default_value'  => variable_get('email_custom_pdf_filename', variable_get('site_name', $base_url))
  );
  
  return system_settings_form($form);
}

/**
 * Form validation handler for email_custom_pdf_settings().
 *
 * @see email_custom_pdf_settings()
 * @ingroup forms
 */
function email_custom_pdf_settings_validate($form, &$form_state) {
  
  // Validate the FROM email address
  if (!valid_email_address(($form_state['values']['email_custom_pdf_from_email']))) {
    form_set_error('email_custom_pdf_from_email', t("Invalid email address."));
  }
  
  // Force text fields to be plain-text only.
  $form_state['values']['email_custom_pdf_message_body'] = check_plain($form_state['values']['email_custom_pdf_message_body']);
  $form_state['values']['email_custom_pdf_message_subject'] = check_plain($form_state['values']['email_custom_pdf_message_subject']);
  $form_state['values']['email_custom_pdf_filename'] = email_custom_pdf_sanitize_string($form_state['values']['email_custom_pdf_filename']);
}

/**
 * Sanitize strings for use as filenames.
 * 
 * @source http://stackoverflow.com/questions/11330480/strip-php-variable-replace-white-spaces-with-dashes
 */
function email_custom_pdf_sanitize_string($string) {
    //Lower case everything
    $string = strtolower($string);
    
    //Make alphanumeric (removes all other characters)
    $string = preg_replace("/[^a-z0-9_\s-]/", "", $string);
    
    //Clean up multiple dashes or whitespaces
    $string = preg_replace("/[\s-]+/", " ", $string);
    
    //Convert whitespaces and underscore to dash
    $string = preg_replace("/[\s_]/", "-", $string);
    
    return $string;
}