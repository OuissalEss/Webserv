<html>
<body>

<form method="get" action="myCgi.php">
  Name: <input type="text" name="fname" id="fname">
  <input type="submit">
</form>

<?php
if ($_SERVER["REQUEST_METHOD"] == "GET") {
  // collect value of input field
  if (isset($_GET['fname']))
  {
      $name = $_GET['fname'];
    
    if (empty($name)) {
      echo "Name is empty";
    } else {
      echo $name;
    }
  }
  
}
?>

</body>
</html>