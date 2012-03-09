<?php

/**
 * @file
 *   Example drush command.
 *
 *   To run this *fun* command, execute `sudo drush --include=./examples mmas`
 *   from within your drush directory.
 *
 *   See `drush topic docs-commands` for more information about command authoring. 
 *
 *   You can copy this file to any of the following
 *     1. A .drush folder in your HOME folder.
 *     2. Anywhere in a folder tree below an active module on your site.
 *     3. /usr/share/drush/commands (configurable)
 *     4. In an arbitrary folder specified with the --include option.
 */

/**
 * Implementation of hook_drush_command().
 *
 * In this hook, you specify which commands your
 * drush module makes available, what it does and
 * description.
 *
 * Notice how this structure closely resembles how
 * you define menu hooks.
 *
 * See `drush topic docs-commands` for a list of recognized keys.
 *
 * @return
 *   An associative array describing your command(s).
 */
function jsk_drush_command() {
  $items = array();

  $items['jsk-category'] = array(
    'description' => dt('Personal commands'),
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(dirname(__FILE__) . '/sandwich-topic.txt'),
  );

  $items['jsk-path-generate'] = array(
    'description' => "Generates a path for a given type",
    'arguments' => array('type' => 'node, taxonomy, etc'),
    'options' => array('delete' => 'If the aliases of that type should be deleted first'),
    'drupal_dependencies' => array('pathauto'),
  );

  $items['jsk-node-delete'] = array(
    'callback' => 'my_drush_node_delete',
    'description' => 'Performs a bulk delete operation on nodes.',
    'aliases' => array('ndel'),
    'examples' => array(
      'drush --user=1 node-delete blog' =>  'Deletes all nodes of type blog.',
    ),
    'arguments' => array(
      'type' => 'Type of node to delete. Using all will delete all nodes.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
  );

  $items['jsk-term-delete'] = array(
    'callback' => 'my_drush_term_delete',
    'description' => 'Performs a bulk delete operation on terms.',
    'aliases' => array('tdel'),
    'examples' => array(
      'drush --user=1 term-delete 1' =>  'Deletes all terms in vocabulary 1.',
    ),
    'arguments' => array(
      'type' => 'Vocabulary ID of terms to delete. Using all will delete all terms.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
  );

  $items['jsk-smartqueue-fill'] = array(
    'callback' => 'jsk_drush_smartqueue_fill',
    'description' => 'Creates all taxonomy smartqueues by loading a node associated with each type',
    'aliases' => array('jsk-sf'),
    'examples' => array(
      'drush jsk-sf 1' =>  'Deletes loads a node associated with every term in vocabulary 1.',
    ),
    'arguments' => array(
      'type' => 'Vocabulary ID of terms to use',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
  );

  $items['jsk-apachesolr-reindex-node'] = array(
    'callback' => 'jsk_drush_apachesolr_reindex_node',
    'description' => 'Reindexes a single node. Used for testing custom apachesolr results',
    'aliases' => array('jsk-rin'),
    'examples' => array(
      'drush jsk-as-m 183' =>  'Reindexes a node 183.',
    ),
    'arguments' => array(
      'nid' => 'ID of node to reindex',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_MAX,
  );

  return $items;
}

/**
 * Implementation of hook_drush_help().
 *
 * This function is called whenever a drush user calls
 * 'drush help <name-of-your-command>'. This hook is optional. If a command
 * does not implement this hook, the command's description is used instead.
 *
 * This hook is also used to look up help metadata, such as help
 * category title and summary.  See the comments below for a description.
 *
 * @param
 *   A string with the help section (prepend with 'drush:')
 *
 * @return
 *   A string with the help text for your command.
 */
function jsk_drush_help($section) {
  switch ($section) {
    case 'drush:jsk-path-generate':
      return dt("");
      break;
  }
}

/**
 * Implementation of drush_hook_COMMAND_validate().
 *
 * The validate command should exit with
 * `return drush_set_error(...)` to stop execution of
 * the command.  In practice, calling drush_set_error
 * OR returning FALSE is sufficient.  See drush.api.php
 * for more details.
 */
function drush_jsk_path_generate_validate() {
}

/**
 * Example drush command callback. This is where the action takes place.
 *
 * The function name should be same as command name but with dashes turned to
 * underscores and 'drush_commandfile_' prepended, where 'commandfile' is
 * taken from the file 'commandfile.drush.inc', which in this case is 'sandwich'.
 * Note also that a simplification step is also done in instances where
 * the commandfile name is the same as the beginning of the command name,
 * "drush_example_example_foo" is simplified to just "drush_example_foo".
 * To also implement a hook that is called before your command, implement
 * "drush_hook_pre_example_foo".  For a list of all available hooks for a
 * given command, run drush in --debug mode.
 *
 * If for some reason you do not want your hook function to be named
 * after your command, you may define a 'callback' item in your command
 * object that specifies the exact name of the function that should be
 * called.  However, the specified callback function must still begin
 * with "drush_commandfile_" (e.g. 'callback' => "drush_example_foo_execute")
 * if you want that all hook functions are still called (e.g.
 * drush_example_pre_foo_execute, and so on).
 *
 * In this function, all of Drupal's API is (usually) available, including
 * any functions you have added in your own modules/themes.
 *
 * @see drush_invoke()
 * @see drush.api.php
 */
function drush_jsk_path_generate($type = 'node') {

  _pathauto_include();

  $function = $type .'_pathauto_bulkupdate';
  drush_print('Calling '. $function);
  if (function_exists($function)) { 
    call_user_func($function);
  }
  else { 
    drush_print('Can\'t find that function:'. $function);
  }

}

function my_drush_term_delete($vid = '') { 
  if(empty($vid)) {
    return drush_set_error('Vocabulary ID argument required.');
  }

  else { 
    $rsc = drush_db_select('term_data', 'tid', 'vid = :vid', array(':vid' => $vid));
  }

  $proceed = drush_confirm('You are about to delete a bunch of terms. Proceed?');
  if($proceed) {
    while ($tid = drush_db_result($rsc)) { 
      $result = taxonomy_del_term($tid);
      drush_log("Deleted term '$tid'", $result == SAVED_DELETED ? 'success' : 'failed' );
    }
  }
  else {
    drush_print("Term deletion cancelled.");
  }
}

function my_drush_node_delete($node_type = '') {
  if(empty($node_type)) {
    return drush_set_error('Node type argument required.');
  }

  if($node_type == 'all') {
    $rsc = drush_db_select('node', 'nid');
  }
  else {
    $rsc = drush_db_select('node', 'nid', 'type = :type', array(':type' => $node_type));
  }

  $nodes = array();
  while ($nid = drush_db_fetch_object($rsc)) {
    $nodes[] = $nid;
  }

  if (empty($nodes)) {
    return drush_set_error(dt('No nodes found.'));
  }

  $proceed = drush_confirm('You are about to delete ' . count($nodes) . ' nodes. Proceed?');
  if($proceed) {
    foreach($nodes as $node) {
      node_delete($node->nid);
    }
    $output = 'Successfully deleted ' . count($nodes) . ' nodes of type ' . $node_type;
    drush_log($output, 'success');
  }
  else {
    drush_print("Node deletion cancelled.");
  }
}

function jsk_drush_smartqueue_fill($vid) { 
  drush_print($vid);
  $vocabulary = taxonomy_vocabulary_load($vid);

  if ($vocabulary) { 
    $queue = nodequeue_load(1);
    $query = db_query('SELECT * FROM nodequeue_subqueue s RIGHT JOIN term_data td ON td.tid = s.reference WHERE s.reference IS NULL AND td.vid = %d', $vocabulary->vid);
    while($row = db_fetch_object($query)) { 
      $reference = $row->tid;
      nodequeue_add_subqueue($queue, smartqueue_taxonomy_nodequeue_subqueue_title($queue, $reference), $reference);
    }
  }
}

/** 
 * Function to (re)index one node
 */
function jsk_drush_apachesolr_reindex_node($nid = 0) { 

  // Make sure we have an nid
  if (!$nid) { 
    drush_log("Please supply a valid NID", 'error');
    return 1;
  }

  // Validate nid
  $node = node_load($nid);
  if (!$node) { 
    drush_log("Node $nid not found!", 'error');
    return 2;
  }

  // Insert or update row in apachesolr index table
  $row = db_result(db_query('SELECT 1 FROM {apachesolr_search_node} WHERE nid = %d', $node->nid));
  if (!$row) { 
    db_query('INSERT INTO {apachesolr_search_node} (nid, status, changed) VALUES (%d, %d, %d)', $node->nid, $node->status, time());
  }
  else { 
    apachesolr_mark_node($node->nid);
  }

  // Grab the row and feed it into the build function
  $row = db_fetch_object(db_query('SELECT nid, changed FROM {apachesolr_search_node} WHERE nid = %d', $node->nid));
  $success = apachesolr_index_nodes(array($row), 'apachesolr_search');

  if (!$success) { 
    drush_log('An error occured :(', 'error');
    return 4;
  }

  drush_log('Node \''. $node->title .'\' ('. $node->nid .') has been indexed.', 'ok');
}
