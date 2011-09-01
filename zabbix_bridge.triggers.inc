<?php

include_once 'zabbix_functions.php';

/**
 * Display a html table with triggers
 *
 * @param <type> $hostid
 */
function zabbix_bridge_trigger_table($hostid){

  //convert the hostid from the drupal id to the zabbix id
  $zhostid = zabbix_bridge_drupal_to_zabbix_hostid($hostid);

  //get all triggers for the host
  $triggers = zabbix_bridge_get_all_triggers_from_zabbix($zhostid);

  //prepare table
  $header = array('Status', 'Name', 'Description', 'Template');
  $rows = array();
  $table_attributes = array('id' => 'zabbix_bridge_trigger_table');

  //go through triggers and build table
  foreach ($triggers as $trigger) {

    $rows[] = array($trigger['status'] ? 'disabled' : 'enabled',
                    $trigger['description'],
                    $trigger['comments'],
                    $trigger['templateid']);

  }

  //create actual html table
  $output = theme('table', $header, $rows);

  return $output;
}

/**
 *
 * @return string a string containing the html code to display an overview of all triggers for a host
 */
function zabbix_bridge_triggers_overview($hostid) {

  drupal_set_title('Triggers');

  $output = "<div id=\"triggers-explain\"><p>Here you can enable/disable the triggers you would like to monitor. </p></div>";
  $output .= zabbix_bridge_trigger_table($hostid);
  
  return $output;
}

?>
