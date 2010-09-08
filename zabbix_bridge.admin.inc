<?php
// $Id: zabbix_bridge.admin.inc,v 1.7 2009/04/06 02:13:18 rmiddle Exp $

/**
 * @file
 * Settings form.
 */

/**
 * Module settings page.
 */
function zabbix_bridge_settings() {
    /*
  $form['user_notify'] = array(
    '#type' => 'fieldset',
    '#title' => t('User Alerts'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );
  $user_notify_opts = array(
    'Custom' => t('Send to a custom email address'),
    'Role' => t('Send to a specific role(s)'),
    'Both' => t('Send to Both a custom email address and a specific role(s)'),
    );
  $form['user_notify']['zabbix_bridge_type'] = array(
    '#type' => 'radios',
    '#options' => $user_notify_opts,
    '#title' => t('Send by Role or Custom Email address?'),
    '#default_value' => variable_get('zabbix_bridge_type', 'Custom'),
    '#prefix' => '<div class="user-register-notify-user-notify">',
    '#suffix' => '</div>',
    );
  $form['user_notify']['zabbix_bridge_mailto'] = array(
    '#type' => 'textfield',
    '#title' => t('Send notifications to this custom email address'),
    '#description' => t('If you leave this blank, the site email address, %mailto, will be used.', array('%mailto' => variable_get('site_mail', ini_get('sendmail_from')))),
    '#default_value' => variable_get('zabbix_bridge_mailto', variable_get('site_mail', ini_get('sendmail_from'))),
  );
  $user_roles = user_roles(TRUE);
  $form['user_notify']['zabbix_bridge_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Send to a specific role(s)'),
    '#options' => $user_roles,
    '#description' => t('All users with these checked roles will receive a notification email when selected.'),
    '#default_value' => variable_get('zabbix_bridge_roles', user_roles(TRUE)),
  );

  $form['zabbix_bridge_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Email Subject'),
    '#default_value' => variable_get('zabbix_bridge_subject', zabbix_bridge_SUBJECT),
    '#required' => TRUE,
    '#description' => t('Subject of user registration messages')
  );
  $form['zabbix_bridge_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Email Body'),
    '#default_value' => variable_get('zabbix_bridge_body', zabbix_bridge_BODY),
    '#rows' => 10,
    '#required' => TRUE,
    '#description' => t('Customize the body of the user registration notification email.') .' '. t('Available variables are:') .' !user_name, !user_mail, !user_view, !user_edit, !user_delete, !site, !uri, !uri_brief, !date, !action, !profile, !og.'
  );
  $alert_options = array(
    'create' => 'Receive Email upon user creation.',
    'update' => 'Receive Email upon user creation and update.',
    );
  $form['zabbix_bridge_alert'] = array(
    '#type' => 'radios',
    '#options' => $alert_options,
    '#title' => t('Page Sort order'),
    '#default_value' => variable_get('zabbix_bridge_alert', 'create'),
    '#description' => t('When to send an Email'),
    '#required' => FALSE,
  );
     */

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

  return system_settings_form($form);
}
