<?php

// $Id$

/**
 * @file
 * User mapping functions. Used for mapping server roles to zabbix templates
 *
 * @author Walter
 */
include_once 'zabbix_functions.php';

/**
 * Generates the form for updating a role (only accessible for admins)
 *
 *
 * @author walterheck
 *
 * @param <type> $form_state
 * @param <type> $action 'update' or 'insert'
 * @param <type> $roleid The id of the role we're updating if this is an update
 * action, otherwise null
 * @return int
 */
function zabbix_bridge_role_mapping_form(&$form_state, $action, $roleid = NULL) {

  // if this is update mode, get the existing data for the role we're updating
  if ($action == 'update') {
    $sql = "select zr.roleid, zr.name, zr.description, zr.typeid, zr.sortorder, zr.imagesrc, zrt.templateid from {zabbix_role} zr left join {zabbix_roles_templates} zrt on zr.roleid = zrt.roleid where zr.roleid = :roleid";
    $result = db_query($sql, array(':roleid' => $roleid));
    $zabbix_role = db_fetch_object($result);
  }

  //buidl all the elements of the form
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

  $form['role_mapping']['sortorder'] = array(
      '#type' => 'textfield',
      '#title' => t('Sort Order'),
      '#value' => $action == 'update' ? $zabbix_role->sortorder : '',
      '#description' => t('The integer value that determins the place in the order for this role'),
  );

  $form['role_mapping']['typeid'] = array(
      '#type' => 'textfield',
      '#title' => t('Type'),
      '#value' => $action == 'update' ? $zabbix_role->typeid : '',
      '#description' => t('The type of the role (1=hardware, 2=operating system, 3=applications)'),
  );

  $form['role_mapping']['imagesrc'] = array(
      '#type' => 'textfield',
      '#title' => t('Image Location'),
      '#value' => $action == 'update' ? $zabbix_role->imagesrc : '',
      '#description' => t('The url relative to the webroot of the image that represents this role'),
  );

  $form['role_mapping']['templateid'] = array(
      '#type' => 'textfield',
      '#title' => t('Zabbix templateid'),
      '#value' => $action == 'update' ? $zabbix_role->templateid : '',
      '#description' => t('The id of the template thsi role is connected to.\n
                             It corresponds to hosts.hostid in the zabbix database (templates are hosts with status == 3 in zabbix). Separate multiple templateids by commas'),
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
  $sortorder = $form_state['clicked_button']['#post']['sortorder'];
  $typeid = $form_state['clicked_button']['#post']['typeid'];
  $imagesrc = $form_state['clicked_button']['#post']['imagesrc'];
  $rolename = $form_state['clicked_button']['#post']['name'] != '' ? $form_state['clicked_button']['#post']['name'] : 'null';
  $roledescription = $form_state['clicked_button']['#post']['description'] != '' ? $form_state['clicked_button']['#post']['description'] : 'null';
  $templateids = explode(',', $form_state['clicked_button']['#post']['templateid']);

  if ($roleid != '') {
    // update the role
    $sql = "update {zabbix_role} set name = '%s', description = '%s', typeid = %d, imagesrc = '%s', sortorder = %d where roleid = %d";
    $result = db_query($sql, $rolename, $roledescription, $typeid, $imagesrc, $sortorder, $roleid);
    if ($result) {
      drupal_set_message(t('Host role updated succesfully'));
    }
    else {
      drupal_set_message(t('An error occured updating the host role'), DRUPAL_MSG_TYPE_ERR);
    }
  }
  else {

    $sql = "insert into {zabbix_role} (name, description, sortorder, typeid, imagesrc) values ('%s', '%s', %d, %d, '%s')";
    $result = db_query($sql, $rolename, $roledescription, $sortorder, $typeid, $imagesrc);
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

/**
 * Confirmation message asking if the user really wants to delete the mapping
 *
 * @param <type> $form_state
 * @param <type> $action
 * @param <type> $id
 * @return <type>
 */
function zabbix_bridge_role_mapping_delete_confirm(&$form_state, $action, $id) {

  $form['_mapping'] = array(
      '#type' => 'value',
      '#value' => $id,
  );

  // TODO: check if no hosts have this role assigned
  return confirm_form(
  $form,
  t("Are you sure you want to delete mapping $id?"),
  isset($_GET['destination']) ? $_GET['destination'] : "admin/settings/zabbix_bridge",
  t("This action cannot be undone.\nWARNING: currently there is no check that no host has this role assigned. Be careful!"),
  t('Delete'),
  t('Cancel')
  );
}

/**
 * Processes the actual submit, deleting teh role mapping
 *
 * @param <type> $form
 * @param <type> $form_state
 */
function zabbix_bridge_role_mapping_delete_confirm_submit($form, &$form_state) {

  if ($form_state['values']['confirm']) {
    $id = $form_state['values']['_mapping'];

    if (zabbix_role_mapping_delete($id)) {
      drupal_set_message(t("Mapping $id has been deleted successfully."));
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
 * Imports all templates from zabbix into the database
 *
 * @return <type>
 */
function zabbix_bridge_import_roles() {

  $templates = array();
  // retrieve a list of templateids from zabbix
  $templates = zabbix_bridge_get_all_templateids_from_zabbix(TRUE);

  zabbix_bridge_debug(print_r($templates, TRUE));

  // go through them and create the drupal mappings. Skip the ones that already exist
  foreach ($templates as $template) {
    $sql = "select * from {zabbix_roles_templates} where templateid = :templateid";
    $results = db_query($sql, array(':templateid' => $template['templateid']));

    $roleid = db_fetch_object($results);

    if ($roleid) {
      drupal_set_message(t('Skipped template ' . $template['templateid'] . ' (' . $template['name'] . '), as it already exists.'));
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
