<?php

/*
*  All the forms and form_submit functions are created here
*/
 
function domain_migrate_generate_book_form($form_state) {
  
   $form = array();
  
   $form['generate_book_form'] = array(
    '#type' => 'fieldset',
    '#title' => t('Generate file for book migration'),
    '#tree' => TRUE,
  ); 
  
    $books = book_get_books();
    $bookarray = array();
    foreach($books as $book) {
      $bookarray[$book['bid']] = $book['title'];   
    }  
    
  
    $form['generate_book_form']['domain_migrate_books'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Choose books to migrate'),
    '#default_value' => variable_get('domain_migrate_books', array('book')),
    '#options' => $bookarray,
  );
  
  $form['generate_book_form']['domain_migrate_books_subdomain_name'] = array(
    '#type' => 'radios',
    '#title' => t('Generate subdomain prefix name'),
    '#default_value' => variable_get('domain_migrate_books_subdomain_name', 'path'),
    '#options' => array('path'=>'Top level book page url path' , 'language'=> 'Top level book page language', 'bid' => 'Book ID'),
    '#required' => TRUE,
  );
  
  $form['generate_book_form']['domain_migrate_books_modify_subdomain'] = array(
    '#type' => 'radios',
    '#title' => t('Modify the subdomain prefix name by adding/subtracting text'),
    '#default_value' => variable_get('domain_migrate_books_modify_subdomain', 'no'),
    '#options' => array('no' => 'Don\'t modify path' , 'add'=>'Add text to beginning of path' , 'subtract'=> 'Subtract text from beginning of path'),
    '#required' => TRUE,
    '#description'=>'The subtract option only makes sense if you have selected the "Top level book page url path" option above, or if you have chosen "Top level book page language" and your root language is the same for each book (e.g. <em>en-IE</em>, <em>en-US</em>, <em>en-AU</em>).', 
  );
  
  $form['generate_book_form']['domain_migrate_books_subdomain_string'] = array(
    '#type' => 'textfield',
    '#title' => t('Text to add/subtract from subdomain prefix'),
    '#default_value' => variable_get('domain_migrate_books_subdomain_string', ''),
  );
  
  $form['generate_book_form']['domain_migrate_books_sitename'] = array(
    '#type' => 'radios',
    '#title' => t('Generate domain sitename'),
    '#default_value' => variable_get('domain_migrate_books_sitename', 'subdomain'),
    '#options' => array('subdomain'=>'The subdomain name (capitalized, underscores and hyphens replaced with spaces)' , 'book_title'=> 'The top level book page title'),
    '#required' => TRUE,
  );
  
  $form['submit'] = array('#type' => 'submit', '#value' => t('Generate migration file'));
  
  return $form;
  
}  


function domain_migrate_generate_book_form_submit($form, &$form_state) {

  $txtFile = drupal_get_path('module','domain_migrate') . '/subdomains.txt';
  $fh = fopen($txtFile, 'w');
   
  $stringData = '"bid","domain","sitename"\n'."\r\n";
  fwrite($fh, $stringData);

  foreach ($form_state['values']['generate_book_form']['domain_migrate_books'] as $bid => $value) {
    
    if($value > 0) {
     
      $sd_name = $form_state['values']['generate_book_form']['domain_migrate_books_subdomain_name'];
      switch($sd_name) {
    
        case 'path': 
          $sd_prefix = drupal_get_path_alias('node/'.$bid);
          break; 
                
        case 'language':
          $node = node_load($bid);
          $sd_prefix = $node->language;     
          break; 
                
        case 'bid': 
          $sd_prefix = $bid;
          break;      
      }   
      
      $sd_string = $form_state['values']['generate_book_form']['domain_migrate_books_subdomain_string'];
      $sd_modify = $form_state['values']['generate_book_form']['domain_migrate_books_modify_subdomain']; 
      domain_migrate_modify_subdomain(&$sd_prefix, $sd_string, $sd_modify);
       
      $sd_snoption = $form_state['values']['generate_book_form']['domain_migrate_books_sitename'];
    
      switch($sd_snoption) {
    
        case 'subdomain': 
          $sd_sitename = str_replace(array('_','-'),' ',$sd_prefix);
          $sd_sitename = ucwords($sd_sitename);
          break; 
                
        case 'book_title':
          $node = node_load($bid);
          $sd_sitename = $node->title;     
          break; 
                     
      }
      $stringData = '"'.$bid .'","'. $sd_prefix .'","'. $sd_sitename.'"\n'."\r\n";
      fwrite($fh, $stringData);
   }             
 }   
 
 variable_set('domain_migrate_books', $form_state['values']['generate_book_form']['domain_migrate_books']);
 variable_set('domain_migrate_books_subdomain_name', $form_state['values']['generate_book_form']['domain_migrate_books_subdomain_name']);
 variable_set('domain_migrate_books_subdomain_string', $form_state['values']['generate_book_form']['domain_migrate_books_subdomain_string']);
 variable_set('domain_migrate_books_modify_subdomain', $form_state['values']['generate_book_form']['domain_migrate_books_modify_subdomain']);
 variable_set('domain_migrate_books_sitename', $form_state['values']['generate_book_form']['domain_migrate_books_sitename']); 
 
 drupal_set_message('Subdomain file written to '.$txtFile);
 fclose($fh); 
  
}  


function domain_migrate_generate_taxonomy_form($form_state) {
  
  $form = array();
  
  $form['generate_taxonomy_form'] = array(
    '#type' => 'fieldset',
    '#title' => t('Generate file for taxonomy migration'),
    '#tree' => TRUE,
  ); 
  
  $result = db_query("SELECT tid, name FROM {term_data}");
  $termarray = array();
  while ($term = db_fetch_array($result)) {
     $termarray[$term['tid']] = $term['name'];   
  }  
    
  
  $form['generate_taxonomy_form']['domain_migrate_terms'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Choose taxonomy terms to migrate'),
    '#default_value' => variable_get('domain_migrate_terms', array('terms')),
    '#options' => $termarray,
  );
  
  $form['generate_taxonomy_form']['domain_migrate_terms_subdomain_name'] = array(
    '#type' => 'radios',
    '#title' => t('Generate subdomain prefix name'),
    '#default_value' => variable_get('domain_migrate_terms_subdomain_name', 'name'),
    '#options' => array('name'=>'Term name' , 'tid' => 'Term ID'),
    '#required' => TRUE,
    '#description'=>'Note that the domain sitename will be automatically generated from the term name.'
  );
  
  $form['generate_taxonomy_form']['domain_migrate_terms_modify_subdomain'] = array(
    '#type' => 'radios',
    '#title' => t('Modify the subdomain prefix name by adding/subtracting text'),
    '#default_value' => variable_get('domain_migrate_terms_modify_subdomain', 'no'),
    '#options' => array('no' => 'Don\'t modify path' , 'add'=>'Add text to beginning of path' , 'subtract'=> 'Subtract text from beginning of path'),
    '#required' => TRUE, 
  );
  
  $form['generate_taxonomy_form']['domain_migrate_terms_subdomain_string'] = array(
    '#type' => 'textfield',
    '#title' => t('Text to add/subtract from subdomain prefix'),
    '#default_value' => variable_get('domain_migrate_terms_subdomain_string', ''),
  );
 
  $form['submit'] = array('#type' => 'submit', '#value' => t('Generate migration file'));
  
  return $form;
  
}  


function domain_migrate_generate_taxonomy_form_submit($form, &$form_state) {

  $txtFile = drupal_get_path('module','domain_migrate') . '/subdomains.txt';
  $fh = fopen($txtFile, 'w');
   
  $stringData = '"tid","domain","sitename"\n'."\r\n";
  fwrite($fh, $stringData);

  foreach ($form_state['values']['generate_taxonomy_form']['domain_migrate_terms'] as $tid => $value) {
    
    if ($value > 0) {
      
      $term = taxonomy_get_term($tid);
     
      $sd_name = $form_state['values']['generate_taxonomy_form']['domain_migrate_terms_subdomain_name'];
      switch($sd_name) {
    
        case 'term': 
          $sd_prefix = strtolower(str_replace(' ','_',$term->name));
          break; 
                
        case 'tid': 
          $sd_prefix = $tid;
          break;      
      }   
  
      $sd_string = $form_state['values']['generate_taxonomy_form']['domain_migrate_terms_subdomain_string'];
      $sd_modify = $form_state['values']['generate_taxonomy_form']['domain_migrate_terms_modify_subdomain']; 
      domain_migrate_modify_subdomain(&$sd_prefix, $sd_string, $sd_modify);
    
      $sd_sitename = ucwords($term->name);
      
      $stringData = '"'.$tid .'","'. $sd_prefix .'","'. $sd_sitename.'"\n'."\r\n";
      fwrite($fh, $stringData);      
                     
    }               
 }   
 
 variable_set('domain_migrate_terms', $form_state['values']['generate_taxonomy_form']['domain_migrate_terms']);
 variable_set('domain_migrate_terms_subdomain_name', $form_state['values']['generate_taxonomy_form']['domain_migrate_terms_subdomain_name']);
 variable_set('domain_migrate_terms_subdomain_string', $form_state['values']['generate_taxonomy_form']['domain_migrate_terms_subdomain_string']);
 variable_set('domain_migrate_terms_modify_subdomain', $form_state['values']['generate_taxonomy_form']['domain_migrate_terms_modify_subdomain']);
 
 drupal_set_message('Subdomain file written to '.$txtFile);
 fclose($fh); 
  
}  
  
function domain_migrate_settings_form($form_state) {
  
  $form = array(); 
  
  $form['domain_migrate_general_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
    '#tree' => FALSE,
  ); 
    
  $form['domain_migrate_general_settings']['domain_migrate_override_grants'] = array(
    '#type' => 'checkbox',
    '#title' => t('Override existing node-domain relationships during migration'),
    '#default_value' => variable_get('domain_migrate_override grants', TRUE),
    '#description'=>'If unchecked, the node will be assigned to the new domain in addition to any domains it was already assigned to. If checked, it will be assigned to the new domain only. This setting will not affect nodes assigned to all affiliates.', 
  );  
   
  $form['domain_migrate_general_settings']['domain_migrate_create_menus'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Create and assign menus for each domain'),
    '#default_value' => variable_get('domain_migrate_create_menus', array()),
    '#options' => array('primary' => 'Primary Links' , 'secondary'=>'Secondary Links' , 'navigation'=> 'Navigation'),
    '#description'=>'For each box checked, a new menu will be created and will appear instead of the sitewide menu for that domain.',
  );
  
  if (module_exists('locale')) {
  
    $form['domain_migrate_language_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Migration language settings'),
      '#tree' => FALSE,
    ); 
    
    $form['domain_migrate_language_settings']['domain_migrate_assign_language'] = array(
      '#type' => 'radios',
      '#title' => t('Assign language to domain'),
      '#default_value' => variable_get('domain_migrate_assign_language', 'no'),
      '#options' => array('no'=>'Don\'t assign a language' , 'book'=> 'Assign language to domainbased on language of top-level book', 'subdomain' => 'Assign a public language to domain based on subdomain name, create it if not already available', 'private'=> 'Create and assign a private domain language to the domain based on subdomain name' ),
      '#required' => TRUE,
      '#description'=>'The third option only makes sense if your subdomain name matches a Google RFC-4646 recognised language code (e.g. <em>en</em>, <em>en-US</em>, <em>de-AT</em>). The fourth option creates a RFC-4646 compliant \'private\' language for your domain (e.g. <em>en-x-media</em>, <em>de-x-blog</em>) - this can serve as a workaround to avoid path conflicts if you will have nodes with the same URL alias on different domains.', 
    ); 
  
    $form['domain_migrate_language_settings']['domain_migrate_language_nodes'] = array(
      '#type' => 'checkbox',
      '#title' => t('Assign domain language to all nodes and aliases on that domain'),
      '#default_value' => variable_get('domain_migrate_language_nodes', FALSE),
      '#description'=>'Recommended if you have enabled domain-based language negotiation and you choose to assign a language to each domain.', 
    );
  } 
  
  if (module_exists('book')) {  
  
    $form['domain_migrate_book_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Book to domain migration settings'),
      '#tree' => FALSE,
    ); 
         
    $form['domain_migrate_book_settings']['domain_migrate_user_role'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Assign author of top level book page node to domain'),
      '#default_value' => variable_get('domain_migrate_user_role', array('user')),
      '#options' => user_roles(TRUE),
      '#description'=>'If you select a role, the author of the top level book page node will be given that role on that domain. If no role is selected, the author is not assigned to the domain. Selecting authenticated user only will assign the author to the domain without giving them any custom roles on that domain.',
    );
  
    $form['domain_migrate_book_settings']['domain_migrate_alter_path'] = array(
      '#type' => 'checkbox',
      '#title' => t('Remove top-level book page path from URL alias of all book nodes'),
      '#default_value' => variable_get('domain_migrate_alter_path', FALSE),
      '#description'=>'Use this if the book hierarchy is reflected in the URL path of the book nodes. Helpful for moving from subfolders to subdomains e.g. <em>www.example.com/sub/path</em> to <em>sub.example.com/path</em>.', 
    ); 
  
    $form['domain_migrate_book_settings']['domain_migrate_htaccess_redirect'] = array(
      '#type' => 'checkbox',
      '#title' => t('Generate .htaccess rules for redirecting book URLs to their subdomains'),
      '#default_value' => variable_get('domain_migrate_htaccess_redirect', FALSE),
      '#description'=>'Again, helpful for moving from subfolders to subdomains. At the moment, it only works if the "Remove top-level book page path" checkbox has been ticked too', 
    );  
  }   
  return system_settings_form($form); 
  
}   
  
function domain_migrate_form($form_state) {
  
  $form = array();  

  $form['domain_migrate_form'] = array(
    '#type' => 'fieldset',
    '#title' => t('Migrate nodes to domains'),
    '#tree' => TRUE,
    '#description' => 'Submitting this form will cause many irreversible changes to your database - please back it up! Also, be sure that your settings have been saved.',
  );

  $form['domain_migrate_form']['subdomains'] = array(
    '#type' => 'textarea',
    '#title' => t('Paste migration file contents'),
    '#description' => t('Comma separated values: (Book ID, Domain prefix, Domain sitename)'),
    '#default_value' => isset($form_state['values']) ? $form_state['values']['drupal_aliases']['subdomains'] :
      file_get_contents(drupal_get_path('module','domain_migrate') . '/subdomains.txt',TRUE),
    '#rows' => 10,
    '#required' => FALSE,
  );
  
  $form['submit'] = array('#type' => 'submit', '#value' => t('Migrate nodes to domain'));

  return $form;
}

/**
 * Submit for migration form
 */
 
function domain_migrate_form_submit($form, &$form_state) {
  
  $is_book = TRUE;
  $subdomains = csv_to_array($form_state['values']['domain_migrate_form']['subdomains'], $is_book);
  domain_migrate_build_domains($subdomains, $is_book);

  // rebuilds the form with entered values
  $form_state['rebuild'] = TRUE;
}

/**
 * Add/subtract text from subdomain string
 */

function domain_migrate_modify_subdomain(&$sd_prefix, $sd_string, $sd_modify) {

  if (!empty($sd_string)) {     
    switch($sd_modify) {
    
      case 'add': 
        $sd_prefix = $sd_string . $sd_prefix;
        break; 
      case 'subtract':
        $sd_pos = strpos($sd_prefix, $sd_string);           
        if($sd_pos === 0) {
          $sd_prefix = substr($sd_prefix, strlen($sd_string));  
        }  
        break;  
      case 'no':
        break;    
    }  
  }     
}      

/*
*  The following 2 functions are taken from the php str_getcsv page (http://php.net/manual/en/function.str-getcsv.php)
*  We are using csv_explode instead of str_getcsv to support PHP < 5.3
*/

function csv_to_array($csv, &$is_book, $delimiter = ',', $enclosure = '"', $escape = '\\', $terminator = '\n') {
    
  $r = array();
  $rows = explode($terminator, trim($csv));
  $names = array_shift($rows);
  $names = csv_explode($names, $delimiter, $enclosure);
  
  // Check wehther the first line is bid or tid
  $is_book = ($names[0] == 'bid');
  
  $nc = count($names);
  foreach ($rows as $row) {
    if (trim($row)) {
      $values = csv_explode($row, $delimiter, $enclosure);
      if (!$values) $values = array_fill(0, $nc, NULL);
      $r[] = array_combine($names, $values);
    }
  }
  return $r;
} 

function csv_explode($str, $delim=',', $enclose='"', $preserve=false){
  $resArr = array();
  $n = 0;
  $expEncArr = explode($enclose, $str);
  foreach($expEncArr as $EncItem){
    if($n++%2){
      array_push($resArr, array_pop($resArr) . ($preserve?$enclose:'') . $EncItem.($preserve?$enclose:''));
    }else{
      $expDelArr = explode($delim, $EncItem);
      array_push($resArr, array_pop($resArr) . array_shift($expDelArr));
      $resArr = array_merge($resArr, $expDelArr);
    }
  }
  return $resArr;
} 


    

