<?php
 //phpinfo();    // php info function to get all the information about php 
function generate_random_string($length) {
    $characters = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';
    $random_string = '';
    for ($i = 0; $i < $length; $i++) {
        $random_string .= $characters[rand(0, strlen($characters) - 1)];
    }
    return $random_string;
}

function print_cgi_headers() {
    header("Content-type: text/html");
    echo "\n";
}

function main() {
    // print_cgi_headers();
    $random_string = generate_random_string(10); // Change the number inside the parentheses to set the desired length of the random string
    echo "<h1>Random String:</h1><p>{$random_string}</p>";
}

main();
?>
