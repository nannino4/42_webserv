<?php

// echo "upload max filesize = ";
// echo ini_get('upload_max_filesize');
// echo "\npost max size = ";
// echo ini_get('post_max_size');
// echo "\nmemory limit = ";
// echo ini_get('memory_limit');
// echo "\n";

// Get reference to uploaded image
$image_file = $_FILES["image"];

// Image not defined, let's exit
if (!isset($image_file)) {
    die('No file uploaded.');
}

echo '<pre>';

// Move the temp image file to the images/ directory
if(!file_exists(__DIR__ . getenv("UPLOAD_PATH")))
    mkdir(__DIR__ . getenv("UPLOAD_PATH"));

if (move_uploaded_file($image_file["tmp_name"], __DIR__ . getenv("UPLOAD_PATH") . $image_file["name"])) {
    echo "File is valid, and was successfully uploaded.\n";
} else {
    echo "Possible file upload attack!\n";
}

// echo "Here is some more debugging info:\n\n";
// echo 'dumping $_SERVER';
// echo "\n";
// var_dump($_SERVER);
// echo 'dumping $_SESSION';
// echo "\n";
// var_dump($_SESSION);
// echo 'dumping $_POST';
// echo "\n";
// var_dump($_POST);
// echo 'dumping $_ENV';
// echo "\n";
// var_dump($_ENV);
// echo 'dumping $_FILES';
// echo "\n";
// print_r($_FILES);

print "</pre>";