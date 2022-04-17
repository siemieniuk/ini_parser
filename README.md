# ini_parser
This script parses standard INI files.

## How to compile?
If you use GCC, simply write command:

<code>gcc -Wall -Wextra main.c utility.c -o iniparser</code>

## How to use script?
### Get value
<code>iniparser <file_path> <section.key></code>

### Do operation
<code>iniparser <file_path> expression "<section1.key1> \<operator> <operator> <section2.key2>"</code>

#### Supported operators:
For int and int:
* <code>\+</code> - addition
* <code>\-</code> - subtraction
* <code>\/</code> - division
* <code>\*</code> - multiplication

For str and str:
* <code>\+</code> - concatenation

Other usages are not available and prints error message.