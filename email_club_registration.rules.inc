<?php
    /**
     * Implements hook_rules_action_info()
     */
     
    function email_club_registration_rules_action_info() {
        //define action parameters for the rules module
        $label = t( 'Send an email when an event takes place' );                        //label for the action
        $group = t( 'Koumbit' );                        //The group to include the element on in the interface
        $parameter = '';                    //
        $provides = '';                     //
         
        //action definitions
        $actions = array(
            'email_club_registration_email_action'  => array(
                'label'     => $label,
                'group'     => $group,
            )
        );
        
        return $actions;
    }