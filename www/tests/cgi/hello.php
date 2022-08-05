<?php

if (!isset($_GET["name"]))
    die('Param unset.');

print('
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Hello ' . htmlspecialchars($_GET["name"]) . '!</title>
</head>
<body>
<h1>Hello ' . htmlspecialchars($_GET["name"]) . '!</h1>
</body>
</html>
');

?>