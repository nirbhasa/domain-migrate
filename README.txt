Domain Migrate Module

Description
------------
Allows nodes in existing Drupal sites to be migrated to their own subdomain.


Installation
-------------
1. Place the entire module directory into your Drupal sites/all/modules/
   directory.

2. Enable the module by navigating to:
     administer > modules

3. Change migration settings by going to Site building >> Domains >> Domain Migrate >> Migration Settings.

4. To migrate from books, go to Site building >> Domains >> Domain Migrate >> Generate book migration file. To migrate from taxonomy, go to Site building >> Domains >> Domain Migrate >> Generate taxonomy migration file.

5. Fill out the settings there and press 'Generate migration file'. A file called subdomains.txt will be created in your module root. 

6. For the final site migration go to Site building >> Domains >> Domain Migrate >> Migrate nodes to domain. You can see the contents of the subdomain.txt file in the texture widow. Each to-be-migrated subdomain will have its own line, of the form:

(book or taxonomy id, subdomain prefix, subdomain site name)\n

For example, if book id 453 is getting migrated to test2.example.org which has a sitename of Test site, the line will read 

(453, test2, Test site)\n

You can make modifications to subdomain prefix and site names here, before proceeding to final migration.

6. Ensure your database is backed up, then press the button.

7. If you are migrating book nodes selected the 'Generate .htaccess redirect rules' option in settings, a htaccess.txt file will be created in the module root folder. Copty these directives into your htaccess file in the specified places. 


Credits
--------
Vasudeva Server (http://www.vasudevaserver.org)