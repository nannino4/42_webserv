<!-- <?php

// Show all information, defaults to INFO_ALL
phpinfo();

// Show just the module information.
// phpinfo(8) yields identical results.
//phpinfo(INFO_MODULES);

?> -->

<?php
ob_start();
phpinfo();
$info = ob_end_clean();
?>