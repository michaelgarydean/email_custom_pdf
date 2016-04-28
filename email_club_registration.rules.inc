<?php

/**
 * @file
 * Implementations of rule's hooks, or function callback for custom rules.
 */

/**
 * Implements hook_rules_action_info()
 */

function email_club_registration_rules_action_info() {
  //define action parameters for the rules module
  // Label for the action.
  $label = t('Email a PDF of the current node to the current user');
  // The group to include the element on in the interface.
  $group = t('Koumbit');
  $parameter = '';
  $provides = '';

  //action definitions
  $actions = array(
    'email_club_registration_email_action'  => array(
      'label'     => $label,
      'group'     => $group,
      )
  );
  return $actions;
}
