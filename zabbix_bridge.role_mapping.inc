<?php

// $Id$

/**
 * @file
 * User mapping functions. Used for mapping drupal users to their zabbix counterparts
 *
 * @author Walter
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
function zabbix_bridge_role_mapping_form(&$form_state, $action, $roleid = NULL) {

  if ($action == 'update') {
    $sql = "select zr.roleid, zr.name, zr.description, zrt.templateid from {zabbix_role} zr left join {zabbix_roles_templates} zrt on zr.roleid = zrt.roleid where zr.roleid = '%s'";
    $result = db_query($sql, $roleid);
    $zabbix_role = db_fetch_object($result);
  }

  $form['role_mapping'] = array(
      '#type' => 'fieldset',
      '#title' => t('Role <=> Template mapping'),
  );

  $form['role_mapping']['roleid'] = array(
      '#type' => 'hidden',
      '#value' => $action == 'update' ? $zabbix_role->roleid : '',
  );

  $form['role_mapping']['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Role name'),
      '#value' => $action == 'update' ? $zabbix_role->name : '',
      '#description' => t('The name of the role'),
  );

  $form['role_mapping']['description'] = array(
      '#type' => 'textfield',
      '#title' => t('Description'),
      '#value' => $action == 'update' ? $zabbix_role->description : '',
      '#description' => t('The description of the role'),
  );

  $form['role_mapping']['templateid'] = array(
      '#type' => 'textfield',
      '#title' => t('Zabbix templateid'),
      '#value' => $action == 'update' ? $zabbix_role->templateid : '',
      '#description' => t('The id of the template thsi role is connected to.\n
                             It corresponds to hosts.hostid in the zabbix database (templates are hosts with status == 3 in zabbix).'),
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
function zabbix_bridge_role_mapping_form_submit($form, &$form_state) {

  $roleid = $form_state['clicked_button']['#post']['roleid'];
  $rolename = $form_state['clicked_button']['#post']['name'] != '' ? $form_state['clicked_button']['#post']['name'] : 'null';
  $roledescription = $form_state['clicked_button']['#post']['description'] != '' ? $form_state['clicked_button']['#post']['description'] : 'null';
  $templateids = explode(',', $form_state['clicked_button']['#post']['templateid']);

  if ($roleid != '') {
    // update the role
    $sql = "update {zabbix_role} set name = '%s', description = '%s' where roleid = '%s'";
    $result = db_query($sql, $rolename, $roledescription, $roleid);
    if ($result) {
      drupal_set_message(t('Host role updated succesfully'));
    }
    else {
      drupal_set_message(t('An error occured updating the host role'), DRUPAL_MSG_TYPE_ERR);
    }
  }
  else {

    $sql = "insert into {zabbix_role} (name, description) values ('%s', '%s')";
    $result = db_query($sql, $rolename, $roledescription);
    if ($result) {
      drupal_set_message(t('Host role added succesfully'));
    }
    else {
      drupal_set_message(t('An error occured adding the host role'), DRUPAL_MSG_TYPE_ERR);
    }
    $roleid = db_last_insert_id('zabbix_role', 'roleid');
  }

  // delete connected template ids
  $sql = "delete from {zabbix_roles_templates} where roleid = '%s'";
  db_query($sql, $roleid);

  // (re-)insert templateids
  if ($templateids[0] != '') {
    foreach ($templateids as $templateid) {
      $sql = "insert into {zabbix_roles_templates}(roleid, templateid) values ('%s', '%s')";
      db_query($sql, $roleid, $templateid);
    }
  }

  drupal_goto('admin/settings/zabbix_bridge');
}

function zabbix_bridge_role_mapping_delete_confirm(&$form_state, $action, $id) {

  $form['_mapping'] = array(
      '#type' => 'value',
      '#value' => $id,
  );

  // TODO: check if no hosts have this role assigned
  return confirm_form(
          $form,
          t('Are you sure you want to delete this mapping?'),
          isset($_GET['destination']) ? $_GET['destination'] : "admin/settings/zabbix_bridge",
          t("This action cannot be undone.\nWARNING: currently there is no check that no host has this role assigned. Be careful!"),
          t('Delete'),
          t('Cancel')
  );
}

function zabbix_bridge_role_mapping_delete_confirm_submit($form, &$form_state) {

  if ($form_state['values']['confirm']) {
    $id = $form_state['values']['_mapping'];

    if (zabbix_role_mapping_delete($id)) {
      drupal_set_message(t('Mapping has been deleted successfully.'));
    }
    else {
      drupal_set_message(t('Mapping deletion unsuccessful'), DRUPAL_MSG_TYPE_ERR);
    }
  }

  drupal_goto("admin/settings/zabbix_bridge");
}

/**
 * Deletes a roles from the database, 
 *
 * @param  array    $formvalues ...
 * @param  int      $hostid     The drupal id of the host to be deleted
 * @return bool
 */
function zabbix_role_mapping_delete($mappingid) {

  $sql = "delete from {zabbix_roles_templates} where roleid = '%s'";
  $result = db_query($sql, $mappingid);


  $sql = "delete from {zabbix_role} where roleid = '%s'";
  $result = db_query($sql, $mappingid);

  return $result;
}

/**
 *
 * @return <type>
 */
function zabbix_bridge_import_roles() {

  $templates = array();
  $templates = zabbix_bridge_get_all_templateids_from_zabbix(TRUE);

  zabbix_bridge_debug(print_r($templates, TRUE));

  foreach ($templates as $template) {
    $sql = "select * from {zabbix_roles_templates} where templateid = '%s'";
    $results = db_query($sql, $template['templateid']);

    $roleid = db_fetch_object($results);

    if ($roleid) {
      drupal_set_message('Skipped template ' . $template['templateid'] . ' (' . $template['name'] . '), as it already exists.');
    }
    else {

      $sql = "insert into {zabbix_role} (name, description) values ('%s', '%s')";
      $results = db_query($sql, $template['name'], $template['name'] . ' role');

      $roleid = db_last_insert_id('zabbix_role', 'roleid');

      $sql = "insert into {zabbix_roles_templates} (roleid, templateid) values ('%s', '%s')";
      $results = db_query($sql, $roleid, $template['templateid']);
    }
  }

  drupal_goto("admin/settings/zabbix_bridge");
}
