<?php

/**
 * @file
 * Implementations of Rule's hooks, or function callback for custom rules.
 */

/**
 * Implements hook_rules_action_info()
 */

function email_custom_pdf_rules_action_info() {
  // Define the action parameters for the Rules module.
  // The label for the action.
  $label = t('Email a PDF of the current node to a pre-defined user.');
  
  // The group to include the element on in the interface.
  $group = t('Koumbit');
  $parameter = '';
  $provides = '';

  // Definitions for custom Rules actions.
  $actions = array(
    'email_custom_pdf_email_action'  => array(
      'label'     => $label,
      'group'     => $group,
      )
  );
  
  return $actions;
}
