<?php
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

function zabbix_bridge_triggers_overview() {
  global $user;

  drupal_set_title('Triggers');


  $output = "<div id=\"hosts-explain\"><p>Here you can enable/disable the triggers you would like to monitor. </p></div>";
  //$output .= zabbix_trigger_table($);
  
  return $output;
}

?>
