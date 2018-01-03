<?php
  session_start();
  if(!empty($_SESSION['username']) && !empty($_SESSION['password']) ){
    header("Location: main/main.php");
  }
?>
<!DOCTYPE html>
<html lang="en" >
<head>
  <meta charset="UTF-8">
  <title>OS File System</title>
  
  
  <link rel='stylesheet prefetch' href='https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/4.0.0-beta.2/css/bootstrap.css'>

      <link rel="stylesheet" href="css/login.css">

  
</head>

<body>
  
<div class="top"><a class="left" href="index.html">
    <div class="title">OS File System</div></a>
  <div class="right"><a id="loginin" href="login.php">LOG IN</a><a id="signup" href="https://accounts.google.com/SignUp">SIGN UP</a></div>
</div>
<div class="container">
  <form class="form-signin" id="post" method="POST" action="session.php">
    <h2>OS File System</h2>
    <label class="sr-only"></label>
    <input class="form-control" id="username" name="username" placeholder="Username"/>
    <label class="sr-only"></label>
    <input class="form-control" id="password" name="password" type="password" placeholder="password"/>
    <div id="information"><img src="http://dnsset.idv.tw/pic/failed.svg"/>
      <p>Login Failed.</p>
    </div>
    <div class="btn btn-lg btn-primary btn-block" id="b_login">Log In</div>
  </form>
</div>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>
<script src='https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/4.0.0-beta/js/bootstrap.min.js'></script>

    <script  src="js/login.js"></script>

</body>
</html>
