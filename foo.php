<?php

$i = "hello";

if($_POST['msg'] != "")
	$i = $_POST['msg'];

$o = system("dic $i");

echo "
<html>
 <head>
  <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">
  <br>
  <br>
   <b>Lookup more</b>
  <br>
  <form method=\"POST\" action=\"foo.php\">
  <input type=\"text\" name=\"msg\" />
  <input type=\"submit\" />
  </form>
 </head>
</html>
";

?>
