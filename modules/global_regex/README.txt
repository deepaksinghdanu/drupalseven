Drupal global_regex module:
------------------------

Overview:
--------
Global Regex module lets the developers and site builders create and update
regular expression types globally and apply regular expression based validations
on custom D7 text fields or fields created with form api of drupal7.

Installation:
------------
1. Download and unpack the Global Regex module directory in your modules folder
   (this will usually be "sites/all/modules/").
2. Go to "Administer" -> "Modules" and enable the module.


Configuration:
-------------
1. Go to "Configuration" -> "Global Regex" -> "Add Global Regex"
2. Add the regex types you want to define
3. On CCK field edit field settings form select the type of regex you want to
validate the content of the text field.

OR

For text fields created with drupal7 form api load the regex type with
"global_regex_type_load" and validate the content of the field with the loaded
regex.
