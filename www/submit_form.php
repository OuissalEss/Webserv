<?php
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        // collect value of input field
        if (isset($_POST['name']))
        {
            $name = $_POST['name'];
            if (empty($name)) {
            echo "Name is empty";
            } else {
            echo $name;
            }
        }
        else
        {
            echo "Name is not set";
        }
    }
?>