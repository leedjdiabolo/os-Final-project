var ok = 0;

$("#b_login").click(function(){
	$(this).removeClass("btn-primary");
	$(this).addClass("btn-warning");
	$(this).text("Processing ... ");
	$(this).css("cursor","progress");
	var username = $("#username").val();
	var password = $("#password").val();

	$.ajax({
    // url: "http://dnsset.idv.tw/1.html?u="+username+"&p="+password,
    url:"http://dnsset.idv.tw:4000/login.cgi?u="+username+"&p="+password,
    type: 'GET',
    success: function(res){
    	console.log(res);
    	ok = res.response;
		check_result();
    },
    error: function(){
    	console.log("Error on ajax");
    }
  });	
});

function check_result(){
	setTimeout(function(){
		if(ok == 0){
			$("#b_login").removeClass("btn-warning");
			$("#b_login").addClass("btn-primary");
			$("#information").css("height","initial");
			$("#b_login").text("Log In");
			$("#b_login").css("cursor","pointer");
		}
		else{
			$("#b_login").removeClass("btn-warning");
			$("#b_login").addClass("btn-success");
			$("#b_login").text("Log In Sucess");
			setTimeout(function(){
				$("#post").submit();			
			},700);
		}
	}, 900);
}