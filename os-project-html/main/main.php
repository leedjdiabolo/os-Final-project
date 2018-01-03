<?php
  session_start();

  if(empty($_SESSION['username']) && empty($_SESSION['password']) ){
    header("Location: ../index.html");
  }
?>
<!DOCTYPE html>
<html lang="en" >
<head>
  <meta charset="UTF-8">
  <title>MainPage</title>
  
  
  <link rel='stylesheet prefetch' href='https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/4.0.0-beta.2/css/bootstrap.css'>

      <link rel="stylesheet" href="css/main.css">

  
</head>

<body>
  
<div id="loadpage">
  <div id="block_loadpage">
    <p>Processing</p>
    <div class="circle"></div>
  </div>
</div>
<div id="showpage">
  <div id="block_showpage">
    <p id="show_file_name">123</p>
    <div id="content"></div>
    <div class="btn btn-primary" id="b_back">Back</div>
  </div>
</div>
<div id="editpage">
  <div id="block_editpage">
    <p id="edit_file_name">123</p>
    <textarea id="textarea"></textarea>
    <div class="btn btn-primary" id="b_save">Save</div>
    <div class="btn btn-info" id="b_edit_back">Back</div>
  </div>
</div>
<div id="searchpage">
  <div id="block_searchpage">
    <div class="search_title">Search</div>
    <div class="input-group input-group-sm"><span class="input-group-addon" v-if="new_folder">Search Name:</span>
      <input class="form-control" id="search_name" type="text" placeholder="Input Here"/><span class="input-group-btn">
        <button class="btn btn-info" id="b_search_search">Search</button></span><span class="input-group-btn">
        <button class="btn btn-danger" id="b_search_cancel">Cancel</button></span>
    </div>
    <div id="search_result_information">Search done</div>
    <table class="table" id="vm_search">
      <thead class="thead-dark">
        <tr>
          <th scope="col">Result</th>
        </tr>
      </thead>
      <tbody>
        <tr v-for="item in search_result_list">
          <td class="td_search_result">{{item.dir_full}}</td>
        </tr>
      </tbody>
    </table>
  </div>
</div>
<div id="movepage">
  <div id="block_movepage">
    <form>
      <div class="form-group">
        <label id="move_title" for="vm_move">Move</label>
        <hr/>
        <select class="form-control" id="vm_move">
          <option v-for="item in move_list" value="{{item}}">{{item}}</option>
        </select>
        <button class="btn btn-primary" type="button" id="b_move_move">Move</button>
        <button class="btn btn-info" type="button" id="b_move_cancel">Cancel</button>
      </div>
    </form>
  </div>
</div>
<div class="top"><a class="left" href="#">
    <div class="title">OS File System</div>
  <div class="right">
    <div id="username"></div><a id="logout" href="clearsession.php">LOG OUT</a>
  </div>
</div>
<div class="container-fluid">
  <div class="row">
    <div id="main">
      <div class="function_bar">
        <div class="function" id="b_home"><img src="http://dnsset.idv.tw/pic/home.svg"/>
          <p>Home</p>
        </div>
        <div class="function" id="b_trash"><img src="http://dnsset.idv.tw/pic/trash.svg"/>
          <p>Trash</p>
        </div>
        <div class="line"></div>
        <div class="function" id="b_search"><img src="http://dnsset.idv.tw/pic/search.svg"/>
          <p>Search</p>
        </div>
        <div class="line"></div>
        <div class="function" id="b_add_file"><img src="http://dnsset.idv.tw/pic/newfile.svg"/>
          <p>Add file</p>
        </div>
        <div class="function" id="b_add_folder"><img src="http://dnsset.idv.tw/pic/newfolder.svg"/>
          <p>Add Folder</p>
        </div>
      </div>
      <div class="function_detail" id="vm0">
        <div class="input-group input-group-sm"><span class="input-group-addon" v-if="new_file">New File Name:</span><span class="input-group-addon" v-if="new_folder">New Folder Name:</span>
          <input class="form-control" id="add_name" type="text" placeholder="Input Here"/><span class="input-group-btn">
            <button class="btn btn-info" id="b_function_add">Add</button></span><span class="input-group-btn">
            <button class="btn btn-danger" id="b_function_cancel">Cancel</button></span>
        </div>
      </div>
      <hr/>
      <div id="result_information">Add File Success</div>
      <div id="location">
        <div class="one_level" v-for="item in level_list" track-by="$index"> 
          <p>/</p>
          <div class="level_name">{{item}} </div>
        </div>
      </div>
      <table class="table" id="vm1">
        <thead class="thead-dark">
          <tr>
            <th scope="col" width="100px">File Type</th>
            <th scope="col">File Name</th>
            <th scope="col">Operation</th>
          </tr>
        </thead>
        <tbody>
          <tr class="tr_line" v-for="item in file_list">
            <td class="td_line"> <img v-if="item.filetype" src="http://dnsset.idv.tw/pic/file.svg"/><img v-if="!item.filetype" src="http://dnsset.idv.tw/pic/folder.svg"/></td>
            <td class="td_line">{{item.filename}}</td>
            <td>
              <button class="b_move btn btn-primary" v-if="!item.deltype">Move</button>
              <button class="b_edit btn btn-primary" v-if="item.filetype && !item.deltype">Edit</button>
              <button class="b_del btn btn-danger" v-if="!item.deltype">Delete</button>
              <button class="b_recover btn btn-info" v-if="item.deltype">Recover</button>
            </td>
          </tr>
        </tbody>
      </table>
    </div>
  </div>
</div>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery/3.1.1/jquery.min.js'></script>
<script src='https://cdnjs.cloudflare.com/ajax/libs/jqueryui/1.12.1/jquery-ui.min.js'></script>
<script src='https://cdnjs.cloudflare.com/ajax/libs/vue/1.0.28/vue.js'></script>
<script src='https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/4.0.0-beta/js/bootstrap.min.js'></script>

    <script type="text/javascript">
      <?php echo 'var username = "'.$_SESSION['username'].'";'; ?>
      <?php echo 'var password = "'.$_SESSION['password'].'";'; ?>
    </script>
    <script  src="js/main.js"></script>

</body>
</html>
