<?php

include_once 'zabbix_functions.php';

/**
 * Display a html table with triggers
 *
 * @param <type> $hostid
 */
function zabbix_bridge_trigger_tables($hostid){

  //convert the hostid from the drupal id to the zabbix id
  $zhostid = zabbix_bridge_drupal_to_zabbix_hostid($hostid);

  //get all triggers for the host
  $triggers = zabbix_bridge_get_all_triggers_from_zabbix($zhostid);
  $hostname = $triggers[0]['hosts'][0]['host'];

  //zabbix_bridge_debug(print_r($triggers, TRUE));

  //prepare table
  $header = array('Status', 'Name', 'Description');
  $rowsets = array();
  $roles = array();

  //go through triggers and build table
  foreach ($triggers as $trigger) {

    /* okay, the way this works:
     * we only have triggers on template level, which means that any trigger can
    * only have items in it from that template. Therefore, getting the template
    * a host-level trigger comes from is jut a matter of getting the
    * trigger.templateid, which points to the triggerid at template level.
    * For that trigger, we get all functions. Those give us the itemids
    * involved in this trigger, so we get the hostid from the first itemid in
    * the first function and look up it's hostid. That is the template this
    * trigger belongs to.
    */

    // TODO: we don't need to get those templates for every item, jsut distinct ones??
    // Unfortunately this is the triggerid of the templated trigger, not the
    // hostid of the template :(
    $template = zabbix_bridge_get_template_by_triggerid($trigger['templateid']);

    if (!isset($rowsets[$template['host']])) {
      $rowsets[$template['host']] = array();
      $roles[$template['host']] = zabbix_bridge_get_rolename_by_templateid($template['hostid']);
    }

    if ($trigger['status'] == 0)
    $link = 'hosts/'.$hostid.'/triggers/'.$trigger['triggerid'].'/disable';
    else
    $link = 'hosts/'.$hostid.'/triggers/'.$trigger['triggerid'].'/enable';

    $rowsets[$template['host']][] = array(
    //l($trigger['status'] ? 'disabled' : 'enabled', $link),
    $trigger['status'] ? 'disabled' : 'enabled',
    str_replace('{HOSTNAME}', '{'.$hostname.'}', $trigger['description']),
    $trigger['comments']);


  }

  reset($rowsets);
  reset($roles);

  //create actual html tables
  foreach ($rowsets as $template => $rowset) {

    $table_attributes = array('id' => 'zabbix_bridge_trigger_table_'.$template);

    $output .= '<h2>Role: '.$roles[$template].'</h2>';

    $output .= theme('table', $header, $rowset, $table_attributes);
  }

  return $output;
}

/**
 *
 * @return string
 *    a string containing the html code to display an overview of all triggers for a host
 */
function zabbix_bridge_triggers_overview($hostid) {

  drupal_set_title('Triggers');

  $output = "<div id=\"triggers-explain\"><p>Here you can enable/disable the triggers you would like to monitor. </p></div>";
  $output .= zabbix_bridge_trigger_tables($hostid);

  return $output;
}

/**
 *
 * @param integer $hostid
 * @param integer $triggerid
 * @param string $action
 * @return
 */
function zabbix_bridge_triggers_toggle_confirm(&$form_state, $hostid, $triggerid, $action){

  $form['_hostid'] = array(
      '#type' => 'hidden',
      '#value' => $hostid,
  );
  $form['_triggerid'] = array(
      '#type' => 'hidden',
      '#value' => $triggerid,
  );

  $form['_action'] = array(
      '#type' => 'hidden',
      '#value' => $action,
  );

  return confirm_form(
  $form,
  t('Are you sure you want to change the enabled state of this trigger?'),
  isset($_GET['destination']) ? $_GET['destination'] : "hosts/$hostid/triggers",
  $action == 'disable' ? t('This is NOT recommended. You will not be alerted of any problems arising with the server concerning this trigger until you reenable it!') :
  t('You will once again receive notifications for this trigger.'),
  t('OK'),
  t('Cancel')
  );
}

function zabbix_bridge_triggers_toggle_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $hostid = $form_state['values']['_hostid'];
    $triggerid = $form_state['values']['_triggerid'];;
    $action = $form_state['values']['_action'];

    if (zabbix_bridge_trigger_toggle($triggerid, $action)) {
      drupal_set_message(t('Trigger has been updated successfully.'));
    }
    else {
      drupal_set_message(t('Trigger update unsuccessful'), DRUPAL_MSG_TYPE_ERR);
    }
  }

  drupal_goto("hosts/$hostid/triggers");
}


?>
