<?php

// Get reference to uploaded image
$image_file = $_FILES["image"];
//var_dump($_POST);

// Image not defined, let's exit
if (!isset($image_file)) {
    die('No file uploaded.');
}

echo '<pre>';

// Move the temp image file to the images/ directory
if(!file_exists(__DIR__ . "/images/"))
    mkdir(__DIR__ . "/images/");

if (move_uploaded_file($image_file["tmp_name"], __DIR__ . "/images/" . $image_file["name"])) {
    echo "File is valid, and was successfully uploaded.\n";
} else {
    echo "Possible file upload attack!\n";
}

// echo "Here is some more debugging info:\n\n";
// print_r($_FILES);

print "</pre>";