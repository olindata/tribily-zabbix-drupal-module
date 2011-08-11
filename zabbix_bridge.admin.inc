<?php

// $Id$

/**
 * @file
 * Settings form for admin configuration
 *
 */

include_once 'zabbix_functions.php';

/**
 *
 * @return <type>
 */
function zabbix_bridge_hosts_form() {

  drupal_set_title('Hosts of zabbix_bridge');

  $output = '<h2>' . t('User defined hosts') . '</h2>';
  $output .= t('These are all the hosts with their roles created by all users.');
  $output .= zabbix_hosts_table();
  $output .= l('Import from Zabbix', 'hosts/import');

  return $output;
}

/**
 *
 * @return <type>
 */
function zabbix_bridge_admin_emails() {

  drupal_set_title('Emails of zabbix_bridge');

  $output = '<h2>' . t('User defined emails') . '</h2>';
  $output .= t('These are all the emails defined by users for notification.');
  $output .= zabbix_emails_table();
  $output .= l('Add email', 'emails/add') . ' | ' . l('Import from zabbix', 'emails/import');

  return $output;
}

/**
 *
 * @return <type>
 */
function zabbix_bridge_admin_jabbers() {

  drupal_set_title('Jabbers of zabbix_bridge');

  $output = '<h2>' . t('User defined jabbers') . '</h2>';
  $output .= t('These are all the jabber accounts defined by users for notification.');
  $output .= zabbix_jabbers_table();
  $output .= l('Add jabber', 'jabbers/add') . ' | ' . l('Import from zabbix', 'jabbers/import');

  return $output;
}

/**
 *
 * @return <type>
 */
function zabbix_bridge_admin_mobiles() {

  drupal_set_title('Mobiles of zabbix_bridge');

  $output = '<h2>' . t('User defined mobiles') . '</h2>';
  $output .= t('These are all the jabber accounts defined by users for notification.');
  $output .= zabbix_mobiles_table();
  $output .= l('Add mobile number', 'mobiles/add') . ' | ' . l('Import from zabbix', 'mobiles/import');

  return $output;
}

/**
 *
 * @return <type>
 */
function zabbix_bridge_admin_user_mappings() {

  drupal_set_title('User mappings of zabbix_bridge');

  $output .= '<h2>' . t('Mapping of Drupal vs Zabbix objects') . '</h2>';
  $output .= t('This is teh list of currently configured mappings for drupal users to their corresponding zabbix object id\s');
  $output .= zabbix_user_mapping_table();
  $output .= l('Add mapping', 'zabbix-user-mapping/add');

  return $output;
}

/**
 *
 * @return <type>
 */
function zabbix_bridge_admin_roles_mapping() {

  drupal_set_title('Mapping of roles of zabbix_bridge');

  $output .= '<h2>' . t('Roles and their corresponding Zabbix Templates') . '</h2>';
  $output .= t('These are all the roles with their corresponding zabbix templateids.');
  $output .= zabbix_roles_table();
  $output .= l('Add mapping', 'zabbix-role-mapping/add') . ' | ' . l('Import from zabbix', 'zabbix-role-mapping/import');

  return $output;
}


/**
 *
 * @return <type>
 */
function zabbix_bridge_settings_form() {
  $form['zabbix_bridge_credentials'] = array(
      '#type' => 'fieldset',
      '#title' => t('Zabbix credentials'),
  );
  $form['zabbix_bridge_credentials']['zabbix_bridge_API_url'] = array(
      '#type' => 'textfield',
      '#title' => t('Zabbix API url'),
      '#default_value' => variable_get('zabbix_bridge_API_url', zabbix_bridge_API_url),
      '#required' => TRUE,
      '#description' => t('Url used for API calls. If you don\'t know what to put here, try the full path to your zabbix install, like: http://example.com/zabbix')
  );

  $form['zabbix_bridge_credentials']['zabbix_bridge_API_user'] = array(
      '#type' => 'textfield',
      '#title' => t('Zabbix API user'),
      '#default_value' => variable_get('zabbix_bridge_API_user', zabbix_bridge_API_user),
      '#description' => t('User used for API calls. Has to have API calling enabled, and has to be a super-admin.')
  );

  $form['zabbix_bridge_credentials']['zabbix_bridge_API_pass'] = array(
      '#type' => 'password_confirm',
      '#title' => t('Zabbix API password'),
      '#size' => 25,
      '#description' => t('Password used for API calls. This is the zabbix password of the API user.')
  );


  // Add zabbix trace deletion options
  $trace_options = array(
      'yes' => 'Yes',
      'no' => 'No'
  );

  $form['zabbix_bridge_trace'] = array(
      '#type' => 'radios',
      '#title' => t('Delete Zabbix Traces Upon Account Deletion'),
      '#default_value' => variable_get('zabbix_bridge_trace', 'no'),
      '#description' => t('If checked, deleting a user account will remove all Zabbix traces (user, usergroup, host, hostgroup, media, actions)'),
      '#options' => $trace_options
  );

  // add debug message options
  $debug_options = array(
      'yes' => 'Yes',
      'no' => 'No'
  );

  $form['zabbix_bridge_debug'] = array(
      '#type' => 'radios',
      '#title' => T('Enable Debug Messages'),
      '#default_value' => variable_get('zabbix_bridge_debug', 'no'),
      '#description' => t('Determines whether debug messages show up or not. This should NOT be enabled in production!'),
      '#options' => $debug_options
  );

  $form['zabbix_bridge_debug_verbose'] = array(
      '#type' => 'radios',
      '#title' => T('Enable Verbose Debug Messages'),
      '#default_value' => variable_get('zabbix_bridge_debug_verbose', 'no'),
      '#description' => t('Determines whether debug messages show full information. Less readable, more debuggable :)'),
      '#options' => $debug_options
  );

  return system_settings_form($form);
}

function zabbix_bridge_alert_notifications() {

  return drupal_get_form('zabbix_bridge_alert_notifications_form');

}

function zabbix_bridge_alert_notifications_form($formstate) {

  $form['zabbix_bridge_notifications'] = array(
      '#type' => 'fieldset',
      '#title' => t('Zabbix Alert Notifications'),
  );

  //average alert
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_average_subject'] = array(
      '#type' => 'textfield',
      '#title' => t('Average alert subject'),
      '#default_value' => variable_get('zabbix_bridge_alert_average_subject', t('Average alert subject')),
      '#required' => TRUE,
      '#description' => t('The subject line of the email that you want to send to customers when they receive an alert of the level <strong>average</strong>. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_average'] = array(
      '#type' => 'textarea',
      '#title' => t('Average alert'),
      '#default_value' => variable_get('zabbix_bridge_alert_average', t('Average alert')),
      '#required' => TRUE,
      '#description' => t('The full email that you want to send to customers when they receive an alert of the level <strong>average</strong>. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_average_subject_recovery'] = array(
      '#type' => 'textfield',
      '#title' => t('Average recovery alert subject'),
      '#default_value' => variable_get('zabbix_bridge_alert_average_subject_recovery', t('Average recovery alert subject')),
      '#required' => TRUE,
      '#description' => t('The subject line of the email that you want to send to customers when they receive an alert of the level <strong>average</strong>. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_average_recovery'] = array(
      '#type' => 'textarea',
      '#title' => t('Average recovery alert'),
      '#default_value' => variable_get('zabbix_bridge_alert_average_recovery', t('Average recovery alert')),
      '#required' => TRUE,
      '#description' => t('The full email that you want to send to customers when they receive an alert of the level <strong>average</strong>. This can use all of the template variables available to zabbix.')
  );

  // high alert
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_high_subject'] = array(
      '#type' => 'textfield',
      '#title' => t('High alert subject'),
      '#default_value' => variable_get('zabbix_bridge_alert_high_subject',  t('High alert subject')),
      '#required' => TRUE,
      '#description' => t('The subject line of the email that you want to send to customers when they receive an alert of the level <strong>high</strong>. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_high'] = array(
      '#type' => 'textarea',
      '#title' => t('High alert'),
      '#default_value' => variable_get('zabbix_bridge_alert_high', t('High alert')),
      '#required' => TRUE,
      '#description' => t('The full email that you want to send to customers when they receive an alert of the level <strong>high</strong>. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_high_subject_recovery'] = array(
      '#type' => 'textfield',
      '#title' => t('High recovery alert subject'),
      '#default_value' => variable_get('zabbix_bridge_alert_high_subject_recovery',  t('High recovery alert subject')),
      '#required' => TRUE,
      '#description' => t('The subject line of the email that you want to send to customers when they receive an alert of the level <strong>high</strong>. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_high_recovery'] = array(
      '#type' => 'textarea',
      '#title' => t('High recovery alert'),
      '#default_value' => variable_get('zabbix_bridge_alert_high_recovery', t('High recovery alert')),
      '#required' => TRUE,
      '#description' => t('The full email that you want to send to customers when they receive an alert of the level <strong>high</strong>. This can use all of the template variables available to zabbix.')
  );

  // disaster alert
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_disaster_subject'] = array(
      '#type' => 'textfield',
      '#title' => t('Disaster alert subject'),
      '#default_value' => variable_get('zabbix_bridge_alert_disaster_subject', t('Disaster alert subject')),
      '#required' => TRUE,
      '#description' => t('The subject line of the email that you want to send to customers when they receive an alert of the level <strong>disaster</strong>. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_disaster'] = array(
      '#type' => 'textarea',
      '#title' => t('Disaster alert'),
      '#default_value' => variable_get('zabbix_bridge_alert_disaster', t('Disaster alert')),
      '#required' => TRUE,
      '#description' => t('The full email that you want to send to customers when they receive an alert of the level <strong>disaster</strong>. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_disaster_subject_recovery'] = array(
      '#type' => 'textfield',
      '#title' => t('Disaster recovery alert subject'),
      '#default_value' => variable_get('zabbix_bridge_alert_disaster_subject_recovery', t('Disaster recovery alert subject')),
      '#required' => TRUE,
      '#description' => t('The subject line of the email that you want to send to customers when they receive an alert of the level <strong>disaster</strong>. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_disaster_recovery'] = array(
      '#type' => 'textarea',
      '#title' => t('Disaster recovery alert'),
      '#default_value' => variable_get('zabbix_bridge_alert_disaster_recovery', t('Disaster recovery alert')),
      '#required' => TRUE,
      '#description' => t('The full email that you want to send to customers when they receive an alert of the level <strong>disaster</strong>. This can use all of the template variables available to zabbix.')
  );

  // info_warn alert
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_info_warn_subject'] = array(
      '#type' => 'textfield',
      '#title' => t('Information & Warning alert subject'),
      '#default_value' => variable_get('zabbix_bridge_alert_info_warn_subject', t('Information & Warning alert subject')),
      '#required' => TRUE,
      '#description' => t('The subject line of the email that you want to send to customers when they receive an alert of the level <strong>Information</strong> or <strong>Warning</strong>. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_info_warn'] = array(
      '#type' => 'textarea',
      '#title' => t('Information & Warning alert'),
      '#default_value' => variable_get('zabbix_bridge_alert_info_warn', t('Information & Warning alert')),
      '#required' => TRUE,
      '#description' => t('The full email that you want to send to customers when they receive an alert of the level <strong>Information</strong> or <strong>Warning</strong>. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_info_warn_subject_recovery'] = array(
      '#type' => 'textfield',
      '#title' => t('Information & Warning recovery alert subject'),
      '#default_value' => variable_get('zabbix_bridge_alert_info_warn_subject_recovery', t('Information & Warning recovery alert subject')),
      '#required' => TRUE,
      '#description' => t('The subject line of the email that you want to send to customers when they receive an alert of the level <strong>Information</strong> or <strong>Warning</strong>. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_info_warn_recovery'] = array(
      '#type' => 'textarea',
      '#title' => t('Information & Warning recovery alert'),
      '#default_value' => variable_get('zabbix_bridge_alert_info_warn_recovery', t('Information & Warning recovery alert')),
      '#required' => TRUE,
      '#description' => t('The full email that you want to send to customers when they receive an alert of the level <strong>Information</strong> or <strong>Warning</strong>. This can use all of the template variables available to zabbix.')
  );

  // sms alert
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_sms_subject'] = array(
      '#type' => 'textfield',
      '#title' => t('SMS alert subject'),
      '#default_value' => variable_get('zabbix_bridge_alert_sms_subject', t('SMS alert subject')),
      '#required' => TRUE,
      '#description' => t('The subject line of the sms that you want to send to customers when they receive an alert by text message. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_sms'] = array(
      '#type' => 'textarea',
      '#title' => t('SMS alert'),
      '#default_value' => variable_get('zabbix_bridge_alert_sms', t('SMS alert')),
      '#required' => TRUE,
      '#description' => t('The full email that you want to send to customers when they receive an alert by text message. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_sms_subject_recovery'] = array(
      '#type' => 'textfield',
      '#title' => t('SMS recovery alert subject'),
      '#default_value' => variable_get('zabbix_bridge_alert_sms_subject_recovery', t('SMS recovery alert subject')),
      '#required' => TRUE,
      '#description' => t('The subject line of the sms that you want to send to customers when they receive an alert by text message. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_sms_recovery'] = array(
      '#type' => 'textarea',
      '#title' => t('SMS recovery alert'),
      '#default_value' => variable_get('zabbix_bridge_alert_sms_recovery', t('SMS recovery alert')),
      '#required' => TRUE,
      '#description' => t('The full email that you want to send to customers when they receive an alert by text message. This can use all of the template variables available to zabbix.')
  );

  // jabber alert
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_jabber_subject'] = array(
      '#type' => 'textfield',
      '#title' => t('Jabber alert subject'),
      '#default_value' => variable_get('zabbix_bridge_alert_jabber_subject', t('Jabber alert subject')),
      '#required' => TRUE,
      '#description' => t('The subject line of the jabber message that you want to send to customers when they receive an alert by jabber. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_jabber'] = array(
      '#type' => 'textarea',
      '#title' => t('Jabber alert'),
      '#default_value' => variable_get('zabbix_bridge_alert_jabber', t('Jabber alert')),
      '#required' => TRUE,
      '#description' => t('The full jabber message that you want to send to customers when they receive an alert by text jabber. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_jabber_subject_recovery'] = array(
      '#type' => 'textfield',
      '#title' => t('Jabber recovery alert subject'),
      '#default_value' => variable_get('zabbix_bridge_alert_jabber_subject_recovery', t('Jabber recovery alert subject')),
      '#required' => TRUE,
      '#description' => t('The subject line of the jabber message that you want to send to customers when they receive an alert by jabber. This can use all of the template variables available to zabbix.')
  );
  $form['zabbix_bridge_notifications']['zabbix_bridge_alert_jabber_recovery'] = array(
      '#type' => 'textarea',
      '#title' => t('Jabber recovery alert'),
      '#default_value' => variable_get('zabbix_bridge_alert_jabber_recovery', t('Jabber recovery alert')),
      '#required' => TRUE,
      '#description' => t('The full jabber message that you want to send to customers when they receive an alert by text jabber. This can use all of the template variables available to zabbix.')
  );

  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save')
  );

  return($form);

}


function zabbix_bridge_alert_notifications_form_submit($form, &$form_state){

  // get the values from the form
  $alert_avg        = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_average'];
  $alert_high       = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_high'];
  $alert_info       = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_info_warn'];
  $alert_disaster   = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_disaster'];
  $alert_sms        = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_sms'];
  $alert_jabber     = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_jabber'];

  $alert_avg_subject          = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_average_subject'];
  $alert_high_subject         = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_high_subject'];
  $alert_info_subject         = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_info_warn_subject'];
  $alert_disaster_subject     = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_disaster_subject'];
  $alert_sms_subject          = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_sms_subject'];
  $alert_jabber_subject       = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_jabber_subject'];

  $alert_avg_recovery        = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_average_recovery'];
  $alert_high_recovery       = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_high_recovery'];
  $alert_info_recovery       = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_info_warn_recovery'];
  $alert_disaster_recovery   = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_disaster_recovery'];
  $alert_sms_recovery        = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_sms_recovery'];
  $alert_jabber_recovery     = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_jabber_recovery'];

  $alert_avg_subject_recovery          = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_average_subject_recovery'];
  $alert_high_subject_recovery         = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_high_subject_recovery'];
  $alert_info_subject_recovery         = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_info_warn_subject_recovery'];
  $alert_disaster_subject_recovery     = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_disaster_subject_recovery'];
  $alert_sms_subject_recovery          = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_sms_subject_recovery'];
  $alert_jabber_subject_recovery       = $form['zabbix_bridge_notifications']['#post']['zabbix_bridge_alert_jabber_subject_recovery'];

  //store the varaiables in drupal
  variable_set('zabbix_bridge_alert_average', $alert_avg);
  variable_set('zabbix_bridge_alert_high', $alert_high);
  variable_set('zabbix_bridge_alert_info_warn', $alert_info);
  variable_set('zabbix_bridge_alert_disaster', $alert_disaster);
  variable_set('zabbix_bridge_alert_sms', $alert_sms);
  variable_set('zabbix_bridge_alert_jabber', $alert_jabber);

  variable_set('zabbix_bridge_alert_average_subject', $alert_avg_subject);
  variable_set('zabbix_bridge_alert_high_subject', $alert_high_subject);
  variable_set('zabbix_bridge_alert_info_warn_subject', $alert_info_subject);
  variable_set('zabbix_bridge_alert_disaster_subject', $alert_disaster_subject);
  variable_set('zabbix_bridge_alert_sms_subject', $alert_sms_subject);
  variable_set('zabbix_bridge_alert_jabber_subject', $alert_jabber_subject);

  variable_set('zabbix_bridge_alert_average_recovery', $alert_avg_recovery);
  variable_set('zabbix_bridge_alert_high_recovery', $alert_high_recovery);
  variable_set('zabbix_bridge_alert_info_warn_recovery', $alert_info_recovery);
  variable_set('zabbix_bridge_alert_disaster_recovery', $alert_disaster_recovery);
  variable_set('zabbix_bridge_alert_sms_recovery', $alert_sms_recovery);
  variable_set('zabbix_bridge_alert_jabber_recovery', $alert_jabber_recovery);

  variable_set('zabbix_bridge_alert_average_subject_recovery', $alert_avg_subject_recovery);
  variable_set('zabbix_bridge_alert_high_subject_recovery', $alert_high_subject_recovery);
  variable_set('zabbix_bridge_alert_info_warn_subject_recovery', $alert_info_subject_recovery);
  variable_set('zabbix_bridge_alert_disaster_subject_recovery', $alert_disaster_subject_recovery);
  variable_set('zabbix_bridge_alert_sms_subject_recovery', $alert_sms_subject_recovery);
  variable_set('zabbix_bridge_alert_jabber_subject_recovery', $alert_jabber_subject_recovery);


  // get a list of all zabbix users from zabbix
  $userids = zabbix_bridge_get_all_users_from_zabbix();
  
  // loop through list
  foreach ($userids as $user => $userid) {
    $name = $userid['alias'];

    if (strlen($name) < 1)
      drupal_set_message("Notification update failed. Error tr-1b.");

    // search in all of the usergroups for this user for a usergroup that has our
    // name structure. if it does, this customer needs to be processed.
    $zabbixusergroupid = 0;
    foreach ($userid['usrgrps'] as $usrgrp) {
      if (!(strpos($usrgrp['name'], STR_CUST_USERGROUP) === FALSE)) {
        $zabbixusergroupid = $usrgrp['usrgrpid'];
      }
    }

    if ($zabbixusergroupid) {
      // for every userid, get the connected hostgroupid (a businessrule for
      // tribily is that 1 user is in 1 usergroup that has rights to just 1 hostgroup)
      $hostgroupid = zabbix_bridge_get_all_hostgroupids_from_drupal($userid['userid']);

      if (!empty($hostgroupid)) {

        // if we did indeed find a hostgroup, get all actions including the operations and conditions
        $actions = array();
        $actions = zabbix_bridge_get_all_actions_from_zabbix($hostgroupid[0]);


        // due to a bug in 1.8.3 API, the media stuff gets messed up if we just try
        // to update an action, so as a workaround we delete and recreate the action
        foreach ($actions as $action) {
          if (!ZabbixAPI::query('action', 'delete', array($action['actionid']))) {
            drupal_set_message("Notification update failed. Error tr-1a.");
          }
        }

        zabbix_bridge_create_basic_actions($name . '_INFO_WARN', TRIGGER_SEVERITY_WARNING, $zabbixusergroupid, $hostgroupid[0]);
        zabbix_bridge_create_basic_actions($name . '_HIGH', TRIGGER_SEVERITY_HIGH, $zabbixusergroupid, $hostgroupid[0]);
        zabbix_bridge_create_basic_actions($name . '_DISASTER', TRIGGER_SEVERITY_DISASTER, $zabbixusergroupid, $hostgroupid[0]);
        zabbix_bridge_create_basic_actions($name . '_AVERAGE', TRIGGER_SEVERITY_AVERAGE, $zabbixusergroupid, $hostgroupid[0]);
        zabbix_bridge_create_mobile_action($name . '_SMS', $zabbixusergroupid, $hostgroupid[0]);
        zabbix_bridge_create_jabber_action($name . '_JABBER', $zabbixusergroupid, $hostgroupid[0]);
      }
    }
  }
}

