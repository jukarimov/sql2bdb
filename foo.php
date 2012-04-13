<?php

$i = "hello";

if($_POST['msg'] != "")
	$i = $_POST['msg'];

echo "
<html>
 <head>
  <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">
 </head>
  <br>
  <br>
";
$o = system("dic $i");

echo "
  <br>
  <br>
   <b>Lookup more</b>
  <br>
  <form method=\"POST\" action=\"foo.php\">
  <input type=\"text\" name=\"msg\" />
  <input type=\"submit\" />
  </form>
</html>
";

?>
