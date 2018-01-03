<?php
	session_start();

	if(!empty($_POST['username']) && !empty($_POST['password']) ){
		echo "not empty , so set session"."<br>";
		$_SESSION['username'] = $_POST['username'];
		$_SESSION['password'] = $_POST['password'];
		echo "username: ".$_SESSION['username']."<br>";
		echo "password: ".$_SESSION['password']."<br>";

		header("Location: main/main.php");
	}
	else{
		echo "someone empty , so cannt pass"."<br>";
	}
?>