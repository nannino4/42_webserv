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
move_uploaded_file(
    // Temp image location
    $image_file["tmp_name"],

    // New image location, __DIR__ is the location of the current PHP file
    __DIR__ . "/images/" . $image_file["name"]
);

echo "Here is some more debugging info:\n\n";
print_r($_FILES);

print "</pre>";