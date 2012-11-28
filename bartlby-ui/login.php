<?php
session_start();
$_SESSION[username]=$_POST[login_name];
$_SESSION[password]=$_POST[login_password];
sleep(1);
header("Location: overview.php");
?>