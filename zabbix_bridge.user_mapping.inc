<?php

// $Id$

/**
 * @file
 * User mapping functions. Used for mapping drupal users to their zabbix counterparts
 *
 * @author raj
 */
include_once 'zabbix_functions.php';

/**
 * @author walterheck
 *
 * @param <type> $form_state
 * @param <type> $action
 * @param <type> $drupal_uid
 * @return int
 */
function zabbix_bridge_user_mapping_form(&$form_state, $action, $id = NULL) {

  if ($action == 'update') {
    $sql = "select id, drupal_uid, zabbix_uid, zabbix_usrgrp_id, zabbix_hostgrp_id from {zabbix_drupal_account} where id = :id";
    $result = db_query($sql, array(':id' => $id));
    $zabbix_drupal_account = db_fetch_object($result);
  }

  $form['user_mapping'] = array(
      '#type' => 'fieldset',
      '#title' => t('Mapping Drupal <=> Zabbix'),
  );

  $form['user_mapping']['id'] = array(
      '#type' => 'hidden',
      '#value' => $action == 'update' ? $zabbix_drupal_account->id : '',
  );

  $form['user_mapping']['drupal_userid'] = array(
      '#type' => 'textfield',
      '#title' => t('Drupal userid'),
      '#value' => $action == 'update' ? $zabbix_drupal_account->drupal_uid : '',
      '#description' => t('The id that this user corresponds to in the {users}.uid field in the drupal database'),
  );

  $form['user_mapping']['zabbix_userid'] = array(
      '#type' => 'textfield',
      '#title' => t('Zabbix userid'),
      '#value' => $action == 'update' ? $zabbix_drupal_account->zabbix_uid : '',
      '#description' => t('The id that this user corresponds to in the users.userid field in the zabbix database'),
  );

  $form['user_mapping']['zabbix_usergroupid'] = array(
      '#type' => 'textfield',
      '#title' => t('Zabbix usergroupid'),
      '#value' => $action == 'update' ? $zabbix_drupal_account->zabbix_usrgrp_id : '',
      '#description' => t('The id of the usergroup this user is connected to.\n
                             The zabbix bridges uses this for permissions\n
                             It corresponds to usrgrp.usrgrpid in the zabbix database.'),
  );

  $form['user_mapping']['zabbix_hostgroupid'] = array(
      '#type' => 'textfield',
      '#title' => t('Zabbix hostgroupid'),
      '#value' => $action == 'update' ? $zabbix_drupal_account->zabbix_hostgrp_id : '',
      '#description' => t('The hostgroup that all hosts this user adds in the zabbix bridge will be added to.\n
                             The zabbix bridges uses this for permissions.\n
                             This corrsponds to groups.groupid in the zabbix database.'),
  );

  $form['hosts']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('submit'),
      '#weight' => 30
  );

  return $form;
}

/**
 *
 * @param <type> $form
 * @param <type> $form_state
 */
function zabbix_bridge_user_mapping_form_submit($form, &$form_state) {

  $id = $form_state['clicked_button']['#post']['id'];
  $drupal_uid = $form_state['clicked_button']['#post']['drupal_userid'] != '' ? $form_state['clicked_button']['#post']['drupal_userid'] : 'null';
  $zabbix_userid = $form_state['clicked_button']['#post']['zabbix_userid'] != '' ? $form_state['clicked_button']['#post']['zabbix_userid'] : 'null';
  $zabbix_usergroupid = $form_state['clicked_button']['#post']['zabbix_usergroupid'] != '' ? $form_state['clicked_button']['#post']['zabbix_usergroupid'] : 'null';
  $zabbix_hostgroupid = $form_state['clicked_button']['#post']['zabbix_hostgroupid'] != '' ? $form_state['clicked_button']['#post']['zabbix_hostgroupid'] : 'null';

  if ($id != '') {
    $sql = "update
                    {zabbix_drupal_account}
                set
                    drupal_uid = %s,
                    zabbix_uid = %s,
                    zabbix_usrgrp_id = %s,
                    zabbix_hostgrp_id = %s
                where
                    id = '%s'";
    $result = db_query($sql, $drupal_uid, $zabbix_userid, $zabbix_usergroupid, $zabbix_hostgroupid, $id);
    if ($result) {
      drupal_set_message(t('Drupal-zabbix account mapping updated succesfully'));
    }
    else {
      drupal_set_message(t('An error occured updating the Drupal-zabbix account mapping'), DRUPAL_MSG_TYPE_ERR);
    }
  }
  else {

    $sql = "insert into
                    {zabbix_drupal_account} (
                        drupal_uid,
                        zabbix_uid,
                        zabbix_usrgrp_id,
                        zabbix_hostgrp_id)
                values (
                    %s,
                    %s,
                    %s,
                    %s)";
    $result = db_query($sql, $drupal_uid, $zabbix_userid, $zabbix_usergroupid, $zabbix_hostgroupid);
    if ($result) {
      drupal_set_message(t('Drupal-zabbix account mapping added succesfully'));
    }
    else {
      drupal_set_message(t('An error occured adding the Drupal-zabbix account mapping'), DRUPAL_MSG_TYPE_ERR);
    }
  }

  drupal_goto('admin/settings/zabbix_bridge');
}

function zabbix_bridge_user_mapping_generate_confirm(&$form_state, $action, $id) {

  $form['_mapping'] = array(
      '#type' => 'value',
      '#value' => $id,
  );

  return confirm_form(
  $form,
  t("Are you sure you want to generate the missing items for this mapping?"),
  isset($_GET['destination']) ? $_GET['destination'] : "admin/settings/zabbix_bridge",
  t("This action cannot be undone.\n
               This will generate a user, usergroup and hostgroup in zabbix."),
  t('Generate'),
  t('Cancel')
  );
}

function zabbix_bridge_user_mapping_generate_confirm_submit($form, &$form_state) {

  $id = $form_state['values']['_mapping'];

  //Get the current mapping
  $sql = "select drupal_uid, zabbix_uid, zabbix_usrgrp_id, zabbix_hostgrp_id from {zabbix_drupal_account} where id = :id";
  $result = db_query($sql, array(':id' => $id));

  $mapping = db_fetch_object($result);

  if (!isset($mapping->drupal_uid) &&
  !isset($mapping->zabbix_uid) &&
  !isset($mapping->zabbix_usrgrp_id) &&
  !isset($mapping->zabbix_hostgrp_id)) {
    //all fields are empty, invalid situation
    drupal_set_message(t('Completely empty mapping, cannot proceed!'), DRUPAL_MSG_TYPE_ERR);
  }
  elseif (isset($mapping->drupal_uid) &&
  !isset($mapping->zabbix_uid) &&
  !isset($mapping->zabbix_usrgrp_id) &&
  !isset($mapping->zabbix_hostgrp_id)) {
    // drupal user exists, rest is empty
    $generatezabbixuser = !isset($mapping->zabbix_uid);
    $generatezabbixusergroup = !isset($mapping->zabbix_usrgrp_id);
    $generatezabbixhostgroup = !isset($mapping->zabbix_hostgrp_id);

    $drupaluser = user_load(array('uid' => $mapping->drupal_uid));

    zabbix_bridge_create_zabbix_user(
    $drupaluser->name,
    $pass,
    $drupaluser->mail,
    $mapping->drupal_uid,
    NULL,
    NULL,
    NULL,
    TRUE,
    TRUE,
    TRUE,
    $id);
  }
  else {
    // other situations not yet supported
    drupal_set_message(t('Not yet implemented, please create the mapping manually.'), DRUPAL_MSG_TYPE_ERR);
  }
  drupal_goto('admin/settings/zabbix_bridge');
}

function zabbix_bridge_user_mapping_delete_confirm(&$form_state, $action, $id) {

  $form['_mapping'] = array(
      '#type' => 'value',
      '#value' => $id,
  );

  return confirm_form(
  $form,
  t('Are you sure you want to delete this mapping?'),
  isset($_GET['destination']) ? $_GET['destination'] : "admin/settings/zabbix_bridge",
  t('This action cannot be undone.'),
  t('Delete'),
  t('Cancel')
  );
}

function zabbix_bridge_user_mapping_delete_confirm_submit($form, &$form_state) {

  if ($form_state['values']['confirm']) {
    $id = $form_state['values']['_mapping'];

    if (zabbix_mapping_delete($id)) {
      drupal_set_message(t('Mapping has been deleted successfully.'));
    }
    else {
      drupal_set_message(t('Mapping deletion unsuccessful'), DRUPAL_MSG_TYPE_ERR);
    }
  }

  drupal_goto("admin/settings/zabbix_bridge");
}

/**
 * Deletes a host and it's assigned roles from the database, both on the drupal
 * side and on the zabbix side
 *
 * @param  array    $formvalues ...
 * @param  int      $hostid     The drupal id of the host to be deleted
 * @return bool
 */
function zabbix_mapping_delete($mappingid) {

  $sql = "delete from {zabbix_drupal_account} where id = '%s'";
  $result = db_query($sql, $mappingid);

  return $result;
}

